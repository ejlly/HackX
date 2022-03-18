CC = clang

FLAGS = -std=c99 

EXECS = client server fancyclient

OBJS = \
		client.o \
		server.o \
		fancyclient.o



REBUILDABLES = $(OBJS) $(EXECS)

all : $(EXECS)

clean:
	rm -f $(REBUILDABLES)

#inspired from gnu make documentation, pattern matches section
$(EXECS): %: %.o
	$(CC) -g -o $@ $^ $(FLAGS)

%.o : %.c
	$(CC) -g -Wall -o $@ -c $< $(FLAGS)

