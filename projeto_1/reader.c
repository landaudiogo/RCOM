#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP = FALSE;

void signal_handler_IO(int status); /* definition of signal handler */
int wait_flag = TRUE;               /* TRUE while no signal received */

int main(int argc, char *argv[])
{
  int fd;
  char buf[1];
  int i, nread;

  //Uses either COM1 or COM2
  if ((argv[1] > 65) && ((argv[1] != 'a') || (argv[1] != 'A') || (argv[1] != 'b') || (argv[1] != 'B')))
  {
    printf("a para ttyS0, b para ttyS1, n para /dev/pts/n");
    exit(1);
  }

  /* open the device to be non-blocking (read will return immediatly) */
  fd = open(argv[1], O_RDWR | O_NOCTTY | O_NONBLOCK);
  //    fd = open(argv[1], O_RDWR | O_NOCTTY);
  if (fd < 0)
  {
    perror(argv[1]);
    exit(-1);
  }

  nread = LLREAD(fd, buf);
}

int LLREAD(int fd, char *buffer) //retorna n de chars lidos
{
  int state;
  int flag1_rcv = 0;
  int a_rcv = 0;
  int c_rcv = 0;
  int bcc1_rcv = 0;
  int bcc2_rcv = 0;
  int flag2_rcv = 0;

  if (STOP == FALSE) //nao sei o q isto e
  {
    state = 0;

    switch (state) //FSM para SET apenas (WIP)
    {

    case 0: //nada, start
    {
      flag1_rcv = isFlag();
      if (flag1_rcv)
        state = 1;
      break;
    }

    case 1:
    {
      a_rcv = isAddress(); //1 se address, 2 se flag, -1 outro
      if (a_rcv == 1)
        state = 2;
      else if (a_rcv == 2)
        state = 1;
      else
        state = 0;
      break;
    }
    case 2:
    {
      c_rcv = isControl(); //1 se c, 2 se f, -1 outro
      if (c_rcv == 1)
        state = 3;
      else if (c_rcv == 2)
        state = 2;
      else
        state = 0;
      break;
    }
    case 3:
    {
      bcc1_rcv = isBcc1(); //1 se bcc1, 2 se f, -1 outro
      if (bcc1_rcv == 1)
        state = 4;
      else if (bcc1_rcv == 2)
        state = 1;
      else
        state = 0;
      break;
    }
    case 4:
    {
      flag2_rcv = isflag();
      if (flag2_rcv == 1)
        state = 5;
      else if (flag2_rcv == -1)
        state = 0;
      break;
    }
    case 5:
    {
      STOP = TRUE; //nao sei o que e isto
      wait_flag = TRUE; //nem isto
      break;
    }
    }
  }
  /* while (STOP == FALSE)
    {
      write(1, ".", 1);
      usleep(100000);
       after receiving SIGIO, wait_flag = FALSE, input is available
         and can be read */
  /*  if (wait_flag == FALSE)
      {
        int res = read(fd, buffer, 255);
        buffer[res] = 0;
        for (int i = 0; i < res; i++)
          printf("%c", buffer[i]);
        if (res == 1)
          STOP = TRUE;    /* stop loop if only a CR was input */
  /*  wait_flag = TRUE; /* wait for new input */
  //  }
}