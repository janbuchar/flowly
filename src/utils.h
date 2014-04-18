#ifndef _UTILS_H_
#define _UTILS_H_

#include <sys/socket.h>

void
addr_mask (struct sockaddr_storage *addr, struct sockaddr_storage *mask);

int
addr_match (struct sockaddr_storage *addr, struct sockaddr_storage *network, struct sockaddr_storage *mask);

#endif