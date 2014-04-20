#include <stdio.h>

#include "common.h"
#include "output.h"
#include "flowstat.h"

void
output (flowly_config_t *config, stat_container_t *stats)
{
	size_t i;
	size_t dir;
	
	for (i = 0; i < config->network_count; i++) {
		for (dir = 0; dir <= 1; dir++) {
			stat_number_t bytes = stat_container_reduce(stats, key_byte_count, sum);
			stat_number_t packets = stat_container_reduce(stats, key_packet_count, sum);
			
			printf(dir == IN ? "%s IN\n" : "%s OUT \n", config->networks[i].name);
			printf("%zu bytes in %zu packets", bytes, packets);
			printf("\n");
			
			stats++;
		}
	}
	
	printf("\n");
}
