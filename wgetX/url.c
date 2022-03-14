#include "url.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//strstr, strchr, sscanf, printf,

void print_url_info(url_info *my_url){
	printf("Protocol:\t%s\n", my_url->protocol);
	printf("Host name:\t%s\n", my_url->hostname);
	printf("Port No.:\t%d\n", my_url->port);
	printf("Path:\t\t/%s\n", my_url->path);
}

int parse_url(char* url_string, url_info *my_url){
	char* protocol_end = strstr(url_string, "://");
	if(protocol_end == NULL){
		my_url->protocol = "http"; //default
		protocol_end = url_string;
	}
	else{
		my_url->protocol = malloc(sizeof(char) * (protocol_end - url_string + 1));
		//printf("pointing values : %p, %p, %ld\n", url_string, protocol_end, protocol_end-url_string);
		strncpy(my_url->protocol, url_string, protocol_end - url_string);
		*(my_url->protocol + (protocol_end - url_string)) = '\0'; //safeguard mecanism

		protocol_end += 3;
	}

	if(strcmp(my_url->protocol, "http") != 0){
		//printf("PROTOCOL IS : %s\n", my_url->protocol);
		return PARSE_URL_PROTOCOL_UNKNOWN;
	}
	
	//protocol_end is now pointing at beginning of hostname
	
	char* find_slash = strchr(protocol_end, '/');
	char* find_twopoint = strchr(protocol_end, ':');
	
	if(find_slash == NULL){ //no slash
		return PARSE_URL_NO_SLASH;
	}
	else if(find_twopoint != NULL && find_twopoint < find_slash){ //there is a port number
		my_url->hostname = malloc(sizeof(char) * (find_twopoint - protocol_end + 1));
		strncpy(my_url->hostname, protocol_end, find_twopoint - protocol_end);

		*(my_url->hostname + (find_twopoint - protocol_end)) = '\0'; //safeguard mecanism

		my_url->port = (int) strtol(find_twopoint+1, &find_slash, 10);
	}
	else{
		my_url->hostname = malloc(sizeof(char) * (int)(find_slash - protocol_end + 1));
		strncpy(my_url->hostname, protocol_end, find_slash - protocol_end);

		*(my_url->hostname + (find_slash - protocol_end)) = '\0'; //safeguard mecanism

		my_url->port = 80; //default
	}
	
	if(my_url->port < 1 || my_url->port > 65535)
		return PARSE_URL_INVALID_PORT;

	my_url->path = malloc(strlen(find_slash+1) * sizeof(char));
	strcpy(my_url->path, find_slash+1);

	return PARSE_URL_OK;
}
