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
output (flowly_config_t *config, stat_container_t *stats)
{
	size_t i;
	size_t dir;
	
	int sock = output_socket();
	size_t item_count = 4 * config->network_count;
	size_t data_size = sizeof (output_header_t) + item_count * sizeof (output_item_t);
	
	output_header_t *header = malloc(data_size);
	output_item_t *items = (output_item_t *) (header + 1);
	
	header->item_count = htonl(item_count);
	header->time = htonl(time(NULL));
	header->nanotime = htonl(0);
	
	for (i = 0; i < config->network_count; i++) {
		for (dir = 0; dir <= 1; dir++) {
			items->direction = dir == 0 ? DIRECTION_IN : DIRECTION_OUT;
			strcpy(items->network, config->networks[i].name);
			strcpy(items->name, "packet_count");
			items->value = htonll(stat_container_reduce(stats, key_packet_count, sum));
			items++;
			
			items->direction = dir == 0 ? DIRECTION_IN : DIRECTION_OUT;
			strcpy(items->network, config->networks[i].name);
			strcpy(items->name, "byte_count");
			items->value = htonll(stat_container_reduce(stats, key_byte_count, sum));
			items++;
			
			stats++;
		}
	}
	
	flowly_client_t *client;
	
	for (client = config->clients; client < config->clients + config->client_count; ++client) {
		sendto(sock, header, data_size, 0, (struct sockaddr *) &client->addr, client->addrlen);
	}
	
	free(header);
}
