#include "dhcp.h"

int main(){
	dhcp_header *offer = (dhcp_header*) malloc(sizeof(dhcp_header));
	
	int sockfd = open_socket();

	fill_offer(offer);

	for(int i = 0; i<10000000; i++){
		randomize_offer(offer);
		send_packet(sockfd, offer);
	}

	free(offer);

	close(sockfd);
	return 0;
}
