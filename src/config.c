#include "config.h"

int 
config_load (flowly_config_t * config, char * path = "/etc/flowly.conf")
{
	config->listen_port = "6343";
	
	return 0;
}

void 
config_free (flowly_config_t * config)
{
	
}