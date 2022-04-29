#include "dhcp.h"

int main(){
	dhcp_header *offer = (dhcp_header*) malloc(sizeof(dhcp_header));

	fill_offer(offer);

	for(int i = 0; i<1000000000; i++){
		randomize_offer(offer);
		send_packet(offer);
	}

	free(offer);
	return 0;
}
