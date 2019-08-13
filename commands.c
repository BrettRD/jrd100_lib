#include "parser.h"
#include "commands.h"

/*
 * builds a command frame.
 */
uint8_t* build_cmd_frame(size_t *buf_len, uint8_t* buf, uint8_t cmd, uint16_t len, uint8_t* payload)
{
    if(*buf_len < (len + 7))
    {
        *buf_len = 0;
        return NULL;
    }
    //copy the payload first
    //use memmove so that payload can overlap buf
    memmove(&buf[5], payload, len);

    buf[0] = FRAME_BEGIN;
    buf[1] = FRAME_TYPE_CMD;
    buf[2] = cmd;
    buf[3] = (len >> 8) & 0xff;
    buf[4] = len & 0xff;
    //payload sits here
    buf[5 + len] = calc_frame_checksum(buf, len);
    buf[6 + len] = FRAME_END;

    *buf_len = len + 7; //total bytes the frame occupies in buf.
    return buf;
}

uint8_t* build_cmd_empty_frame(size_t *buf_len, uint8_t* buf, uint8_t cmd)
{
    return build_cmd_frame(buf_len, buf, cmd, 0, NULL);
}

/*
 * commands that make actions on tags indicate which tag, and a zero error byte
 */
int read_tag_sucess_frame(uint16_t len, uint8_t* payload, uint16_t *pc, uint8_t *epc_len, uint8_t* *epc, uint8_t *error)
{
    int epc_len_bytes = payload[0];
    if((epc_len_bytes+2) != len)
    {
        return PARSER_LENGTH_ERROR;
    }
    *epc_len = payload[0];
    *pc = (payload[1]<<8) + payload[2];
    *epc = &payload[3];
    *error = payload[3+epc_len_bytes];

    return PARSER_SUCCESS;
}


/*
 * a lot of commands use a single byte payload to indicate success
 */
int read_success_frame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    if(len != 1)
    {
        return PARSER_LENGTH_ERROR;
    }
    *error = payload[0];
    return PARSER_SUCCESS;
}


//all error codes have the error number in the first byte, with an optional epc.
int read_error_frame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    *error = payload[0];
    return PARSER_SUCCESS;
}

int read_tag_error_frame(uint16_t len, uint8_t* payload, uint16_t *pc, uint8_t *epc_len, uint8_t* *epc, uint8_t *error)
{
    if(len == 1)
    {
        *pc = 0;
        *epc_len = 0;
        *epc = NULL;
        return read_error_frame(len, payload, error);
    }

    int epc_len_bytes = payload[1];

    if((epc_len_bytes+2) != len)
    {
        return PARSER_LENGTH_ERROR;
    }

    *error = payload[0];
    *epc_len = payload[1];
    *pc = (payload[2]<<8) + payload[3];
    *epc = &payload[4];

    return PARSER_SUCCESS;
}

void find_frame_begin(size_t *buf_len, uint8_t* *buf)
{
    while ((*buf_len > 7) && (**buf != FRAME_BEGIN))
    {
        (*buf)++;
        (*buf_len)--;
    }
}

/*
 *  if the packet does not parse, advance by one byte and try again.
 */
void skip_byte(size_t *buf_len, uint8_t* *buf)
{
    (*buf)++;
    (*buf_len)--;
}


/*
 * reads data from a frame into pointers
 * returns the payload pointer into the frame buffer, or null if the frame is malformed
 */
int read_frame(size_t *buf_len, uint8_t* *buf, uint8_t *frame_type, uint8_t *cmd, uint16_t *len, uint8_t* *payload)
{
    find_frame_begin(buf_len, buf);

    //check we can safely read a header (minimum packet size)
    if(*buf_len < 7)
    {
        return PARSER_UNDERFULL;
    }

    //extract the packet metadata
    uint8_t* frame = *buf;

    uint8_t frame_header = frame[0];
    uint16_t payload_len = (frame[3] << 8) + frame[4];

    //check the packet
    if(*buf_len < (7 + payload_len))
    {
        return PARSER_UNDERFULL;
    }

    //extract the rest of the payload
    uint8_t received_cs = frame[5 + payload_len];
    uint8_t expected_cs = calc_frame_checksum(frame, payload_len);
    uint8_t frame_terminator = frame[6 + payload_len];


    if(frame_header != FRAME_BEGIN)
    {
        return PARSER_MALFORMED_HEADER;
    }
    if(expected_cs != received_cs)
    {
        return PARSER_CHECKSUM_ERROR;
    }
    if(frame_terminator != FRAME_END)
    {
        return PARSER_MALFORMED_TERMINATOR;
    }

    //load the return values
    *frame_type = frame[1];
    *cmd = frame[2];
    *len = payload_len;
    *payload = &frame[5];

    //advance the buffer to the next frame
    *buf = &frame[7 + payload_len];
    *buf_len = *buf_len - (7 + payload_len);

    return PARSER_SUCCESS;
}

/*
 * calculates the checksum, assumes that the first byte is the FRAME_BEGIN header byte
 * len is the payload length as written in the payload.  Not including type, command, and len bytes (4 extra bytes for cs)
 */
uint8_t calc_frame_checksum(uint8_t* frame, uint16_t len)
{
    uint8_t cs = 0x00;
    for(int i=0; i<(4+len); i++)
    {
        cs += frame[1+i];
    }
    return cs;
}

/*
 * param field valid are 0,1,2 MODULE_*_FIELD
 */
uint8_t* BuildGetModuleInfoFrame(size_t *buf_len, uint8_t* buf, uint8_t field)
{
    uint8_t cmd = CMD_GET_MODULE_INFO;
    uint16_t len = 1;
    return build_cmd_frame(buf_len, buf, cmd, len, &field);
}

int ReadGetModuleInfoFrame(uint16_t len, uint8_t* payload, uint8_t *info_type, uint16_t *info_len, uint8_t* *info)
{
    *info_type = payload[0];
    *info_len = len - 1;
    *info = &payload[1];
    return PARSER_SUCCESS;
}

uint8_t* BuildGetRfChannelFrame(size_t *buf_len, uint8_t* buf)
{
    return build_cmd_empty_frame(buf_len, buf, CMD_GET_RF_CHANNEL);
}

int ReadGetRfChannelFrame(uint16_t len, uint8_t* payload, uint8_t *channel_index)
{
    if(len != 1)
    {
        return PARSER_LENGTH_ERROR;
    }
    *channel_index = payload[0];
    return PARSER_SUCCESS;
}

uint8_t* BuildSetRfChannelFrame(size_t *buf_len, uint8_t* buf, uint8_t channel_index)
{
    uint8_t cmd = CMD_SET_RF_CHANNEL;
    uint16_t len = 1;
    return build_cmd_frame(buf_len, buf, cmd, len, &channel_index);
}
int ReadSetRfChannelFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return read_success_frame(len, payload, error);
}


uint8_t* BuildGetSelectFrame(size_t *buf_len, uint8_t* buf)
{
    return build_cmd_empty_frame(buf_len, buf, CMD_GET_SELECT_PARA);
}
int ReadGetSelectFrame(uint16_t len, uint8_t* payload, uint8_t *target, uint8_t *action, uint8_t *bank, uint32_t *pointer, uint8_t *mask_len, uint8_t* *mask, bool *truncate)
{
    uint8_t mask_len_bytes = payload[5]/8;
    if(mask_len_bytes + 7 != len)
    {
        return PARSER_LENGTH_ERROR;
    }

    uint8_t select_byte = payload[0];

    *pointer =  (payload[1]<<24) +
                (payload[2]<<16) +
                (payload[3]<< 8) +
                (payload[4]<< 0);

    *mask_len = payload[5];
    *truncate = (payload[6] == 0x80);
    *mask = &payload[7];

    *target = (select_byte >> 5) & 0x07;
    *action = (select_byte >> 2) & 0x07;
    *bank = select_byte & 0x03;

    return PARSER_SUCCESS;
}
float channel_freq_MHz(uint8_t region_index, uint8_t channel_index)
{
    return REGION_FREQ_STARTS[region_index-1] + (channel_index * REGION_FREQ_SEPARATIONS[region_index-1]);
}


//XXX needs completion with EPC documentation
uint8_t* BuildSetSelectFrame(size_t *buf_len, uint8_t* buf, uint8_t target, uint8_t action, uint8_t bank, uint32_t pointer, uint8_t mask_len_bits, uint8_t* mask, bool truncate)
{
    uint8_t cmd = CMD_SET_SELECT_PARA;
    uint8_t mask_len_bytes = mask_len_bits/8;
    uint16_t len = 7 + mask_len_bytes;
    uint8_t select_byte = ((target &0x7)<<5) | ((action &0x7)<<2) | (bank &0x3);
    uint8_t* payload = buf;

    //move the mask first so memory can overlap
    memmove(&payload[7], mask, mask_len_bytes);

    payload[0] = select_byte;

    payload[1] = (pointer>>24) & 0xff;
    payload[2] = (pointer>>16) & 0xff;
    payload[3] = (pointer>> 8) & 0xff;
    payload[4] = (pointer>> 0) & 0xff;
    
    payload[5] = mask_len_bits; //mask length in bits

    if(truncate)
    {
        payload[6] = 0x80;
    }
    else
    {
        payload[6] = 0x00;
    }

    //mask starts at payload[7]

    return build_cmd_frame(buf_len, buf, cmd, len, payload);
}

int ReadSetSelectFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return read_success_frame(len, payload, error);
}

uint8_t* build_io_payload(uint8_t* payload, uint8_t pin, uint8_t config, uint8_t dir)
{
    //config = 0; //set io direction (input/output)
    //config = 1; //set io output level (high/low)
    //config = 2; //read io input level (dir is meaningless)
    if(config >= 3)
    {
        return NULL;
    }

    //dir = 0; //input, low, fail
    //dir = 1; //output, high, sucess
    if(dir >= 2)
    {
        return NULL;
    }

    //pins are 1 indexed
    if((pin < 1) || (pin > 4))
    {
        return NULL;
    }

    payload[0] = config;
    payload[1] = pin;
    payload[2] = dir;
    return payload;
}

uint8_t* BuildIoControlFrame(size_t *buf_len, uint8_t* buf, uint8_t* io_payload)
{
    if(io_payload == NULL)
    {
        return NULL;
    }
    uint8_t cmd = CMD_IO_CONTROL;
    uint16_t len = 3;
    return build_cmd_frame(buf_len, buf, cmd, len, io_payload);
}

int ReadIoControlFrame(uint16_t len, uint8_t* payload, uint8_t* pin, uint8_t* config, uint8_t* dir)
{
    if(len != 3)
    {
        return PARSER_LENGTH_ERROR;
    }
    *config = payload[0];
    *pin = payload[1];
    *dir = payload[2];
    return PARSER_SUCCESS;
}

/*
 *  read at most tag_count tags
 */
uint8_t* BuildReadSingleFrame(size_t *buf_len, uint8_t* buf)
{
    return build_cmd_empty_frame(buf_len, buf, CMD_INVENTORY);
}

//use ReadTagNotification for both
/*
 *  read at most tag_count tags
 */
uint8_t* BuildReadMultiFrame(size_t *buf_len, uint8_t* buf, uint16_t tag_count)
{
    uint8_t cmd = CMD_READ_MULTI;
    uint16_t len = 3;
    uint8_t payload[3];
    payload[0] = 0x22;  //reserved byte, must be 0x22
    payload[1] = tag_count >> 8;
    payload[2] = tag_count & 0xff;

    return build_cmd_frame(buf_len, buf, cmd, len, payload);
}

int ReadTagNotification(uint16_t len, uint8_t *payload, uint8_t *rssi, uint16_t *pc, uint8_t *epc_len, uint8_t* *epc, uint16_t *crc)
{
    *epc_len = len - 5;
    *rssi = payload[0];
    *pc = (payload[1]<<8) + payload[2];
    *epc = &payload[3]; //read in place
    *crc = (payload[3 + *epc_len] << 8) + payload[4 + *epc_len];
    //XXX check the tag CRC
    return PARSER_SUCCESS;
}

/*
 * stop multiple read mode
 */
uint8_t* BuildStopReadFrame(size_t *buf_len, uint8_t* buf)
{
    return build_cmd_empty_frame(buf_len, buf, CMD_STOP_MULTI);
}

int ReadStopReadFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return read_success_frame(len, payload, error);
}

//baud is baud rate in hz.
//the change is effective immediately
//there is no response from this frame
uint8_t* BuildSetBaudFrame(size_t *buf_len, uint8_t* buf, uint32_t baud_hz)
{
    uint8_t cmd = CMD_SET_BAUD;
    uint16_t len = 2;
    uint8_t payload[2];
    payload[0] = ((baud_hz/100) >> 8) && 0xff;
    payload[1] =  (baud_hz/100) && 0xff;

    return build_cmd_frame(buf_len, buf, cmd, len, payload);
}
// no return message


uint8_t* BuildSetFhssFrame(size_t *buf_len, uint8_t* buf, uint8_t setting)
{
    uint8_t cmd = CMD_SET_FHSS;
    uint16_t len = 1;
    return build_cmd_frame(buf_len, buf, cmd, len, &setting);
}
int ReadSetFhssFrame(uint16_t len, uint8_t *payload, uint8_t *error)
{
    return read_success_frame(len, payload, error);
}

uint8_t* BuildSetModuleSleepFrame(size_t *buf_len, uint8_t* buf)
{
    return build_cmd_empty_frame(buf_len, buf, CMD_SLEEP_MODE);
}
//no return message

uint8_t* BuildGetPaPowerFrame(size_t *buf_len, uint8_t* buf)
{
    return build_cmd_empty_frame(buf_len, buf, CMD_GET_POWER);
}

int ReadGetPaPowerFrame(uint16_t len, uint8_t* payload, uint16_t *power)
{
    if(len != 2)
    {
        return PARSER_LENGTH_ERROR;
    }
    *power = (payload[0]<<8) + payload[1];
    return PARSER_SUCCESS;
}

uint8_t* BuildSetPaPowerFrame(size_t *buf_len, uint8_t* buf, float powerDBm)
{
    uint8_t cmd = CMD_SET_POWER;
    uint16_t len = 2;
    uint8_t payload[2];
    payload[0] = (((uint16_t)(powerDBm*100)) >> 8) && 0xff;
    payload[1] =  ((uint16_t)(powerDBm*100)) && 0xff;
    return build_cmd_frame(buf_len, buf, cmd, len, payload);
}
int ReadSetPaPowerFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return read_success_frame(len, payload, error);
}

uint8_t *BuildSetRegionFrame(size_t *buf_len, uint8_t* buf, uint8_t region_code)
{
    uint8_t cmd = CMD_SET_REGION;
    uint16_t len = 1;
    return build_cmd_frame(buf_len, buf, cmd, len, &region_code);
}
int ReadSetRegionFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return read_success_frame(len, payload, error);
}


uint8_t* BuildReadDataFrame(size_t *buf_len, uint8_t* buf, uint8_t* password, uint8_t bank, uint32_t data_ptr, uint32_t data_len)
{
    uint8_t cmd = CMD_READ_DATA;
    uint16_t len = 9;
    uint16_t data_len_words = data_len/2;
    uint16_t data_ptr_words = data_ptr/2;

    uint8_t payload[9];

    memmove(&payload[0], password, 4);
    payload[4] = bank;
    payload[5] = (data_ptr_words >> 8) & 0xff;
    payload[6] = (data_ptr_words) & 0xff;
    payload[7] = (data_len_words >> 8) & 0xff;
    payload[8] = (data_len_words) & 0xff;
    return build_cmd_frame(buf_len, buf, cmd, len, payload);
}

int ReadReadDataFrame(uint16_t len, uint8_t* payload, uint16_t *pc, uint8_t *epc_len, uint8_t* *epc, uint8_t *data_len, uint8_t* *data)
{
    int epc_len_bytes = payload[0] - 2;
    int data_len_bytes = (len - 3) - epc_len_bytes;
    if(data_len_bytes < 1)
    {
        return PARSER_LENGTH_ERROR;
    }
    
    *epc_len = epc_len_bytes;
    *data_len = data_len_bytes;

    *pc = (payload[1]<<8) + payload[2];
    *epc = &payload[3];                     //read in-place
    *data = &payload[3 + epc_len_bytes];    //read in-place

    return PARSER_SUCCESS;
}

uint8_t* BuildWriteDataFrame(size_t *buf_len, uint8_t* buf, uint8_t* password, uint8_t bank, uint32_t data_ptr, uint32_t data_len, uint8_t* data)
{
    uint8_t cmd = CMD_WRITE_DATA;
    uint16_t len = 9 + data_len;
    //check we're not clobbering buf
    if(*buf_len < (len + 7))
    {
        *buf_len = 0;
        return NULL;
    }

    uint8_t* payload = buf;  //moved to correct frame position by build_cmd_frame
    uint8_t data_len_words = data_len/2;
    uint8_t data_ptr_words = data_ptr/2;
    //data and password may overlap with buf
    uint8_t pwd[4];



    memcpy(pwd, password, 4);
    memmove(&payload[9], data, data_len);
    memcpy(payload, pwd, 4);
    payload[4] = bank;
    payload[5] = (data_ptr_words >> 8) & 0xff;
    payload[6] = (data_ptr_words >> 0) & 0xff;
    payload[7] = (data_len_words >> 8) & 0xff;
    payload[8] = (data_len_words >> 0) & 0xff;

    return build_cmd_frame(buf_len, buf, cmd, len, payload);
}

int ReadWriteDataFrame(uint16_t len, uint8_t* payload, uint16_t *pc, uint8_t *epc_len, uint8_t* *epc, uint8_t *error)
{
    return read_tag_sucess_frame(len, payload, pc, epc_len, epc, error);
}

uint8_t* BuildLockFrame(size_t *buf_len, uint8_t* buf, uint8_t* password, uint8_t* lock_payload)
{
    uint8_t cmd = CMD_LOCK_UNLOCK;
    uint16_t len = 7;
    uint8_t payload[7];

    //allow the variables to overloap with buf
    uint8_t pwd[4];
    memcpy(pwd, password, 4); 
    memmove(&payload[4], lock_payload, 3);
    memcpy(payload, pwd, 4);

    return build_cmd_frame(buf_len, buf, cmd, len, payload);
}
int ReadLockFrame(uint16_t len, uint8_t* payload, uint16_t *pc, uint8_t *epc_len, uint8_t* *epc, uint8_t *error)
{
    return read_tag_sucess_frame(len, payload, pc, epc_len, epc, error);
}

uint8_t* BuildKillFrame(size_t *buf_len, uint8_t* buf, uint8_t* password)
{
    return build_cmd_frame(buf_len, buf, CMD_KILL, 4, password);
}
int ReadKillFrame(uint16_t len, uint8_t* payload, uint16_t *pc, uint8_t *epc_len, uint8_t* *epc, uint8_t *error)
{
    return read_tag_sucess_frame(len, payload, pc, epc_len, epc, error);
}


uint8_t* BuildScanJammerFrame(size_t *buf_len, uint8_t* buf)
{
    return build_cmd_empty_frame(buf_len, buf, CMD_SCAN_JAMMER);
}
int ReadScanJammerFrame(uint16_t len, uint8_t* payload, uint8_t *ch_start, uint8_t *ch_end, uint8_t* *channel_noise)
{
    int channel_count = (payload[1] - payload[0]) + 1;
    if(len != channel_count + 2)
    {
        return PARSER_LENGTH_ERROR;
    }
    *ch_start = payload[0];
    *ch_end = payload[1];
    *channel_noise =  &payload[2];
    return PARSER_SUCCESS;
}

uint8_t* BuildScanRssiFrame(size_t *buf_len, uint8_t* buf)
{
    return build_cmd_empty_frame(buf_len, buf, CMD_SCAN_RSSI);
}
int ReadScanRssiFrame(uint16_t len, uint8_t* payload, uint8_t *ch_start, uint8_t *ch_end, uint8_t* *channel_rssi)
{
    int channel_count = (payload[1] - payload[0]) + 1;
    if(len != channel_count + 2)
    {
        return PARSER_LENGTH_ERROR;
    }
    *ch_start = payload[0];
    *ch_end = payload[1];
    *channel_rssi = &payload[2];
    return PARSER_SUCCESS;
}

uint8_t* BuildSetInventoryModeFrame(size_t *buf_len, uint8_t* buf, uint8_t mode)
{
    return build_cmd_frame(buf_len, buf, CMD_SET_INVENTORY_MODE, 1, &mode);
}
int ReadSetInventoryModeFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return read_success_frame(len, payload, error);
}

uint8_t* BuildGetQueryFrame(size_t *buf_len, uint8_t* buf)
{
    return build_cmd_empty_frame(buf_len, buf, CMD_GET_QUERY);
}
int ReadGetQueryFrame(uint16_t len, uint8_t* payload, uint8_t *dr, uint8_t *m, uint8_t *trext, uint8_t *sel, uint8_t *session, uint8_t *target, uint8_t *q)
{
    if(len != 2)
    {
        return PARSER_LENGTH_ERROR;
    }
    uint16_t bitmap = (payload[1] << 8) + payload[0];
    *dr      = (bitmap >> 15) & 0x01;
    *m       = (bitmap >> 13) & 0x03;
    *trext   = (bitmap >> 12) & 0x01;
    *sel     = (bitmap >> 10) & 0x03;
    *session = (bitmap >>  8) & 0x03;
    *target  = (bitmap >>  7) & 0x01;
    *q       = (bitmap >>  3) & 0x0F;

    return PARSER_SUCCESS;
}


uint8_t* BuildSetQueryFrame(size_t *buf_len, uint8_t* buf, uint8_t dr, uint8_t m, uint8_t trext, uint8_t sel, uint8_t session, uint8_t target, uint8_t q)
{
    uint8_t cmd = CMD_SET_QUERY;
    uint16_t len = 2;
    uint8_t payload[2];    
    uint16_t bitmap =
        ((dr         & 0x01)<<15) |
        ((m          & 0x03)<<13) |
        ((trext      & 0x01)<<12) |
        ((sel        & 0x03)<<10) |
        ((session    & 0x03)<< 8) |
        ((target     & 0x01)<< 7) |
        ((q          & 0x0F)<< 3) ;
     //lowest three bits are zero.
    
    payload[0] = bitmap >> 8;
    payload[1] = bitmap & 0xff;
    return build_cmd_frame(buf_len, buf, cmd, len, payload);
}

int ReadSetQueryFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return read_success_frame(len, payload, error);
}

/*
 * set the auto-sleep timeout
 * zero never sleeps
 */
uint8_t* BuildSetSleepTimeFrame(size_t *buf_len, uint8_t* buf, uint8_t idle_minutes)
{
    uint8_t cmd = CMD_SET_SLEEP_TIME;
    uint16_t len = 1;
    return build_cmd_frame(buf_len, buf, cmd, len, &idle_minutes);
}

int ReadSetSleepTimeFrame(uint16_t len, uint8_t* payload, uint8_t *idle_minutes)
{
    if(len != 1)
    {
        return PARSER_LENGTH_ERROR;
    }
    *idle_minutes = payload[0];
    return PARSER_SUCCESS;
}


/*
//not complete



uint8_t* BuildNXPChangeEasFrame(size_t *buf_len, uint8_t* buf, strAccessPasswd, cbxSetEas.Checked)
{
    uint8_t cmd = CMD_NXP_CHANGE_EAS;
    uint16_t len = 0;
    return build_cmd_frame(buf_len, buf, cmd, len, NULL);
}

uint8_t* BuildNXPEasAlarmFrame(size_t *buf_len, uint8_t* buf)
{
    uint8_t cmd = CMD_NXP_EAS_ALARM;
    uint16_t len = 0;
    return build_cmd_frame(buf_len, buf, cmd, len, NULL);
}

uint8_t* BuildNXPReadProtectFrame(size_t *buf_len, uint8_t* buf, strAccessPasswd, cbxReadProtectReset.Checked)
{
    uint8_t cmd = CMD_NXP_READPROTECT;
    uint16_t len = 0;
    return build_cmd_frame(buf_len, buf, cmd, len, NULL);
}


uint8_t* BuildNXPResetReadProtectFrame(size_t *buf_len, uint8_t* buf, strAccessPasswd, cbxReadProtectReset.Checked)
{
    uint8_t cmd = CMD_NXP_RESET_READPROTECT;
    uint16_t len = 0;
    return build_cmd_frame(buf_len, buf, cmd, len, NULL);
}










//undocumented commands


//XXX undocumented command
uint8_t* BuildSetCWFrame(size_t *buf_len, uint8_t* buf, uint8_t setting)
{
    uint8_t cmd = CMD_SET_CW;
    uint16_t len = 1;
    return build_cmd_frame(buf_len, buf, cmd, len, &setting);
}
int ReadSetCWFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return PARSER_NEEDS_WORK;
}


uint8_t* BuildInsertRfChFrame(size_t *buf_len, uint8_t* buf, channelNum, channelList)
{
    //CMD_INSERT_FHSS_CHANNEL ?
    uint8_t cmd = ;
    uint16_t len = 0;
    uint8_t* payload = buf;

    return build_cmd_frame(buf_len, buf, cmd, len, );
}
int ReadInsertRfChFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return PARSER_NEEDS_WORK;
}


uint8_t* BuildSaveConfigToNvFrame(size_t *buf_len, uint8_t* buf, NV_enable)
{
    uint8_t cmd = CMD_SAVE_NV_CONFIG;
    uint16_t len = ;
    return build_cmd_frame(buf_len, buf, cmd, len, NULL);
}

uint8_t* BuildMonzaQTFrame(size_t *buf_len, uint8_t* buf, strAccessPasswd, false, cbxMonzaQT_SR.Checked, cbxMonzaQT_MEM.Checked)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf_len, buf, cmd, len, NULL);
}
uint8_t* BuildNXPChangeConfigFrame(size_t *buf_len, uint8_t* buf, strAccessPasswd, Convert.ToInt32(txtConfigData.Text.Replace(" ",""), 16))
{
    uint8_t cmd = CMD_NXP_CHANGE_CONFIG;
    uint16_t len = 0;
    return build_cmd_frame(buf_len, buf, cmd, len, NULL);
}

uint8_t* BuildReadModemParaFrame(size_t *buf_len, uint8_t* buf)
{
    uint8_t cmd = CMD_READ_MODEM_PARA;
    uint16_t len = 0;
    return build_cmd_frame(buf_len, buf, cmd, len, NULL);
}

uint8_t* BuildSetModemParaFrame(size_t *buf_len, uint8_t* buf, mixerGain, IFAmpGain, signalTh)
{
    uint8_t cmd = CMD_SET_MODEM_PARA;
    uint16_t len = 0;
    return build_cmd_frame(buf_len, buf, cmd, len, NULL);
}

uint8_t* BuildSetReaderEnvModeFrame(size_t *buf_len, uint8_t* buf, (byte)cbxMode.SelectedIndex)
{
    uint8_t cmd = CMD_SET_READER_ENV_MODE;
    uint16_t len = 0;
    return build_cmd_frame(buf_len, buf, cmd, len, NULL);
}
*/