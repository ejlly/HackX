#ifndef DHCP_HEADER_NC
#define DHCP_HEADER_NC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <errno.h>

#define DNS_SERVER "1.2.3.4"

#define DEST_ADDRESS "255.255.255.255"
#define SOURCE_ADDRESS "0.0.0.1"
#define SRC_PORT 68
#define DEST_PORT 67

#define MY_IP "127.0.0.1"

#define OPTION_SIZE 1022

#define DHCPDISCOVER 1
#define DHCPOFFER 2
#define DHCPREQUEST 3
#define DHCPDECLINE 4
#define DHCPACK 5
#define DHCPNAK 6
#define DHCPRELEASE 7
#define DHCPINFORM 8

typedef struct dhcp_header{
	uint8_t op;
	uint8_t htype;
	uint8_t hlen;
	uint8_t hops;
	uint32_t xid;
	uint16_t secs;
	uint16_t flags;
	uint32_t ciaddr;
	uint32_t yiaddr;
	uint32_t siaddr;
	uint32_t giaddr;
	char chaddr[16]; 
	char sname[64];
	char file[128];
	uint32_t cookie;
	unsigned char options[OPTION_SIZE];

} dhcp_header;

//structs from Tutorial 4
#define ETH_ALEN	6		/* Octets in one ethernet addr	 */

struct ethhdr {
	unsigned char	h_dest[ETH_ALEN];	/* destination eth addr	*/
	unsigned char	h_source[ETH_ALEN];	/* source ether addr	*/
	unsigned short	h_proto;		/* packet type ID field	*/
} __attribute__((packed));

struct iphdr{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int ihl:4;
    unsigned int version:4;
#elif __BYTE_ORDER == __BIG_ENDIAN
    unsigned int version:4;
    unsigned int ihl:4;
#else
# error	"Please fix <bits/endian.h>"
#endif
    u_int8_t tos;
    u_int16_t tot_len;
    u_int16_t id;
    u_int16_t frag_off;
    u_int8_t ttl;
    u_int8_t protocol;
    u_int16_t check;
    u_int32_t saddr;
    u_int32_t daddr;
    /*The options start here. */
};

struct pseudo_udp_header{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t udp_length;
};

struct udphdr{
  u_int16_t	source;
  u_int16_t	dest;
  u_int16_t	len;
  u_int16_t	check;
};

#define MAX_LIST_SIZE 65536

struct list_ip{
	uint32_t ips[MAX_LIST_SIZE];
	int len;
	int index;
};

void add_ip(struct list_ip *ips, uint32_t ip);

unsigned short checksum(unsigned short *ptr, int nbytes);

int packet_type(dhcp_header *dhcp, unsigned int size);
void modify_packet_type(dhcp_header *dhcp, unsigned int size, int new_type);

void add_dns_server(dhcp_header *dhcp, unsigned int *size);

void fill_offer(dhcp_header* offer_msg);

void randomize_offer(dhcp_header* offer_msg);

int open_socket();

void send_packet_poison(int sockfd, dhcp_header* offer_msg);

void send_packet(int sockfd, dhcp_header* dhcp, unsigned int size);

unsigned int fill_dhcp_packet(dhcp_header **dhcph, const u_char *Buffer, int Size);

#endif
