#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "trafficLight.h"
#include "vehicle.h"
#include "simulator.h"

 

void main() {
 	int clientSocket;
 	struct sockaddr_in serverAddress;
 	int status, bytesRcv;
 	 
 	unsigned char buffer[3]; // stores user input from keyboard
 // Create the client socket
 
 	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
 	 
 	if (clientSocket < 0) {
 		printf("*** CLIENT ERROR: Could not open socket.\n");
 		exit(-1);
 	}
// Setup address
 	memset(&serverAddress, 0, sizeof(serverAddress));
 	serverAddress.sin_family = AF_INET;
 	serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
 	serverAddress.sin_port = htons((unsigned short) SERVER_PORT);
 // Connect to server
 	status = connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
 	if (status < 0) {
 		printf("*** CLIENT ERROR: Could not connect.\n");
 		exit(-1);
 	}
 // Go into loop to commuincate with server now
	 while (1) {
 
 		
 		buffer[0]=SHUTDOWN;
 		 
 		send(clientSocket, buffer, strlen(buffer), 0);
 // Get response from server, should be "OK"
 		bytesRcv = recv(clientSocket, buffer, 80, 0);
 		buffer[bytesRcv] = 0; // put a 0 at the end so we can display the string
		printf("CLIENT: Got back response \"%s\" from server.\n", buffer);
		 
 			break;
 			 
 		}
 	close(clientSocket); // Don't forget to close the socket !
 	printf("CLIENT: Shutting down.\n");
 	exit(-1);

}
