#ifndef SHARED_H
#define SHARED_H

#define BAUD B38400

// simple definitions
#include "identifiers.h"
#include "state.h"


// include functions
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <strings.h>

#include "funcs.h"




/**********************************
***********************************
           API FUNCTIONS
***********************************
**********************************/
void 
initialize_alarm();

// 1. Lock the file
// 2. Modify the termios control to well defined parameters
// 3. Store the old termios paramters 
int 
llopen(int fd, int role);

// 1. remove the lock from the file descriptor
// 2. reset the old termios controls on the serial port driver
int 
llclose(int fd, int role);

#endif
