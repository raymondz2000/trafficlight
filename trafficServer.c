#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 

// Handle client requests coming in through the server socket.  This code should run
// indefinitiely.  It should wait for a client to send a request, process it, and then
// close the client connection and wait for another client.  The requests that may be
// handles are SHUTDOWN, CONNECT and UPDATE.  
//unsigned short changespeed(unsigned short cur_light, unsigned short dis_light, unsigned short dis_car,unsigned short aheadcar_speed);
//101158903 RAYMOND ZHU

//THIS program is not perfect works for me and im still thinking is problem on my sending and receving, sometimes will get hit the car.
unsigned short changespeed(unsigned short cur_light, unsigned short dis_light, unsigned short dis_car,unsigned short aheadcar_speed,unsigned short cur_carspeed){
    unsigned short updatedspeed; 

 	if(dis_car == MAX_DISTANCE){
 	   if((cur_light != GREEN) && (dis_light != MAX_DISTANCE)){
 	   	if (dis_light <= (2*(VEHICLE_WIDTH + cur_carspeed))){
 	   		updatedspeed = 0;
            return updatedspeed;
 	   	}
 	   	else if (dis_light <= (3 * VEHICLE_WIDTH * cur_carspeed)){
				//printf("slow speed\n");
 	   		if(cur_carspeed > 1){
 	   	     cur_carspeed= cur_carspeed / 1.5;
             updatedspeed = cur_carspeed;
 	   	   	 return updatedspeed;
 	   	   	}
 	   	}
 	   }else{
 	   		if(cur_carspeed < VEHICLE_TOP_SPEED){
 		 	//printf("speeding\n");
 			cur_carspeed ++;
 			updatedspeed = cur_carspeed;
 			return updatedspeed;
 			}
 	   	}
 	}else {
 		if((cur_light != GREEN) && (dis_light != MAX_DISTANCE)){
 			if(dis_light <= (2*(VEHICLE_WIDTH + cur_carspeed))){
 				updatedspeed = 0;
                return updatedspeed;
 	   			 
 			}
 			else if (dis_light <= (3*VEHICLE_WIDTH * cur_carspeed)){
 				if(cur_carspeed > 1){
 	   	        cur_carspeed = cur_carspeed / 1.5;
                updatedspeed = cur_carspeed;
 	   	   	    return updatedspeed;
 	   	   	}
 			}
 		}if (aheadcar_speed < cur_carspeed){
 			if(dis_car <= (4 *(VEHICLE_WIDTH + cur_carspeed))){
 				updatedspeed = 0;
                return updatedspeed;
 	   			 
 			}else if(dis_light <= (3*VEHICLE_WIDTH * cur_carspeed)){
 				if(cur_carspeed> 1){
 				    cur_carspeed --;
 				    updatedspeed = cur_carspeed;
 	   			    return updatedspeed;
 				}
 			}
 		}else if(((aheadcar_speed  > cur_carspeed)&&(dis_light == MAX_DISTANCE))||(cur_light == GREEN)){
 			if(cur_carspeed < aheadcar_speed){
 				cur_carspeed++;
 				updatedspeed = cur_carspeed;
 	   			return updatedspeed;
 			}
 		}
 	 
 	}
}
void *handleIncomingRequests(void *inter) {

	int cmd;
	 Intersection     *interMonitor = inter;
	 
	//set up TCP server
	int serverSocket, clientSocket;
 	struct sockaddr_in serverAddress, clientAddr;
	int status, addrSize, bytesRcv;

	unsigned short buffersend[8];
    unsigned short bufferget[8];

 	char *response = "YES";
 	 
  
    	  
 // Create the server socket
 	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	 if (serverSocket < 0) {
		 printf("*** SERVER ERROR: Could not open socket.\n");
 		exit(-1);
 	}
 	
 // Setup the server address
	memset(&serverAddress, 0, sizeof(serverAddress)); // zeros the struct
 	serverAddress.sin_family = AF_INET;
 	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
 	serverAddress.sin_port = htons((unsigned short) SERVER_PORT);
 // Bind the server socket
 	status = bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
 	
 	if (status < 0) {
 		printf("*** SERVER ERROR: Could not bind socket.\n");
 		exit(-1);
 	}
 	printf("SERVER:before monitor online status is %d \n",interMonitor->monitor.online);
 	//make monitor online when bind successful
 	interMonitor->monitor.online = 1;
 	printf("SERVER:monitor online status is %d \n",interMonitor->monitor.online);
 	
 	 // Set up the line-up to handle up to 5 clients in line 
	status = listen(serverSocket, MAX_CONNECTIONS  );
	 if (status < 0) {
 		printf("*** SERVER ERROR: Could not listen on socket.\n");
 		exit(-1);
	 }
	  
/* ------------------------- // Wait for clients now ------------------------ */

	while(1){
  		addrSize = sizeof(clientAddr);
 		clientSocket = accept(serverSocket,(struct sockaddr *)&clientAddr,&addrSize);
  		if (clientSocket < 0) {
 			printf("*** SERVER ERROR: Could accept incoming client connection.\n");
 			exit(-1);
 		}
 		printf("SERVER: Received client connection.\n");
  		 
 		bytesRcv = recv(clientSocket, bufferget, sizeof(bufferget), 0);

 		printf("SERVER:receive byte%d\n",bytesRcv);
 		 
 		
 		
 		//   for (int i =0; i <8;i++){
 		//   printf("SERVER: Received client request: %d\n",  bufferget[i]);
 		//   }
 		cmd =  bufferget[0];
 		
    		switch(cmd) {

/* ---------- // Someone has requested that we shut down, so do it. --------- */

    			case SHUTDOWN:  send(clientSocket, response, strlen(response), 0);
    				printf("SERVER: stop program\n");
    				interMonitor->monitor.online = 0;
    				printf("SERVER:run stop commend, monitor online status is %d \n",interMonitor->monitor.online);
    				printf("SERVER: Closing client connection.\n");
 					close(clientSocket); // Close this client's socket
 		 			break;
 					
/* ---------- // A vehicle wants to connect to the traffic monitor ---------- */

    			case CONNECT:  printf("SERVER:Receive Car register connect request\n");
			        sem_wait(&sem);
                    unsigned short dir = bufferget[1];
                    unsigned short speed = bufferget[2];

    			 
    				printf("SERVER:Receive direction  is %d\n",dir);
    				printf("SERVER:receive speed  is %d\n",speed);
    				printf("SERVER:lineup count is %d\n",interMonitor->monitor.traffic[dir].lineCount);
    					 
    				if(interMonitor->monitor.traffic[dir].lineCount < MAX_CONNECTIONS){
    				 
    					interMonitor->monitor.traffic[dir].lineup[interMonitor->monitor.traffic[dir].lineCount].x=interMonitor->monitor.traffic[dir].entryX;
    	  				interMonitor->monitor.traffic[dir].lineup[interMonitor->monitor.traffic[dir].lineCount].y=interMonitor->monitor.traffic[dir].entryY;
    	  				   
    	  				interMonitor->monitor.traffic[dir].lineup[interMonitor->monitor.traffic[dir].lineCount].speed = speed; 
    	  				     
    	 				interMonitor->monitor.traffic[dir].lineup[interMonitor->monitor.traffic[dir].lineCount].id = interMonitor->monitor.idCounter[dir];
    	 				      //for response
    	 				
    	 				if( interMonitor->monitor.idCounter[dir]==255){
    	 				    printf("SERVER: 255 true");
    	 				    interMonitor->monitor.idCounter[dir]=1;
    	 				    interMonitor->monitor.traffic[dir].lineCount--;
    	 				}
                        //return speed
                        buffersend[0] = YES;
                        buffersend[1] = interMonitor->monitor.idCounter[dir];
    	 				
                        interMonitor->monitor.idCounter[dir]++;
    	  				interMonitor->monitor.traffic[dir].lineCount++;
    	  				      
                           
    	  			 
    	  				      
    	  				send(clientSocket,buffersend, sizeof(buffersend), 0);
    	  				    
    	  				printf("SERVER:lineup count is %d\n",interMonitor->monitor.traffic[dir].lineCount);   
    					}
    					else{
    						buffersend[0]=NO;
    						printf("SERVER:Add Car Fail, Exceed max connection\n");
    						send(clientSocket, buffersend, sizeof(buffersend), 0);
    					}
    					break;
			
/* ----------------------- keep updating and send data ---------------------- */

    			case UPDATE:   printf("SERVER: Receive Car update request\n");
    			// A vehicle is sending new speed data 
    			
    			 
    				unsigned short updatedir;
    				unsigned short updateID;

    			    unsigned short aheadcar_speed;
                    unsigned short curcar_speed;
                    unsigned short dis_light;
                    unsigned short dis_car;
                    unsigned short cur_light;

    			 //receive speed send from vehicle.
    				updatedir = bufferget[1];
    				updateID = bufferget[2];
				    sem_wait(&sem);  
    				int found;
    				int flag;
    				printf("SERVER: updating ID %d car\n",updateID);

    				//linecount should be correct
    				printf("SERVER: In update server ,count is %d\n",interMonitor->monitor.traffic[updatedir].lineCount);
    				for(int m = 0; m < interMonitor->monitor.traffic[updatedir].lineCount; m++){
    					printf("SERVER: line count %d, for index %d update id is%d\n",interMonitor->monitor.traffic[updatedir].lineCount,m,interMonitor->monitor.traffic[updatedir].lineup[m].id);
    				 	if(interMonitor->monitor.traffic[updatedir].lineup[m].id == updateID){
    				 		flag = 1;
    				 		found = m;
    				 		break;
    				 	}
    				}
    				if(flag!=1){
    						printf("SERVER: Car ID is not found\n");
    				 		buffersend[0] = NO;
    					    send(clientSocket,buffersend , sizeof(buffersend), 0);
    				}
    			
    				if(updatedir == 0){
							printf("SERVER(N): DIRECTION NORTHBOUD\n");
							if(interMonitor->monitor.traffic[updatedir].lineup[found].y < 0){
                                //for removing
								for(int k = 0; k< interMonitor->monitor.traffic[updatedir].lineCount;k++){
									interMonitor->monitor.traffic[updatedir].lineup[k] = interMonitor->monitor.traffic[updatedir].lineup[k+1];
    					    	}
								interMonitor->monitor.traffic[updatedir].lineCount--;

                                //stop vehicle.
								buffersend[0] = NO;
								send(clientSocket,buffersend , sizeof(buffersend), 0);
    					   		break;
    					}else{

							printf("SERVER(N): car is not left line\n");
							buffersend[0] = YES;

							if(interMonitor->monitor.traffic[updatedir].lineup[found].y >= interMonitor->monitor.traffic[updatedir].stopY){
									printf("SERVER(N): car still left stop line\n");
 
									dis_light = ((interMonitor->monitor.traffic[updatedir].lineup[found].y) -  (interMonitor->monitor.traffic[updatedir].stopY)); 
									cur_light = interMonitor->lights[updatedir].currentState;
    						  
    						  //for checking part 8
									printf("SERVER(N): found  is %d \n",found);

									//not head element
							if(found != 0){

									aheadcar_speed = interMonitor->monitor.traffic[updatedir].lineup[found-1].speed; //after speed
    						   		dis_car = interMonitor->monitor.traffic[updatedir].lineup[found].y - interMonitor->monitor.traffic[updatedir].lineup[found-1].y; //after distance
    						   		 
    						}
    						else{
    						  		aheadcar_speed =  VEHICLE_TOP_SPEED + 1;
    						  		dis_car =  MAX_DISTANCE;  //after distance
    						}
    						  
    						printf("SERVER(N): distance is %d\n", dis_light);
    						       
    					 
    						}
    						else if (interMonitor->monitor.traffic[updatedir].lineup[found].y < interMonitor->monitor.traffic[updatedir].stopY){
    						 		printf("SERVER(N): out of line\n");
    						 		 
    						 		cur_light= GREEN;
    						 		dis_light= MAX_DISTANCE;
    						   //for checking part 8
    						if(found !=0){
    						   	aheadcar_speed = interMonitor->monitor.traffic[updatedir].lineup[found-1].speed; //after speed
    						   	dis_car = interMonitor->monitor.traffic[updatedir].lineup[found].y - interMonitor->monitor.traffic[updatedir].lineup[found-1].y; //after distance
    						}
    						else{
    							aheadcar_speed = VEHICLE_TOP_SPEED + 1;
    						  	dis_car =  MAX_DISTANCE;  //after distance
    						}
                                    
    						 }
    						 
    					 }

                         
    				}	
    				if(updatedir == 1){
						printf("SERVER(S): DIRECTION SOUTHBOUND RUNNING \n");
    					if(interMonitor->monitor.traffic[updatedir].lineup[found].y > INTERSECTION_HEIGHT ){
								for(int k = 0; k< interMonitor->monitor.traffic[updatedir].lineCount;k++){
									interMonitor->monitor.traffic[updatedir].lineup[k] = interMonitor->monitor.traffic[updatedir].lineup[k+1];
    					    	}
								interMonitor->monitor.traffic[updatedir].lineCount--;
								buffersend[0] = NO;
								send(clientSocket,buffersend , sizeof(buffersend), 0);
    					   		break;
    					}else{

							printf("SERVER(S): car is not left line  \n");
							buffersend[0] = YES;
							
							if(interMonitor->monitor.traffic[updatedir].lineup[found].y <= interMonitor->monitor.traffic[updatedir].stopY){
									printf("SERVER(S): car still left stop line\n");
									dis_light = ((interMonitor->monitor.traffic[updatedir].stopY) -  (interMonitor->monitor.traffic[updatedir].lineup[found].y)); 
									cur_light = interMonitor->lights[updatedir].currentState;
    						  
    						  //for checking part 8
									printf("SERVER(S): found is %d \n",found);
									//not head element
							if(found != 0){
									aheadcar_speed = interMonitor->monitor.traffic[updatedir].lineup[found-1].speed; //after speed
    						   		dis_car = interMonitor->monitor.traffic[updatedir].lineup[found-1].y - interMonitor->monitor.traffic[updatedir].lineup[found].y; //after distance
    						   		 
    						}
    						else{
    						  		aheadcar_speed =  VEHICLE_TOP_SPEED + 1;
    						  		dis_car =  MAX_DISTANCE;  
    						}
    						  
    						printf("SERVER(S): distance is %d\n",dis_light);
    						
                                    
    						}
    						else if (interMonitor->monitor.traffic[updatedir].lineup[found].y > interMonitor->monitor.traffic[updatedir].stopY){
    						 		printf("SERVER(S): out of line\n");
    						 		 cur_light= GREEN;
    						 		 dis_light= MAX_DISTANCE;
    						   //for checking part 8
    						if(found !=0 ){
    						    aheadcar_speed = interMonitor->monitor.traffic[updatedir].lineup[found-1].speed; //after speed
    						   	dis_car = interMonitor->monitor.traffic[updatedir].lineup[found].y - interMonitor->monitor.traffic[updatedir].lineup[found-1].y; //after distance
    						   
    						    
    						}
    						else{
    							aheadcar_speed =VEHICLE_TOP_SPEED + 1;
    						  	dis_car =  MAX_DISTANCE; 
    						}
    						        
								 
    						}
    						  
						}
    				 
    				}  
    				if(updatedir == 2){
    					printf("SERVER(E): DIRECTION EASTBOUND RUNNING\n");
    					if(interMonitor->monitor.traffic[updatedir].lineup[found].x > INTERSECTION_WIDTH ){
								for(int k = 0; k< interMonitor->monitor.traffic[updatedir].lineCount;k++){
									interMonitor->monitor.traffic[updatedir].lineup[k] = interMonitor->monitor.traffic[updatedir].lineup[k+1];
    					    	}
								interMonitor->monitor.traffic[updatedir].lineCount--;
								buffersend[0] = NO;
								send(clientSocket,buffersend , sizeof(buffersend), 0);
    					   		break;
    					}else{

							printf("SERVER(E): car is not left line  \n");
							buffersend[0] = YES;
							if(interMonitor->monitor.traffic[updatedir].lineup[found].x <= interMonitor->monitor.traffic[updatedir].stopX){
									printf("SERVER(E): car still left stop line\n");
								 dis_light = ( (interMonitor->monitor.traffic[updatedir].stopX)-(interMonitor->monitor.traffic[updatedir].lineup[found].x)); 
								 cur_light = interMonitor->lights[updatedir].currentState;
    						  //for checking part 8
									printf("SERVER(E): found is %d \n",found);
									//not head element
							if(found != 0){
									aheadcar_speed = interMonitor->monitor.traffic[updatedir].lineup[found-1].speed; 
    						   		dis_car = interMonitor->monitor.traffic[updatedir].lineup[found-1].x - interMonitor->monitor.traffic[updatedir].lineup[found].x; 
    						   		 
    						}
    						else{
    						  		aheadcar_speed = VEHICLE_TOP_SPEED + 1;
    						  		dis_car =  MAX_DISTANCE;  //after distance
    						}
    						  
    						printf("SERVER(E): distance is %d\n",dis_light);
    				 
   
    						  	 
    						}
    						else if (interMonitor->monitor.traffic[updatedir].lineup[found].x > interMonitor->monitor.traffic[updatedir].stopX){
    						 		printf("SERVER(E): out of line\n");
    						 		cur_light= GREEN;
    						 		dis_light= MAX_DISTANCE;
    						   //for checking part 8
    						if(found !=0 ){
    						   	aheadcar_speed = interMonitor->monitor.traffic[updatedir].lineup[found-1].speed; //after speed
    						   	dis_car = interMonitor->monitor.traffic[updatedir].lineup[found-1].x - interMonitor->monitor.traffic[updatedir].lineup[found].x; //after distance
    						    
    						}
    						else{
    							aheadcar_speed  = VEHICLE_TOP_SPEED + 1;
    							dis_car =  MAX_DISTANCE;  //after distance
    						}
    						       
								 
    						 }
    						 

						}
    				     
    				}	
    				if(updatedir == 3){

    					printf("SERVER(W): DIRECTION WEST");
							if(interMonitor->monitor.traffic[updatedir].lineup[found].x < 0){
								for(int k = 0; k< interMonitor->monitor.traffic[updatedir].lineCount;k++){
									interMonitor->monitor.traffic[updatedir].lineup[k] = interMonitor->monitor.traffic[updatedir].lineup[k+1];
    					    	}
								interMonitor->monitor.traffic[updatedir].lineCount--;
								buffersend[0] = NO;
								send(clientSocket,buffersend , sizeof(buffersend), 0);
    					   		break;
    					}else{

							printf("SERVER(W): car is not left line  \n");
							buffersend[0] = YES;
							 
							if(interMonitor->monitor.traffic[updatedir].lineup[found].x >= interMonitor->monitor.traffic[updatedir].stopX){
									printf("SERVER(W): car still left stop line\n");
									dis_light = ((interMonitor->monitor.traffic[updatedir].lineup[found].x) -  (interMonitor->monitor.traffic[updatedir].stopX)); 
								    cur_light = interMonitor->lights[updatedir].currentState;
    						  //for checking part 8
									printf("SERVER(W): found is%d \n",found);
									//not head element
							if(found != 0){
									aheadcar_speed = interMonitor->monitor.traffic[updatedir].lineup[found-1].speed; //after speed
    						   		dis_car = interMonitor->monitor.traffic[updatedir].lineup[found].x - interMonitor->monitor.traffic[updatedir].lineup[found-1].x; //after distance
    						   		
    						}
    						else{
    						  		aheadcar_speed =  VEHICLE_TOP_SPEED + 1;
    						  		dis_car =  MAX_DISTANCE;   //after distance
    						}
    				 
    						}
    						else if (interMonitor->monitor.traffic[updatedir].lineup[found].x < interMonitor->monitor.traffic[updatedir].stopX){
    						 		printf("SERVER(W): out of line\n");
    						 		cur_light= GREEN;
    						 		dis_light= MAX_DISTANCE;
    						   //for checking part 8
    						if(found !=0){
    						   	aheadcar_speed = interMonitor->monitor.traffic[updatedir].lineup[found-1].speed; //after speed
    						   	dis_car = interMonitor->monitor.traffic[updatedir].lineup[found].x - interMonitor->monitor.traffic[updatedir].lineup[found-1].x; //after distance
    						   
    						    
    						}
    						else{
    							aheadcar_speed = VEHICLE_TOP_SPEED + 1;
    							dis_car=  MAX_DISTANCE;  //after distance
    						}
    						  
				 
    						 }
    						  
    					 
    					}
 
    				}

                     // for checking crash and some senario , 
                     //but i dont decided to put this part in to client side and make it complicated for me , i have made that version but not prrfect work
                         if(dis_car == MAX_DISTANCE){
 	                        if((cur_light != GREEN) && (dis_light != MAX_DISTANCE)){
 	   	                        if (dis_light <= (2*VEHICLE_WIDTH + interMonitor->monitor.traffic[updatedir].lineup[found].speed )){
 	   		                   interMonitor->monitor.traffic[updatedir].lineup[found].speed = 0;
            
 	   	                        }
 	   	                        else if (dis_light <= (3 * VEHICLE_WIDTH * interMonitor->monitor.traffic[updatedir].lineup[found].speed )){
				 
 	   		                    if(interMonitor->monitor.traffic[updatedir].lineup[found].speed  > 1){
 	   	                           interMonitor->monitor.traffic[updatedir].lineup[found].speed  = interMonitor->monitor.traffic[updatedir].lineup[found].speed / 1.5;
 
 	   	                    	}
 	   	                        }
 	                            }else{
 	   		                        if(interMonitor->monitor.traffic[updatedir].lineup[found].speed  < VEHICLE_TOP_SPEED){
 		 	 
 			                        interMonitor->monitor.traffic[updatedir].lineup[found].speed  ++;

 			                    }
 	   	                        }
 	                            }else {
 		                            if((cur_light != GREEN) && (dis_light != MAX_DISTANCE)){
 			                            if(dis_light <= (2*VEHICLE_WIDTH + interMonitor->monitor.traffic[updatedir].lineup[found].speed )){
 				                                    interMonitor->monitor.traffic[updatedir].lineup[found].speed  = 0;
 	   			 
 			                                }
 			                            else if (dis_light <= (3*VEHICLE_WIDTH * interMonitor->monitor.traffic[updatedir].lineup[found].speed)){
 				                            if(interMonitor->monitor.traffic[updatedir].lineup[found].speed  > 1){
 	   	                                        interMonitor->monitor.traffic[updatedir].lineup[found].speed  = interMonitor->monitor.traffic[updatedir].lineup[found].speed  / 1.5;
                 
 	   	                                	}
 			                            }
 		                                }if (aheadcar_speed < interMonitor->monitor.traffic[updatedir].lineup[found].speed ){
 			                                if(dis_car <= (4 *VEHICLE_WIDTH + interMonitor->monitor.traffic[updatedir].lineup[found].speed )){
                                                        interMonitor->monitor.traffic[updatedir].lineup[found].speed  = 0;
 	   			 
 			                            }else if(dis_light <= (3*VEHICLE_WIDTH * interMonitor->monitor.traffic[updatedir].lineup[found].speed )){
 				                            if(interMonitor->monitor.traffic[updatedir].lineup[found].speed  > 1){
 				                                  interMonitor->monitor.traffic[updatedir].lineup[found].speed  --;

 				                            }
 			                            }
 		                                }else if(((aheadcar_speed  > interMonitor->monitor.traffic[updatedir].lineup[found].speed )&&(dis_light == MAX_DISTANCE))||(cur_light == GREEN)){
 			                                if(interMonitor->monitor.traffic[updatedir].lineup[found].speed  < aheadcar_speed){
 				                                interMonitor->monitor.traffic[updatedir].lineup[found].speed ++;
 			                                }
 		                                }
 	 
 	                                }

    			 }
    
      			printf("SERVER: Closing client connection .\n");
 			close(clientSocket); // Close this client's socket
 		 
		
		if(SHUTDOWN == cmd){
			break;
		}
  	  }
  	
  	
  	 close(serverSocket);
  	 printf("SERVER: shut down\n");
  	 
}