#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include <sys/types.h>
#include <time.h>

#include "common.h"
#include "config.h"
#include "flowstat.h"

typedef struct {
	u_int32_t version;
	u_int32_t time;
	u_int32_t nanotime;
	u_int32_t network_count;
	u_int32_t stat_count;
} output_header_t;

typedef struct {
	char name[STAT_NAME_LENGTH];
} output_stat_header_t;

typedef struct {
	char network[NET_NAME_LENGTH];
} output_network_header_t;

typedef struct {
	stat_number_t value_in;
	stat_number_t value_out;
} output_item_t;

void
output (flowly_config_t *config, stat_container_t *stats, struct timespec *threshold);

#endif