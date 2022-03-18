#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <errno.h>

#define BUF_SIZE 5

int main(int argc, char **argv){
	if(argc < 3){
		printf("Not enough arguments\n");
		return 1;
	}

	int sockfd = 0;
	
	//if adress contains ':', its an ipv6 adress, else ipv4
	char *find_two_point = strchr(argv[1], ':');
	
	struct sockaddr *s;
	socklen_t len;

	if(find_two_point == NULL) //ipv4
	{
		sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		struct sockaddr_in *ipv4_adress = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
		memset(ipv4_adress, 0, sizeof(struct sockaddr_in));
			
		ipv4_adress->sin_family = AF_INET; //family
		ipv4_adress->sin_port = htons(atoi(argv[2])); //port number
		if(inet_pton(AF_INET, argv[1], &(ipv4_adress->sin_addr)) <= 0) //adress
			printf("Error : %s\n", strerror(errno));

		len = sizeof(struct sockaddr_in);
		s = (struct sockaddr *) ipv4_adress;
	}
	else //ipv6
	{
		sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
		struct sockaddr_in6 *ipv6_adress = (struct sockaddr_in6 *) malloc(sizeof(struct sockaddr_in6));

		ipv6_adress->sin6_family = AF_INET6; //family
		ipv6_adress->sin6_port = htons(atoi(argv[2])); //port number
		if(inet_pton(AF_INET6, argv[1], &(ipv6_adress->sin6_addr)) <= 0) //adress
			printf("Error : %s\n", strerror(errno));

		len = sizeof(struct sockaddr_in6);
		s = (struct sockaddr *) ipv6_adress;
	}
	
	char *msg_to_send = (char *) malloc(BUF_SIZE * sizeof(char));

	while(fgets(msg_to_send, BUF_SIZE, stdin) != NULL){
		sendto(sockfd, msg_to_send, strlen(msg_to_send), 0, s, len);
	}

	free(msg_to_send);
	free(s);
	return 0;
}
