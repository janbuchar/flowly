#include <string.h>
#include <arpa/inet.h>

#include "sflow.h"

void *
byte_increment (void *ptr, size_t n)
{
	return ((char *) ptr) + n;
}

sflow_flow_sample_t *
get_flow_sample (sflow_sample_data_t *sample)
{
	return (sflow_flow_sample_t *) (sample + 1);
}

int
next_sample (void *packet, size_t packet_size, sflow_sample_data_t **sample)
{
	void *end = byte_increment(packet, packet_size);
	
	if (*sample == NULL) {
		sflow_data_header_t * header = (sflow_data_header_t *) packet;
		sflow_data_body_t * body = NULL;
		
		switch (ntohl(header->ip_version)) {
		case 1: // IPv4
			body = (sflow_data_body_t *) (((char *) packet) + sizeof (sflow_data_header_t) + IPV4_SIZE);
			break;
		case 2: // IPv6
			body = (sflow_data_body_t *) (((char *) packet) + sizeof (sflow_data_header_t) + IPV6_SIZE);
			break;
		default:
			return -1;
		}
		
		*sample = (sflow_sample_data_t *) (body + 1);
	} else {
		*sample = (sflow_sample_data_t *) byte_increment(*sample + 1, ntohl((*sample)->length));
	}
	
	return (void *) *sample < end;
}

sflow_raw_header_t *
get_raw_header (sflow_flow_record_t *record)
{
	return (sflow_raw_header_t *) (record + 1);
}


int
next_record (sflow_sample_data_t *sample, sflow_flow_record_t **record)
{
	void *end = byte_increment(sample + 1, ntohl(sample->length));
	
	if (*record == NULL) {
		*record = (sflow_flow_record_t *) byte_increment(sample + 1, sizeof (sflow_flow_sample_t));
	} else {
		*record = (sflow_flow_record_t *) byte_increment(*record + 1, ntohl((*record)->length));
	}
	
	return (void *) *record < end;
}

int
is_sample_format (sflow_sample_data_t *sample, sflow_sample_format_t format)
{
	return ntohl(sample->format) == format;
}

int 
is_record_format (sflow_flow_record_t *record, sflow_record_format_t format)
{
	return ntohl(record->format) == format;
}

int
get_ip_family (sflow_raw_header_t *header)
{
	return (*((u_int8_t *) byte_increment(header + 1, ETH_HEADER_SIZE)) & 0xf0) >> 4; // The first four bits of the header
}

uint32_t *
get_addressblock_ipv4 (sflow_raw_header_t *header)
{
	return byte_increment(header + 1, ETH_HEADER_SIZE + 12);
}

uint32_t *
get_addressblock_ipv6 (sflow_raw_header_t *header)
{
	return byte_increment(header + 1, ETH_HEADER_SIZE + 8);
}

int 
get_source (sflow_raw_header_t *header, struct sockaddr_storage *dst)
{
	uint32_t *address = NULL;
	if (ntohl(header->protocol) != ETHERNET) {
		return 0;
	}
	
	switch (get_ip_family(header)) {
	case 4:
		address = get_addressblock_ipv4(header);
		memcpy(&((struct sockaddr_in *) dst)->sin_addr, address, IPV4_SIZE);
		((struct sockaddr_in *) dst)->sin_family = AF_INET;
		return 1;
	case 6:
		address = get_addressblock_ipv6(header);
		memcpy(&((struct sockaddr_in6 *) dst)->sin6_addr, address, IPV6_SIZE); // TODO endian?
		((struct sockaddr_in6 *) dst)->sin6_family = AF_INET6;
		return 1;
	default:
		return 0;
	}
}

int
get_destination (sflow_raw_header_t *header, struct sockaddr_storage *dst)
{
	uint32_t *address = NULL;
	if (ntohl(header->protocol) != ETHERNET) {
		return 0;
	}
	
	switch (get_ip_family(header)) {
	case 4:
		address = byte_increment(get_addressblock_ipv4(header), IPV4_SIZE);
		memcpy(&((struct sockaddr_in *) dst)->sin_addr, address, IPV4_SIZE);
		((struct sockaddr_in *) dst)->sin_family = AF_INET;
		return 1;
	case 6:
		address = byte_increment(get_addressblock_ipv6(header), IPV6_SIZE);
		memcpy(&((struct sockaddr_in6 *) dst)->sin6_addr, address, IPV6_SIZE);
		((struct sockaddr_in6 *) dst)->sin6_family = AF_INET6;
		return 1;
	default:
		return 0;
	}
}
