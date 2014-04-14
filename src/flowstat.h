#include <sys/types.h>
#include <time.h>

#include "common.h"

typedef u_int32_t stat_number_t;

typedef struct {
	stat_number_t packet_count;
	stat_number_t byte_count;
	time_t time;
} flowstat_t;

typedef struct {
	size_t size;
	size_t full;
	size_t next;
	flowstat_t items[STAT_COUNT];
} stat_container_t;

typedef stat_number_t (* reduce_fnc_t) (stat_number_t, stat_number_t);

typedef stat_number_t (* key_fnc_t) (const flowstat_t *);

void 
stat_container_init (stat_container_t * q);

flowstat_t *
stat_container_next (stat_container_t * q);

stat_number_t
stat_container_reduce (stat_container_t * q, key_fnc_t key, reduce_fnc_t fnc);

void
stat_container_free (stat_container_t * q);