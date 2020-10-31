#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <inttypes.h>

int main(int argc, char **argv) {
    int fd;

    if(argc<1 || strncmp(argv[1], "/dev/pts/", 9) != 0) 
    {
        printf("Expected first argument must be of type: \"/dev/pts/*\"\n");
    }

    return 0;
}
