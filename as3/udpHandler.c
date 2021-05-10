#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>

#include "udpHandler.h"
#include "beatbox.h"
#include "audioMixer.h"

#define MAX_LEN 1024
#define PORT 12345

static void listenForCommand();
static void sendResponse(char* msg);
static void handleRequest(char* command);

struct sockaddr_in sinLocal;
struct sockaddr_in sinRemote;
int socketDescriptor;

pthread_t listener_thread;
pthread_mutex_t udpLock;
int listenerShutdownFlag = 1;



void UDPHandler_StartListening(){

   memset(&sinLocal, 0, sizeof(sinLocal));
   sinLocal.sin_family = AF_INET;
   sinLocal.sin_addr.s_addr = htonl(INADDR_ANY);
   sinLocal.sin_port = htons(PORT);

   socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
   bind(socketDescriptor, (struct sockaddr*) &sinLocal, sizeof(sinLocal));

   pthread_mutex_init(&udpLock, NULL); 
   pthread_create(&listener_thread, NULL, (void *) &listenForCommand, NULL);

}


void UDPHandler_WaitForExit(){
    pthread_join(listener_thread, NULL);
    pthread_mutex_destroy(&udpLock);
    close(socketDescriptor);
}

static void listenForCommand(){

    while(listenerShutdownFlag == 1){
        unsigned int sin_len = sizeof(sinRemote);
        char messageRx[MAX_LEN];
        
        int bytesRx = recvfrom(socketDescriptor, messageRx, MAX_LEN, 0, (struct sockaddr *) &sinRemote, &sin_len);
        int terminateIdx = (bytesRx < MAX_LEN) ? bytesRx : MAX_LEN -1;
        messageRx[terminateIdx] = 0;
        handleRequest(messageRx);
    }
    pthread_exit(NULL);

}

static void handleRequest(char* command){
    char responseMessage[32];
    if(!strcmp(command, "bpmUp")){
        int bpm = Beatbox_increaseBpm();
        sprintf(responseMessage, "bpm %d", bpm);
        sendResponse(responseMessage);
    } else if(!strcmp(command, "bpmDown")){
        int bpm = Beatbox_decreaseBpm();
        sprintf(responseMessage, "bpm %d", bpm);
        sendResponse(responseMessage);
    }  else if(!strcmp(command, "volUp")){
        int volume = Beatbox_increaseVolume();
       sprintf(responseMessage, "volume %d", volume);
        sendResponse(responseMessage);
    } else if(!strcmp(command, "volDown")){
        int volume = Beatbox_decreaseVolume();
        sprintf(responseMessage, "volume %d", volume);
        sendResponse(responseMessage);
    } else if(!strncmp(command, "mode", 4)){
        switch (command[5])
        {
        case '0':
            Beatbox_setMode(0);
            sprintf(responseMessage, "mode none");
            break;
        case '1':
            Beatbox_setMode(1);
            sprintf(responseMessage, "mode Standard Rock");
            break;
        case '2':
            Beatbox_setMode(2);
            sprintf(responseMessage, "mode Stellar Custom Beat");
            break;
        default:
         sprintf(responseMessage, "err Undefined mode selection");
            break;
        }
        sendResponse(responseMessage);
    } else if(!strcmp(command, "base")){
        Beatbox_playSound(BASE);
        sprintf(responseMessage, "played base");
        sendResponse(responseMessage);
    } else if(!strcmp(command, "hihat")){
        Beatbox_playSound(HIHAT);
        sprintf(responseMessage, "played hi-hat");
        sendResponse(responseMessage);
    } else if(!strcmp(command, "snare")){
        Beatbox_playSound(SNARE);
        sprintf(responseMessage, "played snare");
        sendResponse(responseMessage);
    } else if(!strcmp(command, "getVolume")){
        int volume = AudioMixer_getVolume();
        sprintf(responseMessage, "volume %d", volume);
        sendResponse(responseMessage);
    } else if(!strcmp(command, "getBpm")){
        int bpm = Beatbox_getBpm();
        sprintf(responseMessage, "bpm %d", bpm);
        sendResponse(responseMessage);
    } else if(!strcmp(command, "getMode")){
        int mode = Beatbox_getMode();
         switch (mode)
        {
        case 0:
            sprintf(responseMessage, "mode None");
            break;
        case 1:
            sprintf(responseMessage, "mode Standard Rock");
            break;
        case 2:
            sprintf(responseMessage, "mode Stellar Custom Beat");
            break;
        default:
         sprintf(responseMessage, "err Undefined mode selection");
            break;
        }
        sendResponse(responseMessage);
    } 
      
}


static void sendResponse(char* msg){
    int sin_len = sizeof(sinRemote);
    sendto(socketDescriptor, msg, strlen(msg), 0, (struct sockaddr *) &sinRemote, sin_len);
}





