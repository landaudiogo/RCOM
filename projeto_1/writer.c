#include "writer.h"
#include <string.h>


int 
main(int argc, char **argv) {
    int fd;
    struct termios oldtio;

    if (argc<1 || strncmp(argv[1], "/dev/pts/", 9) != 0) {
        printf("Expected first argument must be of type: \"/dev/pts/*\"\n");
        exit(1);
    }
    // assign the file descriptor from the filepath given
    if ( (fd = open(argv[1], O_RDWR | O_NOCTTY)) == -1) {
        perror(argv[1]); exit(1); }

    llopen(fd, TRANSMITTER);

    unsigned char controlMessage[] = {FLAG, A0, SET, A0^SET, FLAG};
    sendCommandMessage(fd, controlMessage);

    llclose(fd);
    return 0;
}
