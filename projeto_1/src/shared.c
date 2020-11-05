#include "../headers/shared.h"

#include <unistd.h>
#include <signal.h>

struct termios newtio, oldtio;
int retry;
int FLAG_ALARM=0;

void 
alarm_handler() {
    printf(RED "Alarm triggered\n" RESET);
    retry++;
    FLAG_ALARM = 1;
}

void 
initialize_alarm() {
    signal(SIGALRM, alarm_handler);
    FLAG_ALARM = 0;
    retry = 0;
}

int
llopen(int fd, int role) {
    // store the information on the old settings to be reset at the end of transmission
    if (role != TRANSMITTER && role != RECEIVER) {
        char *error = "role belongs to {TRANSMITTER, RECEIVER}"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); return -1; 
    }

    if (tcgetattr(fd, &oldtio) == -1) {
        char *error = "tcgetattr()"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); 
        return -1; 
    }

    // new termios structure
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUD | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;
    // defining the control characters
    newtio.c_cc[VMIN] = 0; // disables return when a certain amount of bytes is received
    newtio.c_cc[VTIME] = 1; // sets an overall timer for read so it doesn't block
    if (tcflush(fd, TCIOFLUSH) == -1) {
        char *error = "tcflush()"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error);
        return -1; 
    }
    if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
        char *error = "tcsetattr()"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error);
        return -1; 
    }
    
    // setting connection between tx and rx
    if (role == TRANSMITTER) {
        initialize_alarm();
        while(retry <= MAX_RETRY) {
            FLAG_ALARM = 0;
            alarm(TIMEOUT); // start the timer
            unsigned char command[] = {FLAG, A1, SET, A1^SET, FLAG};
            if (sendCommandMessage(fd, command) == -1) return -1; // send SET
            if (receiveCommandMessage(fd, UA) == 0) {
                alarm(0);
                return 0;
            }
        }
        return -1;
    }
    else {
        if (receiveCommandMessage(fd, SET) == -1) return -1;
        unsigned char command[] = {FLAG, A0, UA, A0^UA, FLAG};
        if (sendCommandMessage(fd, command) == -1) return -1;
    }

    return 0;
}

int 
llclose(int fd, int role) {
    if (role != TRANSMITTER && role != RECEIVER) {
        char *error = "role belongs to {TRANSMITTER, RECEIVER}"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); 
        return -1; 
    }

    if (role == TRANSMITTER) {
        unsigned char command[] = {FLAG, A1, DISC, A1^DISC, FLAG};
        if (sendCommandMessage(fd, command) == -1) return -1;
        if (receiveCommandMessage(fd, DISC) == -1) return -1;
        unsigned char command1[] = {FLAG, A1, UA, A1^UA, FLAG};
        if (sendCommandMessage(fd, command1) == -1) return -1;
    } 
    else {
        if (receiveCommandMessage(fd, DISC) == -1) return -1;
        unsigned char command[] = {FLAG, A0, DISC, A0^DISC, FLAG};
        if (sendCommandMessage(fd, command) == -1) return -1;
        if (receiveCommandMessage(fd, UA) == -1) return -1;
    }

    if (tcsetattr(fd, TCSANOW, &oldtio) == -1) {
        char *error = "tcsetattr()"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error);
        return -1; 
    }
    if (close(fd) == -1) {
        char *error = "close()"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); 
        return -1; 
    }

    return 0;
}


int 
llread(int fd, unsigned char **message) {
    message = (unsigned char **)calloc(1, sizeof(unsigned char *));
    *message = (unsigned char *)calloc(0, sizeof(unsigned char));
    char c;
    int bytes_read = 0, state = 0;

    return bytes_read;
} 


