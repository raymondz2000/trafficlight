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

//what you need to send here is only the direction and speed, there will create the car in the server side
// A vehicle starts be specifying (as command-line arguments) its incoming direction 
// (i.e., NORTHBOUND=0, SOUTHBOUND=1, EASTBOUND =2, WESTBOUND=3) as well as its speed 
// (i.e., 1 to 100% of its top speed).
 
int main(int argc, char * argv[]) {

	unsigned short updateresponse[3];
	unsigned short bufferupdate[3];

	unsigned short bufferresponse[4]; 
	unsigned short buffer[3];


	unsigned short  speed=0;
	unsigned short  direction=0;

	int status, bytesRcv,clientSocket;
	struct sockaddr_in serverAddress;
 	 // send for connect
 	
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
 	
 	 // WRITE CODE HERE TOO
	
	//direction is from 0 -3
	//speed from 0 to 100 ( 0-8 in pixel);
    //convert the speed
	if((atoi(argv[1])<=3 && atoi(argv[1]) >= 0) && (atoi(argv[2])<=100 && (atoi(argv[2])>=0))){
	 direction = atoi(argv[1]); //0-3
	 speed = atoi(argv[2]);
	
	}
	else{
	printf("please enter correct direction or speed\n");
	exit(-1);
	}
  	speed *= (0.08);
	 //0-100  0-8 in pixel   0.08 =1kmh 4 =50kmh
	 
  	// Go into an infinite loop to keep sending/getting updates from traffic monitor
 // Go into loop to commuincate with server now
	
 //SENDING
 		buffer[0] = CONNECT;
 		buffer[1] = direction;
 		buffer[2] = speed;
 		
 		usleep(50000);  // A delay to slow things down a little
 		
 		send(clientSocket,buffer, sizeof(buffer),  0);
 		 
 	    //	for(int i =0; i< sizeof(buffer); i++){
 	    	//printf("buffer [%d] have %u\n",i,buffer[i]);
 	    //	}
 	    	
         // Get response from server, should be "OK"
 		bytesRcv = recv(clientSocket, bufferresponse, sizeof(bufferresponse), 0);
 		//buffer[bytesRcv] = 0; // put a 0 at the end so we can display the string
		//printf("CLIENT: Got back response Dir %d, speed %d, ID %d from server code %d.\n", bufferresponse[0], bufferresponse[1], bufferresponse[2],bufferresponse[3]);
		
		 //if return NO exit
		if(bufferresponse[0]==NO){
		   close(clientSocket); // Don't forget to close the socket !
 		   printf("CLIENT: Add fail ,Shutting down vehicle.\n");
 		   exit(-1);
		}
		 
	 
 
 	close(clientSocket); // Don't forget to close the socket !
 	printf("CLIENT: Shutting down.\n");
 	
 	
	while (1) {
	
//speed id and direction
 	
	int status, bytesRcv,clientSocket;
 
	struct sockaddr_in serverAddress;
	
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
 	
	//send for update 
	bufferupdate[0]= UPDATE;
	bufferupdate[1]= direction;
	//sending the ID
	bufferupdate[2]= bufferresponse[1];
	
	send(clientSocket,bufferupdate, sizeof(bufferupdate),  0);

 	bytesRcv = recv(clientSocket, updateresponse, sizeof(updateresponse), 0);
 	
  
	 if (updateresponse[0] == NO){
 	 	break;		
 	 }
	usleep(50000);
 	 }
 	close(clientSocket); // Don't forget to close the socket !
 	printf("CLIENT: Update Shutting down.\n");
 
}
 
