#include "../headers/reader.h"
#include <string.h>

int role = RECEIVER; 

int 
main(int argc, char **argv) {
    if (argc<=1 || (strncmp(argv[1], "/dev/pts/", 9) != 0 && strncmp(argv[1], "/dev/tty", 8) != 0)) {
        char *error = "Arguments expected to be of type:\n1) /dev/pts/N or /dev/ttyXX (where N represents a postive Integer)\n2) relative or absolute file_path to the file\n"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError -> %s\n\n" RESET, __FILE__, __func__, error); 
        exit(1);
    }

    int fd; // file descriptor for the serial port
    if ( (fd = open(argv[1], O_RDWR | O_NOCTTY)) == -1) {
        perror(argv[1]); exit(1); }

    // set a connection
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
llread(int fd, unsigned char **buffer) {
    int original_size = 0;
    unsigned char *original = NULL;
    unsigned char expected_seq = IC0;
    int idx = 0;

    while(1) { // define a control packet to break this cycle
        int message_size=0;
        unsigned char *message=NULL;
        unsigned char ctrl = receiveFrame(fd, &message, &message_size); // message has the stuffed info
        unsigned char *aux = message; // going to be used to free original message
        message = de_stuff(message, message_size, &message_size); free(aux);

        if (ctrl == expected_seq) { // is the information frame we expected
            if (BCC2_check(message, message_size) == 0) { // BCC2_check returned OK
                message_size -= 2; // size not including FLAG and BCC2
                original = realloc(original, original_size+=message_size);
                memcpy(original+idx, message, message_size);

                expected_seq ^= (IC0^IC1);
                idx+=message_size;
                
                // sending ACK
                unsigned char RR = (ctrl == IC0) ? RR1 : RR0;
                unsigned char command1[] ={FLAG, A1, RR, A1^RR, FLAG};
                write(fd, command1, 5);
            }
            else { // BCC2 check FAILED
                // sending NACK
                unsigned char REJ = (ctrl == IC0) ? REJ0 : REJ1;
                unsigned char command1[] ={FLAG, A1, REJ, A1^REJ, FLAG};
                write(fd, command1, 5);
                break; 
            }
        }
        else if (ctrl == (expected_seq^(IC0^IC1)) ) { // is the duplicate information frame
            // sending ACK for a repeated packet
            unsigned char RR = (ctrl == IC0) ? RR1 : RR0;
            unsigned char command1[] ={FLAG, A1, RR, A1^RR, FLAG};
            write(fd, command1, 5);
            printf("\n\n === repeated data === %x\n", ctrl);
        }
        else if (ctrl == SET) { // if RECEIVER left llopen and TRANSMITTER didn't receive UA 
            char *error = "TRANSMITTER did not leave llopen. Sending UA"; 
            fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error);
            unsigned char command1[] ={FLAG, A0, UA, A0^UA, FLAG};
            write(fd, command1, 5); // send UA
        }
        else { // is any other command frame (in reading mode we should not be receiving a command frame other than information)
            char *error = "undefined behaviour... ignoring"; 
            fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error);
            // print_array(original, original_size);
            printf("original_size: %d\n", original_size);
            break; 
        }
        free(message);
    }
    FILE *f = fopen("testpenguin.gif", "wb");
    fwrite(original, 1, original_size, f);
    free(original);
    return 0;
}

