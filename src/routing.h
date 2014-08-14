#ifndef _ROUTING_H_
#define _ROUTING_H_

#include <sys/socket.h>

#include "config.h"

/**
 * Initialize helper structures needed for route matching
 * @param config Configuration
 * @return 0 on success, an error code on failure
 */
int
routing_init (const flowly_config_t *config);

/**
 * Find the longest prefix match for @a addr
 * @param addr The address to be matched
 * @return A corresponding network ID on success, 0 when no matching route is found
 */
size_t
route_match (struct sockaddr_storage *addr);

/**
 * Clean up any helper structures used by route matching
 */
void
routing_destroy ();

#endif