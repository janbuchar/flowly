#include <stdlib.h>

#include "../src/routing.c"
#include "../src/utils.c"

int
main (int argc, char **argv)
{
	fct_init();
	
	struct sockaddr_storage addr;
	flowly_config_t config;
	
	config.route_count = 3;
	config.routes = malloc(config.route_count * sizeof (flowly_route_t));
	
	config.routes[0].net_id = 1;
	inet_pton(AF_INET, "123.123.0.0", &config.routes[0].net.addr);
	config.routes[0].net.mask = 16;
	config.routes[0].net.family = AF_INET;
	
	config.routes[1].net_id = 2;
	inet_pton(AF_INET, "123.123.123.0", &config.routes[1].net.addr);
	config.routes[1].net.mask = 24;
	config.routes[1].net.family = AF_INET;
	
	config.routes[2].net_id = 3;
	inet_pton(AF_INET, "192.0.0.0", &config.routes[2].net.addr);
	config.routes[2].net.mask = 8;
	config.routes[2].net.family = AF_INET;
	
	assert_int(0, routing_init(&config), "Routing initialization");
	
	addr.ss_family = AF_INET;
	inet_pton(AF_INET, "123.123.123.123", &((struct sockaddr_in *) &addr)->sin_addr.s_addr);
	
	assert_int(2, route_match(&addr), "Match longest prefix");
	
	addr.ss_family = AF_INET;
	inet_pton(AF_INET, "255.0.0.0", &((struct sockaddr_in *) &addr)->sin_addr.s_addr);
	
	assert_int(0, route_match(&addr), "Unmatched address");
	
	fct_send();
}