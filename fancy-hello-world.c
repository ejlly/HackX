#include <stdio.h>
#include "fancy-hello-world.h"

int main(){
	char name[10];
	char output[] = "Hello World, hello ";

	scanf("%s", name);
	hello_string(name, output);
}

void hello_string(char* name, char* output){
	printf("%s%s", output, name);
}
