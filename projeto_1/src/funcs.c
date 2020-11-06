#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <strings.h>
#include <sys/stat.h>

#include "../headers/funcs.h"

int FLAG_ALARM;

int
sendCommandMessage(int fd, unsigned char *ctrl_message) {
    int b_written = write(fd, ctrl_message, 5);
    if (b_written < 0) {
        char *error = "write()"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); 
        return -1; 
    }
    printf("wrote %d bytes\n", b_written); 
    printf("%x %x %x %x %x\n\n", ctrl_message[0], ctrl_message[1], ctrl_message[2], ctrl_message[3], ctrl_message[4]);
    return 0;
}


int 
receiveCommandMessage(int fd, unsigned char command) {
    unsigned char c;
    int state = 0;
    
    unsigned char address = 0, ctrl = 0;
    while (state != STOP && FLAG_ALARM != 1) {
        if (read(fd, &c, 1) <= 0)  {
            continue;
        }
        switch (state) {
            case START:
                state = (c == FLAG) ? 1 : 0;
                break;
            case FLAG_RCV:;
                unsigned char set[] = {A0, A1};
                state = FLAG_RCV*(c == FLAG)
                      + A_RCV*(in_set(c, set, 2));
                address = in_set(c, set, 2) ? c : 0;
                break;
            case A_RCV:;
                state = FLAG_RCV*(c == FLAG)
                      + C_RCV*(c == command);
                ctrl = (c == command) ? c : 0;
                if (state == 0) return -1;
                break;
            case C_RCV:
                state = FLAG_RCV*(c == FLAG)
                      + BCC_OK*((address^ctrl) == c);
                break;
            case BCC_OK:
                state = STOP*(c == FLAG);
                break;
        }
    }
    return (state == STOP && FLAG_ALARM != 1) ? 0 : -1;
}


int
in_set(unsigned char value, unsigned char *set, int size) {
    for (int i=0; i<size; i++) { if (value == set[i]) return 1; }
    return 0;
}


unsigned char 
*readFile(char *file_path, int *file_size) {
    FILE *f = fopen(file_path, "rb");
    if (f == NULL) {
        char *error = "fopen()"; 
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); return NULL; 
    }

    char *file_data;
    struct stat st;

    stat(file_path, &st); *file_size = st.st_size; // get file size
    file_data = (unsigned char *) calloc(*file_size, sizeof(unsigned char));
    fread(file_data, 1, *file_size, f);

    return file_data;
}


unsigned char
*stuff(unsigned char *original, int og_size, int *stuffed_size) {
    unsigned char *stuffed = (unsigned char *) calloc(og_size, sizeof(unsigned char));
    *stuffed_size = og_size;
    int idx_og=0, idx_st=0; // indices for the arrays (st -> stuffed, og -> original)

    for (idx_og=0; idx_og < og_size; idx_og++) {
        unsigned char set[]={FLAG, ESCAPE};
        if (in_set(original[idx_og], set, 2))  {
            stuffed = (unsigned char *) realloc(stuffed, (++(*stuffed_size))*sizeof(unsigned char) );
            stuffed[idx_st] = ESCAPE;
            stuffed[++idx_st] = XOR_BYTE^original[idx_og];
        } else {
            stuffed[idx_st] = original[idx_og];
        }
        idx_st++;
    }

    return stuffed;
}


unsigned char
*de_stuff(unsigned char *stuffed, int stuffed_size, int *og_size) {
    unsigned char *original = (unsigned char *) calloc(stuffed_size, stuffed_size*sizeof(unsigned char));
    *og_size = stuffed_size;
    int idx_og=0, idx_st=0, times=0;

    for (idx_st=0; idx_st < stuffed_size; idx_st++) {
        if (stuffed[idx_st] == ESCAPE) {
            original[idx_og] = stuffed[++idx_st]^XOR_BYTE; // second byte
            (*og_size)--;
            times++;
        }
        else {
            original[idx_og] = stuffed[idx_st]; 
        }
        idx_og++;
    }

    return original;
}


unsigned char
*build_command_header(unsigned char C, int role) {
    unsigned char *header = (unsigned char *) calloc(4, sizeof(unsigned char));

    header[0] = FLAG;
    header[1] = (role == TRANSMITTER) ? A1 : A0;
    header[2] = C;
    header[3] = header[1]^header[2];
}





