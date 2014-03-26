#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>

#define MAX_THREAD_COUNT 5
#define SOCKET_PORT 2028

int curThreadNum = 0;
pthread_t threads[MAX_THREAD_COUNT];

void *PrintHello(void *threadid)
{
   long tid;
   tid = (long)threadid;
   printf("Hello World! It's me, thread #%ld!\n", tid);
   pthread_exit(NULL);
}

void createNewThread(){
    int rc;
    long t = curThreadNum++;
    printf("creating thread %ld\n", t);
    rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
    if (rc){
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }
}

void *listThread(void* param){
    printf("I'm listening\n");
    listen(SOCKET_PORT, MAX_THREAD_COUNT);
}

int main (int argc, char *argv[])
{
   pthread_t listenThread;
   int rc;
   rc = pthread_create(&listenThread, NULL, listThread, NULL);
   if (rc){
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);
   }


   /* Last thing that main() should do */
   pthread_exit(NULL);
}

