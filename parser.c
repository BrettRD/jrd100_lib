#include "parser.h"
#include "commands.h"

int parse_frame(uint8_t* frame)
{
    int parser_error = PARSER_NEEDS_WORK;
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
                parser_error = parse_ans_frame(cmd, len, payload);
            }
            break;
            case FRAME_TYPE_CMD:
            {
                parser_error = parse_cmd_frame(cmd, len, payload);
            }
            break;
            case FRAME_TYPE_INFO:
            {
                parser_error = parse_info_frame(cmd, len, payload);
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
        parser_error = PARSER_MALFORMED_PACKET; //malformed packet
    }
    return parser_error;
}

int parse_ans_frame(uint8_t cmd, uint16_t len, uint8_t* payload)
{
    int parser_error = PARSER_NEEDS_WORK;
    switch(cmd)
    {
        case CMD_EXE_FAILED:
        {
            parser_error = parse_error_frame(len, payload);
        }
        break;
        case CMD_GET_MODULE_INFO:
        {
            uint8_t info_type;
            uint16_t info_len;
            uint8_t* info;
            parser_error = ReadGetModuleInfoFrame(len, payload, &info_type, &info_len, &info)
            if((parser_error == PARSER_SUCCESS) && (cb_module_info))
            {
                cb_module_info(info_type, info_len, info);
            }
        }
        break;
        case CMD_GET_POWER:
        {

            uint16_t power;
            parser_error = ReadGetPaPowerFrame(len, payload, &power);
            if((parser_error == PARSER_SUCCESS) && (cb_get_power))
            {
                cb_get_power(power);
            }
        }
        break;
        case CMD_SET_POWER:
        {
            uint8_t error;
            parser_error = ReadSetPaPowerFrame(len, payload, &error);
            if((parser_error == PARSER_SUCCESS) && (cb_set_power))
            {
                cb_set_power(error);
            }
        }
        break;
        case CMD_GET_QUERY:
        {
            uint8_t dr;
            uint8_t m;
            uint8_t trext;
            uint8_t sel;
            uint8_t session;
            uint8_t target;
            uint8_t q;
            parser_error = ReadGetQueryFrame(len, payload, &dr, &m, &trext, &sel, &session, &target, &q);
            if((parser_error == PARSER_SUCCESS) && (cb_get_query))
            {
                cb_get_query(dr, m, trext, sel, session, target, q);
            }
        }
        break;
        case CMD_SET_QUERY:
        {
            uint8_t error;
            parser_error = ReadSetQueryFrame(len, payload, &error);
            if((parser_error == PARSER_SUCCESS) && (cb_set_query))
            {
                cb_set_query(error);
            }
        }
        break;

        case CMD_GET_RF_CHANNEL:
        {
            uint8_t channel_index;
            parser_error = int ReadGetRfChannelFrame(len, payload, &channel_index);
            if((parser_error == PARSER_SUCCESS) && (cb_get_rf_channel))
            {
                cb_get_rf_channel(channel_index);
            }
        }
        break;
        case CMD_SET_RF_CHANNEL:
        {
            uint8_t error;
            parser_error = ReadSetRfChannelFrame(len, payload, &error);
            if((parser_error == PARSER_SUCCESS) && (cb_set_rf_channel))
            {
                cb_set_rf_channel(error);
            }
        }
        break;
        case CMD_GET_SELECT_PARA:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case CMD_SET_SELECT_PARA:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case CMD_INVENTORY:
        case CMD_READ_MULTI:
        {
            uint8_t rssi;
            uint16_t pc;
            uint8_t epc_len;
            uint8_t* epc;
            uint16_t crc;
            parser_error = ReadTagNotification(len, payload, &rssi, &pc, &epc_len, &epc, &crc);
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
            parser_error = ReadStopReadFrame(len, payload, &param);
            //XXX callback
            //param should always be zero, docs unclear
            if((parser_error == PARSER_SUCCESS) && (cb_io_frame))
            {
                cb_stop_frame(error);
            }
        }
        break;

        case CMD_IO_CONTROL:
        {
            uint8_t pin;
            uint8_t config;
            uint8_t dir;
            parser_error = ReadIoControlFrame(len, payload, &pin, &config, &dir)
            //run callback
            //figure out how to track pin state
            //probably direciton and level as high and low nibbles
            if((parser_error == PARSER_SUCCESS) && (cb_io_frame))
            {
                cb_io_frame(pin, config, dir);
            }
        }
        break;
        case CMD_KILL:
        {
            uint8_t epc_len;
            uint8_t pc[2];
            uint8_t epc[64];
            uint8_t error;
            parser_error = ReadKillFrame(len, payload, &epc_len, pc, epc, &error);

            if((parser_error == PARSER_SUCCESS) && (cb_kill))
            {
                cb_kill(epc_len, pc, epc, error);
            }
        }
        break;
        case CMD_LOCK_UNLOCK:
        {
            parser_error = ReadLockFrame(uint16_t len, uint8_t* payload, uint8_t *error);

        }
        break;
        case CMD_READ_DATA:
        {
            uint16_t pc;
            uint8_t epc_len;
            uint8_t* epc;
            uint8_t data_len;
            uint8_t* data;

            parser_error = ReadReadDataFrame(len, payload, &pc, &epc_len, &epc, &data_len, &data);

            if((parser_error == PARSER_SUCCESS) && (cb_read_data))
            {
                cb_read_data(pc, epc_len, epc, data_len, data);
            }
        }
        break;
        case CMD_RESTART:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case CMD_SCAN_JAMMER:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case CMD_SCAN_RSSI:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case CMD_SET_FHSS:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case CMD_SET_INVENTORY_MODE:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case CMD_SET_REGION:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case CMD_SET_SLEEP_TIME:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case CMD_WRITE_DATA:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;

        case CMD_LOAD_NV_CONFIG:
        case CMD_SAVE_NV_CONFIG:
        case CMD_NXP_CHANGE_CONFIG:
        case CMD_NXP_CHANGE_EAS:
        case CMD_NXP_EAS_ALARM:
        case CMD_NXP_READPROTECT:
        case CMD_NXP_RESET_READPROTECT:
        case CMD_READ_MODEM_PARA:
        case CMD_SET_CW:
        case CMD_SET_MODEM_PARA:
        case CMD_SET_READER_ENV_MODE:
        case CMD_INSERT_FHSS_CHANNEL:
        {
            parser_error = PARSER_UNDOCUMENTED_CMD;
        }
        default:
        {
            parser_error = PARSER_UNKNOWN_CMD;
        }
    }
}



void parse_error_frame(uint16_t len, uint8_t* payload)
{
    int parser_error = PARSER_NEEDS_WORK;
    uint8_t error = payload[0];
    switch(error)
    {
        case FAIL_ACCESS_PWD_ERROR:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case FAIL_CUSTOM_CMD_BASE:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case FAIL_FHSS_FAIL:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case FAIL_INVALID_CMD:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case FAIL_INVALID_PARA:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case FAIL_INVENTORY_TAG_TIMEOUT:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case FAIL_KILL_ERROR_CODE_BASE:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case FAIL_KILL_NO_TAG:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case FAIL_LOCK_ERROR_CODE_BASE:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case FAIL_LOCK_NO_TAG:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case FAIL_NXP_CHANGE_CONFIG_NO_TAG:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case FAIL_NXP_CHANGE_EAS_NOT_SECURE:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case FAIL_NXP_CHANGE_EAS_NO_TAG:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case FAIL_NXP_EAS_ALARM_NO_TAG:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case FAIL_NXP_READPROTECT_NO_TAG:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case FAIL_NXP_RESET_READPROTECT_NO_TAG:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case FAIL_READ_ERROR_CODE_BASE:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case FAIL_READ_MEMORY_NO_TAG:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case FAIL_WRITE_ERROR_CODE_BASE:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        case FAIL_WRITE_MEMORY_NO_TAG:
        {
            parser_error = ;
            if((parser_error == PARSER_SUCCESS) && (cb__))
            {
                cb__();
            }
        }
        break;
        default:
        {
            //unknown error code
            parser_error = PARSER_UNKNOWN_CMD;
        }
    }
}

int parse_info_frame(uint8_t cmd, uint16_t len, uint8_t* payload)
{
    int parser_error = PARSER_NEEDS_WORK;
    return parser_error;
}
