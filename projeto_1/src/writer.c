#include "../headers/writer.h"
#include "../headers/linklayer.h"

#include <signal.h>
#include <string.h>

unsigned char *dummy_message;
int dummy_size;

// definition of external variable to be carried to other functions
int fd, frame_sequence = 0;

int 
<<<<<<< HEAD
main(int argc, char **argv) {
    if (argc<=2 || strncmp(argv[1], "/dev/pts/", 9) != 0) {
        printf("Expecting 2 arguments:\n1) serial port (/dev/pts/N)\n2) file path to be transfered (../penguin.gif)\n");
        exit(1);
    }

    int fd;
    if ( (fd = open(argv[1], O_RDWR | O_NOCTTY)) == -1) {
        perror(argv[1]); exit(1); }
    
    // 1) set the connection
    if (llopen(fd, role) == -1) {
        char *error = "Failed to establish connection"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); 
        exit(1); 
    }
    printf("LLOPEN: [" GREEN "OK" RESET "]\n\n\n");


    // 2) sending the file
    // 2.1) get the file information 
    char *file_name, *tok;
    int original_size;
    unsigned char *original = readFile(argv[2], &original_size);

    tok = strtok(argv[2], "/");
    while (tok != NULL) { file_name = tok; tok = strtok(NULL, "/"); } // filename stored in file_name
    printf("%s: %d\n", file_name, original_size);

    // 2.2) send the data
    if (llwrite(fd, original, original_size) ) {
        char *error = "Failed to write"; 
        fprintf(stderr, RED "\n\nModule: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error);
    }
    free(original); original = NULL;
    
    // close the connection 
    if (llclose(fd, role) == -1) {
        char *error = "Failed to close connection"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); 
        return -1;
    }
    printf("LLCLOSE: [" GREEN "OK" RESET "]\n\n\n");
    return 0;
}


int 
llwrite(int fd, unsigned char *buffer, int buffer_size) {
    int num_packets = (buffer_size % MAX_PACKET_SIZE) ?  (int)buffer_size/MAX_PACKET_SIZE+1 : (int)buffer_size/MAX_PACKET_SIZE;
    int last_byte_sent = 0;

    printf("num_packets: %d\n", num_packets);

    for (int i=0; i<num_packets; i++) {
        initialize_alarm();

        int num_bytes_to_send = ((buffer_size - last_byte_sent) > MAX_PACKET_SIZE) ? MAX_PACKET_SIZE : (buffer_size - last_byte_sent);
        // BUILD FRAME
        // -header
        unsigned char C = (i%2 == 0) ? IC0 : IC1;
        int header_size = 4;
        unsigned char *header = build_command_header(C, role);
        // -data field
        int data_field_size = num_bytes_to_send;
        unsigned char *data_field = slice(buffer, last_byte_sent, last_byte_sent + data_field_size);
        // --creating BCC & stuffing
        unsigned char BCC = 0x0;
        for (int j=0; j<data_field_size; j++) BCC ^= data_field[j];
        int data_BCC_size = data_field_size+1;
        unsigned char *data_BCC;
        data_BCC = realloc(data_field, data_BCC_size);
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

        while (retry < MAX_RETRY) {
            FLAG_ALARM = 0;
            write(fd, frame, frame_size); // send the frame
            alarm(TIMEOUT); // start the timer
            
            unsigned char ack = (i%2 == 1) ? RR0 : RR1;
            unsigned char nack = (i%2 == 0) ? REJ0 : REJ1; // received when the data is corrupted
            unsigned char command = receiveFrame(fd, &dummy_message, &dummy_size);
            free(dummy_message);
            if (command == nack) { // repeat transmission
                alarm(0);
                printf(" === received nack ===\n");
            }
            else if (command == ack) { // move on to next transmission
                alarm(0);
                last_byte_sent += num_bytes_to_send;
                break;
=======
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
            stats.nacks++;
            printf(" === received nack ===\n");
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
>>>>>>> 2c90c30f53eee45ca93f705c7f9fadbe862de3f1
            }
        }
    }
    free(frame); return 0;
}



