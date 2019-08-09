#include "parser.h"



uint8_t* build_cmd_empty_frame(uint8_t*buf, uint8_t cmd)
{
    return build_cmd_frame(buf, cmd, 0, NULL)

}

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


/*
 * reads data from a frame into pointers
 * returns the payload pointer into the frame buffer, or null if the frame is malformed
 */

uint8_t* read_frame(uint8_t* frame, uint16_t *len)
{
    uint8_t frame_header = frame[0];
    uint8_t frame_type = frame[1];
    uint8_t cmd = frame[2];
    *len = (frame[3] << 8) + frame[4];
    *payload = frame[5];
    uint8_t received_cs = frame[5 + *len];
    uint8_t frame_terminator = frame[6 + *len];
    uint8_t expected_cs = calc_frame_checksum(buf, *len);

    if(frame_header == FRAME_BEGIN)
    {
        if(expected_cs == received_cs)
        {
            if(frame_terminator == FRAME_END)
            {
                return *payload;
            }
        }
    }

    //no access to failed frames
    *payload = NULL;
    *len = 0;
    return *payload;
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
BuildGetModuleInfoFrame(uint8_t* buf, uint8_t field)
{
    uint8_t cmd = CMD_GET_MODULE_INFO;
    uint16_t len = 1;
    return build_cmd_frame(buf, cmd, len, field);
}

BuildGetPaPowerFrame(uint8_t* buf)
{
    return build_cmd_empty_frame(buf, CMD_GET_POWER);
}
BuildGetQueryFrame(uint8_t* buf)
{
    return build_cmd_empty_frame(buf, CMD_GET_QUERY);
}
BuildGetRfChannelFrame(uint8_t* buf)
{
    return build_cmd_empty_frame(buf, CMD_GET_RF_CHANNEL);
}
BuildGetSelectFrame(uint8_t* buf, )
{
    return build_cmd_frame(buf, CMD_GET_SELECT_PARA);
}
BuildInsertRfChFrame(uint8_t* buf, channelNum, channelList)
{
    //CMD_INSERT_FHSS_CHANNEL ?
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, );
}
uint8_t* build_io_payload(uint8_t* payload, uint8_t pin, uint8_t config, uint8_t dir)
{
    //config = 0; //set io direction (input vs output)
    //config = 1; //set io output level
    //config = 2; //read io input level

    //dir = 0; //input, low, fail
    //dir = 1; //output, high, sucess

    //check the config makes sense
    if(config >= 3)
    {
        return NULL;
    }

    //check the pin is in range
    if((pin == 0) || (pin > 4))
    {
        return NULL;
    }

    //check the config makes sense
    if(dir >= 2)
    {
        return NULL;
    }

    payload[0] = config;
    payload[1] = pin;
    payload[2] = dir;
    return payload;
}

void read_io_payload(uint8_t* payload, uint8_t* pin, uint8_t* config, uint8_t* dir)
{
    *config = payload[0];
    *pin = payload[1];
    *dir = payload[2];
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

/*
 *  read at most tag_count tags
 */
BuildReadMultiFrame(uint8_t* buf, uint16_t tag_count)
{
    uint8_t cmd = CMD_READ_MULTI;
    uint16_t len = 3;
    uint8_t[3] payload;
    payload[0] = 0x22;  //reserved byte, must be 0x22
    payload[1] = tag_count >> 8;
    payload[2] = tag_count & 0xff;

    return build_cmd_frame(buf, cmd, len, payload);
}
BuildStopReadFrame(uint8_t* buf)
{
    return build_cmd_empty_frame(buf, CMD_STOP_MULTI);
}

BuildReadSingleFrame(uint8_t* buf)
{
    return build_cmd_empty_frame(buf, CMD_INVENTORY);
}


BuildKillFrame(uint8_t* buf, uint8_t* password)
{
    return build_cmd_frame(buf, CMD_KILL, 4, password);
}

//baud is baud rate in hz.
//the change is effective immediately
BuildSetBaudFrame(uint8_t* buf, uint32_t baud_hz)
{
    uint8_t cmd = CMD_SET_BAUD;
    uint16_t len = 2;
    uint8_t payload[2];
    payload[0] = ((baud/100) >> 8) && 0xff;
    payload[1] =  (baud/100) && 0xff;

    return build_cmd_frame(buf, cmd, len, payload);
}

//XXX undocumented command
BuildSetCWFrame(uint8_t* buf, uint8_t setting)
{
    uint8_t cmd = CMD_SET_CW;
    uint16_t len = 1;
    return build_cmd_frame(buf, cmd, len, &setting);
}

BuildSetFhssFrame(uint8_t* buf, uint8_t setting)
{
    uint8_t cmd = CMD_SET_FHSS;
    uint16_t len = 1;
    return build_cmd_frame(buf, cmd, len, &setting);
}

BuildSetModuleSleepFrame(uint8_t* buf)
{
    return build_cmd_frame(buf, CMD_SLEEP_MODE);
}

BuildSetPaPowerFrame(uint8_t* buf, float powerDBm)
{
    uint8_t cmd = CMD_SET_POWER;
    uint16_t len = 2;
    uint8_t payload[2];
    payload[0] = (((uint16_t)(powerDBm*100)) >> 8) && 0xff;
    payload[1] =  ((uint16_t)(powerDBm*100)) && 0xff;
    return build_cmd_frame(buf, cmd, len, payload);
}
BuildSetRegionFrame(uint8_t* buf, uint8_t region_code)
{
    uint8_t cmd = CMD_SET_REGION;
    uint16_t len = 1;
    return build_cmd_frame(buf, cmd, len, &region_code);
}
BuildSetRfChannelFrame(uint8_t* buf, uint8_t channel_index)
{
    uint8_t cmd = CMD_SET_RF_CHANNEL;
    uint16_t len = 1;
    return build_cmd_frame(buf, cmd, len, &channel_index);
}

//XXX needs completion with EPC documentation
BuildSetSelectFrame(uint8_t* buf, uint8_t target, uint8_t action, uint8_t bank, uint32_t pointer, uint8_t mask_len, uint8_t* mask, bool truncate)
{
    uint8_t cmd = CMD_SET_SELECT_PARA;
    uint16_t len = 7 + mask_len;
    uint8_t select_byte = ((target &0x3)<<5) | ((action &0x3)<<2 | (bank &0x2);

    
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

BuildWriteDataFrame(uint8_t* buf, uint8_t* password, uint8_t bank, uint32_t data_ptr, uint32_t data_len, uint8_t* data);
{
    uint8_t cmd = CMD_WRITE_DATA;
    uint16_t len = 9 + data_len;
    uint8_t* payload = buf;
    uint8_t data_len_words = data_len/2;
    uint8_t data_ptr_words = data_ptr/2;

    //data may overlap with buf
    memmove(payload[9], data, data_len);

    memmove(payload, password, 4);

    payload[4] = bank;
    payload[5] = (data_ptr_words >> 8) & 0xff;
    payload[6] = (data_ptr_words >> 0) & 0xff;
    payload[7] = (data_len_words >> 8) & 0xff;
    payload[8] = (data_len_words >> 0) & 0xff;
    
    return build_cmd_frame(buf, cmd, len, payload);
}

BuildReadDataFrame(uint8_t* buf, strAccessPasswd, intMemBank, wordPtr, wordCnt)
{
    uint8_t cmd = CMD_READ_DATA;
    uint16_t len = 9;
    uint8_t data_len_words = data_len/2;
    uint8_t data_ptr_words = data_ptr/2;

    uint8_t* payload = buf;

    memmove(payload, password, 4);
    payload[4] = bank;
    payload[5] = (data_ptr_words >> 8) & 0xff;
    payload[6] = (data_ptr_words) & 0xff;
    payload[7] = (data_len_words >> 8) & 0xff;
    payload[8] = (data_len_words) & 0xff;
    
    return build_cmd_frame(buf, cmd, len, NULL);
}

BuildLockFrame(uint8_t* buf, uint8_t* password, uint8_t* lock_payload)
{
    uint8_t cmd = CMD_LOCK_UNLOCK;
    uint16_t len = 7;
    uint8_t* payload = buf;

    memmove(&payload[4], lock_payload, 3)

    memmove(payload, password, 4);

    return build_cmd_frame(buf, cmd, len, payload);
}

BuildScanJammerFrame(uint8_t* buf)
{
    return build_cmd_frame(buf, CMD_SCAN_JAMMER);
}
BuildScanRssiFrame(uint8_t* buf)
{
    return build_cmd_empty_frame(buf, CMD_SCAN_RSSI);
}
BuildSetInventoryModeFrame(uint8_t* buf, uint8_t mode);
{
    return build_cmd_frame(CMD_SET_INVENTORY_MODE, cmd, 1, &mode);
}



BuildSetQueryFrame(uint8_t* buf, intDR, intM, intTRext, intSel, intSession, intTarget, intQ)
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
     (Q & 0x0F)<<3)
    
    payload[0] = bitmap >> 8;
    payload[1] = bitmap & 0xff
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
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildNXPChangeEasFrame(uint8_t* buf, strAccessPasswd, cbxSetEas.Checked)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildNXPEasAlarmFrame(uint8_t* buf)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildNXPReadProtectFrame(uint8_t* buf, strAccessPasswd, cbxReadProtectReset.Checked)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}

BuildReadModemParaFrame(uint8_t* buf)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}

BuildSaveConfigToNvFrame(uint8_t* buf, NV_enable)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}

BuildSetModemParaFrame(uint8_t* buf, mixerGain, IFAmpGain, signalTh)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}




BuildSetReaderEnvModeFrame(uint8_t* buf, (byte)cbxMode.SelectedIndex)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}