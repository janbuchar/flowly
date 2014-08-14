#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <netdb.h>
#include <err.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>

#include "common.h"
#include "utils.h"
#include "config.h"
#include "sflow.h"
#include "routing.h"
#include "flowstat.h"
#include "output.h"

int
create_socket (flowly_config_t *config)
{
	struct addrinfo *r, *rorig, hint;
	int error;
	int sock;
	
	memset(&hint, 0, sizeof (hint));
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_DGRAM;
	hint.ai_flags = AI_PASSIVE;
	
	if ((error = getaddrinfo(NULL, config->listen_port, &hint, &r)) != 0) {
		errx(1, "%s", gai_strerror(error));
	}
	
	for (rorig = r; r != NULL; r = r->ai_next) {
		sock = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
		
		int optval = 1;
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval)) == -1) {
			err(1, "setsockopt");
		}
		
		if (bind(sock, r->ai_addr, r->ai_addrlen) == 0) {
			break;
		}
		
		close(sock);
	}
	
	freeaddrinfo(rorig);
	
	if (r == NULL) {
		errx(1, "getaddrinfo: %s", strerror(errno));
	}
	
	return sock;
}

void
store_stats (stat_container_t *stats, size_t net_id, flow_direction_t dir, sflow_flow_sample_t *sample, sflow_raw_header_t *header)
{
	flowstat_t *item = stat_container_next(&stats[2 * net_id + dir]);
	clock_gettime(CLOCK_REALTIME, &item->time);
	item->byte_count = ntohl(sample->sample_rate) * ntohl(header->frame_length);
	item->packet_count = ntohl(sample->sample_rate);
}

/**
 * A global container for everything that needs to be shared between threads
 */
struct {
	flowly_config_t config;
	stat_container_t *stats;
	stat_container_t *stats_copy;
	size_t stats_count;
	pthread_mutex_t mutex;
	pthread_t receiver;
	int input_socket;
} context;

void *
output_thread (void *arg)
{
	struct timespec wait = {
		.tv_sec = context.config.send_interval / 1000,
		.tv_nsec = (context.config.send_interval * 1000000) % 1000000000
	};
	struct timespec last = {
		.tv_sec = 0,
		.tv_nsec = 0
	};
	struct timespec remaining;
	
	for (;;) {
		pthread_mutex_lock(&context.mutex);
		memcpy(context.stats_copy, context.stats, context.stats_count * sizeof (stat_container_t));
		pthread_mutex_unlock(&context.mutex);
		
		output(&context.config, context.stats_copy, &last);
		clock_gettime(CLOCK_REALTIME, &last);
		
		nanosleep(&wait, &remaining);
	}
	
	return arg; // Never reached
}

void
load_context ()
{
	flowly_config_error_t cfg_err;
	int rc;
	
	config_free(&context.config);
	
	if (context.stats != NULL) {
		free(context.stats);
	}
	
	if (context.stats_copy != NULL) {
		free(context.stats_copy);
	}
	
	if ((rc = config_load(&context.config, "flowly.conf", &cfg_err)) < 0) {
		errx(1, "config_load: %s on line %zu\n%s", config_strerror(rc), cfg_err.line_number, cfg_err.line);
	}
	
	context.stats_count = 2 * context.config.network_count; // a container for each network and direction
	context.stats = malloc(context.stats_count * sizeof (stat_container_t));
	context.stats_copy = malloc(context.stats_count * sizeof (stat_container_t));
	
	if (context.stats == NULL || context.stats_copy == NULL) {
		err(1, "Could not allocate memory");
	}
	
	stat_container_t *stat_it;
	for (stat_it = context.stats; stat_it - context.stats < 2 * context.config.network_count; ++stat_it) {
		stat_container_init(stat_it);
	}
}

void *
sig_thread (void *arg)
{
	sigset_t *sigset = (sigset_t *) arg;
	int sig_number;
	
	for (;;) {
		sigwait(sigset, &sig_number);
		
		switch (sig_number) {
		case SIGHUP:
			pthread_mutex_lock(&context.mutex);
			load_context();
			pthread_mutex_unlock(&context.mutex);
			break;
		case SIGINT:
		case SIGTERM:
			pthread_cancel(context.receiver);
			break;
		}
	}
	
	return arg; // Never reached
}

void *
receiver_thread (void *packet)
{
	struct sockaddr_storage addr;
	size_t net_id;
	sflow_sample_data_t *sample;
	sflow_flow_record_t *record;
	
	for (;;) {
		int n = recvfrom(context.input_socket, packet, MAX_SFLOW_PACKET_SIZE, 0, NULL, 0);
		
		if (n <= 0) {
			break;
		}
		
		if (n == MAX_SFLOW_PACKET_SIZE) {
			continue; // Now that's a big packet... How about a message?
		}
		
		pthread_mutex_lock(&context.mutex);
		
		sample = NULL;
		while (next_sample(packet, n, &sample) > 0) {
			if (!is_sample_format(sample, FLOW_SAMPLE)) {
				continue;
			}
			
			record = NULL;
			while (next_record(sample, &record) > 0) {
				if (!is_record_format(record, RAW_HEADER)) {
					continue;
				}
				
				if (get_source(get_raw_header(record), &addr)) {
					if ((net_id = route_match(&addr))) {
						store_stats(context.stats, net_id, OUT, get_flow_sample(sample), get_raw_header(record));
					}
				}
				
				if (get_destination(get_raw_header(record), &addr)) {
					if ((net_id = route_match(&addr))) {
						store_stats(context.stats, net_id, IN, get_flow_sample(sample), get_raw_header(record));
					}
				}
			}
		}
		
		pthread_mutex_unlock(&context.mutex);
	}
	
	return packet;
}

int 
main (int argc, char **argv) 
{
	load_context();
	
	sigset_t sig;
	sigemptyset(&sig);
	sigaddset(&sig, SIGHUP);
	sigaddset(&sig, SIGINT);
	sigaddset(&sig, SIGTERM);
	
	if (pthread_sigmask(SIG_BLOCK, &sig, NULL) != 0) {
		errx(1, "sigmask");
	}
	
	pthread_t outputter, sighandler;
	pthread_mutex_init(&context.mutex, NULL);
	pthread_create(&sighandler, NULL, sig_thread, &sig);
	pthread_create(&outputter, NULL, output_thread, &context);
	
	context.input_socket = create_socket(&context.config);
	
	void *packet = malloc(MAX_SFLOW_PACKET_SIZE);
	
	if (packet == NULL) {
		err(1, "Could not allocate memory");
	}
	
	routing_init(&(context.config));
	
	pthread_create(&context.receiver, NULL, receiver_thread, packet);
	pthread_join(context.receiver, NULL);
	
	routing_destroy();
	
	free(packet);
	free(context.stats);
	free(context.stats_copy);
	
	return 0;
}
