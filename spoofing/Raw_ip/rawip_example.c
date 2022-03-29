/*
 * rawip_example.c
 *
 *  Created on: May 4, 2016
 *      Author: jiaziyi
 */


#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include <errno.h>

#include "header.h"

#define SRC_IP  "1.2.3.4" //set your source ip here. It can be a fake one
#define SRC_PORT 54321 //set the source port here. It can be a fake one

#define DEST_IP "178.98.18.1" //set your destination ip here
#define DEST_PORT 5555 //set the destination port here
#define TEST_STRING "you've been spoofed, and it was a pleasure" //a test string as packet payload, min 8 bits

int main(int argc, char *argv[])
{
	char source_ip[] = SRC_IP;
	char dest_ip[] = DEST_IP;

	int fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

    int hincl = 1;                  /* 1 = on, 0 = off */
    setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &hincl, sizeof(hincl));

	if(fd < 0)
	{
	perror("Error creating raw socket ");
		exit(1);
	}
	
	char packet[65536], *data;
	char data_string[] = TEST_STRING;
	memset(packet, 0, 65536);

	//IP header pointer
	struct iphdr *iph = (struct iphdr *)packet;

	//UDP header pointer
	struct udphdr *udph = (struct udphdr *)(packet + sizeof(struct iphdr));
	struct pseudo_udp_header psh; //pseudo header

	//data section pointer
	data = packet + sizeof(struct iphdr) + sizeof(struct udphdr);

	//fill the data section
	strncpy(data, data_string, strlen(data_string));
	
	int const size_iph = sizeof(struct iphdr), size_udph = sizeof(struct udphdr), size_pseudo = sizeof(struct pseudo_udp_header), size_data = strlen(data);
	int const tot_size = sizeof(struct iphdr) + size_udph + size_data;


	//fill the IP header here
	iph->version = 4;
	iph->ihl = 5;
	iph->tos = 0;
	iph->tot_len = htons(size_iph + size_udph + size_data);
	iph->id = 0; //random value
	iph->frag_off = htons(0);
	iph->ttl = 128;
	iph->protocol = 17;
	iph->saddr = inet_addr(source_ip); //no htonl
	iph->daddr = inet_addr(dest_ip); //no htonl
	iph->check = htons(checksum((short unsigned int *) iph, size_iph));


	//fill the UDP header
	udph->source = htons(SRC_PORT);
	udph->dest = htons(DEST_PORT);
	udph->len = htons(size_udph + size_data);
	udph->check = 0;

	psh.source_address = iph->saddr;
	psh.dest_address = iph->daddr;
	psh.protocol = iph->protocol;
	psh.udp_length = udph->len;


	
	//printf("total msg size : %d\n", tot_size);

	

	//checksum calculation
	
	char *check_calc = malloc(size_udph + size_pseudo + size_data);
	
	memcpy(check_calc, &psh, size_pseudo);
	memcpy(check_calc + size_pseudo, udph, size_udph);
	memcpy(check_calc + size_pseudo + size_udph, data, size_data);

	udph->check = checksum((short unsigned int*) check_calc, size_udph + size_pseudo + size_data);

	//send the packet
	
	struct sockaddr_in dest;

	dest.sin_family = AF_INET;
	dest.sin_port = htons(DEST_PORT);
	dest.sin_addr.s_addr = htons(inet_addr(DEST_IP));

	if(sendto(fd, packet, tot_size, 0, (struct sockaddr *) &dest, sizeof(dest)) < 0)
		printf("Error : %s\n", strerror(errno));

	close(fd);

	return 0;

}
