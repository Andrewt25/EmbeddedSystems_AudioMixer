#include <stdio.h>
#include <pthread.h>
#include "joystick.h"
#include "beatbox.h"
#include "audioMixer.h"
#include "accelerometer.h"
#include "udpHandler.h"


int main() {
    

    AudioMixer_init();
    Beatbox_init();
    Accelerometer_init();
    Joystick_init();
    UDPHandler_StartListening();
    Joystick_cleanup();
    Accelerometer_cleanup();
}