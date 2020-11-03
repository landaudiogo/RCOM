#ifndef SHARED_H
#define SHARED_H

// colored messages
#define RESET "\033[0m" 
#define RED   "\033[31m"
#define GREEN "\033[32m"

// frame MACROS
#define FLAG 0x7e // frame delimiter
// address bytes
#define A0   0x01 // Command rx->tx || Reply tx->rx
#define A1   0x03 // Command tx->rx || Reply rx->tx
// control bytes for UNNUMBERED and SUPERVISION frames
#define SET  0x03
#define DISC 0x0b
#define UA   0x07
#define RR0  0x01
#define RR1  0x21
#define REJ0 0x01
#define REJ1 0x21
// control bytes for UNNUMBERED and SUPERVISION frames
#define IC0  0x00
#define IC1  0x02

#define BAUD B38400

#define TRANSMITTER 240
#define RECEIVER 241

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <strings.h>





/**********************************
***********************************
           API FUNCTIONS
***********************************
**********************************/

// 1. Lock the file
// 2. Modify the termios control to well defined parameters
// 3. Store the old termios paramters 
int 
llopen(int fd, int role);

// 1. remove the lock from the file descriptor
// 2. reset the old termios controls on the serial port driver
int 
llclose(int fd);

// 1. reading a command message
// 2. reading an information message
char
*llread(int fd, int *sizemessage);





/**********************************
***********************************
        INTERNAL FUNCTIONS 
***********************************
**********************************/
int 
sendCommandMessage(int fd, unsigned char *ctrl_message);

unsigned char 
*createCommand(unsigned char command, int role);

unsigned char
*receiveCommadnMessage(int fd);
// sendData(int fd)

//bitStuffing()

#endif
