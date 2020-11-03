#include "shared.h"

int
llopen(int fd) {
    // store the information on the old settings to be reset at the end of transmission
    tcgetattr(fd, &oldtio);

    // defining the new termios structure
    bzero(newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    // defining the control characters
    newtio.c_cc[VMIN] = 0; // disables return when a certain amount of bytes is received
    newtio.c_cc[VTIME] = 1; // 0.1 seconds read timeout

    tcflush(fd, TCIOFLUSH);
    
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
