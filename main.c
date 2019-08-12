#include "parser.h"
#include "commands.h"
#include "callbacks.h"

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>






//serial port and write buffer
int port_fd = -1;
uint8_t write_buffer[BUFSIZ];

//crappy state machine to go through the motions
int seq_step = 0;
bool seq_tx = false;


//return false when finished
bool step_sequence_tx()
{

    size_t len = BUFSIZ;
    uint8_t *frame = write_buffer;
    if(seq_tx)
    {
        switch(seq_step)
        {
            case 0:
                frame = BuildGetModuleInfoFrame(&len, write_buffer, MODULE_HARDWARE_VERSION_FIELD);
                write(port_fd, frame, len);
            break;
            case 1:
                frame = BuildGetModuleInfoFrame(&len, write_buffer, MODULE_SOFTWARE_VERSION_FIELD);
                write(port_fd, frame, len);
            break;
            case 2:
                frame = BuildGetModuleInfoFrame(&len, write_buffer, MODULE_MANUFACTURE_INFO_FIELD);
                write(port_fd, frame, len);
            break;
            case 3:
                frame = BuildScanJammerFrame(&len, write_buffer);
                write(port_fd, frame, len);
            break;
            case 4:
                frame = BuildScanRssiFrame(&len, write_buffer);
                write(port_fd, frame, len);
            break;
            case 5:
                frame = BuildReadSingleFrame(&len, write_buffer);
                write(port_fd, frame, len);
            break;
            default:
                return false;
        }

        seq_step++;
        seq_tx = false;
    }
    return true;
}

void step_sequence_rx()
{
    if(!seq_tx)
    {
        seq_tx = true;
    }
}


void print_module_info(uint8_t info_type, uint16_t info_len, uint8_t* info)
{
    info[info_len] = '\0';  //This edits the read buffer, but we're already finished with that data.
    switch (info_type)
    {
        case MODULE_HARDWARE_VERSION_FIELD:
        {
            printf("Hardware version: %s\n", info);
        }
        break;
        case MODULE_MANUFACTURE_INFO_FIELD:
        {
            printf("Manufaturer version: %s\n", info);
        }
        break;
        case MODULE_SOFTWARE_VERSION_FIELD:
        {
            printf("Software version: %s\n", info);
        }
        break;
    }
    step_sequence_rx();
}

void print_noise_scan(uint8_t ch_start, uint8_t ch_end, uint8_t* channel_noise)
{
    printf("Noise scan:\n");
    for(int i=ch_start; i<ch_end; i++)
    {
        printf("channel %d: %d", i, channel_noise[i - ch_start]);
    }
    step_sequence_rx();
}
void print_rssi_scan(uint8_t ch_start, uint8_t ch_end, uint8_t* channel_rssi)
{
    printf("RSSI scan:\n");
    for(int i=ch_start; i<ch_end; i++)
    {
        printf("channel %d: %d", i, channel_rssi[i - ch_start]);
    }
    step_sequence_rx();
}

void print_discovered_tag(uint8_t rssi, uint16_t pc, uint8_t epc_len, uint8_t* epc, uint16_t crc)
{
    printf("Found tag\n    epc = ");
    for(int i=0; i<epc_len; i++)
    {
        printf("%c%c",
                "0123456789abcdef"[epc[epc_len-1-i]>>4],
                "0123456789abcdef"[epc[epc_len-1-i]&0xf]);
    }
    printf("\n    rssi = %d\n", rssi);
    step_sequence_rx();
}




char* port_path = "/dev/ttyUSB0";

int main()
{


    cb_module_info = print_module_info;
    cb_scan_jammer = print_noise_scan;
    cb_scan_rssi = print_rssi_scan;
    cb_tag_single_notification = print_discovered_tag;

    port_fd = open(port_path, O_RDWR | O_NOCTTY);

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

    while(1)
    {

        //check the space left in the buffer
        buf_left = BUFSIZ - buf_len;
        //read that many bytes (without blocking)
        buf_len += read(port_fd, ptr_end, buf_left);
        //update the end pointer
        ptr_end = &ptr_start[buf_len];

        //read a packet and advance the pointers for used frames
        parser_error = parse_packet(&buf_len, &ptr_start);


        //move the pointers back to the beginning of the read buffer
        if((parser_error == PARSER_UNDERFULL) && (buf_left == 0))
        {
            memmove(read_buf, ptr_start, buf_len);
            ptr_start = read_buf;
            ptr_end = &ptr_start[buf_len];
            buf_left = BUFSIZ - buf_len;
        }
    }


    return 0;
}