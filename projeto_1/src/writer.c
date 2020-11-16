#include "../headers/writer.h"
#include "../headers/linklayer.h"

#include <signal.h>
#include <string.h>

unsigned char *dummy_message;
int dummy_size;

// definition of external variable to be carried to other functions
int fd, frame_sequence = 0;

int 
llwrite(char *buffer, int buffer_size) {
    initialize_alarm();

    // BUILD FRAME
    // -header
    unsigned char C = (frame_sequence%2 == 0) ? IC0 : IC1;
    int header_size = 4;
    unsigned char *header = build_command_header(C, linkRole.role);
    // -data field
    int data_field_size = buffer_size;
    unsigned char *data_field = buffer;
    // --creating BCC & stuffing
    unsigned char BCC = 0x0;
    for (int j=0; j<data_field_size; j++) BCC ^= data_field[j];
    int data_BCC_size = data_field_size+1;
    unsigned char *data_BCC = calloc(1, data_BCC_size);
    memcpy(data_BCC, buffer, buffer_size);
    data_BCC[data_BCC_size-1] = BCC;
    // stuffing
    int data_stuffed_size;
    unsigned char *data_stuffed;
    data_stuffed = stuff(data_BCC, data_BCC_size, &data_stuffed_size); free(data_BCC);
    // header + stuffed + FLAG
    int frame_size = 4+data_stuffed_size+1;
    unsigned char *frame = header;
    frame = realloc(frame, frame_size); 
    memcpy(frame+4, data_stuffed, data_stuffed_size); free(data_stuffed);
    frame[frame_size-1] = FLAG; // terminate the data field

    while (retry < MAX_RETRANSMISSIONS_DEFAULT) {
        FLAG_ALARM = 0;
        alarm(TIMEOUT_DEFAULT); // start the timer
        write(fd, frame, frame_size); // send the frame
        unsigned char ack = (frame_sequence%2 == 1) ? RR0 : RR1;
        unsigned char nack = (frame_sequence%2 == 0) ? REJ0 : REJ1; // received when the data is corrupted
        unsigned char command = receiveFrame(fd, &dummy_message, &dummy_size);
        free(dummy_message);
        if (command == nack) { // repeat transmission
            alarm(0);
            retry = 0;
            stats.nacks++;
            notworking
            // printf(" === received nack ===\n");
        }
        else if (command == ack) { // move on to next transmission
            alarm(0);
            frame_sequence++;
            stats.acks++;
            break;
        }
        else if (retry == MAX_RETRANSMISSIONS_DEFAULT) {
            char *error = "Attempting to restart connection"; 
            fprintf(stderr, RED "\n\nModule: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error);
            if (llopen(linkRole) != 0) {
                free(frame);
                return -1;
            }
        }
    }
    free(frame); return 0;
}



