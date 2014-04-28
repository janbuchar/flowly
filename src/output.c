#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>

#include "common.h"
#include "utils.h"
#include "output.h"
#include "flowstat.h"

#define FLOWLY_PROTO_VERSION 2

int
output_socket ()
{
	struct addrinfo *r, hint;
	int error;
	int sock;
	
	memset(&hint, 0, sizeof (hint));
	hint.ai_family = AF_INET; // TODO version agnostic
	hint.ai_socktype = SOCK_DGRAM;
	
	if ((error = getaddrinfo("localhost", NULL, &hint, &r)) != 0) {
		return -1;
	}
	
	sock = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
	freeaddrinfo(r);
	
	return sock;
}

void
output (flowly_config_t *config, stat_container_t *stats, struct timespec *threshold)
{
	size_t i;
	
	int sock = output_socket();
	size_t stat_count = 2;
	size_t data_size = // Header
		sizeof (output_header_t) 
		// The names of the stats
		+ stat_count * sizeof (output_stat_header_t)
		// A header and stat values for each network
		+ config->network_count * (sizeof (output_network_header_t) + stat_count * sizeof (output_item_t));
	
	output_header_t *header = malloc(data_size);
	
	header->version = htonl(FLOWLY_PROTO_VERSION);
	header->time = htonl(time(NULL));
	header->nanotime = htonl(0);
	header->network_count = htonl(config->network_count);
	header->stat_count = htonl(stat_count);
	
	output_stat_header_t *stat_header = (output_stat_header_t *) (header + 1);
	
	strcpy(stat_header->name, "packet_count");
	stat_header++;
	strcpy(stat_header->name, "byte_count");
	stat_header++;
	
	output_network_header_t *network_header = (output_network_header_t *) stat_header;
	output_item_t *item;
	
	for (i = 0; i < config->network_count; i++) {
		stat_container_t *in = &stats[0];
		stat_container_t *out = &stats[1];
		
		struct timespec interval_in;
		struct timespec interval_out;
		stat_container_interval(in, &interval_in);
		stat_container_interval(out, &interval_out);
		
		strcpy(network_header->network, config->networks[i].name);
		if (interval_in.tv_sec * 1000 + interval_in.tv_nsec / 1000000 <= config->send_interval) {
			network_header->time_in = htonl(interval_in.tv_sec);
			network_header->nanotime_in = htonl(interval_in.tv_nsec);
		} else {
			network_header->time_in = htonl(config->send_interval / 1000);
			network_header->nanotime_in = htonl((config->send_interval % 1000) * 1000000);
		}
		if (interval_out.tv_sec * 1000 + interval_out.tv_nsec / 1000000 <= config->send_interval) {
			network_header->time_out = htonl(interval_out.tv_sec);
			network_header->nanotime_out = htonl(interval_out.tv_nsec);
		} else {
			network_header->time_out = htonl(config->send_interval / 1000);
			network_header->nanotime_out = htonl((config->send_interval % 1000) * 1000000);
		}
		
		item = (output_item_t *) (network_header + 1);
		item->value_in = htonll(stat_container_reduce(in, key_packet_count, sum, threshold));
		item->value_out = htonll(stat_container_reduce(out, key_packet_count, sum, threshold));
		++item;
		
		item->value_in = htonll(stat_container_reduce(in, key_byte_count, sum, threshold));
		item->value_out = htonll(stat_container_reduce(out, key_byte_count, sum, threshold));
		++item;
		
		stats += 2; // in and out
		
		network_header = (output_network_header_t *) item;
	}
	
	flowly_client_t *client;
	
	for (client = config->clients; client < config->clients + config->client_count; ++client) {
		sendto(sock, header, data_size, 0, (struct sockaddr *) &client->addr, client->addrlen);
	}
	
	free(header);
}
