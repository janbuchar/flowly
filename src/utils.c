#include <arpa/inet.h>
#include <string.h>

#include "utils.h"

void 
addr_mask_ipv4 (struct sockaddr_in *addr, size_t n)
{
	if (n > 32) {
		return;
	}
	
	if (n == 0) {
		addr->sin_addr.s_addr = 0;
		return;
	}
	
	uint32_t ones;
	memset(&ones, 255, sizeof (ones));
	ones <<= (32 - n);
	
	addr->sin_addr.s_addr &= htonl(ones);
}

void 
addr_mask_ipv6 (struct sockaddr_in6 *addr, size_t n)
{
	if (n > 128) {
		return;
	}
	
	size_t i;
	uint32_t *target = (uint32_t *) &addr->sin6_addr;
	
	for (i = 0; i < 4; i++) {
		if (n == 0) {
			memset(target, 0, 4);
		} else if (n < 32) {
			uint32_t ones;
			memset(&ones, 255, sizeof(ones));
			ones <<= (32 - n);
			
			*target &= htonl(ones);
		}
		
		target++;
		
		if (n > 32) {
			n -= 32;
		} else {
			n = 0;
		}
	}
}

void
addr_mask (struct sockaddr_storage *addr, size_t n)
{
	switch (addr->ss_family) {
	case AF_INET:
		addr_mask_ipv4((struct sockaddr_in *) addr, n);
		break;
	case AF_INET6:
		addr_mask_ipv6((struct sockaddr_in6 *) addr, n);
		break;
	}
}

int
addr_match (struct sockaddr_storage *addr, struct sockaddr_storage *network, size_t mask)
{
	struct sockaddr_storage addr_masked = *addr;
	addr_mask(&addr_masked, mask);
	
	return memcmp(addr_get_raw(&addr_masked), addr_get_raw(network), addr_len(network)) == 0;
}

void *
addr_get_raw (struct sockaddr_storage *addr)
{
	switch (addr->ss_family) {
	case AF_INET:
		return &(((struct sockaddr_in *) addr)->sin_addr.s_addr);
	case AF_INET6:
		return &(((struct sockaddr_in6 *) addr)->sin6_addr);
	default:
		return NULL;
	}
}

size_t 
addr_len (struct sockaddr_storage *addr)
{
	switch (addr->ss_family) {
	case AF_INET:
		return 4;
	case AF_INET6:
		return 16;
	default:
		return 0;
	}
}


u_int64_t
htonll_ (u_int64_t val)
{
	if (htonl(1) == 1) {
		return val;
	}
	
	u_int32_t a = *((u_int32_t *) &val);
	u_int32_t b = *(((u_int32_t *) &val) + 1);
	
	val = htonl(a);
	val = val << 32 | htonl(b);
	
	return val;
}
