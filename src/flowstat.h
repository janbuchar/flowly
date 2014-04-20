#ifndef _FLOWSTAT_H_
#define _FLOWSTAT_H_

#include <sys/types.h>
#include <time.h>

#include "common.h"

typedef u_int32_t stat_number_t;

typedef struct {
	stat_number_t packet_count;
	stat_number_t byte_count;
	struct timespec time;
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
stat_container_interval (stat_container_t *s, struct timespec *res);

void
stat_container_free (stat_container_t * q);

stat_number_t
sum (stat_number_t a, stat_number_t b);

stat_number_t
key_packet_count (const flowstat_t * stat);

stat_number_t
key_byte_count (const flowstat_t * stat);

#endif