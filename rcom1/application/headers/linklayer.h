#ifndef LINKLAYER
#define LINKLAYER

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct linkLayer{
    char serialPort[50];
    int role; //defines the role of the program: 0==Transmitter, 1=Receiver
    int baudRate;
    int numTries;
    int timeOut;
} linkLayer;

//ROLE
#define NOT_DEFINED -1
#define TRANSMITTER 0
#define RECEIVER 1


//SIZE of maximum acceptable payload; maximum number of bytes that application layer should send to link layer
#define MAX_PAYLOAD_SIZE 100

//CONNECTION deafault values
#define BAUDRATE_DEFAULT B38400
#define MAX_RETRANSMISSIONS_DEFAULT 3
#define TIMEOUT_DEFAULT 2
#define _POSIX_SOURCE 1 /* POSIX compliant source */

//MISC
#define FALSE 0
#define TRUE 1

// Opens a conection using the "port" parameters defined in struct linkLayer, returns "-1" on error and "1" on sucess
int llopen(linkLayer connectionParameters);


// DESCRIPTION
//      This function deals with several steps:
//      1) stuff the buffer received as input
//      2) create a frame including HEADER + STUFFED + FLAG
//      3) send the frame and start a timer
//          1) if an ACK is received move on to the next frame
//          2) if a NACK is received repeat transmission
//          3) if timer expires, resend the data until MAX_RETRY
//              1) if we reached MAX_RETRY, attempt to reset connection llopen()
//              2) if 1 fails then return -1 (error)
// INPTUS
//      buf: contains the bytes to be stuffed into a frame
//      bufSize: contains the amount of bytes in buf
// RETURNS
//      <0 in case there is an error while writing
//      0 in case of success
int llwrite(char* buf, int bufSize);

// DESCRIPTION
//      The function is responsible for receiving one frame at a time
//      1) receive a frame
//          1) if expected information frame
//          de-stuff the data and verify whether the data is corrupted
//              1) if the data is corrupted, send a NACK
//              2) else send an ACK
//          2) if receive the previous frame
//          send an ACK for the received frame 
//          3) if receive a SET frame
//          this scenario occurs in the case the transmitter is trying to re-establish communication after the number of retries has exceeded
//          send a UA frame
//          4) any other frame being received, this is undefined behaviour so we return an error
// INPUTS
//      packet: this is a pointer to where the data must be written
// RETURNS
//      the amount of bytes received
//      -1 in case of error
int llread(char* packet);
// Closes previously opened connection; if showStatistics==TRUE, link layer should print statistics in the console on close
int llclose(int showStatistics);


#endif



