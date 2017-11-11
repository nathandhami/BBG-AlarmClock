#include "alarm.h"
#include "LiquidCrystal_I2C.h"
#include <stdio.h>
#include <ctime>
#include <chrono>
#include <iostream>
#include <pthread.h>
#include <mutex>
#include <string>
#include <fstream>
#include "json.hpp"
extern "C" {
	#include "deviceread.h"
	#include "utils.h"
}
using namespace std;
using json = nlohmann::json;
// beep-06 wave file are taken from "https://www.soundjay.com/beep-sounds-1.html"
#define SOURCE_FILE "wave-files/beep-06.wav"
#define UPJOYSTICK "/sys/class/gpio/gpio26/value"
#define EXPORTFILE "/sys/class/gpio/export"

#define SAMPLE_RATE   44100
#define NUM_CHANNELS  1
#define SAMPLE_SIZE   (sizeof(short))
#define ALARM_SIZE 20

static pthread_t alarm_thread;
static pthread_t display_time_thread;
static int size;
static int today;
static Alarm_t alarm_clock[ALARM_SIZE];

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
// Store data of a single wave file read into memory.
// Space is dynamically allocated; must be freed correctly!
typedef struct {
	int numSamples;
	short *pData;
} wavedata_t;

// Prototypes:
static snd_pcm_t *Audio_openDevice();
static void Audio_readWaveFileIntoMemory(char *fileName, wavedata_t *pWaveStruct);
static void Audio_playFile(snd_pcm_t *handle, wavedata_t *pWaveData);
static void* displayTimeThread(void*);
static void* alarmThread(void*);
static void writeToFile(char *fileName, char* value);
static void testUser();

void waitDelay(long sec, long nanoSec){
	long seconds = sec;
	long nanoSeconds = nanoSec;
	struct timespec delay = {seconds, nanoSeconds};
	nanosleep(&delay, (struct timespec *) NULL);
}


void startProgram(){
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
	//today's date
	today = 0;

	int thread1 = pthread_create(&alarm_thread, NULL, alarmThread, (void *)0);
	if(thread1 != 0){
		printf("Error creating time thread\n");
	}

	int thread2 = pthread_create(&display_time_thread, NULL, displayTimeThread, (void *)1);
	if(thread2 != 0){
		printf("Error creating time thread\n");
	}
}

//check if the alarm has to rang or not
void checkAlarm(int hour, int minute){
	for(int i = 0; i < size; i++){
		if(hour == alarm_clock[i].hours && minute == alarm_clock[i].minutes && alarm_clock[i].has_beep == false){
			beep(&alarm_clock[i].has_beep);
			
		}
	}
}

//makes the alarm can only rings once per day. (when day changes, the alarm schedule will reset)
void resetAlarmBeep(int day){
	if(today != day){
		today = day;
		for(int i = 0; i < size; i++){
			alarm_clock[i].has_beep = false;
		}
	}
}

//alarm sound (mostly from Brian's wave_player.c code)
//alarm sound still not good enough, most probably using thread to play the audio file.
void beep(bool* beep){
	// Configure Output Device
	snd_pcm_t *handle = Audio_openDevice();

	// Load wave file we want to play:
	wavedata_t sampleFile;
	Audio_readWaveFileIntoMemory(SOURCE_FILE, &sampleFile);

	while(*beep == false){
		Audio_playFile(handle, &sampleFile);
		waitDelay(0,15000000);
		if(stopAlarm()){
			*beep = true;
		}
	}

	// Cleanup, letting the music in buffer play out (drain), then close and free.
	snd_pcm_drain(handle);
	snd_pcm_hw_free(handle);
	snd_pcm_close(handle);
	free(sampleFile.pData);
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

//function to add alarm
void addAlarm(int hour, int minute){
	if((hour>=0 && hour<=24) && (minute>=0 && minute<=59)){
		alarm_clock[size].hours = hour;
		alarm_clock[size].minutes = minute;
		alarm_clock[size].has_beep = false;
		size++;
	}
}

//function to edit alarm (not yet used)
void editAlarm(int hour, int minute, int index){
	if((hour>=0 && hour<=24) && (minute>=0 && minute<=59)){
		alarm_clock[index].hours = hour;
		alarm_clock[index].minutes = minute;
	}
	
}

//delete alarm
void deleteAlarm(int index){
	int i = index;
	while(i < size-1){
		alarm_clock[i].hours = alarm_clock[i+1].hours;
		alarm_clock[i].minutes = alarm_clock[i+1].minutes;
		i++;
	}	
	size--;	
}

//function to get all alarm that's been added
void getAlarm(){
	for(int i = 0; i < size; i++){
		printf("%d.  %.02d:%.02d\n",i+1,alarm_clock[i].hours,alarm_clock[i].minutes);
	}
	
}

// Open the PCM audio output device and configure it.
// Returns a handle to the PCM device; needed for other actions.
snd_pcm_t *Audio_openDevice()
{
	snd_pcm_t *handle;

	// Open the PCM output
	int err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) {
		printf("Play-back open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	// Configure parameters of PCM output
	err = snd_pcm_set_params(handle,
			SND_PCM_FORMAT_S16_LE,
			SND_PCM_ACCESS_RW_INTERLEAVED,
			NUM_CHANNELS,
			SAMPLE_RATE,
			1,			// Allow software resampling
			50000);		// 0.05 seconds per buffer
	if (err < 0) {
		printf("Play-back configuration error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	return handle;
}

// Read in the file to dynamically allocated memory.
// !! Client code must free memory in wavedata_t !!
void Audio_readWaveFileIntoMemory(char *fileName, wavedata_t *pWaveStruct)
{
	assert(pWaveStruct);

	// Wave file has 44 bytes of header data. This code assumes file
	// is correct format.
	const int DATA_OFFSET_INTO_WAVE = 44;

	// Open file
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		fprintf(stderr, "ERROR: Unable to open file %s.\n", fileName);
		exit(EXIT_FAILURE);
	}

	// Get file size
	fseek(file, 0, SEEK_END);
	int sizeInBytes = ftell(file) - DATA_OFFSET_INTO_WAVE;
	fseek(file, DATA_OFFSET_INTO_WAVE, SEEK_SET);
	pWaveStruct->numSamples = sizeInBytes / SAMPLE_SIZE;

	// Allocate Space
	pWaveStruct->pData = (short int*)malloc(sizeInBytes);
	if (pWaveStruct->pData == NULL) {
		fprintf(stderr, "ERROR: Unable to allocate %d bytes for file %s.\n",
				sizeInBytes, fileName);
		exit(EXIT_FAILURE);
	}

	// Read data:
	int samplesRead = fread(pWaveStruct->pData, SAMPLE_SIZE, pWaveStruct->numSamples, file);
	if (samplesRead != pWaveStruct->numSamples) {
		fprintf(stderr, "ERROR: Unable to read %d samples from file %s (read %d).\n",
				pWaveStruct->numSamples, fileName, samplesRead);
		exit(EXIT_FAILURE);
	}

	fclose(file);
}

// Play the audio file (blocking)
void Audio_playFile(snd_pcm_t *handle, wavedata_t *pWaveData)
{
	// If anything is waiting to be written to screen, can be delayed unless flushed.
	fflush(stdout);

	// Write data and play sound (blocking)
	snd_pcm_sframes_t frames = snd_pcm_writei(handle, pWaveData->pData, pWaveData->numSamples);

	// Check for errors
	if (frames < 0)
		frames = snd_pcm_recover(handle, frames, 0);
	if (frames < 0) {
		fprintf(stderr, "ERROR: Failed writing audio with snd_pcm_writei(): %li\n", frames);
		exit(EXIT_FAILURE);
	}
	if (frames > 0 && frames < pWaveData->numSamples)
		printf("Short write (expected %d, wrote %li)\n", pWaveData->numSamples, frames);
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
	bool initialize_today = false;
	while(true) {
		auto now = std::chrono::system_clock::now();
		time_t t = chrono::system_clock::to_time_t(now);
		tm local_tm = *localtime(&t);
		int hour = local_tm.tm_hour;
		int day = local_tm.tm_wday;
		int minute = local_tm.tm_min;
		if(initialize_today == false){
			today = day;
			initialize_today = true;
		}
		checkAlarm(hour, minute);
		resetAlarmBeep(day);
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
