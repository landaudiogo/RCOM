#include "../headers/reader.h"
<<<<<<< HEAD
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
=======
#include "../headers/linklayer.h"
>>>>>>> 2c90c30f53eee45ca93f705c7f9fadbe862de3f1

#include <string.h>
// definition of external variables
int fd, expected_seq = IC0;

int 
llread(char *packet) {
    while (1) {
        int message_size=0;
        unsigned char *message=NULL;
        unsigned char ctrl = receiveFrame(fd, &message, &message_size); // message has the stuffed info
        int packet_size = 0; 

        if (ctrl == expected_seq) { // is the information frame we expected
            unsigned char BCC;
            if (de_stuff(message, message_size, packet, &packet_size, &BCC) == -1) { // this only occurs when a frame is badly delimited
                free(message);
                continue;
            }
            if (BCC2_check(packet, packet_size, BCC) == TRUE) { // BCC2_check returned OK
                expected_seq ^= (IC0^IC1);
                // sending ACK
                unsigned char RR = (ctrl == IC0) ? RR1 : RR0;
                unsigned char command1[] ={FLAG, A1, RR, A1^RR, FLAG};
                write(fd, command1, 5);
                free(message);
                stats.acks++;
                return packet_size;
            }
            else { // BCC2 check FAILED
                // sending NACK
                unsigned char REJ = (ctrl == IC0) ? REJ0 : REJ1;
                unsigned char command1[] ={FLAG, A1, REJ, A1^REJ, FLAG};
                write(fd, command1, 5);
                stats.nacks++;
                return -1;
            }
        }
        else if (ctrl == (expected_seq^(IC0^IC1)) ) { // is a duplicate information frame
            // sending ACK for a previous packet
            unsigned char RR = (ctrl == IC0) ? RR1 : RR0;
            unsigned char command1[] ={FLAG, A1, RR, A1^RR, FLAG};
            write(fd, command1, 5);
            stats.repeated_acks++;
            printf("\n\n---repeated data--- %x\n", ctrl);
        }
        else if (ctrl == SET) { // in case tx is trying to re-establish communication
            char *error = "TRANSMITTER did not leave llopen. Sending UA"; 
            fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error);
            unsigned char command1[] ={FLAG, A0, UA, A0^UA, FLAG};
            write(fd, command1, 5); // send UA
        }
        else { // is any other command frame (in reading mode we should not be receiving a command frame other than information)
            char *error = "undefined behaviour... ignoring"; 
            fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error);
            free(message); // in case a message 
            return -1; 
        }
    }
}

