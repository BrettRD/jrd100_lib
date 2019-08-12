#include "parser.h"

#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */ 
#include <errno.h>   /* ERROR Number Definitions           */




uint8_t buf[BUFSIZ];
char* port_path = "/dec/ttyUSB0";

int main()
{
    int port_fd = open(port_path, O_RDWR | O_NOCTTY);

    return 0;
}