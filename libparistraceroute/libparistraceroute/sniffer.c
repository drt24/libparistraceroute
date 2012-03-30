#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <stdlib.h>
#include <fcntl.h>

#include "sniffer.h"
#include "network.h"

#define BUFLEN 4096

sniffer_t * sniffer_create(network_t *network, void (*callback)(network_t *network, packet_t *packet))
{
    sniffer_t *sniffer;

    sniffer = malloc(sizeof(sniffer_t));
    sniffer->network = network;
    sniffer->callback = callback;

    return sniffer;
}

void sniffer_free(sniffer_t *sniffer)
{
    close(sniffer->socket);

    free(sniffer);
    sniffer = NULL;
}

int sniffer_get_fd(sniffer_t *sniffer)
{
    return sniffer->socket;
}

int sniffer_create_raw_socket(sniffer_t *sniffer)
{
	struct sockaddr_in saddr;
    unsigned short port = 0; /* could be a parameter */
    unsigned int proto = 0; /* TODO */
    int res;

	// Create a raw socket (man 7 ip)
	sniffer->socket  = socket(PF_INET, SOCK_RAW, proto);
	if (sniffer->socket < 0) {
		return -1;
    }

    /* Make the socket non-blocking */
    res = fcntl(sniffer->socket, F_SETFD, O_NONBLOCK);
    if (res != 0) {
        return -1;
    }
	
	// Bind it to 0.0.0.0
	// Can we set port to 0 to capture all packets wheter ICMP, UDP or TCP?
	memset(&saddr, 0, sizeof(struct sockaddr_in));
	saddr.sin_family      = AF_INET;
	saddr.sin_addr.s_addr = INADDR_ANY;
	saddr.sin_port        = htons(port);
	res = bind(sniffer->socket, (struct sockaddr*)&saddr,
			sizeof(struct sockaddr_in));
	if (res < 0) {
		return -1;
    }
	
    return 0;
}

void process_packets(sniffer_t *sniffer)
{
    unsigned char data[BUFLEN];
	int data_len = recv(sniffer->socket, data, BUFLEN, 0);
	if (data_len >= 4) {
		// We have to make some modifications on the datagram
		// received because the raw format varies between
		// OSes:
		//  - Linux: the whole packet is in network endianess
		//  - NetBSD: the packet is in network endianess ecxept
		//  IP total length and frag ofs(?) are in host-endian
		//  - FreeBSD: same as NetBSD?
		//  - Apple: same as NetBSD?
		//  Bug? On NetBSD, the IP length seems incorrect
#if defined __APPLE__ || __NetBSD__ || __FreeBSD__
		uint16_t ip_len = read16(data, 2);
		writebe16(data, 2, ip_len);
#endif
		if (sniffer->callback != NULL)
			sniffer->callback(sniffer->network, NULL); // currently we return a NULL packet FIXME 
	}

}