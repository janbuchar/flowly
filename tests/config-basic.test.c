#include <stdio.h>
#include <arpa/inet.h>

#include "assert_ip.c"
#include "../src/list.c"
#include "../src/config.c"
#include "../src/utils.c"

int main (int argc, char **argv)
{
	fct_init();
	
	flowly_config_t config;
	int rc = config_load(&config, "config-basic.conf");
	
	char dst[255];
	
	assert_int(rc, 0, "config_load() return value");
	
	assert_int(2, config.network_count, "Correct network grouping");
	assert_int(3, config.route_count, "Route loading");
	assert_int(3, config.client_count, "Client loading");
	
	assert_string("6666", config.listen_port, "Port loading");
	assert_int(6666, config.send_interval, "Sending interval parsing");
	
	assert_int(0, config.routes[0].net_id, "Route #1: net_id");
	assert_ip("8.8.0.0", &config.routes[0].addr, "Route #1: address");
	assert_ip("255.255.0.0", &config.routes[0].mask, "Route #1: mask");
	
	assert_int(0, config.routes[1].net_id, "Route #2: net_id");
	assert_ip("8.0.0.0", &config.routes[1].addr, "Route #2: address");
	assert_ip("255.0.0.0", &config.routes[1].mask, "Route #2: mask");
	
	assert_int(1, config.routes[2].net_id, "Route #3: net_id");
	assert_ip("10.0.0.0", &config.routes[2].addr, "Route #3: address");
	assert_ip("255.255.255.0", &config.routes[2].mask, "Route #3: mask");
	
	assert_string("network1", config.networks[0].name, "Network #1: name");
	assert_string("network2", config.networks[1].name, "Network #2: name");
	
	assert_int(RAW, config.clients[0].format, "Client #1: format");
	assert_ip("127.0.0.1", &config.clients[0].addr, "Client #1: address");
	
	assert_int(RAW, config.clients[1].format, "Client #2: format");
	assert_ip("192.168.0.1", &config.clients[1].addr, "Client #2: address");
	
	assert_int(RAW, config.clients[2].format, "Client #3: format");
	assert_ip6("2001:db8::1428:57ab", &config.clients[2].addr, "Client #3: address");
	
	config_free(&config);
	fct_send();
	return 0;
}