#include "parser.h"
#include "commands.h"
#include "callbacks.h"



const float REGION_FREQ_STARTS[] = 
    {
        REGION_CHN2_FREQ_START,
        REGION_US_FREQ_START,
        REGION_EUR_FREQ_START,
        REGION_CHN1_FREQ_START,
        REGION_JAPAN_FREQ_START,
        REGION_KOREA_FREQ_START
    };

const float REGION_FREQ_SEPARATIONS[] = 
    {
        REGION_CHN2_FREQ_SEPARATION,
        REGION_US_FREQ_SEPARATION,
        REGION_EUR_FREQ_SEPARATION,
        REGION_CHN1_FREQ_SEPARATION,
        REGION_JAPAN_FREQ_SEPARATION,
        REGION_KOREA_FREQ_SEPARATION
    };

const char* parser_error_strings[] = {
    "PARSER_SUCCESS",
    "PARSER_UNDERFULL",
    "PARSER_MALFORMED_HEADER",
    "PARSER_MALFORMED_TERMINATOR",
    "PARSER_LENGTH_ERROR",
    "PARSER_CHECKSUM_ERROR",
    "PARSER_UNKNOWN_FRAME_TYPE",
    "PARSER_UNKNOWN_CMD",
    "PARSER_UNKNOWN_ERROR_CODE",
    "PARSER_UNDOCUMENTED_CMD",
    "PARSER_NEEDS_WORK"
};



int parse_packet(jdm_100_cb_t *cb, size_t *buf_len, uint8_t* *buf)
{
    int parser_error = PARSER_NEEDS_WORK;

    uint8_t frame_type = 0;
    uint8_t cmd = 0;
    uint16_t len = 0;
    uint8_t* payload = NULL;

    //find the first frame that will parse to the checksum and termination byte
    while((parser_error != PARSER_SUCCESS) && (parser_error != PARSER_UNDERFULL))
    {
        parser_error = read_frame(buf_len, buf, &frame_type, &cmd, &len, &payload);
        if((parser_error != PARSER_SUCCESS) && (parser_error != PARSER_UNDERFULL))
        {
            skip_byte(buf_len, buf);
        }
    }
    if(parser_error != PARSER_SUCCESS)
    {
        return parser_error;
    }

    parser_error = parse_frame(cb, frame_type, cmd, len, payload);

    if
    ((
        (parser_error == PARSER_NEEDS_WORK) ||
        (parser_error == PARSER_UNKNOWN_FRAME_TYPE) ||
        (parser_error == PARSER_UNDOCUMENTED_CMD) ||
        (parser_error == PARSER_UNKNOWN_CMD) ||
        (parser_error == PARSER_UNKNOWN_ERROR_CODE)
    ) && (cb->cb_parser_error))
    {
        cb->cb_parser_error(cb->context, parser_error, frame_type, cmd, len, payload);
    }

    return parser_error;
}


int parse_frame(jdm_100_cb_t *cb, uint8_t frame_type, uint8_t cmd, uint16_t len, uint8_t* payload)
{
    int parser_error = PARSER_NEEDS_WORK;
    switch(frame_type)
    {
        case FRAME_TYPE_ANS:
        {
            parser_error = parse_ans_frame(cb, cmd, len, payload);
        }
        break;
        case FRAME_TYPE_CMD:
        {
            parser_error = parse_cmd_frame(cb, cmd, len, payload);
        }
        break;
        case FRAME_TYPE_INFO:
        {
            parser_error = parse_info_frame(cb, cmd, len, payload);
        }
        break;
        default:
        {
            //unknown frame type
            parser_error = PARSER_UNKNOWN_FRAME_TYPE;
        }
        break;
    }

    return parser_error;
}

int parse_ans_frame(jdm_100_cb_t *cb, uint8_t cmd, uint16_t len, uint8_t* payload)
{
    int parser_error = PARSER_NEEDS_WORK;
    switch(cmd)
    {
        case CMD_EXE_FAILED:
        {
            parser_error = parse_error_frame(cb, len, payload);
        }
        break;
        case CMD_GET_MODULE_INFO:
        {
            uint8_t info_type;
            uint16_t info_len;
            uint8_t* info;
            parser_error = ReadGetModuleInfoFrame(len, payload, &info_type, &info_len, &info);
            if((parser_error == PARSER_SUCCESS) && (cb->cb_module_info))
            {
                cb->cb_module_info(cb->context, info_type, info_len, info);
            }
        }
        break;
        case CMD_GET_POWER:
        {
            uint16_t power;
            parser_error = ReadGetPaPowerFrame(len, payload, &power);
            if((parser_error == PARSER_SUCCESS) && (cb->cb_get_power))
            {
                cb->cb_get_power(cb->context, power);
            }
        }
        break;
        case CMD_SET_POWER:
        {
            uint8_t error;
            parser_error = ReadSetPaPowerFrame(len, payload, &error);
            if((parser_error == PARSER_SUCCESS) && (cb->cb_set_power))
            {
                cb->cb_set_power(cb->context, error);
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
            if((parser_error == PARSER_SUCCESS) && (cb->cb_get_query))
            {
                cb->cb_get_query(cb->context, dr, m, trext, sel, session, target, q);
            }
        }
        break;
        case CMD_SET_QUERY:
        {
            uint8_t error;
            parser_error = ReadSetQueryFrame(len, payload, &error);
            if((parser_error == PARSER_SUCCESS) && (cb->cb_set_query))
            {
                cb->cb_set_query(cb->context, error);
            }
        }
        break;

        case CMD_GET_RF_CHANNEL:
        {
            uint8_t channel_index;
            parser_error = ReadGetRfChannelFrame(len, payload, &channel_index);
            if((parser_error == PARSER_SUCCESS) && (cb->cb_get_rf_channel))
            {
                cb->cb_get_rf_channel(cb->context, channel_index);
            }
        }
        break;
        case CMD_SET_RF_CHANNEL:
        {
            uint8_t error;
            parser_error = ReadSetRfChannelFrame(len, payload, &error);
            if((parser_error == PARSER_SUCCESS) && (cb->cb_set_rf_channel))
            {
                cb->cb_set_rf_channel(cb->context, error);
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
            if((parser_error == PARSER_SUCCESS) && (cb->cb_get_select))
            {
                cb->cb_get_select(cb->context, target, action, bank, pointer, mask_len, mask, truncate);
            }
        }
        break;
        case CMD_SET_SELECT_PARA:
        {
            uint8_t error;
            parser_error = ReadSetSelectFrame(len, payload, &error);
            if((parser_error == PARSER_SUCCESS) && (cb->cb_set_Select))
            {
                cb->cb_set_Select(cb->context, error);
            }
        }
        break;
        case CMD_STOP_MULTI:
        {
            uint8_t error;
            parser_error = ReadStopReadFrame(len, payload, &error);
            if((parser_error == PARSER_SUCCESS) && (cb->cb_stop_frame))
            {
                cb->cb_stop_frame(cb->context, error);
            }
        }
        break;
        case CMD_IO_CONTROL:
        {
            uint8_t pin;
            uint8_t config;
            uint8_t dir;
            parser_error = ReadIoControlFrame(len, payload, &pin, &config, &dir);
            //figure out how to track pin state
            //probably direciton and level as high and low nibbles
            if((parser_error == PARSER_SUCCESS) && (cb->cb_io_frame))
            {
                cb->cb_io_frame(cb->context, pin, config, dir);
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
            if((parser_error == PARSER_SUCCESS) && (cb->cb_read_data))
            {
                cb->cb_read_data(cb->context, pc, epc_len, epc, data_len, data);
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
            if((parser_error == PARSER_SUCCESS) && (cb->cb_write_data))
            {
                cb->cb_write_data(cb->context, pc, epc_len, epc, error);
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
            if((parser_error == PARSER_SUCCESS) && (cb->cb_kill))
            {
                cb->cb_kill(cb->context, pc, epc_len, epc, error);
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
            if((parser_error == PARSER_SUCCESS) && (cb->cb_lock))
            {
                cb->cb_lock(cb->context, pc, epc_len, epc, error);
            }
        }
        break;
        case CMD_SCAN_JAMMER:
        {
            uint8_t ch_start;
            uint8_t ch_end;
            uint8_t* channel_noise;
            parser_error = ReadScanJammerFrame(len, payload, &ch_start, &ch_end, &channel_noise);
            if((parser_error == PARSER_SUCCESS) && (cb->cb_scan_jammer))
            {
                cb->cb_scan_jammer(cb->context, ch_start, ch_end, channel_noise);
            }
        }
        break;
        case CMD_SCAN_RSSI:
        {
            uint8_t ch_start;
            uint8_t ch_end;
            uint8_t* channel_rssi;
            parser_error = ReadScanRssiFrame(len, payload, &ch_start, &ch_end, &channel_rssi);
            if((parser_error == PARSER_SUCCESS) && (cb->cb_scan_rssi))
            {
                cb->cb_scan_rssi(cb->context, ch_start, ch_end, channel_rssi);
            }
        }
        break;
        case CMD_SET_FHSS:
        {
            uint8_t error;
            parser_error = ReadSetFhssFrame(len, payload, &error);
            if((parser_error == PARSER_SUCCESS) && (cb->cb_set_fhss))
            {
                cb->cb_set_fhss(cb->context, error);
            }
        }
        break;
        case CMD_SET_INVENTORY_MODE:
        {
            uint8_t error;
            parser_error = ReadSetInventoryModeFrame(len, payload, &error);
            if((parser_error == PARSER_SUCCESS) && (cb->cb_set_inventory_mode))
            {
                cb->cb_set_inventory_mode(cb->context, error);
            }
        }
        break;
        case CMD_SET_REGION:
        {
            uint8_t error;
            parser_error = ReadSetRegionFrame(len, payload, &error);
            if((parser_error == PARSER_SUCCESS) && (cb->cb_set_region))
            {
                cb->cb_set_region(cb->context, error);
            }
        }
        break;
        case CMD_SET_SLEEP_TIME:
        {
            uint8_t idle_minutes;
            parser_error = ReadSetSleepTimeFrame(len, payload, &idle_minutes);
            if((parser_error == PARSER_SUCCESS) && (cb->cb_set_sleep_time))
            {
                cb->cb_set_sleep_time(cb->context, idle_minutes);
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
        break;
        default:
        {
            parser_error = PARSER_UNKNOWN_CMD;
        }
        break;
    }
    return parser_error;
}



int parse_error_frame(jdm_100_cb_t *cb, uint16_t len, uint8_t* payload)
{
    int parser_error = PARSER_NEEDS_WORK;
    uint16_t pc = 0;
    uint8_t epc_len = 0;
    uint8_t* epc = NULL;
    uint8_t error = 0;
    parser_error = read_tag_error_frame(len, payload, &pc, &epc_len, &epc, &error);
    if(parser_error == PARSER_SUCCESS)
    {
        switch(error)
        {
            case FAIL_ACCESS_PWD_ERROR:
            {
                if(cb->cb_error_access_pwd)
                {
                    cb->cb_error_access_pwd(cb->context, pc, epc_len, epc);
                }
            }
            break;
            case FAIL_CUSTOM_CMD_BASE:
            {
                if(cb->cb_error_custom_cmd)
                {
                    cb->cb_error_custom_cmd(cb->context, pc, epc_len, epc);
                }
            }
            break;
            case FAIL_FHSS_FAIL:
            {
                if(cb->cb_error_fhss_fail)
                {
                    cb->cb_error_fhss_fail(cb->context, pc, epc_len, epc);
                }
            }
            break;
            case FAIL_INVALID_CMD:
            {
                if(cb->cb_error_invalid_cmd)
                {
                    cb->cb_error_invalid_cmd(cb->context, pc, epc_len, epc);
                }
            }
            break;
            case FAIL_INVALID_PARA:
            {
                if(cb->cb_error_invalid_para)
                {
                    cb->cb_error_invalid_para(cb->context, pc, epc_len, epc);
                }
            }
            break;
            case FAIL_INVENTORY_TAG_TIMEOUT:
            {
                if(cb->cb_error_inventory_tag_timeout)
                {
                    cb->cb_error_inventory_tag_timeout(cb->context, pc, epc_len, epc);
                }
            }
            break;
            case FAIL_KILL_ERROR_CODE_BASE:
            {
                if(cb->cb_error_kill_base)
                {
                    cb->cb_error_kill_base(cb->context, pc, epc_len, epc);
                }
            }
            break;
            case FAIL_KILL_NO_TAG:
            {
                if(cb->cb_error_kill_no_tag)
                {
                    cb->cb_error_kill_no_tag(cb->context, pc, epc_len, epc);
                }
            }
            break;
            case FAIL_LOCK_ERROR_CODE_BASE:
            {
                if(cb->cb_error_lock_base)
                {
                    cb->cb_error_lock_base(cb->context, pc, epc_len, epc);
                }
            }
            break;
            case FAIL_LOCK_NO_TAG:
            {
                if(cb->cb_error_lock_no_tag)
                {
                    cb->cb_error_lock_no_tag(cb->context, pc, epc_len, epc);
                }
            }
            break;
            case FAIL_NXP_CHANGE_CONFIG_NO_TAG:
            {
                if(cb->cb_error_nxp_change_config_no_tag)
                {
                    cb->cb_error_nxp_change_config_no_tag(cb->context, pc, epc_len, epc);
                }
            }
            break;
            case FAIL_NXP_CHANGE_EAS_NOT_SECURE:
            {
                if(cb->cb_error_nxp_eas_not_secure)
                {
                    cb->cb_error_nxp_eas_not_secure(cb->context, pc, epc_len, epc);
                }
            }
            break;
            case FAIL_NXP_CHANGE_EAS_NO_TAG:
            {
                if(cb->cb_error_nxp_change_eas_no_tag)
                {
                    cb->cb_error_nxp_change_eas_no_tag(cb->context, pc, epc_len, epc);
                }
            }
            break;
            case FAIL_NXP_EAS_ALARM_NO_TAG:
            {
                if(cb->cb_error_nxp_eas_alarm_no_tag)
                {
                    cb->cb_error_nxp_eas_alarm_no_tag(cb->context, pc, epc_len, epc);
                }
            }
            break;
            case FAIL_NXP_READPROTECT_NO_TAG:
            {
                if(cb->cb_error_nxp_readprotect_no_tag)
                {
                    cb->cb_error_nxp_readprotect_no_tag(cb->context, pc, epc_len, epc);
                }
            }
            break;
            case FAIL_NXP_RESET_READPROTECT_NO_TAG:
            {
                if(cb->cb_error_nxp_reset_readprotect_no_tag)
                {
                    cb->cb_error_nxp_reset_readprotect_no_tag(cb->context, pc, epc_len, epc);
                }
            }
            break;
            case FAIL_READ_ERROR_CODE_BASE:
            {
                if(cb->cb_error_read_base)
                {
                    cb->cb_error_read_base(cb->context, pc, epc_len, epc);
                }
            }
            break;
            case FAIL_READ_MEMORY_NO_TAG:
            {
                if(cb->cb_error_read_no_tag)
                {
                    cb->cb_error_read_no_tag(cb->context, pc, epc_len, epc);
                }
            }
            break;
            case FAIL_WRITE_ERROR_CODE_BASE:
            {
                if(cb->cb_error_write_base)
                {
                    cb->cb_error_write_base(cb->context, pc, epc_len, epc);
                }
            }
            break;
            case FAIL_WRITE_MEMORY_NO_TAG:
            {
                if(cb->cb_error_write_no_tag)
                {
                    cb->cb_error_write_no_tag(cb->context, pc, epc_len, epc);
                }
            }
            break;
            default:
            {
                //unknown error code
                parser_error = PARSER_UNKNOWN_ERROR_CODE;
            }
            break;
        }
    }
    return parser_error;
}

int parse_info_frame(jdm_100_cb_t *cb, uint8_t cmd, uint16_t len, uint8_t* payload)
{
    int parser_error = PARSER_NEEDS_WORK;

    switch(cmd)
    {
        case CMD_INVENTORY:
        case CMD_READ_MULTI:
        {
            uint8_t rssi;
            uint16_t pc;
            uint8_t epc_len;
            uint8_t* epc;
            uint16_t crc;
            parser_error = ReadTagNotification(len, payload, &rssi, &pc, &epc_len, &epc, &crc);
            if(cmd == CMD_INVENTORY)
            {
                cb->cb_tag_single_notification(cb->context, rssi, pc, epc_len, epc, crc);
            }
            else
            {
                cb->cb_tag_multi_notification(cb->context, rssi, pc, epc_len, epc, crc);
            }
        }
        break;
        default:
        {
            //unknown info code
            parser_error = PARSER_NEEDS_WORK;
        }
        break;
    }
    return parser_error;
}

int parse_cmd_frame(jdm_100_cb_t *cb, uint8_t cmd, uint16_t len, uint8_t* payload)
{
    int parser_error = PARSER_NEEDS_WORK;
    return parser_error;
}
