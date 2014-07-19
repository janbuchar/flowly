#ifndef _UTILS_H_
#define _UTILS_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/**
 * Mask @a addr with @a mask
 * @param addr The network address
 * @param mask The subnet mask for @a network
 */
void
addr_mask (struct sockaddr_storage *addr, struct sockaddr_storage *mask);

/**
 * Match @a addr against @a network
 * @param addr The address to be matched
 * @param network The address of the network to match against (Required to be already masked)
 * @param mask The subnet mask for @a network
 * @return 1 if @a addr matches, 0 if it doesn't
 */
int
addr_match (struct sockaddr_storage *addr, struct sockaddr_storage *network, struct sockaddr_storage *mask);

/**
 * Mask @a addr so that only @a n top bits are kept and the rest is set to zero
 * @param addr The network address
 * @param n The amount of top bits to be kept
 * @return 0 on success, -1 on error
 */
int
addr_cidr (struct sockaddr_storage *addr, int n);

/**
 * A custom byte swap function for 64-bit numbers
 * @param val A number in host byte order
 * @return @a val in network byte order
 */
u_int64_t
htonll_ (u_int64_t val);

#endif
