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

#define DEST_ADDRESS "255.255.255.255"

#define SOURCE_ADDRESS "0.0.0.1"

#define SRC_PORT 67
#define DEST_PORT 68

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
	unsigned char *options;

} dhcp_header;

//iphdr from 
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


unsigned short checksum(unsigned short *ptr, int nbytes);

void fill_offer(dhcp_header* offer_msg);

void randomize_offer(dhcp_header* offer_msg);

int open_socket();

void send_packet(int sockfd, dhcp_header* offer_msg);

#endif
