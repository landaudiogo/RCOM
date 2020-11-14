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
#include "linklayer.h"

extern linkLayer linkRole; 

typedef struct _Statistics {
    int nacks;
    int acks;
    int repeated_acks;
    int alarms;
} Statistics;

extern Statistics stats;

void 
initialize_alarm();

#endif
