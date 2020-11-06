#ifndef FUNCS_H
#define FUNCS_H

// colored messages
#define RESET "\033[0m" 
#define RED   "\033[31m"
#define GREEN "\033[32m"

#define BAUD B38400

#define MAX_PACKET_SIZE 4

#include "state.h"
#include "identifiers.h"

// shared variable
extern int FLAG_ALARM;
extern int retry;

/**********************************
***********************************
        INTERNAL FUNCTIONS 
***********************************
**********************************/

// returns the command byte
unsigned char
receiveCommandMessage(int fd);

// returns 1 if value in set[] else 0 if not
int
in_set(unsigned char value, unsigned char *set, int size);

unsigned char 
*readFile(char *file_path, int *file_size);

unsigned char
*stuff(unsigned char *original, int og_size, int *stuffed_size);

unsigned char
*de_stuff(unsigned char *stuffed, int stuffed_size, int *og_size);

unsigned char
*build_command_header(unsigned char C, int role);

unsigned char
*slice(unsigned char *data, int st, int end);

void
print_array(unsigned char *array, int size);

#endif
