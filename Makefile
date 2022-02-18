CC := clang
LANGUAGE := GERMAN

all: hello_world fancy-hello-world international-hello-world

hello_world: hello_world.o
	$(CC) -o hello_world hello_world.o

hello_world.o: hello_world.c
	$(CC) -c -Wall -Werror hello_world.c

fancy-hello-world: fancy-hello-world.o
	$(CC) -o fancy-hello-world fancy-hello-world.o

fancy-hello-world.o: fancy-hello-world.c
	$(CC) -c -Wall -Werror fancy-hello-world.c

international-hello-world: international-hello-world.o
	$(CC) -o international-hello-world international-hello-world.o

international-hello-world.o : international-hello-world.c
	$(CC) -c -Wall -Werror international-hello-world.c -D$(LANGUAGE)

clean_international:
	rm international-hello-world.o international-hello-world

clean:
	rm hello_world.o hello_world international-hello-world.o international-hello-world fancy-hello-world.o fancy-hello-world

#in case we change the language in the makefile
recompile: clean_international international-hello-world
