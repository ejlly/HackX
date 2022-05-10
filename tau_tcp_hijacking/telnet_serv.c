#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <errno.h>

#include <arpa/inet.h>

#define USERNAME "a\r\n"
#define PASSWORD "a\r\n"

//#define SERVER_IP "129.104.230.157"
#define SERVER_IP "127.0.0.57"
#define PORT 1234
#define BUF_SIZE 2048

void reconnect(int *socket_fd, struct sockaddr_in *address){
	close(*socket_fd);

	*socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(connect(*socket_fd, (struct sockaddr *) address, sizeof(struct sockaddr_in)) != 0){
		printf("Error connecting to server\n");
	}
	
	//Evacuate the "What's your name" string
	char buf[BUF_SIZE];
	recv(*socket_fd, buf, BUF_SIZE, 0);
}

int command_injector(int *socket_fd, char *cmd, struct sockaddr_in *adress){
	//the command shoudln't be too long as not to overflow buffer
	
	//send(*socket_fd, command_string, EXPLOIT_SIZE+CMD_SIZE, 0);

	char buf[BUF_SIZE];
	recv(*socket_fd, buf, BUF_SIZE, 0);
	
	printf("buffer is : %s\n", buf);

	reconnect(socket_fd, adress);

	return 0;
}



int main(){
	struct sockaddr_in serv_address;
	serv_address.sin_family = AF_INET;
	serv_address.sin_port = htons(PORT);
	serv_address.sin_addr.s_addr = inet_addr(SERVER_IP);

	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd == -1){
		printf("Error creating socket : %s\n", strerror(errno));
		return 1;
	}

	int option = 1;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	if(bind(socket_fd, (struct sockaddr *)&serv_address, sizeof(struct sockaddr_in))){
		printf("Error binding socket : %s\n", strerror(errno));
		close(socket_fd);
		return 2;
	}

	if(listen(socket_fd, 10) == -1){
		printf("Could not listen: %s\n", strerror(errno));
		close(socket_fd);
		return 3;
    }

	char buf[BUF_SIZE];

	struct sockaddr_in client;
	socklen_t size;

	while(1){
		int connfd = accept(socket_fd, (struct sockaddr *) &client, &size);

		if(send(connfd, "Username : ", 12, 0) < 0){
			printf("Error sending : %s\n", strerror(errno));
			close(socket_fd);
			return 4;
		}

		//Receive username
		if(recv(connfd, buf, BUF_SIZE, 0) < 0){
			printf("Failed to recv : %s\n", strerror(errno));
			close(socket_fd);
			return 5;
		}

		//CHECK if username is correct
		if(strcmp(USERNAME, buf) != 0){
			/*
			   printf("%s != %s %d %d\n", USERNAME, buf, strlen(USERNAME), strlen(buf));
			   printf("chars : %d %d %d\n", USERNAME[0], USERNAME[1], USERNAME[2]);
			   printf("chars : %d %d %d\n", buf[0], buf[1], buf[2]);
			   */
			close(connfd);
			close(socket_fd);
			return 6;
		}


		if(send(connfd, "Password : ", 12, 0) < 0){
			printf("Error sending : %s\n", strerror(errno));
			close(socket_fd);
			return 7;
		}

		//Receive password
		if(recv(connfd, buf, BUF_SIZE, 0) < 0){
			printf("Failed to recv : %s\n", strerror(errno));
			close(socket_fd);
			return 8;
		}

		//CHECK if password is correct
		if(strcmp(PASSWORD, buf) != 0){
			close(connfd);
			close(socket_fd);
			return 9;
		}

		while(1){
			if(send(connfd, ">", 2, 0) < 0){
				printf("Error sending : %s\n", strerror(errno));
				close(socket_fd);
				return 10;
			}

			if(recv(connfd, buf, BUF_SIZE, 0) < 0){
				printf("Failed to recv : %s\n", strerror(errno));
				close(socket_fd);
				return 11;
			}

			if(send(connfd, buf, strlen(buf), 0) < 0){
				printf("Failed to recv : %s\n", strerror(errno));
				close(socket_fd);
				return 12;
			}
		}
	}
	

	close(socket_fd);
	return 0;
}
