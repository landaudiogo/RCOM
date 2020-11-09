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

// internal functions  
// internal static definitions
#include "funcs.h"
#include "state.h"
#include "identifiers.h"


/**********************************
***********************************
           API FUNCTIONS
***********************************
**********************************/
void 
initialize_alarm();

int 
llopen(int fd, int role);

int 
llclose(int fd, int role);

#endif
