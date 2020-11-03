#ifndef SHARED_H
#define SHARED_H

#define FLAG 0x7e
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

// 1. Lock the file
// 2. Modify the termios control to well defined parameters
int llopen(int fd, int role);

// 1. remove the lock from the file descriptor
// 2. reset the old termios controls on the serial port driver
int llclose(int fd);

#endif
