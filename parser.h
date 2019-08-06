#ifndef PARSER_H
#define PARSER_H

const uint8_t CMD_EXE_FAILED = 0xff;//-0x1;
const uint8_t CMD_GET_MODULE_INFO = 0x3;
const uint8_t CMD_GET_POWER = 0xb7;//-0x49;
const uint8_t CMD_GET_QUERY = 0xd;
const uint8_t CMD_GET_RF_CHANNEL = 0xaa;//-0x56;
const uint8_t CMD_GET_SELECT_PARA = 0xb;
const uint8_t CMD_INSERT_FHSS_CHANNEL = 0xa9;//-0x57;
const uint8_t CMD_INVENTORY = 0x22;
const uint8_t CMD_IO_CONTROL = 0x1a;
const uint8_t CMD_KILL = 0x65;
const uint8_t CMD_LOAD_NV_CONFIG = 0xa;
const uint8_t CMD_LOCK_UNLOCK = 0x82;//-0x7e;
const uint8_t CMD_NXP_CHANGE_CONFIG = 0xE0//-0x20;
const uint8_t CMD_NXP_CHANGE_EAS = 0xe3;//-0x1d;
const uint8_t CMD_NXP_EAS_ALARM = 0xe4;//-0x1c;
const uint8_t CMD_NXP_READPROTECT = 0xe1;//-0x1f;
const uint8_t CMD_NXP_RESET_READPROTECT = 0xe2;//-0x1e;
const uint8_t CMD_READ_DATA = 0x39;
const uint8_t CMD_READ_MODEM_PARA = 0x01;//-0xf;
const uint8_t CMD_READ_MULTI = 0x27;
const uint8_t CMD_RESTART = 0x19;
const uint8_t CMD_SAVE_NV_CONFIG = 0x9;
const uint8_t CMD_SCAN_JAMMER = 0x02;//-0xe;
const uint8_t CMD_SCAN_RSSI = 0x03;//-0xd;
const uint8_t CMD_SET_CW = 0xa1;//-0x50;
const uint8_t CMD_SET_FHSS = 0xad;//-0x53;
const uint8_t CMD_SET_INVENTORY_MODE = 0x12;
const uint8_t CMD_SET_MODEM_PARA = 0xf0;//-0x10;
const uint8_t CMD_SET_POWER = 0xb6;//-0x4a;
const uint8_t CMD_SET_QUERY = 0xe;
const uint8_t CMD_SET_READER_ENV_MODE = 0x05;//-0xb;
const uint8_t CMD_SET_REGION = 0x7;
const uint8_t CMD_SET_RF_CHANNEL = 0xab;//-0x55;
const uint8_t CMD_SET_SELECT_PARA = 0xc;
const uint8_t CMD_SET_SLEEP_TIME = 0x1d;
const uint8_t CMD_SLEEP_MODE = 0x17;
const uint8_t CMD_STOP_MULTI = 0x28;
const uint8_t CMD_WRITE_DATA = 0x49;

//no response is given from the following
const uint8_t CMD_SET_BAUD = 0x11;


const uint8_t ERROR_CODE_INSUFFICIENT_POWER = 0xb;
const uint8_t ERROR_CODE_MEM_LOCKED = 0x4;
const uint8_t ERROR_CODE_MEM_OVERRUN = 0x3;
const uint8_t ERROR_CODE_NON_SPEC_ERROR = 0xf;
const uint8_t ERROR_CODE_OTHER_ERROR = 0x0;
const uint8_t FAIL_ACCESS_PWD_ERROR = 0x16;
const uint8_t FAIL_CUSTOM_CMD_BASE = 0xe0;//-0x20;
const uint8_t FAIL_FHSS_FAIL = 0x20;
const uint8_t FAIL_INVALID_CMD = 0x17;
const uint8_t FAIL_INVALID_PARA = 0xe;
const uint8_t FAIL_INVENTORY_TAG_TIMEOUT = 0x15;
const uint8_t FAIL_KILL_ERROR_CODE_BASE = 0xd0;//-0x30;
const uint8_t FAIL_KILL_NO_TAG = 0x12;
const uint8_t FAIL_LOCK_ERROR_CODE_BASE = 0xc0;//-0x40;
const uint8_t FAIL_LOCK_NO_TAG = 0x13;
const uint8_t FAIL_NXP_CHANGE_CONFIG_NO_TAG = 0x1a;
const uint8_t FAIL_NXP_CHANGE_EAS_NOT_SECURE = 0x1c;
const uint8_t FAIL_NXP_CHANGE_EAS_NO_TAG = 0x1b;
const uint8_t FAIL_NXP_EAS_ALARM_NO_TAG = 0x1d;
const uint8_t FAIL_NXP_READPROTECT_NO_TAG = 0x2a;
const uint8_t FAIL_NXP_RESET_READPROTECT_NO_TAG = 0x2b;
const uint8_t FAIL_READ_ERROR_CODE_BASE = 0xa0;//-0x60;
const uint8_t FAIL_READ_MEMORY_NO_TAG = 0x9;
const uint8_t FAIL_WRITE_ERROR_CODE_BASE = 0xb0;//-0x50;
const uint8_t FAIL_WRITE_MEMORY_NO_TAG = 0x10;
const uint8_t FRAME_BEGIN = 0xbb;//-0x45;
const uint8_t FRAME_END = 0x7e;
const uint8_t FRAME_TYPE_ANS = 0x1;
const uint8_t FRAME_TYPE_CMD = 0x0;
const uint8_t FRAME_TYPE_INFO = 0x2;
const uint8_t INVENTORY_MODE0 = 0x0;
const uint8_t INVENTORY_MODE1 = 0x1;
const uint8_t INVENTORY_MODE2 = 0x2;
const uint8_t MODULE_HARDWARE_VERSION_FIELD = 0x0;
const uint8_t MODULE_MANUFACTURE_INFO_FIELD = 0x2;
const uint8_t MODULE_SOFTWARE_VERSION_FIELD = 0x1;
const uint8_t REGION_CODE_CHN1 = 0x4;
const uint8_t REGION_CODE_CHN2 = 0x1;
const uint8_t REGION_CODE_EUR = 0x3;
const uint8_t REGION_CODE_JAPAN = 0x5;
const uint8_t REGION_CODE_KOREA = 0x6;
const uint8_t REGION_CODE_US = 0x2;
const uint8_t SET_OFF = 0x0;
const uint8_t SET_ON = 0xff;//-0x1;
const uint8_t SUCCESS_MSG_DATA:B


#endif //PARSER_H