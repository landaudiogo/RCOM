#include "../headers/shared.h"

#include <unistd.h>
#include <signal.h>
#include <time.h>

struct termios oldtio, newtio;
int retry;
int FLAG_ALARM=0;

// useful when calling receiveFrame for a control frame
unsigned char *dummy_message; 
int dummy_size; 
int fd; 

linkLayer linkRole;
Statistics stats = {0,0,0,0};

void 
alarm_handler() {
    char *error = "ALARM"; 
    fprintf(stderr, RED "== %s ==\n" RESET, error); 
    retry++;
    FLAG_ALARM = 1;
    (stats.alarms)++;
}

void 
initialize_alarm() {
    signal(SIGALRM, alarm_handler);
    FLAG_ALARM = 0;
    retry = 0;
}

int
llopen(linkLayer connectionParameters) { 
    if (connectionParameters.role != TRANSMITTER && connectionParameters.role != RECEIVER) {
        char *error = "role belongs to {TRANSMITTER, RECEIVER}"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); return -1; 
    }
    // store the information on the old settings to be reset at the end of transmission
    if ( (fd = open(connectionParameters.serialPort, O_RDWR | O_NOCTTY)) == -1) return -1;
    if (tcgetattr(fd, &oldtio) == -1) {
        char *error = "tcgetattr()"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); 
        return -1; 
    }
    // new termios structure
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE_DEFAULT | CS8 | CLOCAL | CREAD;
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
    if (connectionParameters.role == TRANSMITTER) {
        initialize_alarm(); 
        while(retry <= MAX_RETRY) {
            FLAG_ALARM = 0;
            unsigned char command[] = {FLAG, A1, SET, A1^SET, FLAG};
            if (write(fd, command, 5) == -1) return -1; // send SET
            alarm(TIMEOUT); 
            if (receiveFrame(fd, &dummy_message, &dummy_size) == UA) { // receive UA
                alarm(0);
                free(dummy_message); dummy_message = NULL; 
                linkRole = connectionParameters;
                return 0;
            }
            free(dummy_message); dummy_message = NULL; 
        }
        return -1;
    }
    else if (connectionParameters.role == RECEIVER) {
        if (receiveFrame(fd, &dummy_message, &dummy_size) != SET) return -1; // receive SET
        free(dummy_message); dummy_message = NULL; 
        unsigned char command[] = {FLAG, A0, UA, A0^UA, FLAG};
        if (write(fd, command, 5) == -1) return -1; // send UA
        linkRole = connectionParameters;
        return 0;
    }
    return -1;

}

int 
llclose(int showStatistics) {
    // printing statistics
    if (showStatistics) {
        fprintf(stdout, "\n\n===========================================\n===========================================\n");
        fprintf(stdout, "[" GREEN "ACKS" RESET "]:  \t %d\n", stats.acks);
        // fprintf(stdout, "[" GREEN "ELAPSED TIME" RESET "]: %f\n", secs);
        if (linkRole.role == RECEIVER)
            fprintf(stdout, "[" RED "REPEATED_ACKS" RESET "]: %d\n", stats.repeated_acks);
        fprintf(stdout, "[" RED "NACKS" RESET "]:  \t %d\n", stats.nacks);
        fprintf(stdout, "[" RED "ALARMS" RESET "]:  \t %d\n", stats.alarms);
        fprintf(stdout, "===========================================\n===========================================\n\n");
    }
    if (linkRole.role != TRANSMITTER && linkRole.role != RECEIVER) {
        char *error = "role belongs to {TRANSMITTER, RECEIVER}"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); 
        return -1; 
    }

    if (linkRole.role == TRANSMITTER) {
        unsigned char command[] = {FLAG, A1, DISC, A1^DISC, FLAG};
        
        initialize_alarm();
        while (retry < MAX_RETRY) { 
            FLAG_ALARM = 0;
            if (write(fd, command, 5) == -1) return -1; // send DISC
            alarm(TIMEOUT);
            if (receiveFrame(fd, &dummy_message, &dummy_size) == DISC) { // receive DISC
                alarm(0);
                unsigned char command1[] = {FLAG, A1, UA, A1^UA, FLAG};
                if (write(fd, command1, 5) == -1) return -1; // send UA
                free(dummy_message); dummy_message = NULL; 
                return 0;
            } 
            free(dummy_message); dummy_message = NULL; 
        }
        return -1;
    } 
    else if (linkRole.role == RECEIVER) {
        if (receiveFrame(fd, &dummy_message, &dummy_size) != DISC) return -1; // receive DISC
        free(dummy_message); dummy_message = NULL; 
        unsigned char command[] = {FLAG, A0, DISC, A0^DISC, FLAG};

        initialize_alarm();
        while (retry < MAX_RETRY) { 
            FLAG_ALARM = 0;
            if (write(fd, command, 5) == -1) return -1; // send DISC
            alarm(TIMEOUT);
            if (receiveFrame(fd, &dummy_message, &dummy_size) == UA) { // receive UA
                alarm(0);
                free(dummy_message); dummy_message = NULL; 
                return 0;
            }
            free(dummy_message); dummy_message = NULL; 
        }
        return -1; // exceeded retransmission times
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


