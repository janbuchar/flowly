#include <arpa/inet.h>
#include <string.h>

#include "utils.h"

void 
addr_mask_ipv4 (struct sockaddr_in *addr, struct sockaddr_in *mask)
{
	addr->sin_addr.s_addr &= mask->sin_addr.s_addr;
}

void 
addr_mask_ipv6 (struct sockaddr_in6 *addr, struct sockaddr_in6 *mask)
{
	size_t i;
	u_int32_t *src = (u_int32_t *) &mask->sin6_addr;
	u_int32_t *dst = (u_int32_t *) &addr->sin6_addr;
	
	for (i = 0; i < 4; i++) {
		*dst &= *src;
		src++; dst++;
	}
}

void
addr_mask (struct sockaddr_storage *addr, struct sockaddr_storage *mask)
{
	switch (addr->ss_family) {
	case AF_INET:
		addr_mask_ipv4((struct sockaddr_in *) addr, (struct sockaddr_in *) mask);
		break;
	case AF_INET6:
		addr_mask_ipv6((struct sockaddr_in6 *) addr, (struct sockaddr_in6 *) mask);
		break;
	}
}

int 
addr_match_ipv4 (struct sockaddr_in *addr, struct sockaddr_in *network)
{
	return addr->sin_addr.s_addr == network->sin_addr.s_addr;
}

int 
addr_match_ipv6 (struct sockaddr_in6 *addr, struct sockaddr_in6 *network)
{
	size_t i;
	u_int32_t *a = (u_int32_t *) &addr->sin6_addr;
	u_int32_t *b = (u_int32_t *) &network->sin6_addr;
	
	for (i = 0; i < 4; i++) {
		if (*a != *b) {
			return 0;
		}
		a++; b++;
	}
	
	return 1;
}

/**
 * Match @a addr against @a network
 * @param addr The address to be matched
 * @param network The address of the network to match against (Required to be already masked)
 * @param mask The subnet mask for network
 * @return 1 if @a addr matches, 0 if it doesn't
 */
int
addr_match (struct sockaddr_storage *addr, struct sockaddr_storage *network, struct sockaddr_storage *mask)
{
	struct sockaddr_storage addr_masked = *addr;
	addr_mask(&addr_masked, mask);
	
	switch (addr->ss_family) {
	case AF_INET:
		return addr_match_ipv4((struct sockaddr_in *) &addr_masked, (struct sockaddr_in *) network);
	case AF_INET6:
		return addr_match_ipv6((struct sockaddr_in6 *) &addr_masked, (struct sockaddr_in6 *) network);
	default:
		return -1;
	}
}

int
addr_cidr_ipv4 (struct sockaddr_in *addr, int n)
{
	if (n < 0 || n > 32) {
		return -1;
	}
	
	// Fill the address with ones and shift it to create a zero gap
	memset(&addr->sin_addr.s_addr, 255, sizeof (addr->sin_addr.s_addr));
	addr->sin_addr.s_addr >>= (32 - n);
	
	return 1;
}

int
addr_cidr_ipv6 (struct sockaddr_in6 *addr, int n)
{
	if (n < 0 || n > 128) {
		return -1;
	}
	
	u_int32_t *a = (u_int32_t *) &addr->sin6_addr;
	memset(a, 255, 4 * sizeof (u_int32_t));
	
	return -1; // TODO
}

int 
addr_cidr (struct sockaddr_storage *addr, int n)
{
	switch (addr->ss_family) {
	case AF_INET:
		return addr_cidr_ipv4((struct sockaddr_in *) addr, n);
	case AF_INET6:
		return addr_cidr_ipv6((struct sockaddr_in6 *) addr, n);
	default:
		return -1;
	}
}
