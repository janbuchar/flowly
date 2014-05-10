#ifndef _COMMON_H_
#define _COMMON_H_

#define MAX_SFLOW_PACKET_SIZE 65536
#define STAT_COUNT 256
#define STAT_NAME_LENGTH 64
#define NET_NAME_LENGTH 64

typedef enum {
	IN,
	OUT
} flow_direction_t;

#endif
