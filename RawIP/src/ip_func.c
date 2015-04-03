/*
 * In The Name Of God
 * ========================================
 * [] File Name : ip.c
 *
 * [] Creation Date : 03-04-2015
 *
 * [] Last Modified : Sat 04 Apr 2015 01:23:09 AM IRDT
 *
 * [] Created By : Parham Alvani (parham.alvani@gmail.com)
 * =======================================
*/
#include <netinet/ip.h>
#include <stdint.h>

#include "ip_func.h"
#include "common.h"

uint16_t ip_checksum(const struct iphdr *ip)
{
	uint32_t sum = 0;
	uint16_t checksum = 0;
	int len = ip->ihl * 4;
	const uint16_t *ip_header = (const uint16_t *) ip;

	while (len > 1) {
		sum += *ip_header++;
		len -= sizeof(uint16_t);
	}
	if (len)
		sum += *((uint8_t *)(ip_header));

	/* handle sum carry !! :-) this is good !! */
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);

	/* calculate 1's complement of the sum */
	checksum = (uint16_t) ~sum;

	return checksum;
}
