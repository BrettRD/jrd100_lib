#include "parser.h"
#include "commands.h"

int parse_packet(size_t *buf_len, uint8_t* *buf)
{
    int parser_error = PARSER_NEEDS_WORK;

    uint8_t frame_type = 0;
    uint8_t cmd = 0;
    uint16_t len = 0;
    uint8_t* payload = NULL;

    parser_error = find_frame_begin(buf_len, buf);
    if(parser_error != PARSER_SUCCESS)
    {
        return parser_error;
    }

    parser_error = read_frame(buf_len, buf, &frame_type, &cmd, &len, &payload);
    if(parser_error != PARSER_SUCCESS)
    {
        return parser_error;
    }

    parser_error = parse_frame(frame_type, cmd, len, payload);

    return parser_error;
}


int parse_frame(uint8_t frame_type uint8_t cmd, uint16_t len, uint8_t* payload)
{
    int parser_error = PARSER_NEEDS_WORK;
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
            uint8_t target;
            uint8_t action;
            uint8_t bank;
            uint32_t pointer;
            uint8_t mask_len;
            uint8_t* mask;
            bool truncate;
            parser_error = ReadGetSelectFrame(len, payload, &target, &action, &bank, &pointer, &mask_len, &mask, &truncate);
            if((parser_error == PARSER_SUCCESS) && (cb_get_select))
            {
                cb_get_select(target, action, bank, pointer, mask_len, mask, truncate);
            }
        }
        break;
        case CMD_SET_SELECT_PARA:
        {
            uint8_t error;
            parser_error = ReadSetSelectFrame(len, payload, &error);
            if((parser_error == PARSER_SUCCESS) && (cb_set_Select))
            {
                cb_set_Select(error);
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
            //figure out how to track pin state
            //probably direciton and level as high and low nibbles
            if((parser_error == PARSER_SUCCESS) && (cb_io_frame))
            {
                cb_io_frame(pin, config, dir);
            }
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
        case CMD_WRITE_DATA:
        {
            uint8_t epc_len;
            uint16_t pc;
            uint8_t* epc;
            uint8_t error;
            parser_error = ReadWriteDataFrame(len, payload, &pc, &epc_len, &epc, &error);
            if((parser_error == PARSER_SUCCESS) && (cb_write_data))
            {
                cb_write_data(epc_len, pc, epc, error);
            }
        }
        break;
        case CMD_KILL:
        {
            uint8_t epc_len;
            uint16_t pc;
            uint8_t* epc;
            uint8_t error;
            parser_error = ReadKillFrame(len, payload, &pc, &epc_len, &epc, &error);
            if((parser_error == PARSER_SUCCESS) && (cb_kill))
            {
                cb_kill(epc_len, pc, epc, error);
            }
        }
        break;
        case CMD_LOCK_UNLOCK:
        {
            uint8_t epc_len;
            uint16_t pc;
            uint8_t* epc;
            uint8_t error;
            parser_error = ReadLockFrame(len, payload, &pc, &epc_len, &epc, &error);
            if((parser_error == PARSER_SUCCESS) && (cb_lock))
            {
                cb_lock(epc_len, pc, epc, error);
            }
        }
        break;
        case CMD_SCAN_JAMMER:
        {
            uint8_t ch_start;
            uint8_t ch_end;
            uint8_t* channel_noise;
            parser_error = ReadScanJammerFrame(len, payload, &ch_start, &ch_end, &channel_noise);
            if((parser_error == PARSER_SUCCESS) && (cb_scan_jammer))
            {
                cb_scan_jammer(ch_start, ch_end, channel_noise);
            }
        }
        break;
        case CMD_SCAN_RSSI:
        {
            uint8_t ch_start;
            uint8_t ch_end;
            uint8_t* channel_rssi;
            parser_error = ReadScanRssiFrame(len, payload, &ch_start, &ch_end, &channel_rssi);
            if((parser_error == PARSER_SUCCESS) && (cb_scan_rssi))
            {
                cb_scan_rssi(ch_start, ch_end, channel_rssi);
            }
        }
        break;
        case CMD_SET_FHSS:
        {
            uint8_t error;
            parser_error = ReadSetFhssFrame(len, payload, &error);
            if((parser_error == PARSER_SUCCESS) && (cb_set_fhss))
            {
                cb_set_fhss(error);
            }
        }
        break;
        case CMD_SET_INVENTORY_MODE:
        {
            uint8_t error;
            parser_error = ReadSetInventoryModeFrame(len, payload, &error);
            if((parser_error == PARSER_SUCCESS) && (cb_set_inventory_mode))
            {
                cb_set_inventory_mode(error);
            }
        }
        break;
        case CMD_SET_REGION:
        {
            uint8_t error;
            parser_error = ReadSetRegionFrame(len, payload, &error);
            if((parser_error == PARSER_SUCCESS) && (cb_set_region))
            {
                cb_set_region(error);
            }
        }
        break;
        case CMD_SET_SLEEP_TIME:
        {
            uint8_t idle_minutes;
            parser_error = ReadSetSleepTimeFrame(len, payload, &idle_minutes);
            if((parser_error == PARSER_SUCCESS) && (cb_set_sleep_time))
            {
                cb_set_sleep_time(idle_minutes);
            }
        }
        break;

        case CMD_RESTART:
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
