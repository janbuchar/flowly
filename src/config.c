#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "config.h"
#include "list.h"

#define LINE_SIZE 1024

typedef enum {
	VARIABLES,
	NETWORKS,
	CLIENTS
} config_context_t;

typedef struct {
	char name[NET_NAME_LENGTH];
	size_t id;
} list_item_network_t;

typedef struct {
	char *addr;
	char *mask;
	char network[NET_NAME_LENGTH];
	size_t network_id;
} list_item_route_t;

typedef struct {
	char *addr;
	char *port;
	char *format;
} list_item_client_t;

int
check_context (char *line, config_context_t *context)
{
	if (line[0] == '[') {
		size_t pos = 1;
		
		while (line[pos] != ']') {
			if (line[pos] == 0) {
				return -1; // Syntax error
			}
			++pos;
		}
		
		char *new_context = strtok(line, "[]");
		
		if (strcmp(new_context, "variables") == 0) {
			*context = VARIABLES;
		} else if (strcmp(new_context, "networks") == 0) {
			*context = NETWORKS;
		} else if (strcmp(new_context, "clients") == 0) {
			*context = CLIENTS;
		} else {
			return -1; // Unknown context
		}
		
		return 1;
	}
	
	return 0;
}

int
parse_variable (flowly_config_t *config, char *line)
{
	static char *delimiter = "\t =\n\r";
	char *name = strtok(line, delimiter);
	char *value = strtok(NULL, delimiter);
	
	if (name == NULL || value == NULL) {
		return -1;
	}
	
	if (strcmp(name, "port") == 0) {
		if (atoi(value) > 65535) {
			return -1;
		}
		strcpy(config->listen_port, value);
	} else if (strcmp(name, "send_interval") == 0) {
		config->send_interval = atoi(value);
	} else {
		return -1;
	}
	
	return 0;
}

int
parse_client (list_t *clients, char *line)
{
	static char *delimiter = "\t \n\r";
	
	char *addr = strtok(line, delimiter);
	char *port = strtok(NULL, delimiter);
	char *format = strtok(NULL, delimiter);
	
	if (addr != NULL && port != NULL) {
		list_item_client_t *client = malloc(sizeof (list_item_client_t));
		strcpy(client->addr = malloc(strlen(addr) + 1), addr);
		strcpy(client->port = malloc(strlen(port) + 1), port);
		if (format != NULL) {
			strcpy(client->format = malloc(strlen(format) + 1), format);
		} else {
			client->format = NULL;
		}
		list_add(clients, client);
		return 0;
	}
	return -1;
}

int
parse_route (list_t *routes, char *line)
{
	static char *delimiter = "\t \n\r";
	
	char * addr = strtok(line, delimiter);
	char * mask = strtok(NULL, delimiter);
	char * network = strtok(NULL, delimiter);
	
	if (addr == NULL) {
		return -1; // empty line
	}
	if (network != NULL && strlen(network) < NET_NAME_LENGTH) {
		list_item_route_t *route = malloc(sizeof (list_item_route_t));
		strcpy(route->network, network);
		
		list_add(routes, route);
		return 0;
	} else {
		return -1; // network name too long
	}
}

int
load_client (flowly_client_t *target, list_item_client_t *client)
{	
	struct addrinfo *res, hint;
	memset(&hint, 0, sizeof (hint));
	memset(target, 0, sizeof (flowly_client_t));
	
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_DGRAM;
	
	if (getaddrinfo(client->addr, NULL, &hint, &res) != 0) {
		return -1;
	}
	
	memcpy(&target->addr, res->ai_addr, res->ai_addrlen);
	
	freeaddrinfo(res);
	
	if (atoi(client->port) < 65536) {
		strcpy(target->port, client->port);
	} else {
		return -1;
	}
	
	if (client->format == NULL|| strcmp(client->format, "raw") == 0) {
		target->format = RAW;
	} else {
		return -1;
	}
	
	return 0;
}

int
load_route (flowly_route_t *target, list_item_route_t *route)
{
// 	target->addr = 0;
	
// 	target->mask = 0;
	
	target->net_id = route->network_id;
	
	return 0;
}

int
load_network (flowly_network_t *target, list_item_network_t *network)
{
	target->id = network->id;
	
	
	
	strcpy(target->name, network->name);
	
	return 0;
}

int 
config_load (flowly_config_t *config, char *path)
{
	if (path == NULL) {
		path = "flowly.conf";
	}
	
	// Defaults
	strcpy(config->listen_port, "6343");
	config->send_interval = 1000;
	config->client_count = 0;
	config->route_count = 0;
	config->network_count = 0;
	
	FILE *config_file = fopen(path, "r");
	char line[LINE_SIZE];
	config_context_t context = VARIABLES;
	
	list_t networks;
	list_t routes;
	list_t clients;
	
	list_init(&networks);
	list_init(&routes);
	list_init(&clients);
	
	while (fgets(line, LINE_SIZE, config_file) != NULL) {
		if (check_context(line, &context) || *line == '#') {
			continue;
		}
		
		switch (context) {
		case VARIABLES:
			parse_variable(config, line);
			break;
		case NETWORKS:
			parse_route(&routes, line);
			break;
		case CLIENTS:
			parse_client(&clients, line);
			break;
		}
	}
	
	close(config_file);
	
	list_node_t *cursor, *cursor_old, *cursor_net;
	size_t i = 0, j = 0;
	
	config->client_count = list_count(&clients);
	config->clients = malloc(config->client_count * sizeof (flowly_client_t));
	
	cursor = clients.head;
	
	while (cursor != NULL) {
		load_client(config->clients + i, (list_item_client_t *) cursor->val);
		i++;
		cursor_old = cursor;
		cursor = cursor->next;
		free(((list_item_client_t *) cursor_old->val)->addr);
		free(((list_item_client_t *) cursor_old->val)->port);
		if (((list_item_client_t *) cursor_old->val)->format != NULL) {
			free(((list_item_client_t *) cursor_old->val)->format);
		}
		free(cursor_old->val);
		free(cursor_old);
	}
	
	config->route_count = list_count(&routes);
	config->routes = malloc(config->route_count * sizeof (flowly_route_t));
	
	cursor = routes.head;
	i = 0;
	while (cursor != NULL) {
		char *network = ((list_item_route_t *) cursor->val)->network;
		
		cursor_net = networks.head;
		j = 0;
		while (cursor_net != NULL) {
			if (strcmp(((list_item_network_t *) cursor_net->val)->name, network) == 0) {
				break;
			}
			j++;
			cursor_net = cursor_net->next;
		}
		
		if (cursor_net == NULL) {
			list_item_network_t *item = malloc(sizeof (list_item_network_t));
			strcpy(item->name, network);
			item->id = j;
			list_add(&networks, item);
		}
		
		((list_item_route_t *) cursor->val)->network_id = j;
		load_route(config->routes + i, (list_item_route_t *) cursor->val);
		
		i++;
		cursor_old = cursor;
		cursor = cursor->next;
		free(cursor_old->val);
		free(cursor_old);
	}
	
	config->network_count = list_count(&networks);
	config->networks = malloc(config->network_count * sizeof (flowly_network_t));
	
	cursor = networks.head;
	i = 0;
	while (cursor != NULL) {
		load_network(config->networks + i, (list_item_network_t *) cursor->val);
		
		i++;
		cursor_old = cursor;
		cursor = cursor->next;
		free(cursor_old->val);
		free(cursor_old);
	}
	
	return 0;
}

void 
config_free (flowly_config_t *config)
{
	free(config->clients);
	free(config->networks);
	free(config->routes);
}