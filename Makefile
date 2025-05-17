EXEC = nodebridge           # target executable
CC = gcc                    # compiler 
CFLAGS = -c -Wall -pedantic -O0 -g -Iinclude
LDFLAGS = -lcjson  #-lusb-1.0 -lmicrohttpd -lpthread

OBJECTS = $(patsubst %.c, %.o, $(wildcard src/*.c))

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

clear: clean
	rm -f $(EXEC)

clean:
	rm -f *.o src/*.o

.PHONY: clear clean
