#include <sys/types.h>
#include <stdlib.h>

#include "fixq.h"

void 
fixq_init (fixq_t * q, size_t size)
{
	q->items = malloc(size);
	q->next = 0;
	q->full = 0;
}

void 
fixq_insert (fixq_t * q, void * item)
{
	q->items[q->next] = item;
	
	if (++(q->next) >= q->size) {
		q->next = 0;
	}
	
	if (q->full < q->size) {
		++(q->full);
	}
}

reduce_res_t
fixq_reduce (fixq_t * q, reduce_keyfnc_t key, reduce_fnc_t fnc)
{
	size_t i = 0;
	reduce_res_t result = key(q->items[0]);
	
	for (i = 1; i < q->full; ++i) {
		result = fnc(result, key(q->items[i]));
	}
	
	return result;
}

void
fixq_destroy (fixq_t * q)
{
	free(q->items);
}