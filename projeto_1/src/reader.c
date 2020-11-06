#include "../headers/reader.h"
#include <string.h>

int role = RECEIVER; 

int 
main(int argc, char **argv) {
    if (argc<=1 || strncmp(argv[1], "/dev/pts/", 9) != 0) {
        printf("Expected first argument must be of type: \"/dev/pts/*\"\n");
        exit(1);
    }

    int fd;
    
    // set a connection
    if ( (fd = open(argv[1], O_RDWR | O_NOCTTY)) == -1) {
        perror(argv[1]); exit(1); }

    if (llopen(fd, role) == -1) {
        char *error = "Failed to establish connection"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); 
        exit(1);
    }
    printf("LLOPEN: \t[" GREEN "OK" RESET "]\n\n\n");


    // receive the file
    unsigned char *message; 
    llread(fd, &message);



    // close the connection
    if (llclose(fd, role) == -1) {
        char *error = "Failed to close connection"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); 
        exit(1); 
    }
    printf("LLCLOSE: \t[" GREEN "OK" RESET "]\n\n\n");
    return 0;
}

int 
llread(int fd, unsigned char **message) {
    int stuffed_size = 0;
    unsigned char *stuffed = calloc(0,1);
    unsigned char expected_seq = IC0;
    int idx = 0;

    while(1) {
        int message_size=0;
        unsigned char *message=NULL;
        unsigned char ctrl = receiveFrame(fd, &message, &message_size);

        if (ctrl == expected_seq) { // is the information frame we expected
            printf("expected seq\n");
            if (BCC2_check(message, message_size) == 0) { // BCC2_check returned OK
                message_size -= 2; // remove header and BCC2_check
                stuffed = realloc(stuffed, stuffed_size+message_size);
                stuffed_size += message_size;
                memcpy(stuffed+idx, message, message_size);

                expected_seq ^= (IC0^IC1);
                idx+=message_size;
                
                print_array(stuffed, stuffed_size);
                unsigned char RR = (ctrl == IC0) ? RR1 : RR0;
                unsigned char command1[] ={FLAG, A1, RR, A1^RR, FLAG};
                write(fd, command1, 5);
            }
            else { // BCC_check FAILED
                unsigned char REJ = (ctrl == IC0) ? REJ0 : REJ1;
                unsigned char command1[] ={FLAG, A1, REJ, A1^REJ, FLAG};
                write(fd, command1, 5);
            }
        }
        else if (ctrl == expected_seq^(IC0^IC1)) { // is the duplicate information frame
            unsigned char RR = (ctrl == IC0) ? RR1 : RR0;
            unsigned char command1[] ={FLAG, A1, RR, A1^RR, FLAG};
            write(fd, command1, 5);
        }
        else { // is any other command frame (in reading mode we should not be receiving a command frame other than information)
            char *error = "undefined behaviour... ignoring"; 
            fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error);
        }
        free(message);
    }
    return 0;
}

