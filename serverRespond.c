#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <sys/sysinfo.h>
#include "tools.h"

void respond(int clntSocket, int code);

enum serverStatus{
	INIT = 0,
	UPTIME = 1,
	LOAD = 2,
	DIGIT = 3,
	EXIT = 4,
};

int HandleTCPClient(int clntSocket, int *curConnect) {
	enum serverStatus status = INIT;
	int invalidNum = 0;
	int uptimeLen = 0;
	int loadLen = 0;
	int digitSum = 0;
	int exitLen = 0;
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

		int i;
		for(i=0; i<bytesRec; i++){
			char ch= buffer[i];
			if(status==INIT){
				if(ch=='u'){
					status=UPTIME;
					uptimeLen=1;					
				}else if(ch=='l'){
					status=LOAD;
					loadLen=1;
				}else if(ch=='e'){
					status=EXIT;
					exitLen=1;
				}else if(ch>='0' && ch<='9'){
					status=DIGIT;
					digitSum+= ch-'0';
				}else{
					respond(clntSocket, -1);		
					invalidNum++;
					if(invalidNum>2){
						close(clntSocket);
						return 0;
					}
				}
			}else if(status==UPTIME){
				if(ch=='p' && uptimeLen==1){
					uptimeLen=2;
				}else if(ch=='t' && uptimeLen==2){
					uptimeLen=3;
				}else if(ch=='i' && uptimeLen==3){
					uptimeLen=4;
				}else if(ch=='m' && uptimeLen==4){
					uptimeLen=5;
				}else if(ch=='e' && uptimeLen==5){
					uptimeLen=0;
					status=INIT;
					invalidNum=0;
					struct sysinfo info;
					long sysTime = info.uptime;
					int systime = (int) sysTime; 
					respond(clntSocket, systime);
				}else{
					uptimeLen=0;
					status=INIT;
					respond(clntSocket, -1);		
					invalidNum++;
					if(invalidNum>2){
						close(clntSocket);
						return 0;
					}
					i--;
				}
			}else if(status==LOAD){
				if(ch=='o' && loadLen==1){
					loadLen=2;
				}else if(ch=='a' && loadLen==2){
					loadLen=3;
				}else if(ch=='d' && loadLen==3){
					loadLen=0;
					status=INIT;
					invalidNum=0;
					respond(clntSocket, *curConnect);
				}else{
					loadLen=0;
					status=INIT;
					respond(clntSocket, -1);		
					invalidNum++;
					if(invalidNum>2){
						close(clntSocket);
						return 0;
					}
					i--;
				}
			}else if(status==EXIT){
				if(ch=='x' && exitLen==1){
					exitLen=2;
				}else if(ch=='i' && exitLen==2){
					exitLen=3;
				}else if(ch=='t' && exitLen==3){
					exitLen=0;
					status=INIT;
					invalidNum=0;
					respond(clntSocket, 0);
					close(clntSocket);
					return 0;
				}else{
					exitLen=0;
					status=INIT;
					respond(clntSocket, -1);		
					invalidNum++;
					if(invalidNum>2){
						close(clntSocket);
						return 0;
					}
					i--;
				}
			}else if(status==DIGIT){
				if(ch>='0' && ch<='9'){
					digitSum+= ch-'0';
				}else if(ch==' '){
					status=INIT;
					invalidNum=0;
					respond(clntSocket, digitSum);		
					digitSum=0;
				}else{					
					digitSum=0;
					status=INIT;
					respond(clntSocket, -1);		
					invalidNum++;
					if(invalidNum>2){
						close(clntSocket);
						return 0;
					}
					i--;
				}
			}
		}
	}while( bytesRec>0);

	close(clntSocket); // Close client socket
	return 0;
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

