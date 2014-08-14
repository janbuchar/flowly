#ifndef _UTILS_H_
#define _UTILS_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/**
 * Mask @a addr so that only top @a n bits remain
 * @param addr The network address
 * @param mask The subnet mask for @a network
 */
void
addr_mask (struct sockaddr_storage *addr, size_t n);

/**
 * Match @a addr against @a network
 * @param addr The address to be matched
 * @param network The address of the network to match against (Required to be already masked)
 * @param mask The subnet mask for @a network
 * @return 1 if @a addr matches, 0 if it doesn't
 */
int
addr_match (struct sockaddr_storage *addr, struct sockaddr_storage *network, size_t mask);

/**
 * Get a pointer to the address contained in @a addr
 * @param addr The address storage
 * @return A pointer or NULL on failure
 */
void *
addr_get_raw (struct sockaddr_storage *addr);

/**
 * Get the address length of @a addr
 * @param addr
 * @return The length of the address
 */
size_t
addr_len (struct sockaddr_storage *addr);

/**
 * A custom byte swap function for 64-bit numbers
 * @param val A number in host byte order
 * @return @a val in network byte order
 */
u_int64_t
htonll_ (u_int64_t val);

#endif
