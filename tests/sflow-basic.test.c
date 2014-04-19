#include <stdio.h>

#include "../src/sflow.c"

int main (int argc, char **argv)
{
	fct_init();
	
	/* A captured sFlow datagram (#14) */
	u_int32_t packet[] = {
		htonl(0x00000005), htonl(0x00000001), htonl(0xb2116ff2), htonl(0x00000010), htonl(0x00002eb2), htonl(0x55f10676), htonl(0x00000006), htonl(0x00000001), 
		htonl(0x000000d0), htonl(0x00003bc4), htonl(0x000001fd), htonl(0x000003e8), htonl(0x00e971b8), htonl(0x00000000), htonl(0x000001fd), htonl(0x00000000), 
		htonl(0x00000002), htonl(0x00000001), htonl(0x00000090), htonl(0x00000001), htonl(0x00000552), htonl(0x00000004), htonl(0x00000080), htonl(0x01005e31), 
		htonl(0x1af56c9c), htonl(0xed6213d6), htonl(0x08004500), htonl(0x054034d9), htonl(0x40003a11), htonl(0x7b3ad460), htonl(0xb312e931), htonl(0x1af5090a), 
		htonl(0x090a052c), htonl(0x00004700), htonl(0x651ee36d), htonl(0x579e13dc), htonl(0x65de7811), htonl(0x377a5cf7), htonl(0x9e21eba7), htonl(0x0f8ebcf1), 
		htonl(0x0ecfa7d7), htonl(0xef1810a4), htonl(0x4da7bcf3), htonl(0x3b8dbae7), htonl(0x8a9ef3c1), htonl(0x3bb875e7), htonl(0x9ab9ef3c), htonl(0x17d73cf7), 
		htonl(0x9e06eee7), htonl(0x9ef3c73a), htonl(0xd194e203), htonl(0x01dea7f6), htonl(0xa8da1200), htonl(0x00000108), htonl(0x33cfb36f), htonl(0x000003e9), 
		htonl(0x00000010), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000001), htonl(0x000000d0), htonl(0x00003bc5), 
		htonl(0x000001fd), htonl(0x000003e8), htonl(0x00e975a0), htonl(0x00000000), htonl(0x000001fd), htonl(0x00000000), htonl(0x00000002), htonl(0x00000001), 
		htonl(0x00000090), htonl(0x00000001), htonl(0x00000552), htonl(0x00000004), htonl(0x00000080), htonl(0x01005e31), htonl(0x1a8f6c9c), htonl(0xed6213d6), 
		htonl(0x08004500), htonl(0x0540394a), htonl(0x40003a11), htonl(0x772fd460), htonl(0xb312e931), htonl(0x1a8f090a), htonl(0x090a052c), htonl(0x00004700), 
		htonl(0x65125e8a), htonl(0x2878413c), htonl(0xa8569182), htonl(0x2f223570), htonl(0x558b4499), htonl(0x27625ad6), htonl(0xbabafb77), htonl(0x8d556e64), 
		htonl(0xf68b6690), htonl(0x85d0d9ac), htonl(0x491de099), htonl(0x2159c756), htonl(0xeb74a2a5), htonl(0x7e019e40), htonl(0xc90f57bc), htonl(0x1e66e5b4), 
		htonl(0x9522a3dc), htonl(0x301446f8), htonl(0x31f31f3f), htonl(0xdbf78329), htonl(0x9baf89b3), htonl(0x000003e9), htonl(0x00000010), htonl(0x00000000), 
		htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000001), htonl(0x000000d0), htonl(0x00003bc6), htonl(0x000001fd), htonl(0x000003e8), 
		htonl(0x00e97988), htonl(0x00000000), htonl(0x000001fd), htonl(0x00000000), htonl(0x00000002), htonl(0x00000001), htonl(0x00000090), htonl(0x00000001), 
		htonl(0x00000552), htonl(0x00000004), htonl(0x00000080), htonl(0x01005e31), htonl(0x1a686c9c), htonl(0xed6213d6), htonl(0x08004500), htonl(0x05403d93), 
		htonl(0x40003a11), htonl(0x730dd460), htonl(0xb312e931), htonl(0x1a68090a), htonl(0x090a052c), htonl(0x00004700), htonl(0x651a8c9a), htonl(0xc8698831), 
		htonl(0x20740075), htonl(0x1608600a), htonl(0x0640060e), htonl(0x40788603), htonl(0x003203a0), htonl(0x13815260), htonl(0x0d9060d2), htonl(0xf93129c1), 
		htonl(0x078b801a), htonl(0x803a02a0), htonl(0x06600d80), htonl(0x3302843f), htonl(0x80789fb7), htonl(0x0941c6a3), htonl(0xde806e8e), htonl(0x02941392), 
		htonl(0x19c82dc8), htonl(0x14383213), htonl(0x01007408), htonl(0x000003e9), htonl(0x00000010), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), 
		htonl(0x00000000), htonl(0x00000001), htonl(0x000000d0), htonl(0x00003bc7), htonl(0x000001fd), htonl(0x000003e8), htonl(0x00e97d70), htonl(0x00000000), 
		htonl(0x000001fd), htonl(0x00000000), htonl(0x00000002), htonl(0x00000001), htonl(0x00000090), htonl(0x00000001), htonl(0x00000552), htonl(0x00000004), 
		htonl(0x00000080), htonl(0x01005e31), htonl(0x1a486c9c), htonl(0xed6213d6), htonl(0x08004500), htonl(0x05409771), htonl(0x40003a11), htonl(0x194ed460), 
		htonl(0xb313e931), htonl(0x1a48090a), htonl(0x090a052c), htonl(0x00004700), htonl(0x65120000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), 
		htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), 
		htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), 
		htonl(0x00000000), htonl(0x000003e9), htonl(0x00000010), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000001), 
		htonl(0x000000d0), htonl(0x00003bc8), htonl(0x000001fd), htonl(0x000003e8), htonl(0x00e98158), htonl(0x00000000), htonl(0x000001fd), htonl(0x00000000), 
		htonl(0x00000002), htonl(0x00000001), htonl(0x00000090), htonl(0x00000001), htonl(0x00000552), htonl(0x00000004), htonl(0x00000080), htonl(0x01005e31), 
		htonl(0x1aa16c9c), htonl(0xed6213d6), htonl(0x08004500), htonl(0x05404682), htonl(0x40003a11), htonl(0x69e5d460), htonl(0xb312e931), htonl(0x1aa1090a), 
		htonl(0x090a052c), htonl(0x00004700), htonl(0x6519ccaf), htonl(0x5e165667), htonl(0x3a001dc5), htonl(0x5faf0000), htonl(0x01081b5e), htonl(0x6563af95), 
		htonl(0xebd15aaf), htonl(0x95baf95b), htonl(0xaf656ebc), htonl(0xcac3d7a2), htonl(0xb0000001), htonl(0x091b5f2b), htonl(0x0d5e8af5), htonl(0xecadd7ca), 
		htonl(0xdd7cac75), htonl(0xe656339d), htonl(0x807d6bcc), htonl(0xac000001), htonl(0x0a1b5e8a), htonl(0xc55e32b2), htonl(0xaf95baf6), htonl(0x000003e9), 
		htonl(0x00000010), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000001), htonl(0x000000d0), htonl(0x00003bc9), 
		htonl(0x000001fd), htonl(0x000003e8), htonl(0x00e98540), htonl(0x00000000), htonl(0x000001fd), htonl(0x00000000), htonl(0x00000002), htonl(0x00000001), 
		htonl(0x00000090), htonl(0x00000001), htonl(0x00000552), htonl(0x00000004), htonl(0x00000080), htonl(0x01005e31), htonl(0x1a906c9c), htonl(0xed6213d6), 
		htonl(0x08004500), htonl(0x05404acb), htonl(0x40003a11), htonl(0x65add460), htonl(0xb312e931), htonl(0x1a90090a), htonl(0x090a052c), htonl(0x00004700), 
		htonl(0x651baafe), htonl(0x0bdedfbc), htonl(0xbd602300), htonl(0x2174c3ce), htonl(0x4b2371b8), htonl(0xe6ffdcf5), htonl(0x786be4e8), htonl(0xc0fd26c2), 
		htonl(0xeed3a46f), htonl(0x4c46f9a6), htonl(0x5db1ca70), htonl(0x79466014), htonl(0xad3298d5), htonl(0x0bb5be20), htonl(0x069402c7), htonl(0xf3a2f740), 
		htonl(0x8b297b99), htonl(0x605378ac), htonl(0xccd10ea4), htonl(0x352cb6f5), htonl(0xc00eb400), htonl(0x000003e9), htonl(0x00000010), htonl(0x00000000), 
		htonl(0x00000000), htonl(0x00000000), htonl(0x00000000)
	};
	
	sflow_sample_data_t *sample = NULL;
	sflow_flow_record_t *record = NULL;
	
	assert_int(1, next_sample(&packet, sizeof (packet), &sample), "Load sample #1");
	
	assert_int(1, next_record(sample, &record), "Load record #1:1");
	
	assert_int(1, next_record(sample, &record), "Load record #1:2");
	
	assert_int(0, next_record(sample, &record), "Record #1:3 should fail");
	
	
	assert_int(1, next_sample(&packet, sizeof (packet), &sample), "Load sample #2");
	
	assert_int(1, next_sample(&packet, sizeof (packet), &sample), "Load sample #3");
	
	assert_int(1, next_sample(&packet, sizeof (packet), &sample), "Load sample #4");
	
	assert_int(1, next_sample(&packet, sizeof (packet), &sample), "Load sample #5");
	
	assert_int(1, next_sample(&packet, sizeof (packet), &sample), "Load sample #6");
	
	assert_int(0, next_sample(&packet, sizeof (packet), &sample), "Sample #7 should fail");
	
	fct_send();
	return 0;
}