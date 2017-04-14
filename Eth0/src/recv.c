/*
 * In The Name Of God
 * ========================================
 * [] File Name : recv.c
 *
 * [] Creation Date : 14-04-2017
 *
 * [] Created By : Parham Alvani (parham.alvani@gmail.com)
 * =======================================
 */
/*
 * Copyright (c) 2017 Parham Alvani.
 */
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>

#define DEFAULT_IF	"ens32"
#define BUF_SIZ		1024

int main(int argc, char *argv[])
{
	int sockfd;
	int sockopt;
	ssize_t numbytes;
	struct ifreq ifopts;	/* set promiscuous mode */
	struct sockaddr_storage their_addr;
	uint8_t buf[BUF_SIZ];
	char if_name[IFNAMSIZ];

	/* Get interface name */
	if (argc > 1)
		strcpy(if_name, argv[1]);
	else
		strcpy(if_name, DEFAULT_IF);

	/*
	 * Ethernet header structure
	 *
	 * struct ether_header {
	 *	u_char	ether_dhost[6];
	 *	u_char	ether_shost[6];
	 *	u_short	ether_type;
	 * };
	 */
	struct ether_header *eh = (struct ether_header *) buf;


	/*
	 * Open AF_PACKET socket, listening for EtherType ETHER_TYPE
	 * AF_PACKET: Low level packet interface
	 * SOCK_RAW: Provides raw network protocol access.
	 *
	 * | EtherType | Protocol |
	 * | --------- | -------- |
	 * |  0x0800   |   IPv4   |
	 * |  0x0806   |   ARP    |
	 * |  0x86DD   |   IPv6   |
	 *
	 * +--------------+
	 * |   Network    | -> ARP VLAN IPv4 IPv6
	 * +--------------+
	 * |  Data Link   |
	 * +--------------+
	 *
	 */
	sockfd = socket(AF_PACKET, SOCK_RAW, htons(0x0800));
	if (sockfd == -1)  {
		perror("socket()");
		exit(EXIT_FAILURE);
	}

	/*
	 * Set interface to promiscuous mode - do we need to do this every time?
	 */
	strncpy(ifopts.ifr_name, if_name, IFNAMSIZ-1);
	ioctl(sockfd, SIOCGIFFLAGS, &ifopts);
	ifopts.ifr_flags |= IFF_PROMISC;
	ioctl(sockfd, SIOCSIFFLAGS, &ifopts);

	/*
	 * Allow the socket to be reused - incase connection is closed prematurely
	 */
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1) {
		perror("setsockopt");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	/* Bind to device */
	if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, if_name, IFNAMSIZ-1) == -1)	{
		perror("SO_BINDTODEVICE");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	while (1) {
		printf("listener: Waiting to recvfrom...\n");
		numbytes = recvfrom(sockfd, buf, BUF_SIZ, 0, NULL, NULL);
		printf("listener: got packet (0x%04hx) %lu bytes\n", ntohs(eh->ether_type), numbytes);

		printf("Destination MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
				eh->ether_dhost[0],
				eh->ether_dhost[1],
				eh->ether_dhost[2],
				eh->ether_dhost[3],
				eh->ether_dhost[4],
				eh->ether_dhost[5]);
	}
}
