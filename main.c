#include "alarm.h"

_Bool threadDone = 0;

void getTime(){
	time_t t;
	time(&t);
	printf("Current Time is %s", ctime(&t));
	
}

void* timeThread(){
	_Bool initialize_today = false;
	while(threadDone == 0){
		time_t t;
		time(&t);
		char* temp_str = ctime(&t);
		char* hour = (char*) malloc(sizeof(char)*3);
		hour[0] = temp_str[11];
		hour[1] = temp_str[12];
		hour[2] = '\0';
		char* minute = (char*) malloc(sizeof(char)*3);
		minute[0] = temp_str[14];
		minute[1] = temp_str[15];
		minute[2] = '\0';
		char* day = (char*) malloc(sizeof(char)*3);
		day[0] = temp_str[8];
		day[1] = temp_str[9];
		day[2] = '\0';
		if(initialize_today == false){
			today = atoi(day);
			initialize_today = true;
		}
		checkAlarm(atoi(hour),atoi(minute));
		resetAlarmBeep(atoi(day));
		free(hour);
		free(minute);
		free(day);
	}
	return NULL;
}

int main(int argc, char **argv){
	printf("Welcome to the alarm clock program \n");
	startProgram();
	pthread_t time_thread;

	if(pthread_create(&time_thread, NULL, timeThread, 0) != 0){
		printf("Error creating time thread\n");
	}

	//Commands (We might not use this)
	while(threadDone == 0){
		char* buff = (char*) malloc(sizeof(char)*1024);

		fgets(buff,1024,stdin);
		if(strcmp(buff,"help\n") == 0){
			printf("List of commands:\n");
			printf("help - Display list of commands\n");
			printf("add - set alarm\n");
			printf("get alarm - Displaying all alarm that has been stored\n");
			printf("get time - Displaying current time\n");
			printf("delete - Delete alarm\n");
			printf("stop - Exit program\n");
		}
		else if(strcmp(buff,"add\n") == 0){
			printf("Please enter the hour (min 0, max 23) : ");
			char* temp_hour = (char*) malloc(sizeof(char)*15);
			fgets(temp_hour,15,stdin);
			while(!(atoi(temp_hour) >= 0 && atoi(temp_hour)<24)){
				printf("Please put the right number.\nPlease enter the hour (min 0, max 23) : ");
				fgets(temp_hour,15,stdin);
			}
			if(atoi(temp_hour) >= 0 && atoi(temp_hour)<24){
				printf("Please enter minute (min 0, max 59): ");
				char* temp_minute = (char*) malloc(sizeof(char)*15);
				fgets(temp_minute,15,stdin);
				while(!(atoi(temp_minute) >= 0 && atoi(temp_minute) <=59)){
					printf("Please put the right number.\nPlease enter the minute (min 0, max 59) : ");
					fgets(temp_minute,15,stdin);
				}
				if(atoi(temp_minute) >= 0 && atoi(temp_minute) <=59){
					addAlarm(atoi(temp_hour),atoi(temp_minute));
				}
				free(temp_minute);
			}
			free(temp_hour);
		}
		else if(strcmp(buff,"get alarm\n") == 0){
			getAlarm();
		}
		else if(strcmp(buff,"get time\n") == 0){
			getTime();
		}
		else if(strcmp(buff,"delete\n") == 0){
			getAlarm();
			printf("Please choose the number (from 1 to %d): ", size);
			char* delete_index = (char*) malloc(sizeof(char)*4);
			fgets(delete_index,4,stdin);
			int index = atoi(delete_index);
			if(index > 0 && index <= size){			
				deleteAlarm(index-1);
			}
			free(delete_index);
		}
		else if(strcmp(buff,"stop\n") == 0){
			threadDone = 1;
		}
		else{
			printf("Please enter the appropriate command!\n");
		}
		free(buff);
	}

	if(pthread_join(time_thread,NULL) != 0){
		printf("Error joining time thread\n");
	}

	return 0;
}

