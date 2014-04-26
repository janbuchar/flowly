#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include <sys/types.h>
#include <time.h>

#include "common.h"
#include "config.h"
#include "flowstat.h"

#define DIRECTION_IN 'I'
#define DIRECTION_OUT 'O'

typedef struct {
	u_int32_t item_count;
	u_int32_t time;
	u_int32_t nanotime;
} output_header_t;

typedef struct {
	char direction;
	char network[NET_NAME_LENGTH];
	char name[STAT_NAME_LENGTH];
	stat_number_t value;
} output_item_t;

void
output (flowly_config_t *config, stat_container_t *stats, struct timespec *threshold);

#endif