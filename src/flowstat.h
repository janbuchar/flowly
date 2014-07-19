#ifndef _FLOWSTAT_H_
#define _FLOWSTAT_H_

#include <sys/types.h>
#include <time.h>

#include "common.h"

typedef u_int64_t stat_number_t;

/**
 * A statistic item that contains data from one flow sample
 */
typedef struct {
	stat_number_t packet_count;
	stat_number_t byte_count;
	/** Observation time */
	struct timespec time;
} flowstat_t;

/**
 * A circular buffer that contains the most recent statistic items 
 * for a single network flow and direction
 */
typedef struct {
	/** The amount of fields that have already been filled */
	size_t full;
	/** Index of the field the next item will be added to */
	size_t next;
	/** Statistic items */
	flowstat_t items[STAT_COUNT];
} stat_container_t;

typedef stat_number_t (* reduce_fnc_t) (stat_number_t, stat_number_t);

typedef stat_number_t (* key_fnc_t) (const flowstat_t *);

/**
 * Initialize a statistic container
 * @param container
 */
void 
stat_container_init (stat_container_t * container);

/**
 * Returns a pointer to the next item in @a container
 * @param container
 */
flowstat_t *
stat_container_next (stat_container_t * container);

/**
 * Apply an aggregation function @a fnc to all items of @a container. 
 * The @a key function is used to select the desired field from an item.
 * @param container
 * @param key The key function
 * @param fnc The aggregation function
 * @param threshold If provided, determines which items are too old and should be ignored
 * @return The result of the aggregation
 */
stat_number_t
stat_container_reduce (stat_container_t * container, key_fnc_t key, reduce_fnc_t fnc, struct timespec *threshold);

/**
 * Calculates the time span between the oldest and newest item in the container
 * @param container
 * @param res The result
 */
void
stat_container_interval (stat_container_t *container, struct timespec *res);

/**
 * Free the dynamically allocated fields of @a container (doesn't free @a container itself)
 * @param container
 */
void
stat_container_free (stat_container_t * container);

/**
 * An aggregation function that calculates the sum of numbers
 * @param a The first operand
 * @param b The second operand
 * @return a + b
 */ 
stat_number_t
sum (stat_number_t a, stat_number_t b);

/**
 * A key function that returns the packet count value of a statistic item
 * @param stat
 * @return the packet count
 */
stat_number_t
key_packet_count (const flowstat_t * stat);

/**
 * A key function that returns the byte count value of a statistic item
 * @param stat
 * @return the byte count
 */
stat_number_t
key_byte_count (const flowstat_t * stat);

#endif
