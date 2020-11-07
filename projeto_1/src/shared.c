#include "../headers/shared.h"

#include <unistd.h>
#include <signal.h>

struct termios newtio, oldtio;
int retry;
int FLAG_ALARM=0;


unsigned char *dummy_message;
int dummy_size;


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
    newtio.c_cc[VMIN] = 0; 
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
            unsigned char command[] = {FLAG, A1, SET, A1^SET, FLAG};
            if (write(fd, command, 5) == -1) return -1; // send SET
            alarm(TIMEOUT); // start the timer
            if (receiveFrame(fd, &dummy_message, &dummy_size) == UA) {
                alarm(0);
                return 0;
            }
        }
        return -1;
    }
    else {
        if (receiveFrame(fd, &dummy_message, &dummy_size) != SET) return -1;
        unsigned char command[] = {FLAG, A0, UA, A0^UA, FLAG};
        if (write(fd, command, 5) == -1) return -1;
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
        initialize_alarm();


        while (retry < MAX_RETRY) {
            FLAG_ALARM = 0;
            if (write(fd, command, 5) == -1) return -1;
            alarm(TIMEOUT);
            if (receiveFrame(fd, &dummy_message, &dummy_size) == DISC) {
                unsigned char command1[] = {FLAG, A1, UA, A1^UA, FLAG};
                if (write(fd, command1, 5) == -1) return -1;

                alarm(0);
                return 0;
            } 
        }
        return -1;
    } 
    else {
        if (receiveFrame(fd, &dummy_message, &dummy_size) != DISC) return -1;
        unsigned char command[] = {FLAG, A0, DISC, A0^DISC, FLAG};
        initialize_alarm();
        
        while (retry < MAX_RETRY) {
            FLAG_ALARM = 0;
            if (write(fd, command, 5) == -1) return -1;
            alarm(TIMEOUT);
            if (receiveFrame(fd, &dummy_message, &dummy_size) == UA) {
                alarm(0);
                return 0;
            }
        }

        return -1;
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


