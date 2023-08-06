CC=gcc
CFLAGS=-Wall -Wextra -pedantic -g -std=gnu99 -O3 
SANITIZERS=-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer
LIBS= -lmicrohttpd -lpthread

DEPMAIN=src/main.o src/telescope.o src/AM5.o src/alpaca.o src/alpacaDiscovery.o

all: clean main

main: $(DEPMAIN)
	$(CC) -o $@ $^ $(CFLAGS) $(SANITIZERS) $(LIBS)

%.o : %.c
	$(CC) $(CFLAGS) $(SANITIZERS) $(LIBS) -c $^ -o $@

SUBDIR_ROOTS := src
DIRS := . $(shell find $(SUBDIR_ROOTS) -type d)
GARBAGE_PATTERNS := *.o
GARBAGE := $(foreach DIR,$(DIRS),$(addprefix $(DIR)/,$(GARBAGE_PATTERNS)))

clean:
	rm -rf $(GARBAGE)
	rm -f main
