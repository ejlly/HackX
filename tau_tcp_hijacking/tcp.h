#ifndef MY_OWN_TCP_H
#define MY_OWN_TCP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <errno.h>

#include "header.h"

int open_socket();

void swap_mac_add(uint8_t *buf);

void swap_ips(uint8_t *buf);

void swap_ports(uint8_t *buf);

void send_packet(int sockfd, uint8_t *buf, int size);

#endif
