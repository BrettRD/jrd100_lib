#include "parser.h"
#include "commands.h"
#include "callbacks.h"

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>





void print_module_info(uint8_t info_type, uint16_t info_len, uint8_t* info)
{
    info[info_len] = '\0';  //This edits the read buffer, but we're already finished with that data.
    switch (info_type)
    {
        case MODULE_HARDWARE_VERSION_FIELD:
        {
            printf("Hardware version: %s", info);
        }
        break;
        case MODULE_MANUFACTURE_INFO_FIELD:
        {
            printf("Manufaturer version: %s", info);
        }
        break;
        case MODULE_SOFTWARE_VERSION_FIELD:
        {
            printf("Software version: %s", info);
        }
        break;
    }
}



char* port_path = "/dev/ttyUSB0";

int main()
{


    cb_module_info = print_module_info;


    int port_fd = open(port_path, O_RDWR | O_NOCTTY);

    if(port_fd == -1)
    {
        printf("Error: could not open %s\n", port_path);
        return -1;
    }

    printf("%s opened rw\n", port_path);

    struct termios port_settings;
    tcgetattr(port_fd, &port_settings);

    cfsetispeed(&port_settings, B115200);
    cfsetospeed(&port_settings, B115200);

	port_settings.c_cflag &= ~(PARENB | CSTOPB | CSIZE | CRTSCTS);
    port_settings.c_cflag |=  CS8 | CREAD | CLOCAL;
    port_settings.c_iflag &= ~(IXON | IXOFF | IXANY | ICANON | ECHO | ECHOE | ISIG);
    port_settings.c_oflag &= ~OPOST;
    port_settings.c_cc[VMIN] = 0; //minimum read of zero bytes, don't block.

	if(tcsetattr(port_fd, TCSANOW, &port_settings) != 0)
    {
		printf("Error: could not set port attributes\n");
        return -1;
    }

    uint8_t read_buf[BUFSIZ];
    uint8_t* ptr_end = read_buf;
    uint8_t* ptr_start = read_buf;
    size_t buf_len = 0;
    size_t buf_left = BUFSIZ;

    int parser_error = PARSER_SUCCESS;

    //check the space left in the buffer
    buf_left = (&read_buf[BUFSIZ]) - ptr_end;
    //read that many bytes (without blocking)
	buf_len += read(port_fd, ptr_end, buf_left);
    //update the end pointer
    ptr_end = &ptr_start[buf_len];

    parse_packet(&buf_len, &ptr_start);



    if((parser_error == PARSER_UNDERFULL) && (buf_left == 0))
    {
        memmove(read_buf, ptr_start, buf_len);
        ptr_start = read_buf;
        ptr_end = &ptr_start[buf_len];
        buf_left = (&read_buf[BUFSIZ]) - ptr_end;
    }



    return 0;
}