/**
 *  Jiazi Yi
 *
 * LIX, Ecole Polytechnique
 * jiazi.yi@polytechnique.edu
 *
 * Updated by Pierre Pfister
 *
 * Cisco Systems
 * ppfister@cisco.com
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <arpa/inet.h>

#include "url.h"
#include "wgetX.h"


#define MAXRCVLEN 500

typedef struct addrinfo addrinfo;

int counter_http_redirection = 255;

int main(int argc, char* argv[]) {
    url_info info;
    const char * file_name = "received_page";
    if (argc < 2) {
	fprintf(stderr, "Missing argument. Please enter URL.\n");
	return 1;
    }

    char *url = argv[1];

    // Get optional file name
    if (argc > 2) {
	file_name = argv[2];
    }

    // First parse the URL
    int ret = parse_url(url, &info);
    if (ret) {
	fprintf(stderr, "Could not parse URL '%s': %s\n", url, parse_url_errstr[ret]);
	return 2;
    }

    //If needed for debug
    //print_url_info(&info);

    // Download the page
    struct http_reply reply;

    ret = download_page(&info, &reply);
    if (ret) return 3;


    // Now parse the responses
    char *response = read_http_reply(&reply);
    if (response == NULL) {
	fprintf(stderr, "Could not parse http reply\n");
	return 4;
    }

    // Write response to a file
    write_data(file_name, response, reply.reply_buffer + reply.reply_buffer_length - response);

    // Free allocated memory
    free(reply.reply_buffer);

    // Just tell the user where is the file
    fprintf(stderr, "the file is saved in %s\n.", file_name);
    return 0;
}

int download_page(url_info *info, http_reply *reply) {

    /*
     * To be completed:
     *   You will first need to resolve the hostname into an IP address.
     *
     *   Option 1: Simplistic
     *     Use gethostbyname function.
     *
     *   Option 2: Challenge
     *     Use getaddrinfo and implement a function that works for both IPv4 and IPv6.
     *
     */

	addrinfo *results, *rp, *hints = malloc(sizeof(addrinfo));
	char service[6];

	hints->ai_flags = AI_PASSIVE;
	hints->ai_family = AF_UNSPEC;
	hints->ai_socktype = SOCK_STREAM;
	hints->ai_protocol = 0;
	hints->ai_addrlen = 0;
    hints->ai_addr = NULL;
	hints->ai_canonname = NULL;
    hints->ai_next = NULL;

	sprintf(service, "%d", info->port);

	if(getaddrinfo(info->hostname, service, hints, &results)){
		fprintf(stderr, "Could not fetch IP address !\n");
		return 1;
	}

	free(hints);
	//printf("result is : %s\n", );
	int socket_fd;

	//inspired from getaddrinfo documentation :

    for(rp = results; rp != NULL; rp = rp->ai_next){ //try with all IPv4 ips
		if(rp->ai_family == AF_INET6) continue; //if IPv6, discard

		socket_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

		if(socket_fd == -1) continue;

		char *s = inet_ntoa(((struct sockaddr_in*) rp->ai_addr)->sin_addr);
		printf("address : %s\n", s);

		if(connect(socket_fd, rp->ai_addr, rp->ai_addrlen) == 0) break;
		close(socket_fd);
	}


	if(rp == NULL){
		for(rp = results; rp != NULL; rp = rp->ai_next){ //try with all IPv6 ips
            if(rp->ai_family == AF_INET) continue; //if IPv4, discard

            socket_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

            if(socket_fd == -1) continue;

            char str[INET6_ADDRSTRLEN];
            struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)rp->ai_addr;

            inet_ntop(rp->ai_family, &sin6->sin6_addr, str, INET6_ADDRSTRLEN-1);
            printf("address : %s\n", str);

            if(connect(socket_fd, rp->ai_addr, rp->ai_addrlen) == 0) break;
            close(socket_fd);
        }

		if(rp == NULL){
			fprintf(stderr, "Could not connect to server !\n");
			return 1;
		}
	}

    /*
     * To be completed:
     *   Next, you will need to send the HTTP request.
     *   Use the http_get_request function given to you below.
     *   It uses malloc to allocate memory, and snprintf to format the request as a string.
     *
     *   Use 'write' function to send the request into the socket.
     *
     *   Note: You do not need to send the end-of-string \0 character.
     *   Note2: It is good practice to test if the function returned an error or not.
     *   Note3: Call the shutdown function with SHUT_WR flag after sending the request
     *          to inform the server you have nothing left to send.
     *   Note4: Free the request buffer returned by http_get_request by calling the 'free' function.
     *
     */

	//printf("Error description is : %s\n",strerror(errno));
	char *http_request = http_get_request(info);
	int test = write(socket_fd, http_request, 100 + strlen(info->path) + strlen(info->hostname));

	if(test < 0)
		fprintf(stderr, "Error sending request : %d, errno : %d, errmsg : %s\n", test, errno, strerror(errno));
	printf("nb of bytes transfered : %d\n", test);


	free(http_request);
	shutdown(socket_fd, SHUT_WR);

    /*
     * To be completed:
     *   Now you will need to read the response from the server.
     *   The response must be stored in a buffer allocated with malloc, and its address must be save in reply->reply_buffer.
     *   The length of the reply (not the length of the buffer), must be saved in reply->reply_buffer_length.
     *
     *   Important: calling recv only once might only give you a fragment of the response.
     *              in order to support large file transfers, you have to keep calling 'recv' until it returns 0.
     *
     *   Option 1: Simplistic
     *     Only call recv once and give up on receiving large files.
     *     BUT: Your program must still be able to store the beginning of the file and
     *          display an error message stating the response was truncated, if it was.
     *
     *   Option 2: Challenge
     *     Do it the proper way by calling recv multiple times.
     *     Whenever the allocated reply->reply_buffer is not large enough, use realloc to increase its size:
     *        reply->reply_buffer = realloc(reply->reply_buffer, new_size);
     *
     *
     */

	reply->reply_buffer = (char *) malloc(MAXRCVLEN * sizeof(char));
	reply->reply_buffer_length = 0;

	while(recv(socket_fd, reply->reply_buffer + reply->reply_buffer_length, MAXRCVLEN, MSG_WAITALL)){
		reply->reply_buffer_length += MAXRCVLEN;
		reply->reply_buffer = (char *) realloc(reply->reply_buffer, sizeof(char) * (reply->reply_buffer_length + MAXRCVLEN));
	}

	//snippet to efficiently find actual size of reply_buffer
	reply->reply_buffer_length -= MAXRCVLEN;

	for(char* ptr = reply->reply_buffer + reply->reply_buffer_length; *ptr != '\0'; ptr++)
		reply->reply_buffer_length++;

	//printf("size : %d %lu\n", reply->reply_buffer_length, strlen(reply->reply_buffer));
	//printf("******************************************\nreply :\n%s\n************************************\n", reply->reply_buffer);

	//printf("\n message is received and printed\n");

	close(socket_fd);

    return 0;
}

void write_data(const char *path, const char * data, int len) {
    /*
     * To be completed:
     *   Use fopen, fwrite and fclose functions.
     */
	FILE * file = fopen(path, "w");
	fwrite(data, 1, len, file);
	fclose(file);
}

char* http_get_request(url_info *info) {
    char * request_buffer = (char *) malloc(100 + strlen(info->path) + strlen(info->hostname));
    snprintf(request_buffer, 1024, "GET /%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
	    info->path, info->hostname);
    return request_buffer;
}

char *next_line(char *buff, int len) {
    if (len == 0) {
	return NULL;
    }

    char *last = buff + len - 1;
    while (buff != last) {
	if (*buff == '\r' && *(buff+1) == '\n') {
	    return buff;
	}
	buff++;
    }
    return NULL;
}

char *read_http_reply(struct http_reply *reply) {
	if(!counter_http_redirection){
		fprintf(stderr, "Too many redirections\n");
		return NULL;
	}
    // Let's first isolate the first line of the reply
    char *status_line = next_line(reply->reply_buffer, reply->reply_buffer_length);
    if (status_line == NULL) {
	fprintf(stderr, "Could not find status\n");
	return NULL;
    }
    *status_line = '\0'; // Make the first line is a null-terminated string

    // Now let's read the status (parsing the first line)
    int status;
    double http_version;
    int rv = sscanf(reply->reply_buffer, "HTTP/%lf %d", &http_version, &status);
    if (rv != 2) {
	fprintf(stderr, "Could not parse http response first line (rv=%d, %s)\n", rv, reply->reply_buffer);
	return NULL;
    }

    if (status != 200 && status != 301) {
	fprintf(stderr, "Server returned status %d (should be 200)\n", status);
	return NULL;
    }

	//HTTP REDIRECT :
	status_line++; //remove first '\0' caracter
	
	if(status == 301){
		//printf("test : %s\n", status_line);
		counter_http_redirection--;
		//parse url of next line
		char *url_start = strstr(status_line, "Location:");
		if(url_start == NULL){
			fprintf(stderr, "Failed to seek redirection url 1\n");
			return NULL;
		}
	
		url_start += 10; //put url_start after "Location:" string

		char *url_end = strchr(url_start, '\r');
		if(url_end == NULL){
			fprintf(stderr, "Failed to seek redirection url 2\n");
			return NULL;
		}
		
		*url_end = '\0';
		
		url_info info;
		int ret = parse_url(url_start, &info);
		if (ret) {
			fprintf(stderr, "Could not parse URL '%s': %s\n", url_start, parse_url_errstr[ret]);
			return NULL;
		}
		
		download_page(&info, reply);
		return read_http_reply(reply); //recurse until url found or 256 redirections or error
	}

    /*
     * To be completed:
     *   The previous code only detects and parses the first line of the reply.
     *   But servers typically send additional header lines:
     *     Date: Mon, 05 Aug 2019 12:54:36 GMT<CR><LF>
     *     Content-type: text/css<CR><LF>
     *     Content-Length: 684<CR><LF>
     *     Last-Modified: Mon, 03 Jun 2019 22:46:31 GMT<CR><LF>
     *     <CR><LF>
     *
     *   Keep calling next_line until you read an empty line, and return only what remains (without the empty line).
	 *
     *   Difficult challenge:
     *     If you feel like having a real challenge, go on and implement HTTP redirect support for your client.
     *
     */

	while(status_line != NULL && (*status_line != '\r' || *(status_line+1) != '\n')){
		status_line = next_line(status_line, strlen(status_line));
		status_line += 2;
	}

	if(status_line == NULL){
		fprintf(stderr, "Failed to seek new safe line\n");
		return NULL;
	}


    char *buf = status_line + 2;

    return buf;
}
