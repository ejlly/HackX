#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include <errno.h>

#define MAX_NB_USERS 100

#define OWN_IP "127.0.0.1"
#define BUF_SIZE 100

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;



void *echo(int sockfd, char *buf, int len, struct sockaddr_in *from, socklen_t *fromlen);

void *receive(void *sockfd){

	int socket_fd = *(int *)sockfd;

	char *buf = (char*) malloc(sizeof(char) * (BUF_SIZE+1));
	struct sockaddr_in *from = (struct sockaddr_in*) malloc(sizeof(struct sockaddr_in));
	socklen_t *fromlen = (socklen_t *) malloc(sizeof(socklen_t));
	*fromlen = sizeof(&from);
	

	pthread_mutex_lock(&lock); //lock
	
	printf("in \n");
	int len = recvfrom(socket_fd, buf, sizeof(char)*BUF_SIZE, 0, (struct sockaddr *) from, fromlen);

	printf("\tlen : %d\n", len);

	buf[len] = '\0';
	printf("%s", buf);
	printf("out\n");
	pthread_mutex_unlock(&lock); //unlock

	echo(socket_fd, buf, len, from, fromlen);

	return NULL;
}

void *echo(int sockfd, char *buf, int len, struct sockaddr_in *from, socklen_t *fromlen){
	pthread_mutex_lock(&lock); //lock

	sendto(sockfd, buf, len, 0, (struct sockaddr *) from, *fromlen);

	pthread_mutex_unlock(&lock); //unlock

	free(from);
	free(fromlen);
	free(buf);

	receive(&sockfd);
	return NULL;
}

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
	
	printf("Starting :\n");

	pthread_t users[MAX_NB_USERS];


	for(int i = 0; i<MAX_NB_USERS; i++){
		if(pthread_create(&users[i], NULL, receive, &sockfd)) {
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
	}


	free(ipv4_adress);
}
