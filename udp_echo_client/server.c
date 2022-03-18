#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <errno.h>

#define OWN_IP "127.0.0.1"
#define BUF_SIZE 100

int main(int argc, char **argv){
	if(argc < 2){
		printf("Not enough arguments\n");
		return 1;
	}

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	struct sockaddr_in *ipv4_adress = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));

	memset(ipv4_adress, 0, sizeof(struct sockaddr_in));
	ipv4_adress->sin_family = AF_INET; //family
	ipv4_adress->sin_port = htons(atoi(argv[1])); //port number
	if(inet_pton(AF_INET, OWN_IP, &(ipv4_adress->sin_addr)) <= 0) //adress
		printf("Error : %s\n", strerror(errno));

	if(bind(sockfd, (struct sockaddr*) ipv4_adress, sizeof(struct sockaddr_in)))
		printf("Error : %s\n", strerror(errno));

	char *buf = (char*) malloc(sizeof(char) * (BUF_SIZE+1));

	struct sockaddr_in from;
	socklen_t fromlen = sizeof(&from);

	while(1){
		int len = recvfrom(sockfd, buf, sizeof(char)*BUF_SIZE, 0, (struct sockaddr *) &from, &fromlen);

		buf[len] = '\0';
		printf("%s", buf);

		sendto(sockfd, buf, len, 0, (struct sockaddr *) &from, fromlen);
	}

	free(ipv4_adress);
	free(buf);
}
