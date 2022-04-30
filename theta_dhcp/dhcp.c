#include "dhcp.h"

unsigned short checksum(unsigned short *ptr,int nbytes){
    register long sum;
    unsigned short oddbyte;
    register short answer;

    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }

    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;

    return(answer);
}

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

	for(int i = 0; i<6; i++){
		offer_msg->chaddr[i] = rand()%256;
	}

}

int open_socket(){
	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

	if(sockfd < 0){
		printf("Error creating socket : %s\n", strerror(errno));
		return -1;
	}

	int flag = 1;

	if(setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &flag, sizeof(flag)) < 0){
		printf("Failed to set IP_HDRINCL parameter\n");
		return -1;
	}

	if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag)) < 0){
		printf("Failed to set broadcast parameter\n");
		return -1;
	}

	return sockfd;
}

void send_packet(int sockfd, dhcp_header* offer_msg){
	
	

	char packet[65536], *data;
	memset(packet, 0, 65536);

	//IP header pointer
	struct iphdr *iph = (struct iphdr *) packet;

	//UDP header pointer
	struct udphdr *udph = (struct udphdr *) (packet + sizeof(struct iphdr));
	struct pseudo_udp_header psh; //pseudo header

	//data section pointer
	data = packet + sizeof(struct iphdr) + sizeof(struct udphdr);

	//fill the data section
	memcpy(data, offer_msg, sizeof(dhcp_header));
	
	int const size_iph = sizeof(struct iphdr), size_udph = sizeof(struct udphdr), size_pseudo = sizeof(struct pseudo_udp_header), size_data = sizeof(dhcp_header);
	int const tot_size = sizeof(struct iphdr) + size_udph + size_data;


	//fill the IP header here
	iph->version = 4;
	iph->ihl = 5;
	iph->tos = 0;
	iph->tot_len = htons(size_iph + size_udph + size_data);
	iph->id = rand()%(2<<15); //random value
	iph->frag_off = htons(0);
	iph->ttl = 128;
	iph->protocol = 17;
	iph->saddr = inet_addr(SOURCE_ADDRESS); //no htonl
	iph->daddr = inet_addr(DEST_ADDRESS); //no htonl
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

	
	//checksum calculation
	char *check_calc = malloc(size_udph + size_pseudo + size_data);
	
	memcpy(check_calc, &psh, size_pseudo);
	memcpy(check_calc + size_pseudo, udph, size_udph);
	memcpy(check_calc + size_pseudo + size_udph, data, size_data);

	udph->check = checksum((short unsigned int*) check_calc, size_udph + size_pseudo + size_data);
	
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(DEST_PORT);
	
	if(inet_aton(DEST_ADDRESS, &addr.sin_addr) == 0){
		printf("Couldn't put NULL_ADDR into sockaddr_in\n");
		return;
	}

	if(sendto(sockfd, packet, tot_size, 0, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) < 0){
		printf("Error sending packet : %s\n", strerror(errno));
		return;
	}
	
	free(check_calc);
}
