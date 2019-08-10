#include "parser.h"
#include "commands.h"

int parse_frame(uint8_t* frame)
{
    int retval = PARSER_NEEDS_WORK;
    uint8_t frame_type = 0;
    uint8_t cmd = 0;
    uint16_t len = 0;
    uint8_t* payload = read_frame(uint8_t* frame, &frame_type, &cmd, &len);
    if(payload)
    {
        switch(frame_type)
        {
            case FRAME_TYPE_ANS:
            {
                retval = parse_ans_frame(cmd, len, payload);
            }
            break;
            case FRAME_TYPE_CMD:
            {
                retval = parse_cmd_frame(cmd, len, payload);
            }
            break;
            case FRAME_TYPE_INFO:
            {
                retval = parse_info_frame(cmd, len, payload);
            }
            break;
            default:
            {
                //unknown frame type
            }
            break;
        }
    }
    else
    {
        retval = PARSER_MALFORMED_PACKET; //malformed packet
    }
    return retval;
}

int parse_ans_frame(uint8_t cmd, uint16_t len, uint8_t* payload)
{
    int retval = PARSER_NEEDS_WORK;
    switch(cmd)
    {
        case CMD_EXE_FAILED:
        {
            retval = parse_error_frame(len, payload);
        }
        break;
        case CMD_GET_MODULE_INFO:
        {
            retval = 
        }
        break;
        case CMD_GET_POWER:
        {
            retval = 
        }
        break;
        case CMD_GET_QUERY:
        {
            retval = 
        }
        break;
        case CMD_GET_RF_CHANNEL:
        {
            retval = 
        }
        break;
        case CMD_GET_SELECT_PARA:
        {
            retval = 
        }
        break;
        case CMD_INSERT_FHSS_CHANNEL:
        {
            retval = 
        }
        break;
        case CMD_INVENTORY:
        case CMD_READ_MULTI:
        {
            uint8_t rssi;
            uint16_t pc;
            uint8_t epc_len;
            uint8_t epc[64];    //XXX these probably have a fixed size
            uint8_t crc[2];
            retval = ReadTagNotification(len, payload, &rssi, &pc, &epc_len, epc, crc);
            //XXX callback
            if(cmd == CMD_INVENTORY)
            {
                cb_tag_single_notification(rssi, pc, epc_len, epc, crc);
            }
            else
            {
                cb_tag_multi_notification(rssi, pc, epc_len, epc, crc);
            }
        }
        break;
        case CMD_STOP_MULTI:
        {
            uint8_t param;
            retval = ReadStopReadFrame(len, payload, &param);
            //XXX callback
            //param should always be zero, docs unclear
            if(cb_stop_frame) cb_stop_frame(error);
        }
        break;

        case CMD_IO_CONTROL:
        {
            uint8_t pin;
            uint8_t config;
            uint8_t dir;
            retval = read_io_payload(len, payload, &pin, &config, &dir)
            //run callback
            //figure out how to track pin state
            //probably direciton and level as high and low nibbles
            if(cb_io_frame) cb_io_frame(pin, config, dir);
        }
        break;
        case CMD_KILL:
        {
            uint8_t epc_len;
            uint8_t pc[2];
            uint8_t epc[64];
            uint8_t error;
            retval = ReadKillFrame(len, payload, &epc_len, pc, epc, &error);
            //callback
            if(cb_kill_frame) cb_kill_frame(epc_len, pc, epc, error);
        }
        break;
        case CMD_LOAD_NV_CONFIG:
        {
            //XXX undocumented
            retval = 
        }
        break;
        case CMD_SAVE_NV_CONFIG:
        {
            //XXX undocumented
            retval = 
        }
        break;
        case CMD_LOCK_UNLOCK:
        {
            retval = 
        }
        break;
        case CMD_NXP_CHANGE_CONFIG:
        {
            retval = 
        }
        break;
        case CMD_NXP_CHANGE_EAS:
        {
            retval = 
        }
        break;
        case CMD_NXP_EAS_ALARM:
        {
            retval = 
        }
        break;
        case CMD_NXP_READPROTECT:
        {
            retval = 
        }
        break;
        case CMD_NXP_RESET_READPROTECT:
        {
            retval = 
        }
        break;
        case CMD_READ_DATA:
        {
            retval = 
        }
        break;
        case CMD_READ_MODEM_PARA:
        {
            retval = 
        }
        break;
        case CMD_RESTART:
        {
            retval = 
        }
        break;
        case CMD_SCAN_JAMMER:
        {
            retval = 
        }
        break;
        case CMD_SCAN_RSSI:
        {
            retval = 
        }
        break;
        case CMD_SET_CW:
        {
            retval = 
        }
        break;
        case CMD_SET_FHSS:
        {
            retval = 
        }
        break;
        case CMD_SET_INVENTORY_MODE:
        {
            retval = 
        }
        break;
        case CMD_SET_MODEM_PARA:
        {
            retval = 
        }
        break;
        case CMD_SET_POWER:
        {
            retval = 
        }
        break;
        case CMD_SET_QUERY:
        {
            retval = 
        }
        break;
        case CMD_SET_READER_ENV_MODE:
        {
            retval = 
        }
        break;
        case CMD_SET_REGION:
        {
            retval = 
        }
        break;
        case CMD_SET_RF_CHANNEL:
        {
            retval = 
        }
        break;
        case CMD_SET_SELECT_PARA:
        {
            retval = 
        }
        break;
        case CMD_SET_SLEEP_TIME:
        {
            retval = 
        }
        break;
        case CMD_WRITE_DATA:
        {
            retval = 
        }
        break;

        default:
        {
            //unknown answer code
            retval = PARSER_UNKNOWN_CMD;
        }
    }
}



void parse_error_frame(uint16_t len, uint8_t* payload)
{
    uint8_t error = payload[0];
    switch(error)
    {
        case FAIL_ACCESS_PWD_ERROR:
        {

        }
        break;
        case FAIL_CUSTOM_CMD_BASE:
        {

        }
        break;
        case FAIL_FHSS_FAIL:
        {

        }
        break;
        case FAIL_INVALID_CMD:
        {

        }
        break;
        case FAIL_INVALID_PARA:
        {

        }
        break;
        case FAIL_INVENTORY_TAG_TIMEOUT:
        {

        }
        break;
        case FAIL_KILL_ERROR_CODE_BASE:
        {

        }
        break;
        case FAIL_KILL_NO_TAG:
        {

        }
        break;
        case FAIL_LOCK_ERROR_CODE_BASE:
        {

        }
        break;
        case FAIL_LOCK_NO_TAG:
        {

        }
        break;
        case FAIL_NXP_CHANGE_CONFIG_NO_TAG:
        {

        }
        break;
        case FAIL_NXP_CHANGE_EAS_NOT_SECURE:
        {

        }
        break;
        case FAIL_NXP_CHANGE_EAS_NO_TAG:
        {

        }
        break;
        case FAIL_NXP_EAS_ALARM_NO_TAG:
        {

        }
        break;
        case FAIL_NXP_READPROTECT_NO_TAG:
        {

        }
        break;
        case FAIL_NXP_RESET_READPROTECT_NO_TAG:
        {

        }
        break;
        case FAIL_READ_ERROR_CODE_BASE:
        {

        }
        break;
        case FAIL_READ_MEMORY_NO_TAG:
        {

        }
        break;
        case FAIL_WRITE_ERROR_CODE_BASE:
        {

        }
        break;
        case FAIL_WRITE_MEMORY_NO_TAG:
        {

        }
        break;
        default:
        {
            //unknown error code
            retval = PARSER_UNKNOWN_CMD;
        }
    }
}

int parse_info_frame(uint8_t cmd, uint16_t len, uint8_t* payload)
{
    return PARSER_NEEDS_WORK;
}
