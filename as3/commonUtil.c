#include <stdlib.h>
#include <stdio.h>
#include <time.h>



void CommonUtil_pause(int seconds,  long nanoseconds){
    struct timespec delay = {seconds, nanoseconds};
    nanosleep(&delay, (struct timespec *) NULL);
}

void CommonUtil_exportPin(int pin){
    char command[40];
    sprintf(command,"echo %d > /sys/class/gpio/export", pin);
    FILE *pipe = popen(command, "r");
    fflush(pipe);
    
    if(!pipe){
        printf("Unable to execute Command: %s\n", command);
        exit(1);
    }
    int exitCode = WEXITSTATUS(pclose(pipe));
    if(exitCode != 0){
        printf("Command: %s failed with exit code %d\n", command, exitCode);
    }
}