#include "alarm.h"

// beep-06 wave file are taken from "https://www.soundjay.com/beep-sounds-1.html"
#define SOURCE_FILE "wave-files/beep-06.wav"
#define UPJOYSTICK "/sys/class/gpio/gpio26/value"
#define EXPORTFILE "/sys/class/gpio/export"

#define SAMPLE_RATE   44100
#define NUM_CHANNELS  1
#define SAMPLE_SIZE   (sizeof(short))

// Store data of a single wave file read into memory.
// Space is dynamically allocated; must be freed correctly!
typedef struct {
	int numSamples;
	short *pData;
} wavedata_t;

// Prototypes:
snd_pcm_t *Audio_openDevice();
void Audio_readWaveFileIntoMemory(char *fileName, wavedata_t *pWaveStruct);
void Audio_playFile(snd_pcm_t *handle, wavedata_t *pWaveData);

//Write value to file
void writeToFile(char *fileName, char* value);

void waitDelay(long sec, long nanoSec){
	long seconds = sec;
	long nanoSeconds = nanoSec;
	struct timespec delay = {seconds, nanoSeconds};
	nanosleep(&delay, (struct timespec *) NULL);
}


void startProgram(){
	//opening upjoystick file to stop alarm (temporary)
	writeToFile(EXPORTFILE,"26");
	//size of the alarm
	size = 0;
	//today's date
	today = 0;
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
void beep(_Bool* beep){
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
	pWaveStruct->pData = malloc(sizeInBytes);
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
