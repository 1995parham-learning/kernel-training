/*
 * In The Name Of God
 * ========================================
 * [] File Name : ip.c
 *
 * [] Creation Date : 03-04-2015
 *
 * [] Last Modified : Fri 03 Apr 2015 11:13:09 PM IRDT
 *
 * [] Created By : Parham Alvani (parham.alvani@gmail.com)
 * =======================================
*/
#include <netinet/ip.h>

#include "ip_func.h"

uint16_t ip_checksum(const struct iphdr *ip)
{
	uint16_t checksum = 0;
	int len = 0;
	const void *ip_header = ip;
	
	while (len < ip->ihl * 32) {
		checksum += *((uint16_t *) (ip_header + len));
		len += 16;
	}

	return -checksum;
}

