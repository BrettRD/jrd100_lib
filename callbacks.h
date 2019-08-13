#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

//consider adding a callback context
/*

    struct cb___
    {
        void (*callback)(void* context, args);
        void* context;
    }
//
    if(cb___.callback)
    {
        cb___.callback(cb___.context, args);
    }
*/

extern void (*cb_parser_error)(int parser_error, uint8_t frame_type, uint8_t cmd, uint16_t len, uint8_t* payload);

extern void (*cb_module_info)(uint8_t info_type, uint16_t info_len, uint8_t* info);
extern void (*cb_get_power)(uint16_t power);
extern void (*cb_get_query)(uint8_t dr, uint8_t m, uint8_t trext, uint8_t sel, uint8_t session, uint8_t target, uint8_t q);
extern void (*cb_get_rf_channel)(uint8_t channel_index);
extern void (*cb_get_select)(uint8_t target, uint8_t action, uint8_t bank, uint32_t pointer, uint8_t mask_len, uint8_t* mask, bool truncate);
extern void (*cb_tag_single_notification)(uint8_t rssi, uint16_t pc, uint8_t epc_len, uint8_t* epc, uint16_t crc);
extern void (*cb_tag_multi_notification)(uint8_t rssi, uint16_t pc, uint8_t epc_len, uint8_t* epc, uint16_t crc);
extern void (*cb_io_frame)(uint8_t pin, uint8_t config, uint8_t dir);
extern void (*cb_read_data)(uint16_t pc, uint8_t epc_len, uint8_t* epc, uint8_t data_len, uint8_t* data);
extern void (*cb_scan_jammer)(uint8_t ch_start, uint8_t ch_end, uint8_t* channel_noise);
extern void (*cb_scan_rssi)(uint8_t ch_start, uint8_t ch_end, uint8_t* channel_rssi);
extern void (*cb_set_sleep_time)(uint8_t idle_minutes);

typedef void (*callback_tag_success_t)(uint16_t pc, uint8_t epc_len, uint8_t* epc, uint8_t error);
extern callback_tag_success_t cb_write_data;
extern callback_tag_success_t cb_kill;
extern callback_tag_success_t cb_lock;

typedef void (*callback_success_t)(uint8_t error);
extern callback_success_t cb_set_rf_channel;
extern callback_success_t cb_set_query;
extern callback_success_t cb_set_Select;
extern callback_success_t cb_set_inventory_mode;
extern callback_success_t cb_stop_frame;
extern callback_success_t cb_set_fhss;
extern callback_success_t cb_set_region;
extern callback_success_t cb_set_power;

//error codes
typedef void (*callback_error_t)(uint16_t pc, uint8_t epc_len, uint8_t* epc);
extern callback_error_t cb_error_access_pwd;
extern callback_error_t cb_error_custom_cmd;
extern callback_error_t cb_error_fhss_fail;
extern callback_error_t cb_error_invalid_cmd;
extern callback_error_t cb_error_invalid_para;
extern callback_error_t cb_error_inventory_tag_timeout;
extern callback_error_t cb_error_kill_base;
extern callback_error_t cb_error_kill_no_tag;
extern callback_error_t cb_error_lock_base;
extern callback_error_t cb_error_lock_no_tag;
extern callback_error_t cb_error_nxp_change_config_no_tag;
extern callback_error_t cb_error_nxp_eas_not_secure;
extern callback_error_t cb_error_nxp_change_eas_no_tag;
extern callback_error_t cb_error_nxp_eas_alarm_no_tag;
extern callback_error_t cb_error_nxp_readprotect_no_tag;
extern callback_error_t cb_error_nxp_reset_readprotect_no_tag;
extern callback_error_t cb_error_read_base;
extern callback_error_t cb_error_read_no_tag;
extern callback_error_t cb_error_write_base;
extern callback_error_t cb_error_write_no_tag;

#endif //CALLBACKS_H
