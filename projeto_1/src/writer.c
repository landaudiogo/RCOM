#include "../headers/writer.h"
#include "../headers/linklayer.h"

#include <signal.h>
#include <string.h>

int retry;
int role = TRANSMITTER;

unsigned char *dummy_message;
int dummy_size;

// definition of external variable to be carried to other functions
linkLayer linkRole;
int fd;

int 
main(int argc, char **argv) {
    if (argc<=2 || strncmp(argv[1], "/dev/pts/", 9) != 0) {
        printf("Expecting 2 arguments:\n1) serial port (/dev/pts/N)\n2) file path to be transfered (../penguin.gif)\n");
        exit(1);
    }

    linkLayer connectionParams;
    if (strlen(argv[1]) < 50)
        strcpy(connectionParams.serialPort, argv[1]);
    connectionParams.role = TRANSMITTER;
    connectionParams.baudRate = BAUD;
    connectionParams.numTries = MAX_RETRY;
    connectionParams.timeOut = TIMEOUT;

    if ( (fd = open(connectionParams.serialPort, O_RDWR | O_NOCTTY)) == -1) {
        perror(argv[1]); exit(1); }
    
    // 1) set the connection
    if (llopen(connectionParams) == -1) {
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
    if (llwrite(original, original_size) ) {
        char *error = "Failed to write"; 
        fprintf(stderr, RED "\n\nModule: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error);
    }
    free(original); original = NULL;
    
    // close the connection 
    if (llclose(TRUE) == -1) {
        char *error = "Failed to close connection"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); 
        return -1;
    }
    printf("LLCLOSE: [" GREEN "OK" RESET "]\n\n\n");
    return 0;
}


int 
llwrite(char *buffer, int buffer_size) {
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
            alarm(TIMEOUT); // start the timer
            write(fd, frame, frame_size); // send the frame
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
            }
        }
        free(frame);
        if (retry == MAX_RETRY) {
            llopen(linkRole);
            i=-1;
        }
    }
    printf("finished data transmission\n");
    return 0;
}



