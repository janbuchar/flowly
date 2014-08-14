#include <stdio.h>
#include <arpa/inet.h>

#include "assert_mem.c"
#include "../src/list.c"
#include "../src/config.c"
#include "../src/utils.c"

int main (int argc, char **argv)
{
	fct_init();
	
	flowly_config_t config;
	subnet_t net;
	memset(&net, 0, sizeof( subnet_t));
	
	int rc = config_load(&config, "config-basic.conf", NULL);
	
	char dst[255];
	assert_int(rc, 0, "config_load() return value");
	
	assert_int(2, config.network_count, "Correct network grouping");
	assert_int(5, config.route_count, "Route loading");
	assert_int(3, config.client_count, "Client loading");
	
	assert_string("6666", config.listen_port, "Port loading");
	assert_int(6666, config.send_interval, "Sending interval parsing");
	
	inet_pton(AF_INET, "8.8.0.0", &net.addr);
	assert_mem(&net.addr, &config.routes[0].net.addr, sizeof (net.addr), "Route #1: address");
	assert_int(AF_INET, config.routes[0].net.family, "Route #1: family");
	assert_int(16, config.routes[0].net.mask, "Route #1: mask");
	assert_int(1, config.routes[0].net_id, "Route #1: network id");
	
	inet_pton(AF_INET, "8.0.0.0", &net.addr);
	assert_mem(&net.addr, &config.routes[1].net.addr, sizeof (net.addr), "Route #2: address");
	assert_int(AF_INET, config.routes[1].net.family, "Route #2: family");
	assert_int(8, config.routes[1].net.mask, "Route #2: mask");
	assert_int(1, config.routes[1].net_id, "Route #2: network id");
	
	inet_pton(AF_INET, "10.0.0.0", &net.addr);
	assert_mem(&net.addr, &config.routes[2].net.addr, sizeof (net.addr), "Route #3: address");
	assert_int(AF_INET, config.routes[2].net.family, "Route #3: family");
	assert_int(24, config.routes[2].net.mask, "Route #3: mask");
	assert_int(2, config.routes[2].net_id, "Route #3: network id");
	
	inet_pton(AF_INET, "168.85.58.0", &net.addr);
	assert_mem(&net.addr, &config.routes[3].net.addr, sizeof (net.addr), "Route #4: address");
	assert_int(AF_INET, config.routes[3].net.family, "Route #4: family");
	assert_int(24, config.routes[3].net.mask, "Route #4: mask");
	assert_int(2, config.routes[3].net_id, "Route #4: network id");
	
	inet_pton(AF_INET6, "2001:db8::", &net.addr);
	assert_mem(&net.addr, &config.routes[4].net.addr, sizeof (net.addr), "Route #5: address");
	assert_int(AF_INET6, config.routes[4].net.family, "Route #5: family");
	assert_int(96, config.routes[4].net.mask, "Route #5: mask");
	assert_int(2, config.routes[4].net_id, "Route #5: network id");
	
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
	