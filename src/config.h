#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <sys/types.h>
#include <sys/socket.h>

#include "common.h"

/**
 * The format in which flowly sends data to its clients 
 */
typedef enum {
	RAW
} output_format_t;

/**
 * Contains information about a client
 */
typedef struct {
	struct sockaddr_storage addr;
	socklen_t addrlen;
	output_format_t format;
} flowly_client_t;

/**
 * Contains information about a network
 */
typedef struct {
	char name[NET_NAME_LENGTH];
} flowly_network_t;

/**
 * Contains information about a route
 */
typedef struct {
	struct sockaddr_storage addr;
	struct sockaddr_storage mask;
	size_t net_id;
} flowly_route_t;

/**
 * Contains flowly's configuration
 */
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

/**
 * Information about any error that happens while loading the configuration
 */
typedef struct {
	size_t line_number;
	char *line;
} flowly_config_error_t;

/**
 * Load configuration from file found at @a path
 * @param config The destination
 * @param path The path to the configuration file
 * @param error If an error happens, information about it gets stored here
 * @return 0 on success, error code on failure
 */
int
config_load (flowly_config_t *config, char *path, flowly_config_error_t *error);

/**
 * Free the dynamically allocated fields of @a config (doesn't free @a config itself)
 * @param config
 */
void
config_free (flowly_config_t *config);

/**
 * Gets the textual representation of an error code
 * @param err The error code
 * @return A description of the error
 */
char *
config_strerror (int err);

#endif
