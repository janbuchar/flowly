#include <sys/types.h>
#include <time.h>

#include "common.h"

typedef struct {
	u_int32_t packet_count;
	u_int32_t byte_count;
	time_t time;
	char net_id[NET_ID_LENGTH];
} flowstat_t;