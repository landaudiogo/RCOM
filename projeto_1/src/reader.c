#include "../headers/reader.h"
#include <string.h>


int 
main(int argc, char **argv) {
    int fd;
    struct termios oldtio;

    if (argc<=1 || strncmp(argv[1], "/dev/pts/", 9) != 0) {
        printf("Expected first argument must be of type: \"/dev/pts/*\"\n");
        exit(1);
    }


    // set a connection
    if ( (fd = open(argv[1], O_RDWR | O_NOCTTY)) == -1) {
        perror(argv[1]); exit(1); }

    if (llopen(fd, RECEIVER) == -1) {
        char *error = "Failed to establish connection"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); 
        exit(1);
    }
    printf("LLOPEN: \t[" GREEN "OK" RESET "]\n\n\n");


    // receive the file



    // close the connection
    if (llclose(fd, TRANSMITTER) == -1) {
        char *error = "Failed to close connection"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); 
        exit(1); 
    }
    printf("LLCLOSE: \t[" GREEN "OK" RESET "]\n\n\n");
    return 0;
}

