#include "../src/list.c"
#include "../src/config.c"

int main (int argc, char **argv)
{
	fct_init();
	
	flowly_config_t config;
	int rc = config_load(&config, "config-basic.conf");
	
	assert_int(rc, 0, "config_load() return value");
	
	assert_int(config.network_count, 2, "Correct network grouping");
	assert_int(config.route_count, 3, "Route loading");
	assert_int(config.client_count, 2, "Client loading");
	
	assert_string(config.listen_port, "6666", "Port loading");
	assert_int(config.send_interval, 6666, "Sending interval parsing");
	
	assert_int(config.routes[0].net_id, 0, "Route #1: net_id");
	assert_int(config.routes[1].net_id, 0, "Route #2: net_id");
	assert_int(config.routes[2].net_id, 1, "Route #3: net_id");
	
	assert_string(config.networks[0].name, "network1", "Network #1: name");
	assert_string(config.networks[1].name, "network2", "Network #2: name");
	
	assert_int(config.clients[0].format, RAW, "Client #1: format");
	assert_int(config.clients[1].format, RAW, "Client #2: format");
	
	config_free(&config);
	fct_send();
	return 0;
}