#include "tcp.h"

int open_socket(){
	int sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

	if(sock_fd < 0){
		printf("Error creating socket : %s\n", strerror(errno));
		return -1;
	}

	int flag = 1;

	if(setsockopt(sock_fd, IPPROTO_IP, IP_HDRINCL, &flag, sizeof(flag)) < 0){
		printf("Failed to set IP_HDRINCL parameter\n");
		return -1;
	}

	return sock_fd;
}

void swap_mac_add(uint8_t *buf){

	struct ethhdr *eth = (struct ethhdr*)(buf);

	unsigned char tmp[ETH_ALEN];
	memcpy(tmp, eth->h_dest, ETH_ALEN);
	memcpy(eth->h_dest, eth->h_source, ETH_ALEN);
	memcpy(eth->h_source, tmp, ETH_ALEN);
}

void swap_ips(uint8_t *buf){
	
	struct iphdr *iph = (struct iphdr*)(buf + sizeof(struct ethhdr));

	uint32_t tmp = iph->saddr;
	iph->saddr = iph->daddr;
	iph->daddr = tmp;
}

void swap_ports(uint8_t *buf){
	
	struct tcphdr *tcp = (struct tcphdr*)(buf + sizeof(struct ethhdr) + sizeof(struct iphdr));

	uint16_t tmp = tcp->source;
	tcp->source = tcp->dest;
	tcp->dest = tmp;
}

void send_packet(int sock_fd, uint8_t *buf, int size){
	
	

	

	struct iphdr *iph = (struct iphdr*)(buf);
	struct tcphdr *tcp = (struct tcphdr*)(buf + sizeof(struct iphdr));


	
	struct pseudo_tcp_header psh; //pseudo header
	psh.source_address = iph->saddr;
	psh.dest_address = iph->daddr;
	psh.protocol = iph->protocol;
	psh.tcp_length = size - sizeof(struct iphdr);

	
	//checksum calculation
	int const size_check = size - sizeof(struct iphdr) + sizeof(struct pseudo_tcp_header);
	uint8_t *check_calc = malloc(sizeof(char) * size_check);
	
	memcpy(check_calc, &psh, sizeof(struct pseudo_tcp_header));
	memcpy(check_calc + sizeof(struct pseudo_tcp_header), tcp, size - sizeof(struct iphdr));

	
	tcp->check = checksum((short unsigned int*) check_calc, size_check);

	struct sockaddr_in dest;
	dest.sin_family = AF_INET;
	dest.sin_port = tcp->source;
	printf("port nb : %d and ip adress : %d.%d.%d.%d\n", ntohs(tcp->dest), iph->daddr%256, (iph->daddr/256)%256, (iph->daddr/(256*256))%256, (iph->daddr/(256*256*256))%256);
	dest.sin_addr.s_addr = iph->saddr;

	for(int i = 0; i < size; i++){
		if(i > 0 && i%16 == 0) printf("\n");
		printf("%02x ", buf[i]);
	}
	printf("\n");

	for(int i = 0; i<100; i++)
	if(sendto(sock_fd, buf, size, 0, (struct sockaddr*) &dest, sizeof(struct sockaddr_in)) < 0){
		printf("    Error sending packet : %s\n", strerror(errno));
		break;
	}
}
