#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include "beatbox.h"
#include "audioMixer.h"
#include "commonUtil.h"

struct SoundInfo{
    char* filePath;
    wavedata_t *pSound;
};

struct SoundInfo standardBeat[] = {
    {"beatbox-wav-files/100051__menegass__gui-drum-bd-hard.wav",
    NULL},
    {"beatbox-wav-files/100053__menegass__gui-drum-cc.wav",
    NULL},
    {"beatbox-wav-files/100059__menegass__gui-drum-snare-soft.wav",
    NULL},
};


static void* playBeatThread(void* arg);
static void customBeatPlay(int b);
static void standardBeatPlay(int b);
static void calculateTempo(int tempBpm);

static pthread_t beatboxThread;
static pthread_mutex_t beatboxMutex = PTHREAD_MUTEX_INITIALIZER;

long nano = 500000000;
int seconds = 0;
int bpm = 120;
int selectedBeat = 1;
int beatboxStopFlag = 1;


void Beatbox_init(){

    for(int i = 0; i < sizeof(standardBeat)/sizeof(standardBeat[0]); i++){
        standardBeat[i].pSound = malloc(sizeof(wavedata_t));
        AudioMixer_readWaveFileIntoMemory(standardBeat[i].filePath, standardBeat[i].pSound);
    }
    
    pthread_create(&beatboxThread, NULL, playBeatThread, NULL);
   
};

void *playBeatThread(void* arg){
    int b = 0;
    while(beatboxStopFlag == 1){
        switch (selectedBeat)
        {
        case 1:
            standardBeatPlay(b);
            break;
        case 2:
            customBeatPlay(b);
            break;
        default:
            break;
        }
        b = b >= 2? 0 : b + 1;
        pthread_mutex_lock(&beatboxMutex);
        int tempSeconds = seconds;
        long tempNano = nano;
        pthread_mutex_unlock(&beatboxMutex);
        CommonUtil_pause(tempSeconds, tempNano);
    }
    
    pthread_exit(NULL);
    return NULL;
}


void standardBeatPlay(int b){

    switch (b)
    {
    case 0:
        AudioMixer_queueSound(standardBeat[HIHAT].pSound);
        AudioMixer_queueSound(standardBeat[BASE].pSound);
        break;
    case 1:
        AudioMixer_queueSound(standardBeat[HIHAT].pSound);
        break;
    case 2:
        AudioMixer_queueSound(standardBeat[HIHAT].pSound);
        AudioMixer_queueSound(standardBeat[SNARE].pSound);
        break;
    default:
        printf("Error adding sound to queue (standard beat)");
        break;
    }
}

void customBeatPlay(int b){

    switch (b)
    {
    case 0:
        AudioMixer_queueSound(standardBeat[SNARE].pSound);
        break;
    case 1:
        AudioMixer_queueSound(standardBeat[SNARE].pSound);
        AudioMixer_queueSound(standardBeat[HIHAT].pSound);
        break;
    case 2:
        AudioMixer_queueSound(standardBeat[BASE].pSound);
        AudioMixer_queueSound(standardBeat[HIHAT].pSound);
        break;
    default:
        printf("Error adding sound to queue (standard beat)");
        break;
    }
}



void Beatbox_cleanup(){
    beatboxStopFlag = 0;
    pthread_join(beatboxThread, NULL);
    AudioMixer_freeWaveFileData(standardBeat->pSound);   
}

void Beatbox_cycleBeat(){
    pthread_mutex_lock(&beatboxMutex);
    selectedBeat = selectedBeat >= 2 ? 0 : selectedBeat + 1;
    pthread_mutex_unlock(&beatboxMutex); 
    switch (selectedBeat)
    {
    case 0:
        printf("Beat switchted to: None\n");
        break;
    case 1:
        printf("Beat switchted to: Standard Rock\n");
        break;
    case 2:
        printf("Beat switchted to: Custom\n");
        break;
    default:
        printf("Error: Switching Beats");
        break;
    }
    fflush(stdout);
}

void Beatbox_setMode(int mode){
    mode = mode > 2 ? 2 : mode;
    mode = mode < 0 ? 0 : mode;
    pthread_mutex_lock(&beatboxMutex);
    selectedBeat = mode;
    pthread_mutex_unlock(&beatboxMutex); 
}

int Beatbox_getMode(){
    pthread_mutex_lock(&beatboxMutex);
    int mode = selectedBeat;
    pthread_mutex_unlock(&beatboxMutex);
    return mode;
}

int Beatbox_increaseVolume(){
    int volume = AudioMixer_getVolume();
    if(volume < 100){
        volume += 5;
        AudioMixer_setVolume(volume);
        printf("Volume increased: %d/100\n", volume);
        fflush(stdout);
    }
    return volume; 
}
int Beatbox_decreaseVolume(){
    int volume = AudioMixer_getVolume();
    if(volume > 0){
        volume -= 5;
        AudioMixer_setVolume(volume);
        printf("Volume decreased: %d/100\n", volume);
        fflush(stdout);
    }
    return volume;  
}

int Beatbox_getBpm(){
    pthread_mutex_lock(&beatboxMutex);
    int tempBpm = bpm;
    pthread_mutex_unlock(&beatboxMutex);
    return tempBpm;
}

int Beatbox_increaseBpm(){
    
    int tempBpm = bpm;

    if(tempBpm < 300){
        tempBpm += 5;
        pthread_mutex_lock(&beatboxMutex);
        bpm = tempBpm;
        pthread_mutex_unlock(&beatboxMutex);
        printf("bpm increased: %d\n", tempBpm);
        fflush(stdout);
        calculateTempo(tempBpm);  
    } 
    return tempBpm;
}

int Beatbox_decreaseBpm(){
    int tempBpm = bpm;
    
    if(tempBpm > 40){
        tempBpm -= 5;
        pthread_mutex_lock(&beatboxMutex);
        bpm = tempBpm;
        pthread_mutex_unlock(&beatboxMutex);
        printf("bpm decreased: %d\n", tempBpm);
        fflush(stdout);
        calculateTempo(tempBpm);  
    }
    return tempBpm;
}

void calculateTempo(int tempBpm){
    int tempSeconds = 0;
    long tempNano = 0;

    double milliseconds = (double)60/tempBpm * 1000;
    while(milliseconds >= 1000){
        milliseconds -= 1000;
        tempSeconds += 1;
    }
    tempNano = round(milliseconds * 1000000);

    pthread_mutex_lock(&beatboxMutex);
    seconds = tempSeconds;
    nano = tempNano;
    pthread_mutex_unlock(&beatboxMutex);
}

void Beatbox_playSound(enum eInstruments drum){
    AudioMixer_queueSound(standardBeat[drum].pSound);
}