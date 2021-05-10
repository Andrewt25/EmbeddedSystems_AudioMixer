#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include "commonUtil.h"
#include "joystick.h"
#include "beatbox.h"

static void assertDirectionOk(enum eJoystickDirections direction);
static char readValue(char *filename);
static _Bool isDirectionPressed(enum eJoystickDirections direction);
static void pollInput();

int joystickShutdownFlag = 1;
pthread_t joystick_thread;


struct DirectionInfo {
    const char* name;
    const int portNumber;
};


const struct DirectionInfo directions[] = {
    {"Up", 26,},
    {"Down", 46,},
    {"Left", 65,},
    {"Right", 47,},
    {"Center", 27,},
};

void Joystick_init(){
    CommonUtil_exportPin(directions[JOYSTICK_UP].portNumber);
    CommonUtil_exportPin(directions[JOYSTICK_DOWN].portNumber);
    CommonUtil_exportPin(directions[JOYSTICK_LEFT].portNumber);
    CommonUtil_exportPin(directions[JOYSTICK_RIGHT].portNumber);
    CommonUtil_exportPin(directions[JOYSTICK_CENTER].portNumber);
    CommonUtil_pause(0, 330000000);
    pthread_create(&joystick_thread, NULL, (void *) &pollInput, NULL);
}

void Joystick_cleanup(){
    pthread_join(joystick_thread, NULL);
}

static void pollInput(){
    while(joystickShutdownFlag == 1){
        if(isDirectionPressed(JOYSTICK_UP)){
            Beatbox_increaseVolume();
            CommonUtil_pause(0, 100000000);
        } else if(isDirectionPressed(JOYSTICK_DOWN)){
            Beatbox_decreaseVolume();
            CommonUtil_pause(0, 100000000);
        } else if(isDirectionPressed(JOYSTICK_LEFT)){
            Beatbox_decreaseBpm();
            CommonUtil_pause(0, 100000000);
        } else if(isDirectionPressed(JOYSTICK_RIGHT)){
            Beatbox_increaseBpm();
            CommonUtil_pause(0, 100000000);
        } else if(isDirectionPressed(JOYSTICK_CENTER)){
            Beatbox_cycleBeat();
            CommonUtil_pause(0, 500000000);
        }
        CommonUtil_pause(0,10000000);
    }
    pthread_exit(NULL);
}

static _Bool isDirectionPressed(enum eJoystickDirections direction){
    assertDirectionOk(direction);
    char filename[30];
    sprintf(filename,"/sys/class/gpio/gpio%d/value", directions[direction].portNumber);
    if(readValue(filename) == '0'){
        return 1;
    }
    return 0;
}

static void assertDirectionOk(enum eJoystickDirections direction){
    assert(direction >= 0 && direction < JOYSTICK_NUMBER_OF_DIRECTIONS);
}

static char readValue(char *filename) {
        //Open File
        FILE *pFile = fopen(filename, "r");
        if(pFile == NULL) {
            printf("Error: Unable to open file (%s) for read\n", filename);
            exit(-1);
        } 
        const int MAX_LENGTH = 2;
        char buff[MAX_LENGTH];
        fgets(buff, MAX_LENGTH, pFile);
        fclose(pFile);

        return buff[0];
}
