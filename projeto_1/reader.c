#include "reader.h"
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

    llopen(fd, RECEIVER);

    char *ctrl_message;
    int ctrl_message_size;
    ctrl_message = llread(fd, &ctrl_message_size);

    llclose(fd);
    return 0;
}

