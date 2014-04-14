#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#define LINE_SIZE 1024

typedef enum {
	VARIABLES,
	NETWORKS,
	CLIENTS
} config_context_t;

typedef struct list {
	void *val;
	struct list *next;
} list_t;

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
load_variable (flowly_config_t *config, char *name, char *value)
{
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
load_client (flowly_client_t *target, list_item_client_t *client)
{
// 	target->addr = 0;
	
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

void
list_add (list_t **head, list_t **tail, void *item)
{
	list_t *container = malloc(sizeof (list_t));
	container->val = item;
	container->next = NULL;
	
	if (*head == NULL) {
		*head = container;
	}
	if (*tail != NULL) {
		(*tail)->next = container;
	}
	(*tail) = container;
}

size_t
list_count (list_t *head)
{
	list_t *cursor = head;
	size_t result = 0;
	
	while (cursor != NULL) {
		cursor = cursor->next;
		++result;
	}
	
	return result;
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
	char *token, *token2;
	config_context_t context = VARIABLES;
	
	char *var_delim = "\t =\n\r";
	char *route_delim = "\t \n\r";
	char *client_delim = "\t \n\r";
	
	list_t *network_list_head = NULL, *network_list_tail = NULL;
	list_t *route_list_head = NULL, *route_list_tail = NULL;
	list_t *client_list_head = NULL, *client_list_tail = NULL;
	list_item_route_t * route_item;
	list_item_client_t * client_item;
	
	while (fgets(line, LINE_SIZE, config_file) != NULL) {
		if (check_context(line, &context) || *line == '#') {
			continue;
		}
		
		switch (context) {
		case VARIABLES:
			token = strtok(line, var_delim);
			token2 = strtok(NULL, var_delim);
			load_variable(config, token, token2);
			break;
		case NETWORKS:
			route_item = malloc(sizeof (list_item_route_t));
			route_item->addr = strtok(line, route_delim);
			route_item->mask = strtok(NULL, route_delim);
			token = strtok(NULL, route_delim);
			
			if (route_item->addr == NULL) {
				continue; // empty line
			}
			if (token != NULL && strlen(token) < NET_NAME_LENGTH) {
				strcpy(route_item->network, token);
				list_add(&route_list_head, &route_list_tail, route_item);
			} else {
				return -1; // network name too long
			}
			break;
		case CLIENTS:
			client_item = malloc(sizeof (list_item_client_t));
			client_item->addr = strtok(line, route_delim);
			client_item->port = strtok(NULL, route_delim);
			client_item->format = strtok(NULL, route_delim);
			list_add(&client_list_head, &client_list_tail, client_item);
			break;
		}
	}
	
	close(config_file);
	
	list_t *cursor, *cursor_old, *cursor_net;
	size_t i = 0, j = 0;
	
	config->client_count = list_count(client_list_head);
	config->clients = malloc(config->client_count * sizeof (flowly_client_t));
	
	cursor = client_list_head;
	while (cursor != NULL) {
		load_client(config->clients + i, (list_item_client_t *) cursor->val);
		i++;
		cursor_old = cursor;
		cursor = cursor->next;
		free(cursor_old->val);
		free(cursor_old);
	}
	
	config->route_count = list_count(route_list_head);
	config->routes = malloc(config->route_count * sizeof (flowly_route_t));
	
	cursor = route_list_head;
	i = 0;
	while (cursor != NULL) {
		char *network = ((list_item_route_t *) cursor->val)->network;
		
		cursor_net = network_list_head;
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
			list_add(&network_list_head, &network_list_tail, item);
		}
		
		((list_item_route_t *) cursor->val)->network_id = j;
		load_route(config->routes + i, (list_item_route_t *) cursor->val);
		
		i++;
		cursor_old = cursor;
		cursor = cursor->next;
		free(cursor_old->val);
		free(cursor_old);
	}
	
	config->network_count = list_count(network_list_head);
	config->networks = malloc(config->network_count * sizeof (flowly_network_t));
	
	cursor = network_list_head;
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