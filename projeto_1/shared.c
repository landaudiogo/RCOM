#include "shared.h"
#include <unistd.h>

struct termios newtio, oldtio;

int
llopen(int fd, int role) {
    // store the information on the old settings to be reset at the end of transmission
    if (tcgetattr(fd, &oldtio) == -1) {
        fprintf(stderr, "llopen(): failed tcgetattr()"); return -1; }

    // defining the new termios structure
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUD | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;
    // defining the control characters
    newtio.c_cc[VMIN] = 0; // disables return when a certain amount of bytes is received
    newtio.c_cc[VTIME] = 50; // sets an overall timer for read so it doesn't block

    if (tcflush(fd, TCIOFLUSH) == -1) {
        char *error = "tcflush()"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); return -1; 
    }
    
    if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
        char *error = "tcsetattr()"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); return -1; 
    }
    
    return 0;
}

int 
llclose(int fd) {
    if (tcsetattr(fd, TCSANOW, &oldtio) == -1) {
        char *error = "tcsetattr()"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); return -1; 
    }

    if (close(fd) == -1) {
        char *error = "close()"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); return -1; 
    }

    return 0;
}


char 
*llread(int fd, int *sizemessage) {
    int state = 0;
    char *message = (char *)calloc(5, sizeof(char));
    uint8_t headerState = 0, informationState = 0;

    while (state == 0) {
        int num_bytes_read = read(fd, message, 5);
        if (num_bytes_read != 0) {
            state++;
            printf("read %d bytes: ", num_bytes_read);
            printf("%x %x %x %x %x\n", message[4], message[3], message[2], message[1], message[0]);
        }
    }
    return message;
}


unsigned char 
*createCommand(unsigned char C, int role) {
    if (role != TRANSMITTER && role != RECEIVER) {
        char *error = "role belongs to {TRANSMITTER, RECEIVER}"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); return NULL; 
    }
    unsigned char *message = (unsigned char*)calloc(5, sizeof(unsigned char));
    message[0] = FLAG; 
    message[1] = (role == TRANSMITTER) ? A1 : A0;
    message[2] = C;
    message[3] = message[1]^message[2];
    message[4] = FLAG;
    return message;
}


int
sendCommandMessage(int fd, unsigned char *ctrl_message) {
    int b_written = write(fd, ctrl_message, 5);
    if (b_written < 0) {
        char *error = "write()"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); return -1; 
    }
    printf("wrote %d bytes\n", b_written); 
    printf("%x %x %x %x %x\n\n", ctrl_message[0], ctrl_message[1], ctrl_message[2], ctrl_message[3], ctrl_message[4]);
    return 0;
}
