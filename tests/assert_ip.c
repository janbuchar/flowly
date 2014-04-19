#include <arpa/inet.h>

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