#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "tools.h"

 
void *ThreadMain(void *arg); // Main program of a thread
void NewServerThread(int clntSock, int *curConnect);

// Structure of arguments to pass to client thread
struct ThreadArgs {
  int clntSock; // Socket descriptor for client
  int *curConnect;
};

void NewServerThread(int clntSock, int *curConnect){
	(*curConnect)++;
	    // Create separate memory for client argument
	
	struct ThreadArgs *threadArgs = (struct ThreadArgs *) malloc(
	sizeof(struct ThreadArgs));
	if (threadArgs == NULL)
	    DieWithSystemMessage("malloc() failed");
	threadArgs->clntSock = clntSock;
	threadArgs->curConnect = curConnect;

	// Create client thread
	pthread_t threadID;
	int returnValue = pthread_create(&threadID, NULL, ThreadMain, threadArgs);
	if (returnValue != 0)
	    DieWithUserMessage("pthread_create() failed", strerror(returnValue));
	printf("with thread %ld. Current connections: %d\n", (long int) threadID, *curConnect);
}

void *ThreadMain(void *threadArgs) {
  // Guarantees that thread resources are deallocated upon return
  pthread_detach(pthread_self());

  // Extract socket file descriptor from argument
  int clntSock = ((struct ThreadArgs *) threadArgs)->clntSock;
  int *curConnect = ((struct ThreadArgs *) threadArgs)->curConnect;
  free(threadArgs); // Deallocate memory for argument

  HandleTCPClient(clntSock, curConnect);
  (*curConnect)--;
  return (NULL);
}
