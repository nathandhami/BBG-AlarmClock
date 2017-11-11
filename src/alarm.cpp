#include "alarm.h"
#include "LiquidCrystal_I2C.h"
#include <ctime>
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <fstream>
#include "json.hpp"
extern "C" {
	#include "deviceread.h"
	#include "utils.h"
	#include "audioMixer.h"
}
using namespace std;
using json = nlohmann::json;
// beep-06 wave file are taken from "https://www.soundjay.com/beep-sounds-1.html"
#define BEEP_FILE "wave-files/beep-06.wav"
#define UPJOYSTICK "/sys/class/gpio/gpio26/value"
#define EXPORTFILE "/sys/class/gpio/export"

#define ALARM_SIZE 12

static pthread_t alarm_thread;
static pthread_t display_time_thread;
static int size;
static Alarm_t alarm_clock[ALARM_SIZE];
static wavedata_t alarm_sound;
static wavedata_t question;

static const char * months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
static const char * days[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
// i2c address
static uint8_t i2c=0x3f;
// Control line PINs
static uint8_t en=2;
static uint8_t rw=1;
static uint8_t rs=0;
// Data line PINs
static uint8_t d4=4;
static uint8_t d5=5;
static uint8_t d6=6;
static uint8_t d7=7;
// Backlight PIN
static uint8_t bl=3;
// LCD display size
static uint8_t rows=2;
static uint8_t cols=16;
static LiquidCrystal_I2C lcd("/dev/i2c-1", i2c, en, rw, rs, d4, d5, d6, d7, bl, POSITIVE);
static mutex lcd_mutex;

static void* displayTimeThread(void*);
static void* alarmThread(void*);
static void writeToFile(char *fileName, char* value);
static void testUser();

static void beep();
static _Bool stopAlarm();
static void questionInit(char* problem);

void waitDelay(long sec, long nanoSec){
	long seconds = sec;
	long nanoSeconds = nanoSec;
	struct timespec delay = {seconds, nanoSeconds};
	nanosleep(&delay, (struct timespec *) NULL);
}


void Alarm_startProgram(){
	// set TZ
	setenv("TZ", "PST8PST", 1);
	tzset();
	//initialize random seed
	srand (time(NULL));
	DeviceRead_startReading();
	//opening upjoystick file to stop alarm (temporary)
	writeToFile(EXPORTFILE,"26");
	//size of the alarm
	size = 0;
	AudioMixer_init();
	AudioMixer_readWaveFileIntoMemory(BEEP_FILE, &alarm_sound);
	beep();


	int thread1 = pthread_create(&alarm_thread, NULL, alarmThread, (void *)0);
	if(thread1 != 0){
		printf("Error creating time thread\n");
	}

	int thread2 = pthread_create(&display_time_thread, NULL, displayTimeThread, (void *)1);
	if(thread2 != 0){
		printf("Error creating time thread\n");
	}
}

void Alarm_endProgram(){
	pthread_join(alarm_thread,NULL);
	pthread_join(display_time_thread,NULL);

	AudioMixer_freeWaveFileData(&alarm_sound);
	AudioMixer_cleanup();
}

//check if the alarm has to rang or not
void checkAlarm(int hour, int minute, int sec, int today){
	for(int i = 0; i < size; i++){
		if(hour == alarm_clock[i].hours && minute == alarm_clock[i].minutes && alarm_clock[i].status && alarm_clock[i].days[today] && sec == 0){
			beep();
			
		}
	}
}

void beep(){
	_Bool alarmBeeping = false;

	while(!alarmBeeping){
		AudioMixer_queueSound(&alarm_sound);
		if(stopAlarm()){
			alarmBeeping = true;
		}
		waitDelay(0,700000000);
	}
}


//stopping alarm by pressing up on the joystick (temporary)
_Bool stopAlarm(){
	FILE *up_joystick = fopen(UPJOYSTICK, "r");
	_Bool is_pressed = false;
	if(up_joystick == NULL){
		printf("ERROR: cannot read up joystick file");
		exit(-1);
	}
	const int max_length = 1024;
	char buff[max_length];
	fgets(buff, max_length, up_joystick);

	//up is pressed
	if(atoi(buff) == 0){
		is_pressed = true;
	}

	fclose(up_joystick);
	return is_pressed;
}

// Text to speech function
// queue the sound at different places
void questionInit(char* problem){
	char* command = (char*) malloc(1024*sizeof(char));
	int length = sprintf(command,"pico2wave -w wave-files/question.wav \"%s\"",problem);
	command[length] = '\0';
	system(command);
	AudioMixer_readWaveFileIntoMemory("wave-files/question.wav", &question);
	free(command);
}

//function to add alarm
void Alarm_addAlarm(int hour, int minute, int ids, int diff, _Bool stats, _Bool day[7]){
	if((hour>=0 && hour<=24) && (minute>=0 && minute<=59)){
		alarm_clock[size].hours = hour;
		alarm_clock[size].minutes = minute;
		alarm_clock[size].id = ids;
		alarm_clock[size].difficulty = diff;
		alarm_clock[size].status = stats;
		for(int i = 0; i < 7; i++){
			alarm_clock[size].days[i] = day[i];
		}
		size++;
	}
}

//function to edit alarm (not yet used)
void Alarm_editAlarm(int hour, int minute, int ids, int diff, _Bool stats, _Bool day[7]){
	_Bool found = false;
	int i = -1;
	while(i < size-1 && !found){
		i++;
		if(alarm_clock[i].id == ids){
			found = true;
		}
	}
	if(found){
		alarm_clock[i].hours = hour;
		alarm_clock[i].minutes = minute;
		alarm_clock[i].status = stats;
		alarm_clock[i].difficulty = diff;
		for(int j = 0; j < 7; j++){
			alarm_clock[i].days[j] = day[j];
		}
	}
	else{
		printf("Cannot find alarm with that id");
	}
}

//delete alarm
void Alarm_deleteAlarm(int ids){
	_Bool found = false;
	int i = -1;
	while(i < size-1 && !found){
		i++;
		if(alarm_clock[i].id == ids){
			found = true;
		}
	}
	if(found){
		alarm_clock[i].hours = alarm_clock[size-1].hours;
		alarm_clock[i].minutes = alarm_clock[size-1].minutes;
		alarm_clock[i].id = alarm_clock[size-1].id;
		alarm_clock[i].difficulty = alarm_clock[size-1].difficulty;
		alarm_clock[i].status = alarm_clock[size-1].status;
		for(int j = 0; j < 7; j++){
			alarm_clock[i].days[j] = alarm_clock[size-1].days[j];
		}
		size--;	
	}
	else{
		printf("Cannot find alarm with that id");
	}

	
}

//function to get all alarm that's been added
void Alarm_getAlarm(){
	for(int i = 0; i < size; i++){
		printf("%d.  %.02d:%.02d %d, %d, %d, %d\n" ,i+1,alarm_clock[i].hours,alarm_clock[i].minutes, alarm_clock[i].id, alarm_clock[i].difficulty, alarm_clock[i].status, size);
	}
	
}

void writeToFile(char *fileName, char* value){
	FILE *file = fopen(fileName, "w");
	if(file == NULL){
		printf("Error: cannot open file.\n");
		exit(1);
	}
	fprintf(file, "%s", value);
	fclose(file);
}

void* alarmThread(void*) {
	while(true) {
		auto now = std::chrono::system_clock::now();
		time_t t = chrono::system_clock::to_time_t(now);
		tm local_tm = *localtime(&t);
		int day = local_tm.tm_wday;
		int hour = local_tm.tm_hour;
		int minute = local_tm.tm_min;
		int sec = local_tm.tm_sec;
		checkAlarm(hour, minute, sec, day);
		waitDelay(1, 0);
	}

}

void* displayTimeThread(void*){
	//set virtual cape
	Utils_loadVirtualCape("BB-I2C1");

	lcd.begin(cols, rows);
	lcd.on();
	lcd.clear();

	testUser();

	while(true) {
		lcd.clear();
		auto now = std::chrono::system_clock::now();
		time_t t = chrono::system_clock::to_time_t(now);
		tm local_tm = *localtime(&t);
		int hour = local_tm.tm_hour; 
		int minute = local_tm.tm_min;
		int second = local_tm.tm_sec;
		int weekDay = local_tm.tm_wday;
		int monthDay = local_tm.tm_mday;
		int month = local_tm.tm_mon;
		int year = local_tm.tm_year;
		char buffer[16];
		char buffer2[16];
		sprintf (buffer, "%d:%d:%d", hour, minute, second);
		sprintf (buffer2, "%s %s %d %d", days[weekDay], months[month], monthDay, (year + 1900));
		// printf("%s\n", buffer);
		lcd.print(buffer);
		lcd.setCursor(0, 1);
		lcd.print(buffer2);
		waitDelay(1, 0);
	}
}

void testUser() {
	lcd_mutex.lock();
	int difficulty = 0;
	char question[256];
	bool answered = false;
	int questionType = 0;

	//mutiple choice
	if(questionType == 0) {
		std::ifstream stream("questions/easy.json");
		json questions;
		stream >> questions;
		questions = questions.at("results");
		json questionJson;
		int questionNumber = rand() % questions.size();
		int index = 0;

		for (auto& element : questions) {
			if(index == questionNumber) {
				questionJson = element;
				break;
			}
			index++;
		}
		// std::cout << questionJson.at("question").dump() << '\n';
		string questionSubType = questionJson.at("type");
		sprintf(question, "%s", questionJson.at("question").dump().c_str());
		printf("%s\n", question);
		int answerNumber;
		char answerBuffer[256];
		string answerArray[4];
		if(questionSubType.compare("boolean") == 0) {
			answerNumber = rand() % 2;
			answerArray[answerNumber] = questionJson.at("correct_answer").dump().c_str();
		} else if(questionSubType.compare("multiple") == 0) {
			answerNumber = rand() % 4;
			answerArray[answerNumber] = questionJson.at("correct_answer").dump().c_str();
		}
		json incorrectAnswers = questionJson.at("incorrect_answers");
		index = 0;
		for (auto& element : incorrectAnswers) {
			bool entered = false;
			while(!entered) {
				if(answerArray[index].empty()) {
					answerArray[index] = element.dump().c_str();
					entered = true;
				} else {
					index++;
				}
			}
		}
		if(questionSubType.compare("boolean") == 0) {
			sprintf(answerBuffer, "A. %s, B. %s", answerArray[0].c_str(), answerArray[1].c_str());
		} else if(questionSubType.compare("multiple") == 0) {
			sprintf(answerBuffer, "A. %s, B. %s, C. %s, D. %s", answerArray[0].c_str(), answerArray[1].c_str(), answerArray[2].c_str(), answerArray[3].c_str());
		}
		printf("%s\n", answerBuffer);
		lcd.clear();
		
		lcd.autoscroll();
		// lcd.print(question);
		// lcd.setCursor(0, 1);
		lcd.setCursor(0, 0);
		for(int i = 0; (i <  strlen(question)); i++) {
				// printf("%c\n", question[i]);
				usleep(60000);
				lcd.write(question[i]);
				if (i % 8==0)
					sleep(1);
				// if(i < strlen(question)) {
				// 	lcd.write(question[i]);
				// }
				// lcd.setCursor(0, 1);
				// if(i < strlen(answerBuffer)) {
				// 	lcd.write(answerBuffer[i]);
				// }

			}
		// while(!answered) {
		// 	waitDelay(0, 400000000);
		// }
		sleep(10);
		lcd.clear();

	//Arithmatic
	} else {
		int answer = 0;
		if(difficulty == 0) {
			int questionSubType = rand() % 2;
			int arg1, arg2;
			if(questionSubType == 0) {
				arg1 = rand() % 100 + 1;
				arg2 = rand() % 100 + 1;
				answer = arg1 + arg2;
				sprintf (question, "%d + %d =", arg1, arg2);

			} else if(questionSubType == 1) {
				arg1 = rand() % 10 + 1;
				arg2 = rand() % 10 + 1;
				answer = arg1 * arg2;
				sprintf (question, "%d x %d =", arg1, arg2);
			}

		} else if(difficulty == 1) {

		} else if(difficulty == 2) {

		}
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(question);
		lcd.setCursor(0, 1);
		lcd.cursor();
		lcd.blink();

		char s[32];

		sprintf(s, "%d", answer);
		string enteredAnswer = "";
		while(!answered) {
			char pressed = getPressed();
			if(pressed != 'x') {
				lcd.write(pressed);
				enteredAnswer.push_back(pressed);
				if(pressed == '*') {
					enteredAnswer.pop_back();
					if(stoi(enteredAnswer) == answer) {
						lcd.clear();
						lcd.print("correct");
						answered = true;
					} else {
						enteredAnswer.clear();
						lcd.clear();
						lcd.setCursor(0, 0);
						lcd.print(question);
						lcd.setCursor(0, 1);
						lcd.cursor();
						lcd.blink();
					}
				}
			}

			waitDelay(0, 400000000);

		}
	}
	lcd.noBlink();
	lcd_mutex.unlock();
}
