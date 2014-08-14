#include <string.h>

void
assert_mem(const void * expected, const void * reality, size_t n, const char * msg)
{
	assert_int(0, memcmp(expected, reality, n), msg);
}