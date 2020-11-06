#ifndef WRITER_H
#define WRITER_H

// llwrite state machine
#define W_START   0
#define W_SNDI    1
#define W_STOP    2

#include "shared.h"

int 
llwrite(int fd, unsigned char *stuffed, int stuffed_size);

#endif
