#include <stdio.h>
#include "international-hello-world.h"

int main(){
	print_hello_international();
	return 0;
}

void print_hello_international(){
	#ifdef FRENCH
	printf("Bonjour le monde!\n");
	#endif

	#ifdef ENGLISH
	printf("Hello World!\n");
	#endif

	#ifdef GERMAN
	printf("Guten Tag Weld!\n");
	#endif
}
