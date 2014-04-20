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
			struct timespec interval;
			stat_container_interval(stats, &interval);
			
			printf(dir == IN ? "%s IN\n" : "%s OUT \n", config->networks[i].name);
			printf("%u bytes in %u packets (in %ld.%09ld seconds)", bytes, packets, interval.tv_sec, interval.tv_nsec);
			printf("\n");
			
			stats++;
		}
	}
	
	printf("\n");
}
