/*
 * In The Name Of God
 * ========================================
 * [] File Name : main.c
 *
 * [] Creation Date : 31-03-2015
 *
 * [] Last Modified : Sat 04 Apr 2015 01:24:05 AM IRDT
 *
 * [] Created By : Parham Alvani (parham.alvani@gmail.com)
 * =======================================
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>

#include "common.h"
#include "ip_func.h"

int main(int argc, char *argv[])
{

	if (argc < 3)
		udie("usage: %s <source IP> <destination IP>\n", argv[0]);

	unsigned long daddr;
	unsigned long saddr;
	int sockfd;
	int packet_len;
	char *packet;

	saddr = inet_addr(argv[1]);
	daddr = inet_addr(argv[2]);

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if (sockfd < 0)
		sdie("socket()");

	const int on = 1;

	/* We shall provide IP headers */
	if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL,
				(const char *) &on, sizeof(on)) == -1)
		sdie("setsockopt()");

	/* Calculate total packet size */
	packet_len = sizeof(struct iphdr);
	packet = malloc(packet_len);
	if (!packet)
		sdie("malloc()");
	memset(packet, 0, packet_len);

	/* IP header (do you believe ??) */
	struct iphdr *ip = (struct iphdr *) packet;

	/* Version */
	ip->version = 4;
	/* Header Length */
	/* can you use (packet_len * 8) / 32 ? */
	ip->ihl = 5;
	/* Type Of Service */
	ip->tos = 0;
	/* Total length */
	ip->tot_len = htons(packet_len);
	/* ID */
	ip->id = rand();
	/* Fragmentation Offset */
	ip->frag_off = 0;
	/* Time To Live */
	ip->ttl = 73;
	/* Protocol */
	ip->protocol = 1;
	/* Source Address */
	ip->saddr = saddr;
	/* Destination Address */
	ip->daddr = daddr;
	/* Checksum */
	ulog("0x%04hx\n", htons(ip_checksum(ip)));
	ip->check = htons(ip_checksum(ip));

	struct sockaddr_in servaddr;

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = daddr;
	memset(&servaddr.sin_zero, 0, sizeof(servaddr.sin_zero));

	puts("sending... :-|");
	if (sendto(sockfd, packet, packet_len, 0,
				(struct sockaddr *) &servaddr,
				sizeof(servaddr)) < 1)
		sdie("sendto()");
	puts("sent.... :-)");

	puts("receiving... :-|");
	if (recvfrom(sockfd, packet, packet_len, 0, NULL, NULL) < 1)
		sdie("recvfrom()");
	puts("received.... :-)");

}
