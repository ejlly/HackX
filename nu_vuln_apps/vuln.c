#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#define DEBUG
#ifdef DEBUG
#define DEBUG_OUTPUT printf
#else
#define DEBUG_OUTPUT(...) do{} while(0)
#endif
int parse(int a)
{
	char greeting_text[128];
	char buf[256] = {0};
	// Redirect stdout and stdin to the socket
	//DEBUG_OUTPUT("%p %p\n", buf, greeting_text);

	//sleep(5);
	

	//printf("What is your name?\n");
	//fflush(stdout);
	fgets(buf, sizeof(buf), stdin);
	strcpy(greeting_text, "Hello, dear ");
	strcat(greeting_text, buf);
	//printf("%s\n", greeting_text);
	return 0;
}
int main(int argc, char** argv)
{
	int listenfd = 0,connfd = 0;
	char sendBuff[1024];
	
	signal(SIGCHLD, SIG_IGN);
	parse(0);

	return 0;
}
