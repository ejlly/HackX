#include "dhcp.h"

void fill_offer(dhcp_header* offer_msg){
	
	srand(time(NULL));


	memset(offer_msg, 0, sizeof(dhcp_header));
	
	offer_msg->op = 1;
	offer_msg->htype = 1;
	offer_msg->hlen = 6;

	offer_msg->xid = htonl(rand()%(0xffffffffffffffff));
	
	offer_msg->cookie = htonl(0x63825363);
	

}

void randomize_offer(dhcp_header* offer_msg){
	//only call after fill_offer has been run

	for(int i = 0; i<32; i++){
		offer_msg->chaddr[i] = rand()%256;
	}

}

void send_packet(dhcp_header* offer_msg){
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	if(sockfd < 0){
		printf("Failed to create socket\n");
		return;
	}

	int flag = 1;
	
	if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag)) < 0){
		printf("Failed to set broadcast parameter\n");
		return;
	}

	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(DHCP_PORT);
	
	if(inet_aton(DEST_ADDRESS, &addr.sin_addr) == 0){
		printf("Couldn't put NULL_ADDR into sockaddr_in\n");
		return;
	}

	//printf("size of message : %ld\n", sizeof(dhcp_header));

	if(sendto(sockfd, offer_msg, sizeof(dhcp_header), 0, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) < 0){
		printf("Error sending packet : %s\n", strerror(errno));
		return;
	}
	

	close(sockfd);
}
