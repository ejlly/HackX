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
#define DHCP_PORT 68

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
	unsigned char chaddr[64]; unsigned char sname_and_file[192];
	uint32_t cookie;
	unsigned char *options;

} dhcp_header;

void fill_offer(dhcp_header* offer_msg);

void randomize_offer(dhcp_header* offer_msg);

void send_packet(dhcp_header* offer_msg);

#endif
