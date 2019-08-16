#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "callbacks.h"


int parse_packet(jdm_100_cb_t *cb, size_t *buf_len, uint8_t* *buf);
int parse_frame(jdm_100_cb_t *cb, uint8_t frame_type, uint8_t cmd, uint16_t len, uint8_t* payload);
int parse_cmd_frame(jdm_100_cb_t *cb, uint8_t cmd, uint16_t len, uint8_t* payload);
int parse_ans_frame(jdm_100_cb_t *cb, uint8_t cmd, uint16_t len, uint8_t* payload);
int parse_info_frame(jdm_100_cb_t *cb, uint8_t cmd, uint16_t len, uint8_t* payload);
int parse_error_frame(jdm_100_cb_t *cb, uint16_t len, uint8_t* payload);  //subset of answer frames


#define CMD_EXE_FAILED                      (0xff)    //-0x1;
#define CMD_GET_MODULE_INFO                 (0x03)
#define CMD_GET_POWER                       (0xb7)    //-0x49;
#define CMD_GET_QUERY                       (0x0d)
#define CMD_GET_RF_CHANNEL                  (0xaa)    //-0x56;
#define CMD_GET_SELECT_PARA                 (0x0b)
#define CMD_INSERT_FHSS_CHANNEL             (0xa9)    //-0x57;
#define CMD_INVENTORY                       (0x22)
#define CMD_IO_CONTROL                      (0x1a)
#define CMD_KILL                            (0x65)
#define CMD_LOAD_NV_CONFIG                  (0x0a)
#define CMD_SAVE_NV_CONFIG                  (0x09)
#define CMD_LOCK_UNLOCK                     (0x82)    //-0x7e;
#define CMD_NXP_CHANGE_CONFIG               (0xE0)    //-0x20;
#define CMD_NXP_CHANGE_EAS                  (0xe3)    //-0x1d;
#define CMD_NXP_EAS_ALARM                   (0xe4)    //-0x1c;
#define CMD_NXP_READPROTECT                 (0xe1)    //-0x1f;
#define CMD_NXP_RESET_READPROTECT           (0xe2)    //-0x1e;
#define CMD_READ_DATA                       (0x39)
#define CMD_READ_MODEM_PARA                 (0xf1)    //-0xf;
#define CMD_READ_MULTI                      (0x27)
#define CMD_RESTART                         (0x19)
#define CMD_SCAN_JAMMER                     (0xf2)    //-0xe;
#define CMD_SCAN_RSSI                       (0xf3)    //-0xd;
#define CMD_SET_CW                          (0xa1)    //-0x50;
#define CMD_SET_FHSS                        (0xad)    //-0x53;
#define CMD_SET_INVENTORY_MODE              (0x12)
#define CMD_SET_MODEM_PARA                  (0xf0)    //-0x10;
#define CMD_SET_POWER                       (0xb6)    //-0x4a;
#define CMD_SET_QUERY                       (0x0e)
#define CMD_SET_READER_ENV_MODE             (0xf5)    //-0xb;
#define CMD_SET_REGION                      (0x07)
#define CMD_SET_RF_CHANNEL                  (0xab)    //-0x55;
#define CMD_SET_SELECT_PARA                 (0x0c)
#define CMD_SET_SLEEP_TIME                  (0x1d)
#define CMD_STOP_MULTI                      (0x28)
#define CMD_WRITE_DATA                      (0x49)

//no response is given from the following
#define CMD_SET_BAUD                        (0x11)
#define CMD_SLEEP_MODE                      (0x17)


#define ERROR_CODE_INSUFFICIENT_POWER       (0x0b)    
#define ERROR_CODE_MEM_LOCKED               (0x04)    
#define ERROR_CODE_MEM_OVERRUN              (0x03)    
#define ERROR_CODE_NON_SPEC_ERROR           (0x0f)    
#define ERROR_CODE_OTHER_ERROR              (0x00)    

#define FAIL_ACCESS_PWD_ERROR               (0x16)    
#define FAIL_CUSTOM_CMD_BASE                (0xe0)    //-0x20;
#define FAIL_FHSS_FAIL                      (0x20)    
#define FAIL_INVALID_CMD                    (0x17)    
#define FAIL_INVALID_PARA                   (0x0e)    
#define FAIL_INVENTORY_TAG_TIMEOUT          (0x15)    
#define FAIL_KILL_ERROR_CODE_BASE           (0xd0)    //-0x30;
#define FAIL_KILL_NO_TAG                    (0x12)    
#define FAIL_LOCK_ERROR_CODE_BASE           (0xc0)    //-0x40;
#define FAIL_LOCK_NO_TAG                    (0x13)    
#define FAIL_NXP_CHANGE_CONFIG_NO_TAG       (0x1a)    
#define FAIL_NXP_CHANGE_EAS_NOT_SECURE      (0x1c)    
#define FAIL_NXP_CHANGE_EAS_NO_TAG          (0x1b)    
#define FAIL_NXP_EAS_ALARM_NO_TAG           (0x1d)    
#define FAIL_NXP_READPROTECT_NO_TAG         (0x2a)    
#define FAIL_NXP_RESET_READPROTECT_NO_TAG   (0x2b)    
#define FAIL_READ_ERROR_CODE_BASE           (0xa0)    //-0x60;
#define FAIL_READ_MEMORY_NO_TAG             (0x09)    
#define FAIL_WRITE_ERROR_CODE_BASE          (0xb0)    //-0x50;
#define FAIL_WRITE_MEMORY_NO_TAG            (0x10)    

#define FRAME_BEGIN                         (0xbb)    //-0x45;
#define FRAME_END                           (0x7e)    

#define FRAME_TYPE_ANS                      (0x01)    
#define FRAME_TYPE_CMD                      (0x00)    
#define FRAME_TYPE_INFO                     (0x02)    

#define INVENTORY_MODE0                     (0x00)    
#define INVENTORY_MODE1                     (0x01)    
#define INVENTORY_MODE2                     (0x02)    

#define MODULE_HARDWARE_VERSION_FIELD       (0x00)    
#define MODULE_MANUFACTURE_INFO_FIELD       (0x02)    
#define MODULE_SOFTWARE_VERSION_FIELD       (0x01)    

#define SET_OFF                             (0x00)
#define SET_ON                              (0xff)    //-0x1;



#define REGION_CHN2_CODE                    (0x01)    
#define REGION_US_CODE                      (0x02)    
#define REGION_EUR_CODE                     (0x03)    
#define REGION_CHN1_CODE                    (0x04)    
#define REGION_JAPAN_CODE                   (0x05)    
#define REGION_KOREA_CODE                   (0x06)    

#define REGION_CHN2_FREQ_START              (840.1)
#define REGION_US_FREQ_START                (902.25)
#define REGION_EUR_FREQ_START               (865.1)
#define REGION_CHN1_FREQ_START              (920.125)
#define REGION_JAPAN_FREQ_START             (-1.0)   //not documented
#define REGION_KOREA_FREQ_START             (917.1)

#define REGION_CHN2_FREQ_SEPARATION         (0.25)
#define REGION_US_FREQ_SEPARATION           (0.5)
#define REGION_EUR_FREQ_SEPARATION          (0.2)
#define REGION_CHN1_FREQ_SEPARATION         (0.25)
#define REGION_JAPAN_FREQ_SEPARATION        (-1.0)  //not documented
#define REGION_KOREA_FREQ_SEPARATION        (0.2)

extern const float REGION_FREQ_STARTS[]; /*= 
  {
        REGION_CHN2_FREQ_START,
        REGION_US_FREQ_START,
        REGION_EUR_FREQ_START,
        REGION_CHN1_FREQ_START,
        REGION_JAPAN_FREQ_START,
        REGION_KOREA_FREQ_START
    };*/

extern const float REGION_FREQ_SEPARATIONS[]; /*= 
    {
        REGION_CHN2_FREQ_SEPARATION,
        REGION_US_FREQ_SEPARATION,
        REGION_EUR_FREQ_SEPARATION,
        REGION_CHN1_FREQ_SEPARATION,
        REGION_JAPAN_FREQ_SEPARATION,
        REGION_KOREA_FREQ_SEPARATION
    };*/


enum {
    PARSER_SUCCESS = 0,
    PARSER_UNDERFULL,
    PARSER_MALFORMED_HEADER,
    PARSER_MALFORMED_TERMINATOR,
    PARSER_LENGTH_ERROR,
    PARSER_CHECKSUM_ERROR,
    PARSER_UNKNOWN_FRAME_TYPE,
    PARSER_UNKNOWN_CMD,
    PARSER_UNKNOWN_ERROR_CODE,
    PARSER_UNDOCUMENTED_CMD,
    PARSER_NEEDS_WORK,
};
extern const char* parser_error_strings[];

#endif //PARSER_H