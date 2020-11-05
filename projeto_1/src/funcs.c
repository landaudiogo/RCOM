#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <strings.h>

#include "../headers/funcs.h"

int FLAG_ALARM;

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
    while (state != STOP && FLAG_ALARM != 1) {
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
    return (state == STOP && FLAG_ALARM != 1) ? 0 : -1;
}


int
in_set(unsigned char value, unsigned char *set, int size) {
    for (int i=0; i<size; i++) { if (value == set[i]) return 1; }
    return 0;
}
