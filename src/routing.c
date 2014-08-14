#include <string.h>
#include <stdlib.h>

#include "routing.h"
#include "utils.h"

static size_t route_count;
static flowly_route_t *routes;

static int
route_cmp (const void *a, const void *b)
{
	return ((flowly_route_t *) b)->net.mask - ((flowly_route_t *) a)->net.mask;
}

int 
routing_init (const flowly_config_t *config)
{
	route_count = config->route_count;
	routes = malloc(route_count * sizeof (flowly_route_t));
	
	if (routes == NULL) {
		return -1;
	}
	
	memcpy(routes, config->routes, route_count * sizeof (flowly_route_t));
	qsort(routes, route_count, sizeof (flowly_route_t), route_cmp);
	
	return 0;
}

size_t 
route_match (struct sockaddr_storage *addr)
{
	struct sockaddr_storage addr_masked;
	
	size_t i;
	for (i = 0; i < route_count; i++) {
		if (routes[i].net.family != addr->ss_family) {
			continue;
		}
		
		size_t mask = routes[i].net.mask;
		
		memcpy(&addr_masked, addr, sizeof(struct sockaddr_storage));
		addr_mask(&addr_masked, mask);
		
		if (memcmp(&(routes[i].net.addr), addr_get_raw(&addr_masked), addr_len(&addr_masked)) == 0) {
			return routes[i].net_id;
		}
	}
	
	return 0;
}

void 
routing_destroy ()
{
	free(routes);
}

