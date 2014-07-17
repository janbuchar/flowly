#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <sys/types.h>
#include <sys/socket.h>

#include "common.h"

typedef enum {
	RAW
} output_format_t;

typedef struct {
	struct sockaddr_storage addr;
	socklen_t addrlen;
	output_format_t format;
} flowly_client_t;

typedef struct {
	char name[NET_NAME_LENGTH];
} flowly_network_t;

typedef struct {
	struct sockaddr_storage addr;
	struct sockaddr_storage mask;
	size_t net_id;
} flowly_route_t;

typedef struct {
	flowly_client_t *clients;
	size_t client_count;
	flowly_route_t *routes;
	size_t route_count;
	flowly_network_t *networks;
	size_t network_count;
	size_t send_interval;
	char listen_port[6];
} flowly_config_t;

typedef struct {
	size_t line_number;
	char *line;
} flowly_config_error_t;

int
config_load (flowly_config_t *config, char *path, flowly_config_error_t *error);

void
config_free (flowly_config_t *config);

char *
config_strerror (int err);

#endif
