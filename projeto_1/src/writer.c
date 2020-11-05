#include "../headers/writer.h"

#include <signal.h>
#include <string.h>


int 
main(int argc, char **argv) {
    int fd;
    struct termios oldtio;

    if (argc<=2 || strncmp(argv[1], "/dev/pts/", 9) != 0) {
        printf("Expecting 2 arguments:\n1) serial port (/dev/pts/N)\n2) file path to be transfered (../penguin.gif)\n");
        exit(1);
    }


    // set the connection
    if ( (fd = open(argv[1], O_RDWR | O_NOCTTY)) == -1) {
        perror(argv[1]); exit(1); }

    if (llopen(fd, TRANSMITTER) == -1) {
        char *error = "Failed to establish connection"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); 
        exit(1); 
    }
    printf("LLOPEN: [" GREEN "OK" RESET "]\n\n\n");


    // send the file
    char *file_name, *tok;
    int og_size;
    unsigned char *og_data = readFile(argv[2], &og_size);
    tok = strtok(argv[2], "/");
    while (tok != NULL) { file_name = tok; tok = strtok(NULL, "/"); } // determine file_name
    printf("%s: %d\n", file_name, og_size);

    int stuffed_size;
    unsigned char *stuffed = stuff(og_data, og_size, &stuffed_size);
    printf("og: %d -----> st: %d\n", og_size, stuffed_size);

    int original_size;
    unsigned char *original = de_stuff(stuffed, stuffed_size, &original_size);
    printf("st: %d -----> og: %d\n", stuffed_size, original_size);



    // close the connection 
    if (llclose(fd, TRANSMITTER) == -1) {
        char *error = "Failed to close connection"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); 
        exit(1); 
    }
    printf("LLCLOSE: [" GREEN "OK" RESET "]\n\n\n");
    return 0;
}
