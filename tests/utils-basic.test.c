#include <arpa/inet.h>

#include "../src/utils.c"

void assert_ip (char *expected, struct sockaddr_storage *reality, char *msg)
{
	char dst[255];
	inet_ntop(AF_INET, &((struct sockaddr_in *) reality)->sin_addr, &dst[0], 255);
	assert_string(expected, dst, msg);
}

void assert_ip6 (char *expected, struct sockaddr_storage *reality, char *msg)
{
	char dst[255];
	inet_ntop(AF_INET6, &((struct sockaddr_in6 *) reality)->sin6_addr, &dst[0], 255);
	assert_string(expected, dst, msg);
}

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
	
	addr_mask(&ss_net, &ss_mask);
	assert_ip("192.0.0.0", &ss_net, "IPv4 masking");
	
	assert_int(1, addr_match(&ss_addr, &ss_net, &ss_mask), "IPv4 matching subnet");
	assert_int(0, addr_match(&ss_addr2, &ss_net, &ss_mask), "IPv4 non-matching subnet");
	
	
	ss_addr.ss_family = AF_INET6;
	inet_pton(AF_INET6, "2001:db8::1428:57ab", &((struct sockaddr_in6 *) &ss_addr)->sin6_addr);
	
	ss_addr2.ss_family = AF_INET6;
	inet_pton(AF_INET6, "2002:db8::1428:57ab", &((struct sockaddr_in6 *) &ss_addr2)->sin6_addr);
	
	ss_net.ss_family = AF_INET6;
	inet_pton(AF_INET6, "2001::cdef", &((struct sockaddr_in6 *) &ss_net)->sin6_addr);
	
	ss_mask.ss_family = AF_INET6;
	inet_pton(AF_INET6, "ffff::", &((struct sockaddr_in6 *) &ss_mask)->sin6_addr);
	
	addr_mask(&ss_net, &ss_mask);
	assert_ip6("2001::", &ss_net, "IPv6 masking");
	
	assert_int(1, addr_match(&ss_addr, &ss_net, &ss_mask), "IPv6 matching subnet");
	assert_int(0, addr_match(&ss_addr2, &ss_net, &ss_mask), "IPv6 non-matching subnet");
	
	fct_send();
	return 0;
}