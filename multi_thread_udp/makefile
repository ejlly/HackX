CC = clang

FLAGS = -lpthread

EXECS = multi_client multi_server mutex

OBJS = \
		multi_client.o \
		multi_client.o \
		mutex.o


REBUILDABLES = $(OBJS) $(EXECS)

all : $(EXECS)

clean:
	rm -f $(REBUILDABLES)

#inspired from gnu make documentation, pattern matches section
$(EXECS): %: %.o
	$(CC) -g3 -o $@ $^ $(FLAGS)

%.o : %.c
	$(CC) -g3 -Wall -o $@ -c $< $(FLAGS)

