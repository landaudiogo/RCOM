#ifndef FUNCS_H
#define FUNCS_H

// colored messages
#define RESET "\033[0m" 
#define RED   "\033[31m"
#define GREEN "\033[32m"

#define BAUD B38400

#define PCK_SIZE 10000

#include "state.h"
#include "identifiers.h"

// shared variable
extern int FLAG_ALARM;

/**********************************
***********************************
        INTERNAL FUNCTIONS 
***********************************
**********************************/
int 
sendCommandMessage(int fd, unsigned char *ctrl_message);

int
receiveCommandMessage(int fd, unsigned char command);

int
in_set(unsigned char value, unsigned char *set, int size);

unsigned char 
*readFile(char *file_path, int *file_size);

unsigned char
*stuff(unsigned char *original, int og_size, int *stuffed_size);

unsigned char
*de_stuff(unsigned char *stuffed, int stuffed_size, int *og_size);

#endif
