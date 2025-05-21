EXEC = nodebridge           # target executable
CC = gcc                    # compiler 
CFLAGS = -c -Wall -pedantic -O0 -g -Isrc/header
LDFLAGS = -lcjson -lmicrohttpd #-lusb-1.0 -lpthread

SRC := $(shell find src -name '*.c')
OBJECTS := $(patsubst %.c, %.o, $(SRC))

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
