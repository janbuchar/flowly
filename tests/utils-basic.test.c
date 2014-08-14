#include <stdio.h>
#include <arpa/inet.h>

#include "../src/utils.c"

int main (int argc, char **argv)
{
	fct_init();
	
	struct sockaddr_storage ss_addr;
	ss_addr.ss_family = AF_INET;
	inet_pton(AF_INET, "192.168.0.1", &((struct sockaddr_in *) &ss_addr)->sin_addr);
	
	struct sockaddr_storage ss_addr2;
	ss_addr2.ss_family = AF_INET;
	inet_pton(AF_INET, "127.168.0.1", &((struct sockaddr_in *) &ss_addr2)->sin_addr);
	
	struct sockaddr_storage ss_net;
	ss_net.ss_family = AF_INET;
	inet_pton(AF_INET, "192.168.54.78", &((struct sockaddr_in *) &ss_net)->sin_addr);
	
	struct sockaddr_storage ss_mask;
	ss_mask.ss_family = AF_INET;
	inet_pton(AF_INET, "255.0.0.0", &((struct sockaddr_in *) &ss_mask)->sin_addr);
	
	addr_mask(&ss_net, 8);
	assert_ip("192.0.0.0", &ss_net, "IPv4 masking");
	
	assert_int(1, addr_match(&ss_addr, &ss_net, 8), "IPv4 matching subnet");
	assert_int(0, addr_match(&ss_addr2, &ss_net, 8), "IPv4 non-matching subnet");
	
	addr_mask(&ss_net, 0);
	assert_ip("0.0.0.0", &ss_net, "IPv4 wildcard subnet mask");
	assert_int(1, addr_match(&ss_addr, &ss_net, 0), "IPv4 wildcard subnet matching");
	
	inet_pton(AF_INET, "255.255.255.255", &((struct sockaddr_in *) &ss_addr)->sin_addr);
	addr_mask(&ss_addr, 1);
	assert_ip("128.0.0.0", &ss_addr, "CIDR mask /1");
	
	ss_addr.ss_family = AF_INET6;
	inet_pton(AF_INET6, "2001:db8::1428:57ab", &((struct sockaddr_in6 *) &ss_addr)->sin6_addr);
	
	ss_addr2.ss_family = AF_INET6;
	inet_pton(AF_INET6, "2002:db8::1428:57ab", &((struct sockaddr_in6 *) &ss_addr2)->sin6_addr);
	
	ss_net.ss_family = AF_INET6;
	inet_pton(AF_INET6, "2001::cdef", &((struct sockaddr_in6 *) &ss_net)->sin6_addr);
	
	ss_mask.ss_family = AF_INET6;
	inet_pton(AF_INET6, "ffff::", &((struct sockaddr_in6 *) &ss_mask)->sin6_addr);
	
	addr_mask(&ss_net, 16);
	assert_ip6("2001::", &ss_net, "IPv6 masking");
	
	assert_int(1, addr_match(&ss_addr, &ss_net, 16), "IPv6 matching subnet");
	assert_int(0, addr_match(&ss_addr2, &ss_net, 16), "IPv6 non-matching subnet");
	
	fct_send();
	return 0;
}