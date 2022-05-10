/*
 * dns_server.c
 *
 *  Created on: Apr 26, 2016
 *      Author: jiaziyi
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <time.h>

#include <errno.h>


#include "dns.h"

#define FALSE_IP "1.2.3.4"

int main(int argc, char *argv[]){
	int sockfd;
	struct sockaddr server;

	int port = 53; //the default port of DNS service


	//to keep the information received.
	res_record answers[ANS_SIZE], auth[ANS_SIZE], addit[ANS_SIZE];
	query queries[ANS_SIZE];


	if(argc == 2)
	{
		port = atoi(argv[1]); //if we need to define the DNS to a specific port
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	int enable = 1;

	if(sockfd < 0){
		perror("socket creation error");
		exit_with_error("Socket creation failed");
	}

	//in some operating systems, you probably need to set the REUSEADDR
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0){
	    perror("setsockopt(SO_REUSEADDR) failed");
	}

	//for v4 address
	struct sockaddr_in *server_v4 = (struct sockaddr_in*)(&server);
	server_v4->sin_family = AF_INET;
	server_v4->sin_addr.s_addr = htonl(INADDR_ANY);
	server_v4->sin_port = htons(port);

	//bind the socket
	if(bind(sockfd, &server, sizeof(*server_v4))<0){
		perror("Binding error");
		exit_with_error("Socket binding failed");
	}

	printf("The dns_server is now listening on port %d ... \n", port);
	//print out
	uint8_t buf[BUF_SIZE], send_buf[BUF_SIZE]; //receiving buffer and sending buffer
	struct sockaddr remote;
	socklen_t addr_len = sizeof(remote);
	struct sockaddr_in *remote_v4 = (struct sockaddr_in*)(&remote);

	int a,b,c,d;
	sscanf(FALSE_IP, "%d.%d.%d.%d", &a, &b, &c, &d);


	while(1){
		//an infinite loop that keeps receiving DNS queries and send back a reply
		recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr*) remote_v4, &addr_len);

		int id = parse_dns_query(buf, queries, answers, auth, addit);

		int pos = 0;
		uint8_t qname[HOST_NAME_SIZE], *p = &buf[sizeof(dns_header)];

		get_domain_name(p, buf, qname, &pos);

		p += pos + sizeof(question);

		memcpy(send_buf, buf, BUF_SIZE);

		dns_header *dns = (dns_header *) send_buf;
		build_dns_header(dns, id, 1, 1, 1, 0, 0);

			
		uint8_t answer[] = {0xc0, 0x0c, 0, 1, 0, 1, 0, 0, 1, 0x2c, 0, 4, (uint8_t) a, (uint8_t) b, (uint8_t) c, (uint8_t) d};
		
		memcpy(p, answer, 16);

		
		if(sendto(sockfd, send_buf, sizeof(dns_header) + pos + sizeof(question) + 16, 0, (struct sockaddr*) remote_v4, addr_len) < 0){
			printf("Answer failed : %s\n", strerror(errno));
		}
		else
			printf("Answer sent\n");

		printf("*****************************************************************\n");
	}
}
