#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tools.h"

static int currentConnect = 0;
static int maxConnect;

int main(int argc, char *argv[]) {

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
	NewServerThread(clntSock, &currentConnect);
    }
  }
  // NOT REACHED
}
