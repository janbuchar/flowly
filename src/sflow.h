#ifndef _SFLOW_H_
#define _SFLOW_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define IPV4_SIZE 4
#define IPV6_SIZE 16
#define ETH_HEADER_SIZE 14

/**
 * sFlow datagram header (followed either by an ipv4 or ipv6 address)
 */
typedef struct {
	u_int32_t version;
	u_int32_t ip_version;
} sflow_data_header_t;

/**
 * sFlow datagram body (followed by @a sample_count samples)
 */
typedef struct {
	u_int32_t subagent_id;
	u_int32_t sequence_number;
	u_int32_t switch_uptime;
	u_int32_t sample_count;
} sflow_data_body_t;

/**
 * Information about flow data
 */
typedef struct {
	u_int32_t format; // Enterprise (5B) and format (3B) number
	u_int32_t length;
} sflow_sample_data_t;

/**
 * Enterprise 0 format 1
 * Flow sample (followed by @a record_count records)
 */
typedef struct {
	u_int32_t sequence_number;
	u_int32_t source_id;
	u_int32_t sample_rate;
	u_int32_t sample_pool;
	u_int32_t drops;
	u_int32_t input;
	u_int32_t output;
	u_int32_t record_count;
} sflow_flow_sample_t;

/**
 * sFlow flow record header
 */
typedef struct {
	u_int32_t format; // Enterprise (5B) and format (3B) number
	u_int32_t length;
} sflow_flow_record_t;

/**
 * Raw packet header record (followed by sampled packet header)
 */
typedef struct {
	u_int32_t protocol;
	u_int32_t frame_length;
	u_int32_t stripped;
	u_int32_t header_size;
} sflow_raw_header_t;

typedef enum {
	FLOW_SAMPLE = 1,
	SWITCH_DATA = 1001
} sflow_sample_format_t;

typedef enum {
	RAW_HEADER = 1
} sflow_record_format_t;

typedef enum {
	ETHERNET = 1,
	ISO88024 = 2,
	ISO88025 = 3,
	FDDI = 4,
	FRAME = 5,
	X25 = 6,
	PPP = 7,
	SMDS = 8,                
	AAL5 = 9,
	AAL5IP = 10,
	IPV4 = 11,
	IPV6 = 12,
	MPLS = 13,
	POS = 14
} sflow_header_protocol_t;

/**
 * Get the flow sample that follows the header pointed to by @a sample
 * @param sample The header of the sample
 * @return A pointer to the sample
 */
sflow_flow_sample_t *
get_flow_sample (sflow_sample_data_t *sample);

/**
 * Increment @a sample to point to the next sample in the sFlow datagram
 * @param packet The sFlow datagram
 * @param packet_size The size of the packet
 * @param sample A pointer to the beginning of current sample (NULL if we want the first sample)
 * @return 0 when there are no more samples, 1 otherwise
 */
int
next_sample (void *packet, size_t packet_size, sflow_sample_data_t **sample);

/**
 * Get the raw packet header that follows the record header pointed to by @a record
 * @param record The record header
 * @return A pointer to the packet header
 */
sflow_raw_header_t *
get_raw_header (sflow_flow_record_t *record);

/**
 * Increment @a record to point to the next record in an sFlow sample
 * @param sample The sFlow sample
 * @param record A pointer to the beginning of current record (NULL if we want the first record)
 * @return 0 when there are no more records, 1 otherwise
 */
int
next_record (sflow_sample_data_t *sample, sflow_flow_record_t **record);

/**
 * Check whether @a sample is of format @a format
 * @param sample
 * @param format
 * @return 1 if @a format matches, 0 otherwise
 */
int
is_sample_format (sflow_sample_data_t *sample, sflow_sample_format_t format);

/**
 * Check whether @a record is of format @a format
 * @param record
 * @param format
 * @return 1 if @a format matches, 0 otherwise
 */
int 
is_record_format (sflow_flow_record_t *record, sflow_record_format_t format);

/**
 * Get the source address from @a header and store it in @a dst
 * @param header 
 * @param dst 
 * @return 1 on success, 0 otherwise
 */
int 
get_source (sflow_raw_header_t *header, struct sockaddr_storage *dst);

/**
 * Get the destination address from @a header and store it in @a dst
 * @param header 
 * @param dst 
 * @return 1 on success, 0 otherwise
 */
int
get_destination (sflow_raw_header_t *header, struct sockaddr_storage *dst);

#endif
