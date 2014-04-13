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
	
	config_free(&config);
	fct_send();
	return 0;
}