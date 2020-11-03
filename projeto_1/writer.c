
#include "writer.h"
#include <string.h>




struct termios oldtio, newtio;

int main(int argc, char **argv) {
    int fd;
    struct termios oldtio;

    if (argc<1 || strncmp(argv[1], "/dev/pts/", 9) != 0) {
        printf("Expected first argument must be of type: \"/dev/pts/*\"\n");
    }
    // assign the file descriptor from the filepath given
    if ( (fd = open(argv[1], O_RDWR | O_NOCTTY)) == -1) {
        perror(argv[1]); exit(1); }

    // 
    llopen(fd, TRANSMITTER);


    tcsetattr(fd, TCSANOW, &oldtio);
    close(fd)
    return 0;
}

int
llopen(int fd, int role) {
    // store the information on the old settings to be reset at the end of transmission
    if (tcgetattr(fd, &oldtio) == -1) {
        fprintf(stderr, "llopen(): Failed tcgetattr()\n"); return -1; }

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

    if (tcflush(fd, TCIOFLUSH) == -1) {
        fprintf(stderr, "llopen(): Failed tcflush()\n"); return -1; }
    
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
