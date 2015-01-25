#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "Practical.h"

static int currentConnect = 0;
void *ThreadMain(void *arg); // Main program of a thread

// Structure of arguments to pass to client thread
struct ThreadArgs {
  int clntSock; // Socket descriptor for client
  int *curConnect;
};

int main(int argc, char *argv[]) {

  if (argc != 3) // Test for correct number of arguments
    DieWithUserMessage("Parameters", "<Max number of clients> <Server Port>");

  const int maxConnect= atoi(argv[1]);
  if (maxConnect<1)
    DieWithUserMessage("Parameters failed", "<Max number of clients> must be a positive integer.\nParameters : <Max number of clients> <Server Port>");
  char *servPort = argv[2]; // First arg:  local port
  int servSock = SetupTCPServerSocket(servPort);
  if (servSock < 0)
    DieWithUserMessage("SetupTCPServerSocket() failed", "unable to establish");

//  const int MSGSIZE = 200;
  for (;;) { // Run forever
    int clntSock = AcceptTCPConnection(servSock);
    if (currentConnect >= maxConnect){
	TerminateTCPClient(clntSock);
	printf("Number of concurrent connections reaches the upper limit %d, terminate this new client\n", maxConnect);
    }else{
	currentConnect++;
	    // Create separate memory for client argument
	
	struct ThreadArgs *threadArgs = (struct ThreadArgs *) malloc(
	sizeof(struct ThreadArgs));
	if (threadArgs == NULL)
	    DieWithSystemMessage("malloc() failed");
	threadArgs->clntSock = clntSock;
	threadArgs->curConnect = &currentConnect;

	// Create client thread
	pthread_t threadID;
	int returnValue = pthread_create(&threadID, NULL, ThreadMain, threadArgs);
	if (returnValue != 0)
	    DieWithUserMessage("pthread_create() failed", strerror(returnValue));
	printf("with thread %ld. Current connections: %d\n", (long int) threadID, currentConnect);
    }
  }
  // NOT REACHED
}

void *ThreadMain(void *threadArgs) {
  // Guarantees that thread resources are deallocated upon return
  pthread_detach(pthread_self());

  // Extract socket file descriptor from argument
  int clntSock = ((struct ThreadArgs *) threadArgs)->clntSock;
  int *curConnect = ((struct ThreadArgs *) threadArgs)->curConnect;
  free(threadArgs); // Deallocate memory for argument

  HandleTCPClient(clntSock);
  (*curConnect)--;
  return (NULL);
}
