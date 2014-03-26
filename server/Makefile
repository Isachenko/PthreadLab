CC := $(CROSS_COMPILE)gcc
LIBS = -lpthread

server: server.c
	$(CC) -o server server.c -I. $(LIBS)

clean:
	rm -f server server.o