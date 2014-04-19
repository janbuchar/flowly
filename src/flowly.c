#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <netdb.h>
#include <err.h>
#include <time.h>

#include "common.h"
#include "utils.h"
#include "config.h"
#include "sflow.h"
#include "flowstat.h"

typedef struct {
	int from_found;
	size_t from;
	int to_found;
	size_t to;
} sample_network_t;

typedef enum {
	IN,
	OUT
} flow_direction_t;

int
create_socket (flowly_config_t *config)
{
	struct addrinfo *r, *rorig, hint;
	int error;
	int sock;
	
	memset(&hint, 0, sizeof (hint));
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_DGRAM;
	hint.ai_flags = AI_PASSIVE;
	
	if ((error = getaddrinfo(NULL, config->listen_port, &hint, &r)) != 0) {
		errx(1, "%s", gai_strerror(error));
	}
	
	for (rorig = r; r != NULL; r = r->ai_next) {
		sock = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
		if (!bind(sock, r->ai_addr, r->ai_addrlen)) {
			break;
		}
		close(sock);
	}
	
	freeaddrinfo(rorig);
	
	if (r == NULL) {
		close(sock);
		errx(1, "getaddrinfo");
	}
	
	int optval = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval)) == -1) {
		err(1, "setsockopt");
	}
	
	return sock;
}

void
find_network (flowly_config_t *config, sflow_flow_record_t *record, sample_network_t *result)
{
	struct sockaddr_storage addr;
	size_t i;
	result->from_found = 0;
	result->to_found = 0;
	
	if (get_source((sflow_raw_header_t *) (record + 1), &addr)) {
		for (i = 0; i < config->route_count; i++) {
			if (addr_match(&addr, &config->routes[i].addr, &config->routes[i].mask)) {
				result->from = config->routes[i].net_id;
				result->from_found = 1;
				break;
			}
		}
	}
	
	if (get_destination((sflow_raw_header_t *) (record + 1), &addr)) {
		for (i = 0; i < config->route_count; i++) {
			if (addr_match(&addr, &config->routes[i].addr, &config->routes[i].mask)) {
				result->to = config->routes[i].net_id;
				result->to_found = 1;
				break;
			}
		}
	}
	
}

void
store_stats (stat_container_t *stats, size_t net_id, flow_direction_t dir, sflow_flow_sample_t *sample, sflow_raw_header_t *header)
{
	flowstat_t *item = stat_container_next(&stats[2 * net_id + dir]);
	item->time = time(NULL);
	item->byte_count = 0;
	item->packet_count = 0;
}

int 
main (int argc, char **argv) 
{
	flowly_config_t config;
	
	if (config_load(&config, NULL) == -1) {
		errx(1, "config_load");
	}
	
	int sflow_socket = create_socket(&config);
	
	stat_container_t *stats = malloc(2 * config.network_count * sizeof(stat_container_t)); // a container for each network and direction
	
	int n;
	void *packet = malloc(MAX_SFLOW_PACKET_SIZE);
	sample_network_t net;
	sflow_sample_data_t *sample;
	sflow_flow_record_t *record;
	
	while ((n = recvfrom(sflow_socket, packet, MAX_SFLOW_PACKET_SIZE, 0, NULL, 0)) > 0) {
		if (n == MAX_SFLOW_PACKET_SIZE) {
			continue; // Now that's a big packet... How about a message?
		}
		
		sample = NULL;
		while (next_sample(packet, n, &sample)) {
			if (!is_sample_format(sample, FLOW_SAMPLE)) {
				continue;
			}
			
			record = NULL;
			while (next_record(sample, &record)) {
				if (!is_record_format(record, RAW_HEADER)) {
					continue;
				}
				
				find_network(&config, record, &net);
		
				if (net.from_found) {
					store_stats(stats, net.from, OUT, (sflow_flow_sample_t *) (sample + 1), (sflow_raw_header_t *) (record + 1));
				}
				if (net.to_found) {
					store_stats(stats, net.to, IN, (sflow_flow_sample_t *) (sample + 1), (sflow_raw_header_t *) (record + 1));
				}
			}
		}
	}
	
	return 0;
}
