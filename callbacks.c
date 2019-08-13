#include "callbacks.h"


void (*cb_parser_error)(int parser_error, uint8_t frame_type, uint8_t cmd, uint16_t len, uint8_t* payload) = NULL;

void (*cb_module_info)(uint8_t info_type, uint16_t info_len, uint8_t* info) = NULL;
void (*cb_get_power)(uint16_t power) = NULL;
void (*cb_get_query)(uint8_t dr, uint8_t m, uint8_t trext, uint8_t sel, uint8_t session, uint8_t target, uint8_t q) = NULL;
void (*cb_get_rf_channel)(uint8_t channel_index) = NULL;
void (*cb_get_select)(uint8_t target, uint8_t action, uint8_t bank, uint32_t pointer, uint8_t mask_len, uint8_t* mask, bool truncate) = NULL;
void (*cb_tag_single_notification)(uint8_t rssi, uint16_t pc, uint8_t epc_len, uint8_t* epc, uint16_t crc) = NULL;
void (*cb_tag_multi_notification)(uint8_t rssi, uint16_t pc, uint8_t epc_len, uint8_t* epc, uint16_t crc) = NULL;
void (*cb_io_frame)(uint8_t pin, uint8_t config, uint8_t dir) = NULL;
void (*cb_read_data)(uint16_t pc, uint8_t epc_len, uint8_t* epc, uint8_t data_len, uint8_t* data) = NULL;
void (*cb_scan_jammer)(uint8_t ch_start, uint8_t ch_end, uint8_t* channel_noise) = NULL;
void (*cb_scan_rssi)(uint8_t ch_start, uint8_t ch_end, uint8_t* channel_rssi) = NULL;
void (*cb_set_sleep_time)(uint8_t idle_minutes) = NULL;

callback_tag_success_t cb_write_data = NULL;
callback_tag_success_t cb_kill = NULL;
callback_tag_success_t cb_lock = NULL;

callback_success_t cb_set_rf_channel = NULL;
callback_success_t cb_set_query = NULL;
callback_success_t cb_set_Select = NULL;
callback_success_t cb_set_inventory_mode = NULL;
callback_success_t cb_stop_frame = NULL;
callback_success_t cb_set_fhss = NULL;
callback_success_t cb_set_region = NULL;
callback_success_t cb_set_power = NULL;

//error codes
callback_error_t cb_error_access_pwd = NULL;
callback_error_t cb_error_custom_cmd = NULL;
callback_error_t cb_error_fhss_fail = NULL;
callback_error_t cb_error_invalid_cmd = NULL;
callback_error_t cb_error_invalid_para = NULL;
callback_error_t cb_error_inventory_tag_timeout = NULL;
callback_error_t cb_error_kill_base = NULL;
callback_error_t cb_error_kill_no_tag = NULL;
callback_error_t cb_error_lock_base = NULL;
callback_error_t cb_error_lock_no_tag = NULL;
callback_error_t cb_error_nxp_change_config_no_tag = NULL;
callback_error_t cb_error_nxp_eas_not_secure = NULL;
callback_error_t cb_error_nxp_change_eas_no_tag = NULL;
callback_error_t cb_error_nxp_eas_alarm_no_tag = NULL;
callback_error_t cb_error_nxp_readprotect_no_tag = NULL;
callback_error_t cb_error_nxp_reset_readprotect_no_tag = NULL;
callback_error_t cb_error_read_base = NULL;
callback_error_t cb_error_read_no_tag = NULL;
callback_error_t cb_error_write_base = NULL;
callback_error_t cb_error_write_no_tag = NULL;
