#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <errno.h>

#define BUF_SIZE 100

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *read_send(int socket_fd, struct sockaddr *s, int len){

	char *msg_to_send = (char *) malloc(BUF_SIZE * sizeof(char));

	while(fgets(msg_to_send, BUF_SIZE, stdin) != NULL){
		pthread_mutex_lock(&lock); //lock
		sendto(socket_fd, msg_to_send, strlen(msg_to_send), 0, s, len);
		pthread_mutex_unlock(&lock); //unlock
	}
	free(msg_to_send);
	return NULL;
}

void *recv_answ(void *socket_fd){
	int sockfd = *(int *)socket_fd;

	char *buf = (char*) malloc(sizeof(char) * (BUF_SIZE+1));
	struct sockaddr s;
	socklen_t len = sizeof(struct sockaddr);

	while(1){
		pthread_mutex_lock(&lock); //lock
		int r_msg_size = recvfrom(sockfd, buf, sizeof(char)*BUF_SIZE, MSG_DONTWAIT, &s, &len);
		pthread_mutex_unlock(&lock); //unlock
		if(r_msg_size != -1 && r_msg_size != EAGAIN && r_msg_size != EWOULDBLOCK)
			printf("server reply : %s", buf);
	}

	free(buf);
	return NULL;
}

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

	//Retrieve ip address into sockaddr
	
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
	
	pthread_t listener;
	if(pthread_create(&listener, NULL, recv_answ, &sockfd)) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	read_send(sockfd, s, len);
	

	free(s);
	return 0;
}
