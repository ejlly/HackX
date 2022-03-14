#ifndef MY_OWN_URL
#define MY_OWN_URL

typedef struct url_info{
	char* protocol;
	char* hostname;
	int port;
	char* path;
}url_info;

void print_url_info(url_info *my_url);

int parse_url(char* url_string, url_info *my_url);

// parse_url error codes
#define PARSE_URL_OK 0
#define PARSE_URL_NO_SLASH 1
#define PARSE_URL_INVALID_PORT 2
#define PARSE_URL_PROTOCOL_UNKNOWN 3

// parse_url associated error strings
static const char *parse_url_errstr[] = { "no error" , "no trailing slash", "invalid port", "unknown protocol"};

#endif
