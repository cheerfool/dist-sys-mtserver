#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "tools.h"

int main(int argc, char *argv[]) {

  if (argc < 3 || argc > 4) // Test for correct number of arguments
    DieWithUserMessage("Parameter(s)",
        "<Server Address/Name> <Echo Word> [<Server Port/Service>]");

  char *server = argv[1];     // First arg: server address/name
  char *echoString = argv[2]; // Second arg: string to echo
  // Third arg (optional): server port/service
  char *service = (argc == 4) ? argv[3] : "echo";

  // Create a connected TCP socket
  int sock = SetupTCPClientSocket(server, service);
  if (sock < 0)
    DieWithUserMessage("SetupTCPClientSocket() failed", "unable to connect");

  size_t echoStringLen = strlen(echoString); // Determine input length

/*int count;
char nstr[10];
for(count=10;count>0;count--){
	sprintf(nstr,"%d ",count);
	fputs(nstr,stdout);
	sleep(1);
}
fputs("0\n",stdout);
*/

  // Send the string to the server
  int numBytes = send(sock, echoString, echoStringLen, 0);
  if (numBytes < 0)
    DieWithSystemMessage("send() failed");
  else if (numBytes != echoStringLen)
    DieWithUserMessage("send()", "sent unexpected number of bytes");

  // Receive the same string back from the server
  unsigned int totalBytesRcvd = 0; // Count of total bytes received
  fputs("Received: ", stdout);     // Setup to print the echoed string
  //while ( numBytes >0 ) {
    char buffer[100]; // I/O buffer
    // Receive up to the buffer size (minus 1 to leave space for
    // a null terminator) bytes from the sender
    numBytes = recv(sock, buffer, 100, 0);
    if (numBytes < 0)
      DieWithSystemMessage("recv() failed");
    else if(numBytes==0)
	DieWithUserMessage("recv()", "connection closed prematurely");
    
    //if (numBytes!=4)
//	break;
    uint32_t netInt = *(int *)buffer;
    uint32_t hostInt = ntohl(netInt);
    int finalInt = (int) hostInt;
    printf("Int: %d. \n",finalInt);   
    
/*   if(numBytes>4){
    netInt = *((int *)buffer+1);
    hostInt = ntohl(netInt);
    finalInt = (int) hostInt;
    printf("Int: %d. ",finalInt);   
    }*/
//}
 //   printf("%s ",buffer);
//  }

 // fputc('\n', stdout); // Print a final linefeed


 /* char echoString2[]="hello again from me.";
  int echoLen2=strlen(echoString2); 
  numBytes = send(sock, echoString2, echoLen2, 0);
  if (numBytes < 0)
    DieWithSystemMessage("send() failed");
  else if (numBytes != echoLen2)
    DieWithUserMessage("send()", "sent unexpected number of bytes");

  // Receive the same string back from the server
 // totalBytesRcvd = 0; // Count of total bytes received
  fputs("Received: ", stdout);     // Setup to print the echoed string
 // while (numBytes>0) {
    char buffer2[200]; // I/O buffer
    // Receive up to the buffer size (minus 1 to leave space for
    // a null terminator) bytes from the sender
    numBytes = recv(sock, buffer2,199, 0);
    if (numBytes < 0)
      DieWithSystemMessage("recv() failed");
    else if (0 == numBytes)
	DieWithSystemMessage("recv() closed");
    buffer2[numBytes] = '\0';    // Terminate the string!
    fputs(buffer2, stdout);      // Print the buffer
//  }

  fputc('\n', stdout); // Print a final linefeed
*/
  close(sock);
  exit(0);
}
