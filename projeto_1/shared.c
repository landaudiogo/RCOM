#include "shared.h"
#include <unistd.h>

// state machine 
#define START    0
#define FLAG_RCV 1
#define A_RCV    2
#define C_RCV    3
#define BCC_OK   4
#define I_RCV    5
#define BCC2_OK  6
#define STOP     7


struct termios newtio, oldtio;

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

    // defining the new termios structure
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
        unsigned char command[] = {FLAG, A1, SET, A1^SET, FLAG};
        if (sendCommandMessage(fd, command) == -1) return -1; 
        if (receiveCommandMessage(fd, UA) == -1) return -1;
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


    unsigned char address = 0, ctrl = 0;
    while (state != STOP) {
        if (read(fd, &c, 1) <= 0)  {
            continue;
        }
        switch (state) {
            case START:
                state = (c == FLAG) ? 1 : 0;
                break;
            case FLAG_RCV:;
                unsigned char set[] = {A0, A1};
                state = FLAG_RCV*(c == FLAG)
                      + A_RCV*(in_set(c, set, 2));
                address = in_set(c, set, 2) ? c : 0;
                break;
            case A_RCV:;
                unsigned char set1[] = {SET, DISC, UA, RR0, RR1, REJ0, REJ1};
                state = FLAG_RCV*(c == FLAG)
                      + C_RCV*(in_set(c, set1, 7));
                ctrl = in_set(c, set1, 7) ? c : 0;
                break;
            case C_RCV:
                state = FLAG_RCV*(c == FLAG)
                      + BCC_OK*((address^ctrl) == c);
                break;
            case BCC_OK:
                state = STOP*(c == FLAG);
                break;
            case I_RCV:
                break;
            case BCC2_OK:
                break;
            case STOP:
                break;

        }
    }
    return bytes_read;
}


int
sendCommandMessage(int fd, unsigned char *ctrl_message) {
    int b_written = write(fd, ctrl_message, 5);
    if (b_written < 0) {
        char *error = "write()"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); 
        return -1; 
    }
    printf("wrote %d bytes\n", b_written); 
    printf("%x %x %x %x %x\n\n", ctrl_message[0], ctrl_message[1], ctrl_message[2], ctrl_message[3], ctrl_message[4]);
    return 0;
}


int 
receiveCommandMessage(int fd, unsigned char command) {
    unsigned char c;
    int state = 0;
    
    unsigned char address = 0, ctrl = 0;
    while (state != STOP) {
        if (read(fd, &c, 1) <= 0)  {
            continue;
        }
        switch (state) {
            case START:
                state = (c == FLAG) ? 1 : 0;
                break;
            case FLAG_RCV:;
                unsigned char set[] = {A0, A1};
                state = FLAG_RCV*(c == FLAG)
                      + A_RCV*(in_set(c, set, 2));
                address = in_set(c, set, 2) ? c : 0;
                break;
            case A_RCV:;
                state = FLAG_RCV*(c == FLAG)
                      + C_RCV*(c == command);
                ctrl = (c == command) ? c : 0;
                if (state == 0) return -1;
                break;
            case C_RCV:
                state = FLAG_RCV*(c == FLAG)
                      + BCC_OK*((address^ctrl) == c);
                break;
            case BCC_OK:
                state = STOP*(c == FLAG);
                break;
        }
    }
    return 0;
}


int
in_set(unsigned char value, unsigned char *set, int size) {
    for (int i=0; i<size; i++) {
        if (value == set[i]) return 1;
    }
    return 0;
}
