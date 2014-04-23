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
	stat_number_t item_count;
	struct timespec time;
} output_header_t;

typedef struct {
	char direction;
	char network[NET_NAME_LENGTH];
	char name[STAT_NAME_LENGTH];
	stat_number_t value;
} output_item_t;

void
output (flowly_config_t *config, stat_container_t *stats);

#endif