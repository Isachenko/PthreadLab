#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_THREAD_COUNT 5
#define SOCKET_PORT 2028
#define LENGTH 512

int curThreadNum = 0;
pthread_t threads[MAX_THREAD_COUNT];

void *PrintHello(void *threadid) 
{
   long tid;
   tid = (long)threadid;
   printf("Hello World! It's me, thread #%ld!\n", tid);
   pthread_exit(NULL);
}

void createNewThread()
{
    int rc;
    long t = curThreadNum++;
    printf("creating thread %ld\n", t);
    rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
    if (rc) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }
}

void *waitForClient(void* params)
{
  int newsockfd = (int)params;
  char buffer[256]; 
  printf("new client thread created\n");  
  memset(buffer, 0, sizeof(buffer));
  int n = read(newsockfd, buffer, 255);
  if (n > 0) {
    char* fs_name = buffer;
    char sdbuf[LENGTH]; // Send buffer
    printf("[Server] Sending %s to the Client...", fs_name);
    FILE *fs = fopen(fs_name, "r");
    if(fs == NULL) {
      fprintf(stderr, "ERROR: File %s not found on server. (errno = %d)\n", fs_name, errno);
      exit(1);
    }
    bzero(sdbuf, LENGTH); 
    int fs_block_sz; 
    while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs))>0) {
      if(write(newsockfd, sdbuf, fs_block_sz) < 0) {
          fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fs_name, errno);
          exit(1);
      }
      bzero(sdbuf, LENGTH);
    }
    int success = 1;
    char *endbuf = "this is the end";
    write(newsockfd, endbuf, sizeof(endbuf));
    printf("[Server] Connection with Client closed. Server will wait now...\n");

    #ifdef PTHREAD
    while(waitpid(-1, NULL, WNOHANG) > 0);
    #endif
  } 
  printf("thread is closing\n");
  close(newsockfd);
  printf("thread is closed\n");
}

int startServer(){
  int sockfd, portno;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    error("ERROR opening socket");
  }
  memset((char*) &serv_addr, 0, sizeof(serv_addr));
  portno = 2028;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *) &serv_addr,
          sizeof (serv_addr)) < 0)
      error("ERROR on binding");
  listen(sockfd, 5);
  clilen = sizeof (cli_addr);
  while(1){
    int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
      error("ERROR on accept");
    pthread_t clientThread;
    int rc;
    #ifdef PTHREAD
    rc = pthread_create(&clientThread, NULL, waitForClient, (void*)newsockfd)
    if (rc){
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
    #else
    pid_t childPid = fork();
    if (childPid == 0){
      waitForClient((void*)newsockfd);
      exit(0);
    }
    #endif
  }
  close(sockfd);
  return 0;
}

int main (int argc, char *argv[])
{
  startServer();
  /* Last thing that main() should do */

  #ifdef PTHREAD
  pthread_exit(NULL);
  #endif
}

