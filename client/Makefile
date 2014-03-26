CC := $(CROSS_COMPILE)gcc
LIBS =

client: client.c
	$(CC) -o client client.c -I. $(LIBS)

clean:
	rm -f client client.o