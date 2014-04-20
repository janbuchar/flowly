#include <sys/types.h>

#include "flowstat.h"

void 
stat_container_init (stat_container_t * q)
{
	q->next = 0;
	q->full = 0;
}

flowstat_t *
stat_container_next (stat_container_t * q)
{
	if (q->full < STAT_COUNT) {
		++(q->full);
	}
	
	if (q->next == STAT_COUNT) {
		q->next = 1;
		return &(q->items[0]);
	}
	
	return &(q->items[q->next++]);
}

stat_number_t
stat_container_reduce (stat_container_t * q, key_fnc_t key, reduce_fnc_t fnc)
{
	size_t i = 0;
	stat_number_t result = key(&(q->items[0]));
	
	for (i = 1; i < q->full; ++i) {
		result = fnc(result, key(&(q->items[i])));
	}
	
	return result;
}

time_t
stat_container_interval (stat_container_t *s)
{
	if (s->full == 0) {
		return 0;
	}
	
	size_t low = s->full < STAT_COUNT ? 0 : s->next;
	size_t high = s->next == 0 ? STAT_COUNT - 1 : s->next - 1;
	
	return s->items[high].time - s->items[low].time;
}

void
stat_container_free (stat_container_t * q)
{
	
}

stat_number_t
sum (stat_number_t a, stat_number_t b)
{
	return a + b;
}

stat_number_t 
key_packet_count (const flowstat_t *stat)
{
	return stat->packet_count;
}

stat_number_t 
key_byte_count(const flowstat_t *stat)
{
	return stat->byte_count;
}
