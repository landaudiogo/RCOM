#ifndef READER_H
#define READER_H

// llread state machine
#define R_START   0
#define R_RCVI    1
#define R_RCVSET  2
#define R_STOP    3


#include "shared.h"
// 1. reading a command message
// 2. reading an information message
int
llread(int fd, unsigned char **message);

#endif
