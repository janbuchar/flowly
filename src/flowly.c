#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <netdb.h>
#include <err.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>

#include "common.h"
#include "utils.h"
#include "config.h"
#include "sflow.h"
#include "flowstat.h"
#include "output.h"

typedef struct {
	int from_found;
	size_t from;
	int to_found;
	size_t to;
} sample_network_t;

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
		
		int optval = 1;
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval)) == -1) {
			err(1, "setsockopt");
		}
		
		if (bind(sock, r->ai_addr, r->ai_addrlen) == 0) {
			break;
		}
		
		close(sock);
	}
	
	freeaddrinfo(rorig);
	
	if (r == NULL) {
		errx(1, "getaddrinfo: %s", strerror(errno));
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
	
	if (get_source(get_raw_header(record), &addr)) {
		for (i = 0; i < config->route_count; i++) {
			if (addr_match(&addr, &config->routes[i].addr, &config->routes[i].mask)) {
				result->from = config->routes[i].net_id;
				result->from_found = 1;
				break;
			}
		}
	}
	
	if (get_destination(get_raw_header(record), &addr)) {
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
	clock_gettime(CLOCK_REALTIME, &item->time);
	item->byte_count = ntohl(sample->sample_rate) * ntohl(header->frame_length);
	item->packet_count = ntohl(sample->sample_rate);
}

struct {
	flowly_config_t config;
	stat_container_t *stats;
	size_t stats_count;
	pthread_mutex_t mutex;
} context;

void *
output_thread (void *arg)
{
	stat_container_t *stats = malloc(context.stats_count * sizeof (stat_container_t));
	
	struct timespec wait = {
		.tv_sec = context.config.send_interval / 1000,
		.tv_nsec = (context.config.send_interval * 1000000) % 1000000000
	};
	struct timespec last = {
		.tv_sec = 0,
		.tv_nsec = 0
	};
	struct timespec remaining;
	
	for (;;) {
		pthread_mutex_lock(&context.mutex);
		memcpy(stats, context.stats, context.stats_count * sizeof (stat_container_t));
		pthread_mutex_unlock(&context.mutex);
		
		output(&context.config, stats, &last);
		clock_gettime(CLOCK_REALTIME, &last);
		
		nanosleep(&wait, &remaining);
	}
	
	return arg; // Never reached
}

int 
main (int argc, char **argv) 
{
	flowly_config_error_t cfg_err;
	int rc;
	
	if ((rc = config_load(&context.config, NULL, &cfg_err)) < 0) {
		errx(1, "config_load: %s on line %zu\n%s", config_strerror(rc), cfg_err.line_number, cfg_err.line);
	}
	
	int sflow_socket = create_socket(&context.config);
	
	context.stats_count = 2 * context.config.network_count; // a container for each network and direction
	context.stats = malloc(context.stats_count * sizeof(stat_container_t));
	
	stat_container_t *stat_it;
	for (stat_it = context.stats; stat_it - context.stats < 2 * context.config.network_count; ++stat_it) {
		stat_container_init(stat_it);
	}
	
	int n;
	void *packet = malloc(MAX_SFLOW_PACKET_SIZE);
	sample_network_t net;
	sflow_sample_data_t *sample;
	sflow_flow_record_t *record;
	
	pthread_t outputter;
	pthread_mutex_init(&context.mutex, NULL);
	pthread_create(&outputter, NULL, output_thread, &context);
	
	while ((n = recvfrom(sflow_socket, packet, MAX_SFLOW_PACKET_SIZE, 0, NULL, 0)) > 0) {
		if (n == MAX_SFLOW_PACKET_SIZE) {
			continue; // Now that's a big packet... How about a message?
		}
		
		pthread_mutex_lock(&context.mutex);
		
		sample = NULL;
		while (next_sample(packet, n, &sample) > 0) {
			if (!is_sample_format(sample, FLOW_SAMPLE)) {
				continue;
			}
			
			record = NULL;
			while (next_record(sample, &record) > 0) {
				if (!is_record_format(record, RAW_HEADER)) {
					continue;
				}
				
				find_network(&context.config, record, &net);
				
				if (net.from_found) {
					store_stats(context.stats, net.from, OUT, get_flow_sample(sample), get_raw_header(record));
				}
				if (net.to_found) {
					store_stats(context.stats, net.to, IN, get_flow_sample(sample), get_raw_header(record));
				}
			}
		}
		
		pthread_mutex_unlock(&context.mutex);
	}
	
	return 0;
}
