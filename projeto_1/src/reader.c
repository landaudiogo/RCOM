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
    unsigned char c;
    int stuffed_size = MAX_PACKET_SIZE, message_size = 0;
    unsigned char *stuffed = calloc(stuffed_size,1);
    unsigned char expected_seq = IC0;
    int idx = 0;

    while(1) {
        int state = START;
        int ignore_flag = 0;
        unsigned char header[4];
        while (state != STOP) {
            if (read(fd, &c, 1) <= 0)  {
                continue;
            }
            switch (state) {
                case START:
                    state = (c == FLAG) ? FLAG_RCV : 0;
                    header[0] = (c == FLAG) ? c : 0;
                    break;
                case FLAG_RCV:;
                    unsigned char set[] = {A0, A1};
                    state = FLAG_RCV*(c == FLAG)
                          + A_RCV*(in_set(c, set, 2));
                    header[1] = (in_set(c, set, 2)) ? c : 0;
                    break;
                case A_RCV:;
                    unsigned char set1[] = {SET, DISC, UA, RR0, RR1, REJ0, REJ1, IC0, IC1};
                    state = FLAG_RCV*(c == FLAG)
                          + C_RCV*(in_set(c, set1, 9));  // any other control message
                    header[2] = (in_set(c, set1, 9)) ? c : 0;
                    break;
                case C_RCV: // BCC CHECK
                    state = FLAG_RCV*(c == FLAG)
                          + BCC_OK*((header[1]^header[2]) == c);
                    break;
                case BCC_OK: // receive information
                    if (expected_seq != header[2]) break; // if 
                    if (idx >= stuffed_size) {
                        stuffed = realloc(stuffed, (stuffed_size + MAX_PACKET_SIZE));
                        stuffed_size += MAX_PACKET_SIZE;
                    }
                    stuffed[idx++] = c;
                    state = STOP*(c == FLAG)
                          + BCC_OK*(c != FLAG);
                    break;
            }
        }
        stuffed_size = idx;
        stuffed = realloc(stuffed, stuffed_size);
        unsigned char BCC = 0x0;
        for (int i = 0; i< stuffed_size-2; i++) BCC ^= stuffed[i];
        if(BCC == stuffed[stuffed_size-2]) {
            message_size += stuffed_size-2; // info bytes - 1 header - 1 BCC
            *message = realloc(*message, stuffed, stuffed_size-2);
            expected_seq ^= (IC0^IC1);
        } else {
            if (BCC)
            unsigned char reply[] = {FLAG, A1, REJ}
            write(fd, )
        }
    }
}

