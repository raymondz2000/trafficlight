#include <unistd.h>

void *movecar(void *intersection) {
 Intersection     *interVehicle = intersection;

	while(1){
	
		for (int i=0; i<4; i++) {
			
			
		 
  		 for(int j = 0 ; j <  interVehicle->monitor.traffic[i]. lineCount ;j++){
  		 	
  		 	if(i==0 ){
  		 		 
  		       		interVehicle->monitor.traffic[0].lineup[j].y -= interVehicle->monitor.traffic[0].lineup[j].speed;
  		       		 
  		        }
  		        
  		 	else if(i==1){
  				 
  		 		interVehicle->monitor.traffic[1].lineup[j].y += interVehicle->monitor.traffic[1].lineup[j].speed;
  		 		 
  		 	}
  		 	else if (i==2){
  		 		 
  		       		 interVehicle->monitor.traffic[2].lineup[j].x += interVehicle->monitor.traffic[i].lineup[j].speed;
  		       		 
  		
  		 	}
  		 	else if(i==3){
  		 		 
  		 		interVehicle->monitor.traffic[3].lineup[j].x -= interVehicle->monitor.traffic[3].lineup[j].speed;
				 
  		 	}
  		 	sem_post(&sem);
  		 	
  		 }
  		
  		
  	}
		usleep(20000);
	}
	pthread_exit(NULL);

}