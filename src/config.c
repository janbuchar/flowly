#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <err.h>

#include "config.h"
#include "utils.h"
#include "list.h"

#define LINE_SIZE 1024
#define COMMENT_DELIMITER '#'

typedef enum {
	VARIABLES,
	NETWORKS,
	CLIENTS
} config_context_t;

typedef enum {
	E_UNKNOWN_CONTEXT = -1,
	E_INCOMPLETE_VARIABLE = -2,
	E_UNKNOWN_VARIABLE = -3,
	E_INCOMPLETE_CLIENT = -4,
	E_INCOMPLETE_ROUTE = -5,
	E_INVALID_ADDRESS = -6,
	E_INVALID_PORT = -7,
	E_SYNTAX_ERROR = -8,
	E_UNEXPECTED_TOKEN = -9,
	E_NETWORK_NAME_LONG = -10,
	E_INVALID_FORMAT = -11,
	E_FILE_NOT_FOUND = -12
} config_error_t;

typedef struct {
	char name[NET_NAME_LENGTH];
} list_item_network_t;

typedef struct {
	struct sockaddr_storage addr;
	struct sockaddr_storage mask;
	char network[NET_NAME_LENGTH];
	size_t network_id;
} list_item_route_t;

typedef flowly_client_t list_item_client_t;

int
check_context (char *line, config_context_t *context)
{
	if (line[0] == '[') {
		size_t pos = 1;
		
		while (line[pos] != ']') {
			if (line[pos] == 0) {
				return E_SYNTAX_ERROR;
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
			return E_UNKNOWN_CONTEXT;
		}
		
		return 1;
	}
	
	return 0;
}

int 
str_is_numeric (char *s)
{
	while (*s != 0) {
		if (!isdigit(*s)) {
			return 0;
		}
		++s;
	}
	
	return 1;
}

int
parse_variable (flowly_config_t *config, char *line)
{
	static char *delimiter = "\t =\n\r";
	char *name = strtok(line, delimiter);
	char *value = strtok(NULL, delimiter);
	
	if (name == NULL) {
		return 0; // empty line
	}
	
	if (value == NULL) {
		return E_INCOMPLETE_VARIABLE;
	}
	
	if (strcmp(name, "port") == 0) {
		if (atoi(value) > 65535 || atoi(value) < 0) {
			return E_INVALID_PORT;
		}
		strcpy(config->listen_port, value);
	} else if (strcmp(name, "send_interval") == 0) {
		config->send_interval = atoi(value);
	} else {
		return E_UNKNOWN_VARIABLE;
	}
	
	return 0;
}

int
parse_addr (struct sockaddr_storage *addr, char *addr_str)
{
	struct addrinfo *res, hint;
	memset(&hint, 0, sizeof (hint));
	memset(addr, 0, sizeof (struct sockaddr_storage));
	
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_DGRAM;
	
	if (getaddrinfo(addr_str, NULL, &hint, &res) != 0) {
		return E_INVALID_ADDRESS;
	}
	
	memcpy(addr, res->ai_addr, res->ai_addrlen);
	
	freeaddrinfo(res);
	
	return 0;
}

int
parse_client (list_t *clients, char *line)
{
	static char *delimiter = "\t \n\r";
	
	char *addr = strtok(line, delimiter);
	char *port = strtok(NULL, delimiter);
	char *format = strtok(NULL, delimiter);
	
	if (addr == NULL) {
		return 0; // empty line
	}
	
	if (port == NULL) {
		return E_INCOMPLETE_CLIENT;
	}
	
	if (!str_is_numeric(port) || atoi(port) > 65536 || atoi(port) < 0) {
		return E_INVALID_PORT;
	}
	
	list_item_client_t *client = malloc(sizeof (list_item_client_t));
	
	if (format == NULL || strcmp(format, "raw") == 0) {
		client->format = RAW;
	} else {
		return E_INVALID_FORMAT;
	}
	
	int rc = parse_addr(&client->addr, addr);
	if (rc != 0) {
		return rc;
	}
	
	strcpy(client->port, port);
	
	list_add(clients, client);
	
	return 0;
}

int
parse_route (list_t *routes, char *line)
{
	static char *delimiter = "\t \n\r";
	int rc;
	
	char *addr = strtok(line, "/\t \n\r");
	char *mask = strtok(NULL, delimiter);
	char *network = strtok(NULL, delimiter);
	
	if (addr == NULL) {
		return 0; // empty line
	}
	
	if (strlen(network) >= NET_NAME_LENGTH) {
		return E_NETWORK_NAME_LONG;
	}
	
	if (network == NULL) {
		return E_INCOMPLETE_ROUTE;
	}
	
	list_item_route_t *route = malloc(sizeof (list_item_route_t));
	memset(route, 0, sizeof (list_item_route_t));
	
	strcpy(route->network, network);
	
	rc = parse_addr(&route->addr, addr);
	if (rc != 0) {
		free(route);
		return rc;
	}
	
	if (str_is_numeric(mask)) {
		route->mask.ss_family = route->addr.ss_family;
		rc = addr_cidr(&route->mask, atoi(mask));
	} else {
		rc = parse_addr(&route->mask, mask);
	}
	
	if (rc != 0) {
		free(route);
		return rc;
	}
	
	addr_mask(&route->addr, &route->mask);
	
	list_add(routes, route);
	return 0;
}

int
load_route (flowly_route_t *target, list_item_route_t *route)
{	
	target->addr = route->addr;
	target->mask = route->mask;
	target->net_id = route->network_id;
	
	return 0;
}

int 
config_load (flowly_config_t *config, char *path, flowly_config_error_t *err)
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
	config->clients = NULL;
	config->routes = NULL;
	config->networks = NULL;
	
	int rc;
	
	FILE *config_file = fopen(path, "r");
	
	if (config_file == NULL) {
		rc = E_FILE_NOT_FOUND;
		goto error;
	}
	
	char line[LINE_SIZE];
	size_t line_number = 0;
	config_context_t context = VARIABLES;
	
	list_t networks;
	list_t routes;
	list_t clients;
	
	list_init(&networks);
	list_init(&routes);
	list_init(&clients);
	
	while (fgets(line, LINE_SIZE, config_file) != NULL) {
		line_number++;
		
		if ((rc = check_context(line, &context)) || *line == COMMENT_DELIMITER) {
			continue;
		}
		
		if (rc != 0) {
			goto error;
		}
		
		switch (context) {
		case VARIABLES:
			rc = parse_variable(config, line);
			break;
		case NETWORKS:
			rc = parse_route(&routes, line);
			break;
		case CLIENTS:
			rc = parse_client(&clients, line);
			break;
		}
		
		char *token = strtok(NULL, "\t \n\r");
		
		if (token != NULL && token[0] != COMMENT_DELIMITER) {
			rc = E_UNEXPECTED_TOKEN;
		}
		
		if (rc != 0) {
			goto error;
		}
	}
	
	fclose(config_file);
	
	list_node_t *cursor, *cursor_old, *cursor_net;
	size_t i = 0, j = 0;
	
	config->client_count = list_count(&clients);
	config->clients = malloc(config->client_count * sizeof (flowly_client_t));
	
	cursor = clients.head;
	
	while (cursor != NULL) {
		config->clients[i] = *((flowly_client_t *) cursor->val);
		
		i++;
		cursor_old = cursor;
		cursor = cursor->next;
		
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
			list_add(&networks, item);
		}
		
		((list_item_route_t *) cursor->val)->network_id = j;
		rc = load_route(config->routes + i, (list_item_route_t *) cursor->val);
		if (rc != 0) {
			goto error;
		}
		
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
		strcpy(config->networks[i].name, ((list_item_network_t *) cursor->val)->name);
		i++;
		
		cursor_old = cursor;
		cursor = cursor->next;
		free(cursor_old->val);
		free(cursor_old);
	}
	
	return 0;
error:
	if (err != NULL) {
		err->line = line;
		err->line_number = line_number;
	}
	return rc;
}

void 
config_free (flowly_config_t *config)
{
	if (config->clients)
		free(config->clients);
	if (config->networks)
		free(config->networks);
	if (config->routes)
		free(config->routes);
}

char *
config_strerror (config_error_t err)
{
	switch (err) {
	case E_UNKNOWN_CONTEXT:
		return "Unknown configuration context";
	case E_INCOMPLETE_VARIABLE:
		return "Missing value for a variable";
	case E_UNKNOWN_VARIABLE:
		return "Trying to set an unknown option";
	case E_INCOMPLETE_CLIENT:
		return "Client configuration is not complete";
	case E_INCOMPLETE_ROUTE:
		return "Route configuration is not complete";
	case E_INVALID_ADDRESS:
		return "Invalid IP address";
	case E_INVALID_PORT:
		return "Port number must be between 0 and 65535";
	case E_SYNTAX_ERROR:
		return "Syntax error";
	case E_UNEXPECTED_TOKEN:
		return "Unexpected token at the end of the line";
	case E_NETWORK_NAME_LONG:
		return "Network name is too long";
	case E_INVALID_FORMAT:
		return "Invalid output format specified";
	case E_FILE_NOT_FOUND:
		return "Configuration file not found";
	default:
		return "Unknown error";
	}
}
