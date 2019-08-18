#include "parser.h"
#include "commands.h"
#include "callbacks.h"

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>



#define RFID_BAUD B115200
char* port_path = "/dev/ttyUSB0";


//serial port and write buffer
int port_fd = -1;
uint8_t write_buffer[BUFSIZ];

//crappy state machine to go through the motions
int seq_step = 0;
bool seq_tx = true;

#define MAX_TAGS 1000
size_t n_known_tags = 0;
uint8_t known_tags[MAX_TAGS][12];

uint8_t* find_tag(uint8_t* epc)
{
    for(int i=0; i< n_known_tags; i++)
    {
        if(0 == memcmp(epc, known_tags[i], 12))
        {
            return known_tags[i];
        }
    }
    return NULL;
}

void step_sequence_rx()
{
    seq_tx = true;
}

void insert_tag(uint8_t epc_len, uint8_t* epc)
{
    if(epc_len != 12)
    {
        printf("tag insert error, wrong size\n");
    }
    if(n_known_tags < MAX_TAGS)
    {
        if(NULL == find_tag(epc))
        {
            memcpy(known_tags[n_known_tags++], epc, 12);
            printf("added tag, total is %zu\n", n_known_tags);
        }
    }
    else
    {
        printf("memory full\n");
    }
}

//return false when finished
bool step_sequence_tx()
{
    size_t len = BUFSIZ;
    uint8_t *frame = write_buffer;
    uint16_t power = 2600;

    if(seq_tx)
    {
        switch(seq_step)
        {
            case 0:
                frame = BuildStopReadFrame(&len, write_buffer);
                printf("Stopping previous reads: %zu bytes\n", len);
                write(port_fd, frame, len);
                break;
            case 1:
                frame = BuildGetModuleInfoFrame(&len, write_buffer, MODULE_HARDWARE_VERSION_FIELD);
                printf("Requesting hardware version: %zu bytes\n", len);
                write(port_fd, frame, len);
            break;
            case 2:
                frame = BuildGetModuleInfoFrame(&len, write_buffer, MODULE_SOFTWARE_VERSION_FIELD);
                printf("Requesting software version: %zu bytes\n", len);
                write(port_fd, frame, len);
            break;
            case 3:
                frame = BuildGetModuleInfoFrame(&len, write_buffer, MODULE_MANUFACTURE_INFO_FIELD);
                printf("Requesting munufacturer info: %zu bytes\n", len);
                write(port_fd, frame, len);
            break;
            case 4:
                frame = BuildScanJammerFrame(&len, write_buffer);
                printf("Requesting noise measurement: %zu bytes\n", len);
                write(port_fd, frame, len);
            break;
            case 5:
                frame = BuildScanRssiFrame(&len, write_buffer);
                printf("Requesting rssi measurement: %zu bytes\n", len);
                write(port_fd, frame, len);
            break;
            case 6:
                frame = BuildSetPaPowerFrame(&len, write_buffer, power);
                printf("Setting tx power level to %d: %zu bytes\n", power , len);
                write(port_fd, frame, len);
            break;
            case 7:
                frame = BuildGetPaPowerFrame(&len, write_buffer);
                printf("Requesting tx power level: %zu bytes\n", len);
                write(port_fd, frame, len);
            break;
            case 8:
                frame = BuildReadMultiFrame(&len, write_buffer, 8000);
                //frame = BuildReadSingleFrame(&len, write_buffer);
                printf("Requesting a multiple read: %zu bytes\n", len);
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



void print_module_info(void* context, uint8_t info_type, uint16_t info_len, uint8_t* info)
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

void print_noise_scan(void* context, uint8_t ch_start, uint8_t ch_end, uint8_t* channel_noise)
{
    printf("Noise scan:\n");
    for(int i=ch_start; i<ch_end; i++)
    {
        printf("channel %d: %d\n", i, channel_noise[i - ch_start]);
    }
    step_sequence_rx();
}

void print_rssi_scan(void* context, uint8_t ch_start, uint8_t ch_end, uint8_t* channel_rssi)
{
    printf("RSSI scan:\n");
    for(int i=ch_start; i<ch_end; i++)
    {
        printf("channel %d: %d\n", i, channel_rssi[i - ch_start]);
    }
    step_sequence_rx();
}

void print_tx_power(void* context, uint16_t power)
{
    printf("Tx power: %d\n", power);
    step_sequence_rx();
}

void print_discovered_tag(void* context, uint8_t rssi, uint16_t pc, uint8_t epc_len, uint8_t* epc, uint16_t crc)
{
    //printf("Found tag\n    epc = ");
    //for(int i=0; i<epc_len; i++)
    //{
    //    printf("%.2x", epc[i]);
    //}
    //printf("\n    rssi = %d\n", rssi);
    insert_tag(epc_len, epc);

    //step_sequence_rx();
}

void print_error_cmd(void* context, int parser_error, uint8_t frame_type, uint8_t cmd, uint16_t len, uint8_t* payload)
{
    printf("Parser error: %s\n", parser_error_strings[parser_error]);
    printf("    frame_type: %.2x\n", frame_type);
    printf("    cmd:        %.2x\n", cmd);
    printf("    payload:    ");
    for(int i=0; i<len; i++)
    {
        printf("%.2x", payload[i]);
    }
    printf("\n");
}

void print_tag_timeout(void* context, uint16_t pc, uint8_t epc_len, uint8_t* epc)
{
    printf("No Tags Found, poll timed out\n");
    //step_sequence_rx();
}

void response_success(void* context, uint8_t error)
{
    if(error == 0)
    {
        printf("OK.\n");
    }
    else
    {
        printf("Non-zero return");
    }
    step_sequence_rx();
}

int main()
{

    jdm_100_cb_t cb = {NULL};
    cb.context = NULL;

    cb.cb_parser_error = print_error_cmd;
    cb.cb_set_power = response_success;
    cb.cb_stop_frame = response_success;

    cb.cb_module_info = print_module_info;
    cb.cb_scan_jammer = print_noise_scan;
    cb.cb_scan_rssi = print_rssi_scan;
    cb.cb_get_power = print_tx_power;
    cb.cb_tag_single_notification = print_discovered_tag;
    cb.cb_error_inventory_tag_timeout = print_tag_timeout;

    port_fd = open(port_path, O_RDWR | O_NOCTTY);

    if(port_fd == -1)
    {
        printf("Error: could not open %s\n", port_path);
        return -1;
    }

    printf("%s opened rw\n", port_path);

    struct termios port_settings;
    tcgetattr(port_fd, &port_settings);

    printf("Setting baud\n");

    cfsetispeed(&port_settings, RFID_BAUD);
    cfsetospeed(&port_settings, RFID_BAUD);

    printf("setting flags\n");


    //props to chrisheydrick.com
    port_settings.c_cflag &= ~PARENB;
    port_settings.c_cflag &= ~CSTOPB;
    port_settings.c_cflag &= ~CSIZE;
    port_settings.c_cflag |= CS8;
    /* no hardware flow control */
    port_settings.c_cflag &= ~CRTSCTS;
    /* enable receiver, ignore status lines */
    port_settings.c_cflag |= CREAD | CLOCAL;
    /* disable input/output flow control, disable restart chars */
    port_settings.c_iflag &= ~(IXON | IXOFF | IXANY);
    /* disable canonical input, disable echo,
    disable visually erase chars,
    disable terminal-generated signals */
    port_settings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    /* disable output processing */
    port_settings.c_oflag &= ~OPOST;

    port_settings.c_cc[VTIME] = 0.001;  //short timeout
    port_settings.c_cc[VMIN] = 1;   //read one byte at minimum, blocking

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
    printf("Starting sequence\n");


    while(step_sequence_tx())
    {

        //check the space left in the buffer
        buf_left = &read_buf[BUFSIZ] - ptr_end;
        //read that many bytes (without blocking)
        size_t read_len = read(port_fd, ptr_end, buf_left);
        buf_len += read_len;

        //update the end pointer
        ptr_end = &ptr_start[buf_len];

        //read a packet and advance the pointers for used frames
        parser_error = parse_packet(&cb, &buf_len, &ptr_start);

        //move the active buffer back to the beginning of the read buffer
        if(((parser_error == PARSER_UNDERFULL) && (buf_left == 0)) || (buf_len == 0))
        {
            memmove(read_buf, ptr_start, buf_len);
            ptr_start = read_buf;
            ptr_end = &ptr_start[buf_len];
            buf_left = BUFSIZ - buf_len;
        }

    }


    return 0;
}