#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "tools.h"

pthread_mutex_t mutex;
static int currentConnect = 0;
static int maxConnect;

// Structure of arguments to pass to client thread
struct ThreadArgs {
  int clntSock; // Socket descriptor for client
};

void *ThreadMain(void *arg); // Main program of a thread
void NewServerThread(int clntSock);

int main(int argc, char *argv[]) {
  pthread_mutex_init(&mutex, NULL);

  if (argc != 3) // Test for correct number of arguments
    DieWithUserMessage("Parameters", "<Max number of clients> <Server Port>");

  maxConnect= atoi(argv[1]);
  if (maxConnect<1)
    DieWithUserMessage("Parameters failed", "<Max number of clients> must be a positive integer.\nParameters : <Max number of clients> <Server Port>");
  char *servPort = argv[2]; // First arg:  local port
  int servSock = SetupTCPServerSocket(servPort);
  if (servSock < 0)
    DieWithUserMessage("SetupTCPServerSocket() failed", "unable to establish");

  for (;;) { // Run forever
    int clntSock = AcceptTCPConnection(servSock);
    if (currentConnect >= maxConnect){
	TerminateTCPClient(clntSock);
	printf("Number of concurrent connections reaches the upper limit %d, terminate this new client\n", maxConnect);
    }else{
	NewServerThread(clntSock);
    }
  }
  // NOT REACHED
  pthread_mutex_destroy(&mutex);
}

void NewServerThread(int clntSock){
	pthread_mutex_lock(&mutex);
	currentConnect++;
	pthread_mutex_unlock(&mutex);

	// Create separate memory for client argument
	struct ThreadArgs *threadArgs = (struct ThreadArgs *) malloc(
	sizeof(struct ThreadArgs));
	if (threadArgs == NULL)
	    DieWithSystemMessage("malloc() failed");
	threadArgs->clntSock = clntSock;

	// Create client thread
	pthread_t threadID;
	int returnValue = pthread_create(&threadID, NULL, ThreadMain, threadArgs);
	if (returnValue != 0)
	    DieWithUserMessage("pthread_create() failed", strerror(returnValue));
	printf("with thread %ld. Current connections: %d\n", (long int) threadID, currentConnect);
}

void *ThreadMain(void *threadArgs) {
  // Guarantees that thread resources are deallocated upon return
  pthread_detach(pthread_self());

  // Extract socket file descriptor from argument
  int clntSock = ((struct ThreadArgs *) threadArgs)->clntSock;
  free(threadArgs); // Deallocate memory for argument

  HandleTCPClient(clntSock, &currentConnect);

  pthread_mutex_lock(&mutex);
  currentConnect--;
  pthread_mutex_unlock(&mutex);
  return (NULL);
}
