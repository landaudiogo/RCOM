#ifndef FUNCS_H
#define FUNCS_H

// colored messages
#define RESET "\033[0m" 
#define RED   "\033[31m"
#define GREEN "\033[32m"

#define BAUD B38400
#define MAX_PACKET_SIZE 20

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


// INPUT
//  fd: file descriptor from where we are supposed to read the information;
//  **message: if the frame received is an information frame, then the data field is stored in *message;
//  *message_size: if the frame received is an information frame, the size of the data field is stored in this variable;
// RETURN
//  Command byte 
//  
//  The function also adds to message and message_size the frame without the header
//  to be posteriorly processed to check for any BCC2_check
unsigned char
receiveFrame(int fd, unsigned char **message, int *message_size);


// INPUT
//   data: pointer to an array
//   size: size of the data array
// RETURN
//   1: if the the BCC check on the array comes back valid
//   0: else 
int
BCC2_check(unsigned char *datafield, int size);


// INPUT
//   value: character to be compared
//   set: array of values to be compared to 'value'
//   size: size of set
// RETURN
//   1: if the set contains 'value'
//   0: else
int
in_set(unsigned char value, unsigned char *set, int size);


// INPUT
//   file_path: pointer to a string
//   file_size: size of return array of bytes
// RETURN
//   array with all the data bytes in the file
unsigned char 
*readFile(char *file_path, int *file_size);


// INPUTS
//   original: copy of an array that containing unstuffed bytes
//   og_size: size of original array
//   stuffed_size: size of the final return array
// RETURN
//   stuffed array using original values using the rules indicated by the slides
unsigned char
*stuff(unsigned char *original, int og_size, int *stuffed_size);


// INPUTS
//   stuffed: copy of an array that containing stuffed bytes
//   stuffed_size: size of stuffed array
//   og_size: size of the final return array
// RETURN
//   original array destuffed using the rules indicated by the slides
unsigned char
*de_stuff(unsigned char *stuffed, int stuffed_size, int *og_size);


// INPUTS
//   C: command byte to be included in the header
//   role: machine role (TRANSMITTER, RECEIVER)
// RETURN
//   header containing FLAG, ADDRESS, COMMAND, BCC_HEADER
unsigned char
*build_command_header(unsigned char C, int role);


// INPUTS
//   data: array to be partitioned
//   st: start position of the partition
//   end: end position of the partition (not inclusive)
// RETURN
//   header containing FLAG, ADDRESS, COMMAND, BCC_HEADER
unsigned char
*slice(unsigned char *data, int st, int end);


// INPUTS
//   array: array to be printed
//   size: size of 'array'
void
print_array(unsigned char *array, int size);

#endif
