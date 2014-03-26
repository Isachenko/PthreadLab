#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define LENGTH 512

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char revbuf[LENGTH];

    char buffer[256];
    if (argc < 4) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr,
            (char *) &serv_addr.sin_addr.s_addr,
            server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) error("ERROR connecting");
    bzero(buffer, 256);

    // n = write(sockfd, buffer, strlen(buffer));
    n = send(sockfd, argv[3], strlen(argv[3]), 0);
    if (n < 0) error("ERROR writing to socket");
	char* fr_name = "result";
	FILE *fr = fopen(fr_name, "wb");
	if(fr == NULL) {
		printf("File %s Cannot be opened.\n", fr_name);
	}
	else {
		bzero(revbuf, LENGTH); 
		int fr_block_sz = 0;
	    while((fr_block_sz = recv(sockfd, revbuf, LENGTH, 0)) > 0) {
			int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
	        if(write_sz < fr_block_sz) {
	            error("File write failed.\n");
	        }
	        //printf("%s", revbuf);
			bzero(revbuf, LENGTH);
			if (fr_block_sz == 0 || fr_block_sz != 512) {
				break;
			}
		}
		if(fr_block_sz < 0) {
			if (errno == EAGAIN) {
				printf("recv() timed out.\n");
			} else {
				fprintf(stderr, "recv() failed due to errno = %d\n", errno);
			}
		}
	    fclose(fr);
	}
    bzero(buffer, 256);
    close(sockfd);

    return 0;
}