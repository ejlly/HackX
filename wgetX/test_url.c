#include "url.h"
#include <stdio.h>

int main(int argc, char *argv[]){
	if(argc == 1){
		printf("No argument given\n");
		return 1;
	}

	url_info parsed_url = {NULL, NULL, 0, NULL};

	int result = parse_url(argv[1], &parsed_url);
	
	print_url_info(&parsed_url);

	if(result)
		printf("The URL is not valid, error : %s\n", parse_url_errstr[result]);
	else
		printf("The URL is valid\n");
	

	return 0;
}
