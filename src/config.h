#include <sys/types.h>
#include <sys/socket.h>

#include "common.h"

typedef enum {
	RAW
} output_format_t;

typedef struct {
	struct sockaddr_storage addr;
	char port[6]; // 5 digits of port + \0
	output_format_t format;
} flowly_client_t;

typedef struct {
	size_t id;
	char name[NET_NAME_LENGTH];
} flowly_network_t;

typedef struct {
	struct sockaddr_storage addr;
	struct sockaddr_storage mask;
	size_t net_id;
} flowly_route_t;

typedef struct {
	flowly_client_t * clients;
	size_t client_count;
	flowly_route_t * routes;
	size_t route_count;
	flowly_network_t * networks;
	size_t network_count;
	size_t send_interval;
	char listen_port[6];
} flowly_config_t;

int config_load (flowly_config_t * config, char * path);

void config_free (flowly_config_t * config);