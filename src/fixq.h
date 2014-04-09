#include <sys/types.h>

typedef struct {
	size_t size;
	size_t full;
	size_t next;
	void ** items;
} fixq_t;

typedef u_int32_t reduce_res_t;

typedef reduce_res_t (* reduce_fnc_t) (reduce_res_t, reduce_res_t);

typedef reduce_res_t (* reduce_keyfnc_t) (const void *);

void 
fixq_init (fixq_t * q, size_t size);

void 
fixq_insert (fixq_t * q, void * item);

reduce_res_t
fixq_reduce (fixq_t * q, reduce_keyfnc_t key, reduce_fnc_t fnc);

void
fixq_destroy (fixq_t * q);