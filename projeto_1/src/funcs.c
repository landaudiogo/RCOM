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

unsigned char
receiveFrame(int fd, unsigned char **message, int *message_size) {
    unsigned char c;
    int state = 0;
    unsigned char header[2] = {0,0}; 

    unsigned char *aux=calloc(0, 1); // used to assign the datafield which will be passed on to *message
    int size_aux, idx=0; 
    
    while (state != STOP && FLAG_ALARM != 1) { // stop the cycle if alarm is raised or STOP state is reached
        if (read(fd, &c, 1) <= 0)  {
            continue;
        }
        switch (state) {
            // state: condition
            //   FLAG_RCV: FLAG 
            //   START: !FLAG
            case START: 
                state = FLAG_RCV*(c == FLAG);
                break;
                
            //   FLAG_RCV: FLAG 
            //   A_RCV: {A0, A1}
            //   START: anything else
            case FLAG_RCV:;
                unsigned char set[] = {A0, A1};
                state = FLAG_RCV*(c == FLAG)
                      + A_RCV*(in_set(c, set, 2));
                header[0] = in_set(c, set, 2)*c; // storing the address
                break;

            //   FLAG_RCV: FLAG 
            //   C_RCV: {SET, DISC, UA, RR0, RR1, REJ0, REJ1, IC0, IC1}
            //   START: anything else
            case A_RCV:;
                unsigned char set1[] = {SET, DISC, UA, RR0, RR1, REJ0, REJ1, IC0, IC1};
                state = FLAG_RCV*(c == FLAG)
                      + C_RCV*(in_set(c, set1, 9));
                header[1] = in_set(c,set1, 9)*c; // storing the command
                break;

            //   FLAG_RCV: FLAG 
            //   BCC_OK: {SET, DISC, UA, RR0, RR1, REJ0, REJ1, IC0, IC1}
            //   START: anything else
            case C_RCV:
                state = FLAG_RCV*(c == FLAG)
                      + BCC_OK*((header[0]^header[1]) == c);
                break;

            //   if header[1] in {IC0, IC1}
            //      STOP: FLAG
            //      BCC_OK: Anything else (this state is used to store the datafield)
            //   else:
            //      STOP: FLAG
            //      START: Anything else 
            case BCC_OK:;
                unsigned char set2[] = {IC0, IC1};
                if (in_set(header[1], set2, 2)) { // if the frame received is an information frame
                    state = STOP*(c == FLAG)
                          + BCC_OK*(c != FLAG);
                    aux = realloc(aux, ++idx);
                    aux[idx-1] = c;
                } 
                else { // is another command message not in (IC0, IC1)
                    state = START*(c != FLAG)
                          + STOP*(c == FLAG);
                    free(aux); aux = NULL; // variable not used
                }
                break;
        }
    }

    *message = aux; 
    *message_size = idx;
    return header[1]; // command message received
}


int
BCC2_check(unsigned char *data, int size) {
    unsigned char res=0x0;
    if (size <= 2) { // cannot perform BCC considering the last 2 bytes belong to the FLAG and BCC
        char *error = "Not enough bytes received to perform BCC2_check"; 
        fprintf(stderr, RED "\nModule: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); 
        return -1;
    }
    for (int i=0; i < size-2; i++) {
        res^=data[i];
    }
    return (res == data[size-2]) ? 0 : -1;
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
        fprintf(stderr, RED "Module: %s\nFunction: %s()\nError: %s\n\n" RESET, __FILE__, __func__, error); 
        return NULL; 
    }

    unsigned char *file_data;
    struct stat st;
    stat(file_path, &st); *file_size = st.st_size; // get file size
    file_data = calloc(*file_size, 1); 
    fread(file_data, 1, *file_size, f);

    fclose(f);
    return file_data;
}


unsigned char
*stuff(unsigned char *original, int og_size, int *stuffed_size) {
    unsigned char *stuffed = calloc(og_size, 1);
    *stuffed_size = og_size;
    int idx_og=0, idx_st=0; // indices for the arrays (st -> stuffed, og -> original)

    for (idx_og=0; idx_og < og_size; idx_og++) {
        unsigned char set[]={FLAG, ESCAPE};
        if (in_set(original[idx_og], set, 2))  {  // if it belongs in {FLAG, ESCAPE}
            stuffed = realloc(stuffed, (++(*stuffed_size)) );
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
    unsigned char *original = calloc(stuffed_size, stuffed_size);
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

    original = realloc(original, *og_size);
    return original;
}


unsigned char
*build_command_header(unsigned char C, int role) {
    unsigned char *header = calloc(4, 1);
    header[0] = FLAG;
    header[1] = (role == TRANSMITTER) ? A1 : A0;
    header[2] = C;
    header[3] = header[1]^header[2];
    return header;
}


unsigned char 
*slice(unsigned char *data, int st, int end) { // end is not inclusive
    unsigned char *res = calloc(end-st, 1);
    for (int i=0; i<end-st; i++) {
        res[i] = data[st+i];
    }
    return res;
}


void
print_array(unsigned char *array, int size) {
    printf("\n[");
    for (int i=0; i<size; i++) {
        if (i<size-1) printf("%x, ", array[i]);
        else printf("%x", array[i]);
    }
    printf("]\n");
}


