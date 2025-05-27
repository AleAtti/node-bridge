EXEC = nodebridge           # target executable
CC = gcc                    # compiler 
CFLAGS = -c -Wall -pedantic -O0 -g -Isrc/header
LDFLAGS = -lcjson -lmicrohttpd -lpthread -lhidapi-hidraw #-lusb-1.0

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
	find src -name '*.o' -delete

.PHONY: clear clean
