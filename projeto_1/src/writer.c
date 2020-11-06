#include "../headers/writer.h"

#include <signal.h>
#include <string.h>

int retry;
int role = TRANSMITTER;

unsigned char *dummy_message;
int dummy_size;


int 
main(int argc, char **argv) {
    int fd;
    struct termios oldtio;

    if (argc<=2 || strncmp(argv[1], "/dev/pts/", 9) != 0) {
        printf("Expecting 2 arguments:\n1) serial port (/dev/pts/N)\n2) file path to be transfered (../penguin.gif)\n");
        exit(1);
    }

    // 1) set the connection
    if ( (fd = open(argv[1], O_RDWR | O_NOCTTY)) == -1) {
        perror(argv[1]); exit(1); }

    if (llopen(fd, role) == -1) {
        char *error = "Failed to establish connection"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); 
        exit(1); 
    }
    printf("LLOPEN: [" GREEN "OK" RESET "]\n\n\n");


    // 2) sending the file
    // 2.1) get the file information 
    char *file_name, *tok;
    int og_size;
    unsigned char *og_data = readFile(argv[2], &og_size);

    tok = strtok(argv[2], "/");
    while (tok != NULL) { file_name = tok; tok = strtok(NULL, "/"); } // determine file_name
    
    // 2.2) stuff the data to be sent
    int stuffed_size;
    unsigned char *stuffed = stuff(og_data, og_size, &stuffed_size);
    printf("%s: %d\n", file_name, og_size);
    print_array(stuffed, stuffed_size);

    // 2.3) send the data
    if (llwrite(fd, stuffed, stuffed_size) ) {
        char *error = "Failed to write"; 
        fprintf(stderr, RED "\n\nModule: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error);
    }




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
llwrite(int fd, unsigned char *stuffed, int stuffed_size) {
    int num_packets = (stuffed_size % MAX_PACKET_SIZE) ?  (int)stuffed_size/MAX_PACKET_SIZE+1 : (int)stuffed_size/MAX_PACKET_SIZE;
    int last_byte_sent = 0;

    printf("num_packets: %d\n", num_packets);

    for (int i=0; i<num_packets; i++) {
        initialize_alarm();

        int num_bytes_to_send = ((stuffed_size - last_byte_sent) > MAX_PACKET_SIZE) ? MAX_PACKET_SIZE : (stuffed_size - last_byte_sent);
        printf("to send: %d\tlast_byte_sent: %d\n", num_bytes_to_send, last_byte_sent);

        // building frame
        unsigned char *bytes_to_send = slice(stuffed, last_byte_sent, last_byte_sent + num_bytes_to_send);
        unsigned char C = (i%2 == 0) ? IC0 : IC1;

        unsigned char *header = build_command_header(C, role);
        int header_size = 4;

        // BUILD FRAME
        int frame_size = header_size + num_bytes_to_send+2; // header + info + BCC + flag
        unsigned char *frame = calloc(frame_size,1);
        memcpy(frame, header, header_size);
        memcpy(frame+4, bytes_to_send, num_bytes_to_send);
        free(bytes_to_send); free(header);
        unsigned char BCC = 0x0;
        for (int j=4; j<frame_size-2; j++) BCC ^= frame[j];
        frame[frame_size-2] = BCC;
        frame[frame_size-1] = FLAG;
        

        while (retry < MAX_RETRY) {
            FLAG_ALARM = 0;
            alarm(TIMEOUT); // start the timer
            write(fd, frame, frame_size);
            unsigned char ack = (i%2 == 1) ? RR0 : RR1;
            unsigned char nack = (i%2 == 0) ? REJ0 : REJ1; // received when the data is corrupted
            unsigned char command = receiveFrame(fd, &dummy_message, &dummy_size);
            if (command == nack) { // repeat transmission
                alarm(0);
                printf("received nack\n");
            }
            else if (command == ack) { // move on to next transmission
                alarm(0);
                printf("received ack\n");
                last_byte_sent += num_bytes_to_send;
                break;
            }
            else if (retry == MAX_RETRY) return -1;
        }
    }
    



}



