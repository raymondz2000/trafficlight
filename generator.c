#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "trafficLight.h"
#include "vehicle.h"
#include "simulator.h"


int main() {
	
  srand(time(NULL));
 char buffer1[20];  
   
   while(1) {
  
     int rd1=(int)(rand()/(double)RAND_MAX*4);
     snprintf(buffer1, sizeof(buffer1), "./vehicle %d 50&",rd1);  
   
     
      usleep(500000);                      
      system(buffer1);
       
  
     
   }
    
  
}