#include <sys/types.h>
#include <time.h>

#include "common.h"

typedef struct {
	u_int32_t packet_count;
	u_int32_t byte_count;
	time_t time;
} flowstat_t;