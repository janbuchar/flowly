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
	int socket;
	
	memset(&hint, 0, sizeof (hint));
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_DGRAM;
	hint.ai_flags = AI_PASSIVE;
	
	if ((error = getaddrinfo(NULL, config->listen_port, &hint, &r)) != 0) {
		errx(1, "%s", gai_strerror(error));
	}
	
	for (rorig = r; r != NULL; r = r->ai_next) {
		socket = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
		if (!bind(socket, r->ai_addr, r->ai_addrlen)) {
			break;
		}
		close(socket);
	}
	
	freeaddrinfo(rorig);
	
	if (r == NULL) {
		close(socket);
		errx(1, "getaddrinfo");
	}
	
	int optval = 1;
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval)) == -1) {
		err(1, "setsockopt");
	}
	
	return socket;
}

void
find_network (flowly_config_t *config, void *packet, sample_network_t *result)
{
	struct sockaddr_storage source;
	struct sockaddr_storage destination;
	result->from_found = 0;
	result->to_found = 0;
	
	get_source(packet, &source);
	get_destination(packet, &destination);
	
	size_t i;
	
	for (i = 0; i < config->route_count; i++) {
		if (addr_match(&source, &config->routes[i].addr, &config->routes[i].mask)) {
			result->from = config->routes[i].net_id;
			result->from_found = 1;
			break;
		}
	}
	
	for (i = 0; i < config->route_count; i++) {
		if (addr_match(&destination, &config->routes[i].addr, &config->routes[i].mask)) {
			result->to = config->routes[i].net_id;
			result->to_found = 1;
			break;
		}
	}
}

void
store_stats (stat_container_t *stats, size_t net_id, flow_direction_t dir, void *packet, size_t packet_size)
{
	flowstat_t *item = stat_container_next(stats[net_id][dir]);
	item->time = time(NULL);
	item->byte_count = 0;
	item->packet_count = 0;
}

int 
main (int argc, char **argv) 
{
	flowly_config_t config;
	config_load(&config, NULL);
	
	int sflow_socket = create_socket(&config);
	
	stat_container_t *stats = malloc(2 * config.network_count * sizeof(stat_container_t)); // a container for each network and direction
	
	int n;
	void *packet = malloc(MAX_SFLOW_PACKET_SIZE);
	sample_network_t net;
	
	while ((n = recvfrom(sflow_socket, packet, MAX_SFLOW_PACKET_SIZE, 0, NULL, 0)) > 0) {
		if (n == MAX_SFLOW_PACKET_SIZE) {
			continue; // Now that's a big packet... How about a message?
		}
		
		find_network(&config, packet, &net);
		
		if (net.from_found) {
			store_stats(stats, net.from, OUT, packet);
		}
		if (net.to_found) {
			store_stats(stats, net.to, IN, packet);
		}
	}
	
	return 0;
}
