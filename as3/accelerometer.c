#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include "accelerometer.h"
#include "commonUtil.h"
#include "beatbox.h"


#define I2C_DEVICE_ADDRESS 0x1c
#define CTRL_REG1 0x2a
#define STATUS 0x00
#define OUT_X_MSB 0X01
#define OUT_X_LSB 0X02
#define OUT_Y_MSB 0X03
#define OUT_Y_LSB 0X04
#define OUT_Z_MSB 0X05
#define OUT_Z_LSB 0X06

static int initBus(int address);
static void writeToReg(unsigned char out, unsigned char value);
static void* accelometerThread(void* arg);
static void readValues(unsigned char regAddr);
static void convertValues(void);


pthread_t accelPthread;

int accelStopFlag = 1;
int fileDesc;
char unsigned readingValues[7];
double newValues[3];
double currentValues[3];

void Accelerometer_init(void){
    fileDesc = initBus(I2C_DEVICE_ADDRESS);
    writeToReg(CTRL_REG1, 0x01);
    pthread_create(&accelPthread, NULL, accelometerThread, NULL );

}

void Accelerometer_cleanup(void){
    //accelStopFlag = 0;
    pthread_join(accelPthread, NULL);
    close(fileDesc);

}

static void* accelometerThread(void* arg){
    int firstLoop = 0;
    double threshold = 0.3;
    while(accelStopFlag == 1){
        readValues(OUT_X_MSB);
        convertValues();
        if(firstLoop != 0){
            double changeX = fabs(currentValues[0] - newValues[0]);
            double changeY = fabs(currentValues[1] - newValues[1]);
            double changeZ = fabs(currentValues[2] - newValues[2]);
            if(changeX >= threshold){
                printf("change in X playing sound\n");
                Beatbox_playSound(BASE);
                CommonUtil_pause(0,300000000);
            } else if(changeY >= threshold){
                printf("change in Y playing sound\n");
                Beatbox_playSound(HIHAT);
                CommonUtil_pause(0,300000000);
            } else if(changeZ >= threshold){
                printf("change in Z playing sound\n");
                Beatbox_playSound(SNARE);
                CommonUtil_pause(0,300000000);
            }
            
        }else {
            firstLoop = 1;
        }
        currentValues[0] = newValues[0];
        currentValues[1] = newValues[1];
        currentValues[2] = newValues[2];
        CommonUtil_pause(0,10000000);
    }
    return NULL;
}

static int initBus( int address){
    int fileDescription = open("/dev/i2c-1", O_RDWR);

    int result = ioctl(fileDescription, I2C_SLAVE, address);
    if(result < 0){
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return fileDescription;
} 

static void writeToReg(unsigned char out, unsigned char value){
    unsigned char buff[2];

    buff[0] = out;
    buff[1] = value;
    
    int result = write(fileDesc, buff, 2);
    if(result != 2){
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }
}

static void readValues(unsigned char regAddr){
    int res = write(fileDesc, &regAddr, sizeof(OUT_X_MSB));
    if(res != sizeof(OUT_X_MSB)){
        perror("I2C: Unable to write to i2c register.");
        exit(1);
    }

    res = read(fileDesc, &readingValues, sizeof(readingValues)*sizeof(char));
    if(res != sizeof(readingValues)*sizeof(char)){
        perror("I2C: Unable to read to i2c registers.");
        exit(1);
    }
}

static void convertValues(){

    int16_t val = (readingValues[OUT_X_MSB] << 8 | readingValues[OUT_X_LSB]);
    val >>= 4;
    newValues[0] = val/(double)1024;

    val = (readingValues[OUT_Y_MSB] << 8 | readingValues[OUT_Y_LSB]);
    val >>= 4;
    newValues[1] = val/(double)1024;

    val = (readingValues[OUT_Z_MSB] << 8 | readingValues[OUT_Z_LSB]);
    val >>= 4;
    newValues[2] = val/(double)1024;   
}