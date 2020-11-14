#include "../headers/reader.h"
#include "../headers/linklayer.h"

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

