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
#define DOWNJOYSTICK "/sys/class/gpio/gpio46/value"
#define EXPORTFILE "/sys/class/gpio/export"

#define ALARM_SIZE 12

static pthread_t alarm_thread;
static pthread_t display_time_thread;
static int size;
static Alarm_t alarm_clock[ALARM_SIZE];
static wavedata_t alarm_sound;
static wavedata_t questionAndAnswerWave;
static wavedata_t questionWave;
static wavedata_t answerWave;

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
static _Bool checkUp();
static _Bool checkDown();
static void beep();
static _Bool stopAlarm();
static void speechInit(const char* problem, wavedata_t* file);

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
	writeToFile(EXPORTFILE,"46");
	//size of the alarm
	size = 0;
	AudioMixer_init();
	AudioMixer_readWaveFileIntoMemory(BEEP_FILE, &alarm_sound);
	// beep();


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
void speechInit(const char* sentence, wavedata_t* file){
	char* command = (char*) malloc(1024*sizeof(char));
	int length = sprintf(command,"pico2wave -w wave-files/question.wav \"%s\"", sentence);
	command[length] = '\0';
	system(command);
	AudioMixer_readWaveFileIntoMemory("wave-files/question.wav", file);
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

static void playSentence() {

}

void testUser() {
	lcd_mutex.lock();
	int difficulty = 0;
	char question[512];
	bool answered = false;
	int questionType = 0;
	bool pressedWrong = false;

	//mutiple choice
	if(questionType == 0) {
		//read questions from json
		json questions;
		
		std::ifstream stream("questions/easy.json");
		stream >> questions;
		questions = questions.at("results");

		//pick a random question
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

		//read question, remove quotes
		sprintf(question, "%s", questionJson.at("question").dump().c_str());
		question[0] = ' ';
		question[strlen(question) - 1] = ' ';
		int answerNumber;
		char answerChar;
		char answerBuffer[256];
		string answerArray[4];

		//read correct answer, remove quotes
		string tempAnswer = questionJson.at("correct_answer").dump();
		tempAnswer.erase(remove( tempAnswer.begin(), tempAnswer.end(), '\"' ),tempAnswer.end());

		//assigne letters to correct answer
		string questionSubType = questionJson.at("type");
		if(questionSubType.compare("boolean") == 0) {
			answerNumber = rand() % 2;
			answerArray[answerNumber] = tempAnswer;
			if(answerNumber == 0) {
				answerChar = 'A';
			} else {
				answerChar = 'B';
			}
		} else if(questionSubType.compare("multiple") == 0) {
			answerNumber = rand() % 4;
			answerArray[answerNumber] = tempAnswer;
			if(answerNumber == 0) {
				answerChar = 'A';
			} else if(answerNumber == 1) {
				answerChar = 'B';
			} else if(answerNumber == 2) {
				answerChar = 'C';
			} else {
				answerChar = 'D';
			}
		}

		//read incorrect answers, assgin letters
		json incorrectAnswers = questionJson.at("incorrect_answers");
		index = 0;
		for (auto& element : incorrectAnswers) {
			bool entered = false;
			while(!entered) {
				if(answerArray[index].empty()) {
					string temp = element.dump();
					temp.erase(remove( temp.begin(), temp.end(), '\"' ),temp.end());
					answerArray[index] = temp;
					entered = true;
				} else {
					index++;
				}
			}
		}

		//produce single string with all options
		if(questionSubType.compare("boolean") == 0) {
			sprintf(answerBuffer, "A. %s, B. %s", answerArray[0].c_str(), answerArray[1].c_str());
		} else if(questionSubType.compare("multiple") == 0) {
			sprintf(answerBuffer, "A. %s, B. %s, C. %s, D. %s", answerArray[0].c_str(), answerArray[1].c_str(), answerArray[2].c_str(), answerArray[3].c_str());
		}


		// string questionString(question);
		printf("%s\n", question);
		// questionString.erase(remove( questionString.begin(), questionString.end(), '\"' ),questionString.end());
		printf("%s\n", answerBuffer);
		
		
		speechInit(question, &questionWave);
		speechInit(answerBuffer, &answerWave);
		sprintf(question, "%s, %s", question, answerBuffer);
		speechInit(question, &questionAndAnswerWave);
		AudioMixer_queueSound(&questionAndAnswerWave);
		
		//prepare lcd
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("up: repeat qustn");
		lcd.setCursor(0, 1);
		lcd.print("dwn: repeat ansr");

		
		while(!answered) {
			char pressed = getPressed();
			if(checkUp()) {
				AudioMixer_queueSound(&questionWave);
			} else if(checkDown()) {
				AudioMixer_queueSound(&answerWave);
			} else if(pressed != ' ') {
				answered = true;
				if(pressed != answerChar) {
					pressedWrong = true;
					lcd.clear();
					lcd.print("WRONG!");
					waitDelay(0, 900000000);
				}
			}
			waitDelay(0, 400000000);
		}
		lcd.clear();
		AudioMixer_freeWaveFileData(&questionWave);
		AudioMixer_freeWaveFileData(&answerWave);
		AudioMixer_freeWaveFileData(&questionAndAnswerWave);


	//Arithmatic
	} else {
		int answer = 0;
		if(difficulty == 0) {
			int questionSubType = rand() % 2;
			int arg1, arg2;
			if(questionSubType == 0) {
				arg1 = rand() % 999 + 1;
				arg2 = rand() % 100 + 1;
				answer = arg1 + arg2;
				sprintf (question, "%d + %d =", arg1, arg2);

			} else if(questionSubType == 1) {
				arg1 = rand() % 12 + 1;
				arg2 = rand() % 12 + 1;
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
			if(pressed != ' ') {
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
	lcd.noCursor();
	lcd_mutex.unlock();
	if(pressedWrong) {
		testUser();
	}
}

_Bool checkUp(){
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

_Bool checkDown(){
	FILE *up_joystick = fopen(DOWNJOYSTICK, "r");
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