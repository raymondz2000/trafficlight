#include <unistd.h>

// Set the state of the given traffic light to the newState
void setState(TrafficLight *t, char newState) {

	if(newState==DELAY_RED_2){
		t->currentState = GREEN;
		t->countDownTimer = GREEN_COUNTDOWN ;
	}
	else if(newState==GREEN){
		t->currentState = YELLOW;
		t->countDownTimer =YELLOW_COUNTDOWN  ;
	}	
	
	else if(newState==YELLOW){
		t->currentState = DELAY_RED_1;
		t->countDownTimer=DELAY_RED_COUNTDOWN  ;
	}
	
	else if(newState==DELAY_RED_1){
		t->currentState = RED;
		t->countDownTimer =RED_COUNTDOWN ;
	}
	else if(newState==RED){
		t->currentState = DELAY_RED_2;
		t->countDownTimer=DELAY_RED_COUNTDOWN  ;
	}
	
 	sem_post(&sem);
	 
}


// This code should run in an infinite loop continuously simulating a timed traffic light.
void *runTrafficLight(void *t) {
  	TrafficLight  *light = t;

	while(1) {
		
		
		for(int i= light->countDownTimer; i>0;i--){
			 
			usleep(500000);
			 
		}
		setState(light,light->currentState);
		 
 	}

	pthread_exit(NULL);
}
