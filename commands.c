#include "parser.h"




/*
 * builds a command frame.
 */
uint8_t* build_cmd_frame(uint8_t* buf, uint8_t cmd, uint16_t len, uint8_t* payload)
{
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
}

uint8_t* build_cmd_empty_frame(uint8_t*buf, uint8_t cmd)
{
    return build_cmd_frame(buf, cmd, 0, NULL)
}

/*
 * a lot of commands use a single byte payload to indicate success
 */
int read_empty_frame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    if(len != 1)
    {
        return PARSER_LENGTH_ERROR;
    }
    *error = payload[0];
    return PARSER_SUCCESS;
}

/*
 * reads data from a frame into pointers
 * returns the payload pointer into the frame buffer, or null if the frame is malformed
 */

uint8_t* read_frame(uint8_t* frame, uint8_t *frame_type, uint8_t *cmd, uint16_t *len)
{
    uint8_t frame_header = frame[0];
    *frame_type = frame[1];
    *cmd = frame[2];
    *len = (frame[3] << 8) + frame[4];
    uint8_t* payload = frame[5];
    uint8_t received_cs = payload[5 + *len];
    uint8_t frame_terminator = frame[6 + *len];
    uint8_t expected_cs = calc_frame_checksum(frame, *len);

    if(frame_header == FRAME_BEGIN)
    {
        if(expected_cs == received_cs)
        {
            if(frame_terminator == FRAME_END)
            {
                return payload;
            }
        }
    }

    //no access to failed frames
    *frame_type = 0;
    *cmd = 0;
    *len = 0;
    *payload = NULL;
    return payload;
}

/*
 * calculates the checksum, assumes that the first byte is the FRAME_BEGIN header byte
 */
uint8_t calc_frame_checksum(uint8_t* frame, len)
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
uint8_t* BuildGetModuleInfoFrame(uint8_t* buf, uint8_t field)
{
    uint8_t cmd = CMD_GET_MODULE_INFO;
    uint16_t len = 1;
    return build_cmd_frame(buf, cmd, len, field);
}
//XXX fetch
uint8_t* BuildGetPaPowerFrame(uint8_t* buf)
{
    return build_cmd_empty_frame(buf, CMD_GET_POWER);
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

uint8_t* BuildGetQueryFrame(uint8_t* buf)
{
    return build_cmd_empty_frame(buf, CMD_GET_QUERY);
}
int ReadGetQueryFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return PARSER_NEEDS_WORK;
}
uint8_t* BuildGetRfChannelFrame(uint8_t* buf)
{
    return build_cmd_empty_frame(buf, CMD_GET_RF_CHANNEL);
}
int ReadGetRfChannelFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return PARSER_NEEDS_WORK;
}
uint8_t* BuildGetSelectFrame(uint8_t* buf, )
{
    return build_cmd_frame(buf, CMD_GET_SELECT_PARA);
}
int ReadGetSelectFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return PARSER_NEEDS_WORK;
}
uint8_t* BuildInsertRfChFrame(uint8_t* buf, channelNum, channelList)
{
    //CMD_INSERT_FHSS_CHANNEL ?
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, );
}
int ReadInsertRfChFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return PARSER_NEEDS_WORK;
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

    //check the config makes sense


    //check the pin is in range
    if((pin == 0) || (pin > 4))
    {
        return NULL;
    }

    //check the config makes sense

    payload[0] = config;
    payload[1] = pin;
    payload[2] = dir;
    return payload;
}
uint8_t* BuildIoControlFrame(uint8_t* buf, uint8_t* io_payload)
{
    if(io_payload == NULL)
    {
        return NULL;
    }
    uint8_t cmd = CMD_IO_CONTROL;
    uint16_t len = 3;
    return build_cmd_frame(buf, cmd, len, uint8_t* io_payload);
}

int read_io_payload(uint16_t len, uint8_t* payload, uint8_t* pin, uint8_t* config, uint8_t* dir)
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
uint8_t* BuildReadSingleFrame(uint8_t* buf)
{
    return build_cmd_empty_frame(buf, CMD_INVENTORY);
}
//use ReadTagNotification for both
/*
 *  read at most tag_count tags
 */
uint8_t* BuildReadMultiFrame(uint8_t* buf, uint16_t tag_count)
{
    uint8_t cmd = CMD_READ_MULTI;
    uint16_t len = 3;
    uint8_t[3] payload;
    payload[0] = 0x22;  //reserved byte, must be 0x22
    payload[1] = tag_count >> 8;
    payload[2] = tag_count & 0xff;

    return build_cmd_frame(buf, cmd, len, payload);
}


int ReadTagNotification(uint16_t len, uint8_t *payload, uint8_t *rssi, uint16_t *pc, uint8_t *epc_len, uint8_t* epc, uint16_t *crc)
{
    *epc_len = len - 5;
    memmove(epc, &payload[3], epc_len);
    *rssi = payload[0];
    *pc = (payload[1]<<8) + payload[2];
    //epc
    *crc = (payload[3 + epc_len] << 8) + payload[4 + epc_len];
    //XXX check for CRC
    retun PARSER_SUCCESS;
}

/*
 * stop multiple read mode
 */
uint8_t* BuildStopReadFrame(uint8_t* buf)
{
    return build_cmd_empty_frame(buf, CMD_STOP_MULTI);
}
int ReadStopReadFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return read_empty_frame(len, payload, error);
}

uint8_t* BuildKillFrame(uint8_t* buf, uint8_t* password)
{
    return build_cmd_frame(buf, CMD_KILL, 4, password);
}
int ReadKillFrame(uint16_t len, uint8_t* payload, uint8_t *epc_len, uint8_t* pc, uint8_t* epc, uint8_t *error)
{
    *epc_len = payload[0] - 2;
    if(*epc_len != (len-4))
    {
        //malformed?
        return PARSER_LENGTH_ERROR;
    }
    memmove(pc, &payload[1],2);
    memmove(pc, &payload[3],epc_len-2);
    *error = payload[3 + epc_len];
    return PARSER_SUCCESS;
}

//baud is baud rate in hz.
//the change is effective immediately
//there is no response from this frame
uint8_t* BuildSetBaudFrame(uint8_t* buf, uint32_t baud_hz)
{
    uint8_t cmd = CMD_SET_BAUD;
    uint16_t len = 2;
    uint8_t payload[2];
    payload[0] = ((baud/100) >> 8) && 0xff;
    payload[1] =  (baud/100) && 0xff;

    return build_cmd_frame(buf, cmd, len, payload);
}
// no return message


//XXX undocumented command
uint8_t* BuildSetCWFrame(uint8_t* buf, uint8_t setting)
{
    uint8_t cmd = CMD_SET_CW;
    uint16_t len = 1;
    return build_cmd_frame(buf, cmd, len, &setting);
}
int ReadSetCWFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return PARSER_NEEDS_WORK;
}
uint8_t* BuildSetFhssFrame(uint8_t* buf, uint8_t setting)
{
    uint8_t cmd = CMD_SET_FHSS;
    uint16_t len = 1;
    return build_cmd_frame(buf, cmd, len, &setting);
}
int ReadSetFhssFrame(uint16_t len, uint8_t *payload, uint8_t *error)
{
    return read_empty_frame(len, payload, error);
}

uint8_t* BuildSetModuleSleepFrame(uint8_t* buf)
{
    return build_cmd_empty_frame(buf, CMD_SLEEP_MODE);
}
//no return message

uint8_t* BuildSetPaPowerFrame(uint8_t* buf, float powerDBm)
{
    uint8_t cmd = CMD_SET_POWER;
    uint16_t len = 2;
    uint8_t payload[2];
    payload[0] = (((uint16_t)(powerDBm*100)) >> 8) && 0xff;
    payload[1] =  ((uint16_t)(powerDBm*100)) && 0xff;
    return build_cmd_frame(buf, cmd, len, payload);
}
int ReadSetPaPowerFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return read_empty_frame(len, payload, error);
}


uint8_t *BuildSetRegionFrame(uint8_t* buf, uint8_t region_code)
{
    uint8_t cmd = CMD_SET_REGION;
    uint16_t len = 1;
    return build_cmd_frame(buf, cmd, len, &region_code);
}
int ReadSetRegionFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return read_empty_frame(len, payload, error);
}


uint8_t* BuildSetRfChannelFrame(uint8_t* buf, uint8_t channel_index)
{
    uint8_t cmd = CMD_SET_RF_CHANNEL;
    uint16_t len = 1;
    return build_cmd_frame(buf, cmd, len, &channel_index);
}
int ReadSetRfChannelFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return PARSER_NEEDS_WORK
}

//XXX needs completion with EPC documentation
uint8_t* BuildSetSelectFrame(uint8_t* buf, uint8_t target, uint8_t action, uint8_t bank, uint32_t pointer, uint8_t mask_len, uint8_t* mask, bool truncate)
{
    uint8_t cmd = CMD_SET_SELECT_PARA;
    uint16_t len = 7 + mask_len;
    uint8_t select_byte = ((target &0x7)<<5) | ((action &0x7)<<2) | (bank &0x3);

    //move the mask first so memory can overlap
    memmove(&buf[7], mask, mask_len);

    buf[0] = select_byte;

    buf[1] = (pointer>>24) & 0xff;
    buf[2] = (pointer>>16) & 0xff;
    buf[3] = (pointer>> 8) & 0xff;
    buf[4] = (pointer>> 0) & 0xff;
    
    buf[5] = mask_len;

    if(truncate)
    {
        buf[6] = 0x80;
    }
    else
    {
        buf[6] = 0x00;
    }
    
    //mask starts at buf[7]

    return build_cmd_frame(buf, cmd, len, buf);
}

int ReadSetSelectFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return PARSER_NEEDS_WORK;
}
uint8_t* BuildWriteDataFrame(uint8_t* buf, uint8_t* password, uint8_t bank, uint32_t data_ptr, uint32_t data_len, uint8_t* data);
{
    uint8_t cmd = CMD_WRITE_DATA;
    uint16_t len = 9 + data_len;
    uint8_t* payload = buf;  //moved to correct frame position by build_cmd_frame
    uint8_t data_len_words = data_len/2;
    uint8_t data_ptr_words = data_ptr/2;

    //data and password may overlap with buf
    uint8_t pwd[4];
    memcpy(pwd, password, 4); 
    memmove(payload[9], data, data_len);
    memcpy(payload, pwd, 4);

    payload[4] = bank;
    payload[5] = (data_ptr_words >> 8) & 0xff;
    payload[6] = (data_ptr_words >> 0) & 0xff;
    payload[7] = (data_len_words >> 8) & 0xff;
    payload[8] = (data_len_words >> 0) & 0xff;
    
    return build_cmd_frame(buf, cmd, len, payload);
}
int ReadWriteDataFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return PARSER_NEEDS_WORK;
}
BuildReadDataFrame(uint8_t* buf, uint8_t* password, uint8_t bank, uint32_t data_ptr, uint32_t data_len)
{
    uint8_t cmd = CMD_READ_DATA;
    uint16_t len = 9;
    uint16_t data_len_words = data_len/2;
    uint16_t data_ptr_words = data_ptr/2;

    uint8_t* payload = buf; //moved to correct frame position by build_cmd_frame

    memmove(payload, password, 4);
    payload[4] = bank;
    payload[5] = (data_ptr_words >> 8) & 0xff;
    payload[6] = (data_ptr_words) & 0xff;
    payload[7] = (data_len_words >> 8) & 0xff;
    payload[8] = (data_len_words) & 0xff;
    
    return build_cmd_frame(buf, cmd, len, payload);
}
int ReadReadDataFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return PARSER_NEEDS_WORK;
}
uint8_t* BuildLockFrame(uint8_t* buf, uint8_t* password, uint8_t* lock_payload)
{
    uint8_t cmd = CMD_LOCK_UNLOCK;
    uint16_t len = 7;
    uint8_t* payload = buf; //moved to correct frame position by build_cmd_frame

    //allow the variables to overloap with buf
    uint8_t pwd[4];
    memcpy(pwd, password, 4); 
    memmove(&payload[4], lock_payload, 3)
    memcpy(payload, pwd, 4);

    return build_cmd_frame(buf, cmd, len, payload);
}
int ReadLockFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return PARSER_NEEDS_WORK;
}
uint8_t* BuildScanJammerFrame(uint8_t* buf)
{
    return build_cmd_empty_frame(buf, CMD_SCAN_JAMMER);
}
int ReadScanJammerFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return PARSER_NEEDS_WORK;
}

uint8_t* BuildScanRssiFrame(uint8_t* buf)
{
    return build_cmd_empty_frame(buf, CMD_SCAN_RSSI);
}
int ReadScanRssiFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return PARSER_NEEDS_WORK;
}

uint8_t* BuildSetInventoryModeFrame(uint8_t* buf, uint8_t mode);
{
    return build_cmd_frame(CMD_SET_INVENTORY_MODE, cmd, 1, &mode);
}
int ReadSetInventoryModeFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return PARSER_NEEDS_WORK;
}



uint8_t* BuildSetQueryFrame(uint8_t* buf, intDR, intM, intTRext, intSel, intSession, intTarget, intQ)
{
    uint8_t cmd = CMD_SET_QUERY;
    uint16_t len = 2;
    uint16_t bitmap = 

    uint8_t payload[2];    
    bitmap =
     (d & 0x01)<<15) |
     (m & 0x03)<<13) |
     (trext & 0x01)<<12) |
     (sel & 0x03)<<10) |
     (session & 0x03)<<8) |
     (target & 0x01)<<7) |
     (Q & 0x0F)<<3) ;
     //lowest three bits are zero.
    
    payload[0] = bitmap >> 8;
    payload[1] = bitmap & 0xff
    return build_cmd_frame(buf, cmd, len, payload);
}
int ReadSetQueryFrame(uint16_t len, uint8_t* payload, uint8_t *error)
{
    return PARSER_NEEDS_WORK;
}


/*
//not complete



BuildNXPChangeEasFrame(uint8_t* buf, strAccessPasswd, cbxSetEas.Checked)
{
    uint8_t cmd = CMD_NXP_CHANGE_EAS;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}

BuildNXPEasAlarmFrame(uint8_t* buf)
{
    uint8_t cmd = CMD_NXP_EAS_ALARM;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}

BuildNXPReadProtectFrame(uint8_t* buf, strAccessPasswd, cbxReadProtectReset.Checked)
{
    uint8_t cmd = CMD_NXP_READPROTECT;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}


BuildNXPResetReadProtectFrame(uint8_t* buf, strAccessPasswd, cbxReadProtectReset.Checked)
{
    uint8_t cmd = CMD_NXP_RESET_READPROTECT;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}










//undocumented commands
BuildSaveConfigToNvFrame(uint8_t* buf, NV_enable)
{
    uint8_t cmd = CMD_SAVE_NV_CONFIG;
    uint16_t len = ;
    return build_cmd_frame(buf, cmd, len, NULL);
}

BuildMonzaQTFrame(uint8_t* buf, strAccessPasswd, false, cbxMonzaQT_SR.Checked, cbxMonzaQT_MEM.Checked)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildNXPChangeConfigFrame(uint8_t* buf, strAccessPasswd, Convert.ToInt32(txtConfigData.Text.Replace(" ",""), 16))
{
    uint8_t cmd = CMD_NXP_CHANGE_CONFIG;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}

BuildReadModemParaFrame(uint8_t* buf)
{
    uint8_t cmd = CMD_READ_MODEM_PARA;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}

BuildSetModemParaFrame(uint8_t* buf, mixerGain, IFAmpGain, signalTh)
{
    uint8_t cmd = CMD_SET_MODEM_PARA;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}

BuildSetReaderEnvModeFrame(uint8_t* buf, (byte)cbxMode.SelectedIndex)
{
    uint8_t cmd = CMD_SET_READER_ENV_MODE;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
*/