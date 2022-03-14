CC = clang

OBJ1 = \
		url.o \
		test_url.o 
OBJ2 = \
		url.o \
		wgetX.o

REBUILDABLES = $(OBJS) wgetX test_url

all : wgetX

clean:
	rm -f $(REBUILDABLES)

wgetX: $(OBJ2)
	$(CC) -g -o $@ $^

test_url: $(OBJ1)
	$(CC) -g -o $@ $^

url.o : url.c
	$(CC) -g -Wall -o $@ -c $<

test_url.o : test_url.c
	$(CC) -g -Wall -o $@ -c $<

wgetX.o : wgetX.c
	$(CC) -g -Wall -o $@ -c $<
