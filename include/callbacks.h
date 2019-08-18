#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef void (*callback_tag_success_t)  (void* context, uint16_t pc, uint8_t epc_len, uint8_t* epc, uint8_t error);
typedef void (*callback_success_t)      (void* context, uint8_t error);
typedef void (*callback_error_t)        (void* context, uint16_t pc, uint8_t epc_len, uint8_t* epc);


typedef struct jdm_100_cb_t
{
    void* context;  //can be used to stash a *this pointer

    void (*cb_parser_error)             (void* context, int parser_error, uint8_t frame_type, uint8_t cmd, uint16_t len, uint8_t* payload);
    void (*cb_module_info)              (void* context, uint8_t info_type, uint16_t info_len, uint8_t* info);
    void (*cb_get_power)                (void* context, uint16_t power);
    void (*cb_get_query)                (void* context, uint8_t dr, uint8_t m, uint8_t trext, uint8_t sel, uint8_t session, uint8_t target, uint8_t q);
    void (*cb_get_rf_channel)           (void* context, uint8_t channel_index);
    void (*cb_get_select)               (void* context, uint8_t target, uint8_t action, uint8_t bank, uint32_t pointer, uint8_t mask_len, uint8_t* mask, bool truncate);
    void (*cb_tag_single_notification)  (void* context, uint8_t rssi, uint16_t pc, uint8_t epc_len, uint8_t* epc, uint16_t crc);
    void (*cb_tag_multi_notification)   (void* context, uint8_t rssi, uint16_t pc, uint8_t epc_len, uint8_t* epc, uint16_t crc);
    void (*cb_io_frame)                 (void* context, uint8_t pin, uint8_t config, uint8_t dir);
    void (*cb_read_data)                (void* context, uint16_t pc, uint8_t epc_len, uint8_t* epc, uint8_t data_len, uint8_t* data);
    void (*cb_scan_jammer)              (void* context, uint8_t ch_start, uint8_t ch_end, uint8_t* channel_noise);
    void (*cb_scan_rssi)                (void* context, uint8_t ch_start, uint8_t ch_end, uint8_t* channel_rssi);
    void (*cb_set_sleep_time)           (void* context, uint8_t idle_minutes);

    callback_tag_success_t cb_write_data;
    callback_tag_success_t cb_kill;
    callback_tag_success_t cb_lock;

    callback_success_t cb_set_rf_channel;
    callback_success_t cb_set_query;
    callback_success_t cb_set_Select;
    callback_success_t cb_set_inventory_mode;
    callback_success_t cb_stop_frame;
    callback_success_t cb_set_fhss;
    callback_success_t cb_set_region;
    callback_success_t cb_set_power;

    //error codes
    callback_error_t cb_error_access_pwd;
    callback_error_t cb_error_custom_cmd;
    callback_error_t cb_error_fhss_fail;
    callback_error_t cb_error_invalid_cmd;
    callback_error_t cb_error_invalid_para;
    callback_error_t cb_error_inventory_tag_timeout;
    callback_error_t cb_error_kill_base;
    callback_error_t cb_error_kill_no_tag;
    callback_error_t cb_error_lock_base;
    callback_error_t cb_error_lock_no_tag;
    callback_error_t cb_error_nxp_change_config_no_tag;
    callback_error_t cb_error_nxp_eas_not_secure;
    callback_error_t cb_error_nxp_change_eas_no_tag;
    callback_error_t cb_error_nxp_eas_alarm_no_tag;
    callback_error_t cb_error_nxp_readprotect_no_tag;
    callback_error_t cb_error_nxp_reset_readprotect_no_tag;
    callback_error_t cb_error_read_base;
    callback_error_t cb_error_read_no_tag;
    callback_error_t cb_error_write_base;
    callback_error_t cb_error_write_no_tag;
} jdm_100_cb_t;

#endif //CALLBACKS_H
