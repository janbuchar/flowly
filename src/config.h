#include <sys/types.h>
#include <sys/socket.h>

#define NET_ID_LENGTH 64

typedef enum {
	RAW
} output_format_t;

typedef struct {
	struct sockaddr_storage addr;
	u_int16_t port;
	output_format_t format;
} flowly_client_t;

typedef struct {
	struct sockaddr_storage addr;
	struct sockaddr_storage mask;
	char net_id[NET_ID_LENGTH];
} flowly_route_t;

typedef struct {
	flowly_client_t * clients;
	flowly_route_t * routes;
	size_t send_interval;
	u_int16_t listen_port;
} flowly_config_t;

int config_load (flowly_config_t * config, char * path = "/etc/flowly.conf");

void config_free (flowly_config_t * config);