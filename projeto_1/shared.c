#include "shared.h"

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
    newtio.c_cc[VTIME] = 1; // 0.1 seconds read timeout

    if (tcflush(fd, TCIOFLUSH) == -1) {
        fprintf(stderr, "llopen(): failed tcflush()"); return -1; }
    
    return 0;
}

int 
llclose(int fd) {
    if (tcsetattr(fd, TCSANOW, &oldtio) == -1) {
        perror("tcssetattr"); exit(1); }

    if (close(fd) == -1) {
        perror("close"); exit(1); }

    return 0;
}
