#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include "tools.h"


void respond(int clntSocket, int code);

enum serverStatus{
	INIT = 0,
	UPTIME = 1,
	LOAD = 2,
	DIGIT = 3,
	EXIT = 4,
};

void HandleTCPClient(int clntSocket) {
	char buffer[BUFSIZE]; // Buffer for echo string

	// Receive message from client
	int bytesRec;
	do{
		bytesRec = recv(clntSocket, buffer, BUFSIZE, 0);
		if (bytesRec < 0){
			if(errno==EAGAIN)
				break;
			else
				DieWithSystemMessage("recv() failed");
		}
		if (bytesRec == 0)
			break;
		
		respond(clntSocket, 20);		
/*		int i;
		for(i=0; i<bytesRec; i++){
			char ch= buffer[i];
				
*/		

	}while( bytesRec>0);

	close(clntSocket); // Close client socket
}

void TerminateTCPClient(int clntSocket){
	char serverMsg[] = "Sorry, the number of connections reaches the upper limit, please try later.\n";
	int bytesSent = send(clntSocket, serverMsg, strlen(serverMsg), 0); 
	if (bytesSent<0)
		DieWithSystemMessage("send() failed");
	close(clntSocket);
}

void respond(int clntSocket, int code){
	// Echo message back to client
	uint32_t hostInt = code;
	uint32_t netInt = htonl(hostInt);
	int bytesToSend = sizeof(uint32_t);

	int bytesSent = send(clntSocket, &netInt, bytesToSend , 0);
	if (bytesSent < 0)
		DieWithSystemMessage("send() failed");
	else if (bytesSent != bytesToSend)
		DieWithUserMessage("send()", "sent unexpected number of bytes");
}

