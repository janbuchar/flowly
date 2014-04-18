#include "sflow.h"
#include <arpa/inet.h>

int
next_sample (void *packet, size_t packet_size, sflow_sample_data_t **sample)
{
	void *end = ((char *) packet) + packet_size;
	
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
		*sample = (sflow_sample_data_t *) (((char *) (*sample + 1)) + ntohl((*sample)->length));
	}
	
	return *sample < end;
}

int
next_record (sflow_sample_data_t *sample, sflow_flow_record_t **record)
{
	void *end = ((char *) (sample + 1)) + ntohl(sample->length);
	
	if (*record == NULL) {
		*record = (sflow_flow_record_t *) (sample + 1);
	} else {
		*record = (sflow_flow_record_t *) (((char *) (*record + 1)) + ntohl((*record)->length));
	}
	
	return *record < end;
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
get_source (sflow_raw_header_t *header, sockaddr_storage* dst)
{
	
}

int
get_destination (sflow_raw_header_t *header, sockaddr_storage* dst)
{
	
}