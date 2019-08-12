#ifndef COMMANDS_H
#define COMMANDS_H

#include <stddef.h>
#include <stdint.h>


uint8_t* build_cmd_frame(size_t *buf_len, uint8_t* buf, uint8_t cmd, uint16_t len, uint8_t* payload);
uint8_t* build_cmd_empty_frame(size_t *buf_len, uint8_t *buf, uint8_t cmd);
int read_tag_sucess_frame(uint16_t len, uint8_t* payload, uint16_t *pc, uint8_t *epc_len, uint8_t* *epc, uint8_t *error);
int read_success_frame(uint16_t len, uint8_t* payload, uint8_t *error);
int read_error_frame(uint16_t len, uint8_t* payload, uint8_t *error);
int read_tag_error_frame(uint16_t len, uint8_t* payload, uint16_t *pc, uint8_t *epc_len, uint8_t* *epc, uint8_t *error);
void find_frame_begin(size_t *buf_len, uint8_t* *buf);
void skip_byte(size_t *buf_len, uint8_t* *buf);
int read_frame(size_t *buf_len, uint8_t* *buf, uint8_t *frame_type, uint8_t *cmd, uint16_t *len, uint8_t* *payload);
uint8_t calc_frame_checksum(uint8_t* frame, uint16_t len);


uint8_t* BuildGetModuleInfoFrame(size_t *buf_len, uint8_t* buf, uint8_t field);
int ReadGetModuleInfoFrame(uint16_t len, uint8_t* payload, uint8_t *info_type, uint16_t *info_len, uint8_t* *info);

uint8_t* BuildGetRfChannelFrame(size_t *buf_len, uint8_t* buf);
int ReadGetRfChannelFrame(uint16_t len, uint8_t* payload, uint8_t *channel_index);

uint8_t* BuildSetRfChannelFrame(size_t *buf_len, uint8_t* buf, uint8_t channel_index);
int ReadSetRfChannelFrame(uint16_t len, uint8_t* payload, uint8_t *error);

uint8_t* BuildGetSelectFrame(size_t *buf_len, uint8_t* buf);
int ReadGetSelectFrame(uint16_t len, uint8_t* payload, uint8_t *target, uint8_t *action, uint8_t *bank, uint32_t *pointer, uint8_t *mask_len, uint8_t* *mask, bool *truncate);

float channel_freq_MHz(uint8_t region_index, uint8_t channel_index);
uint8_t* BuildSetSelectFrame(size_t *buf_len, uint8_t* buf, uint8_t target, uint8_t action, uint8_t bank, uint32_t pointer, uint8_t mask_len_bits, uint8_t* mask, bool truncate);
int ReadSetSelectFrame(uint16_t len, uint8_t* payload, uint8_t *error);

uint8_t* build_io_payload(uint8_t* payload, uint8_t pin, uint8_t config, uint8_t dir);
uint8_t* BuildIoControlFrame(size_t *buf_len, uint8_t* buf, uint8_t* io_payload);
int ReadIoControlFrame(uint16_t len, uint8_t* payload, uint8_t* pin, uint8_t* config, uint8_t* dir);

uint8_t* BuildReadSingleFrame(size_t *buf_len, uint8_t* buf);
uint8_t* BuildReadMultiFrame(size_t *buf_len, uint8_t* buf, uint16_t tag_count);
int ReadTagNotification(uint16_t len, uint8_t *payload, uint8_t *rssi, uint16_t *pc, uint8_t *epc_len, uint8_t* *epc, uint16_t *crc);

uint8_t* BuildStopReadFrame(size_t *buf_len, uint8_t* buf);
int ReadStopReadFrame(uint16_t len, uint8_t* payload, uint8_t *error);

uint8_t* BuildSetBaudFrame(size_t *buf_len, uint8_t* buf, uint32_t baud_hz);
uint8_t* BuildSetFhssFrame(size_t *buf_len, uint8_t* buf, uint8_t setting);
int ReadSetFhssFrame(uint16_t len, uint8_t *payload, uint8_t *error);

uint8_t* BuildSetModuleSleepFrame(size_t *buf_len, uint8_t* buf);
uint8_t* BuildGetPaPowerFrame(size_t *buf_len, uint8_t* buf);
int ReadGetPaPowerFrame(uint16_t len, uint8_t* payload, uint16_t *power);

uint8_t* BuildSetPaPowerFrame(size_t *buf_len, uint8_t* buf, float powerDBm);
int ReadSetPaPowerFrame(uint16_t len, uint8_t* payload, uint8_t *error);

uint8_t *BuildSetRegionFrame(size_t *buf_len, uint8_t* buf, uint8_t region_code);
int ReadSetRegionFrame(uint16_t len, uint8_t* payload, uint8_t *error);

uint8_t* BuildReadDataFrame(size_t *buf_len, uint8_t* buf, uint8_t* password, uint8_t bank, uint32_t data_ptr, uint32_t data_len);
int ReadReadDataFrame(uint16_t len, uint8_t* payload, uint16_t *pc, uint8_t *epc_len, uint8_t* *epc, uint8_t *data_len, uint8_t* *data);

int ReadWriteDataFrame(uint16_t len, uint8_t* payload, uint16_t *pc, uint8_t *epc_len, uint8_t* *epc, uint8_t *error);

uint8_t* BuildLockFrame(size_t *buf_len, uint8_t* buf, uint8_t* password, uint8_t* lock_payload);
int ReadLockFrame(uint16_t len, uint8_t* payload, uint16_t *pc, uint8_t *epc_len, uint8_t* *epc, uint8_t *error);

uint8_t* BuildKillFrame(size_t *buf_len, uint8_t* buf, uint8_t* password);
int ReadKillFrame(uint16_t len, uint8_t* payload, uint16_t *pc, uint8_t *epc_len, uint8_t* *epc, uint8_t *error);

uint8_t* BuildScanJammerFrame(size_t *buf_len, uint8_t* buf);
int ReadScanJammerFrame(uint16_t len, uint8_t* payload, uint8_t *ch_start, uint8_t *ch_end, uint8_t* *channel_noise);

uint8_t* BuildScanRssiFrame(size_t *buf_len, uint8_t* buf);
int ReadScanRssiFrame(uint16_t len, uint8_t* payload, uint8_t *ch_start, uint8_t *ch_end, uint8_t* *channel_rssi);

uint8_t* BuildSetInventoryModeFrame(size_t *buf_len, uint8_t* buf, uint8_t mode);
int ReadSetInventoryModeFrame(uint16_t len, uint8_t* payload, uint8_t *error);

uint8_t* BuildGetQueryFrame(size_t *buf_len, uint8_t* buf);
int ReadGetQueryFrame(uint16_t len, uint8_t* payload, uint8_t *dr, uint8_t *m, uint8_t *trext, uint8_t *sel, uint8_t *session, uint8_t *target, uint8_t *q);

uint8_t* BuildSetQueryFrame(size_t *buf_len, uint8_t* buf, uint8_t dr, uint8_t m, uint8_t trext, uint8_t sel, uint8_t session, uint8_t target, uint8_t q);
int ReadSetQueryFrame(uint16_t len, uint8_t* payload, uint8_t *error);

uint8_t* BuildSetSleepTimeFrame(size_t *buf_len, uint8_t* buf, uint8_t idle_minutes);
int ReadSetSleepTimeFrame(uint16_t len, uint8_t* payload, uint8_t *idle_minutes);


/*
//not properly documented, or not complete
uint8_t* BuildNXPChangeEasFrame(size_t *buf_len, uint8_t* buf, strAccessPasswd, cbxSetEas.Checked);
uint8_t* BuildNXPEasAlarmFrame(size_t *buf_len, uint8_t* buf);
uint8_t* BuildNXPReadProtectFrame(size_t *buf_len, uint8_t* buf, strAccessPasswd, cbxReadProtectReset.Checked);
uint8_t* BuildNXPResetReadProtectFrame(size_t *buf_len, uint8_t* buf, strAccessPasswd, cbxReadProtectReset.Checked);
uint8_t* BuildSetCWFrame(size_t *buf_len, uint8_t* buf, uint8_t setting);
int ReadSetCWFrame(uint16_t len, uint8_t* payload, uint8_t *error);
uint8_t* BuildInsertRfChFrame(size_t *buf_len, uint8_t* buf, channelNum, channelList);
int ReadInsertRfChFrame(uint16_t len, uint8_t* payload, uint8_t *error);
uint8_t* BuildSaveConfigToNvFrame(size_t *buf_len, uint8_t* buf, NV_enable);
uint8_t* BuildMonzaQTFrame(size_t *buf_len, uint8_t* buf, strAccessPasswd, false, cbxMonzaQT_SR.Checked, cbxMonzaQT_MEM.Checked);
uint8_t* BuildNXPChangeConfigFrame(size_t *buf_len, uint8_t* buf, strAccessPasswd, Convert.ToInt32(txtConfigData.Text.Replace(" ",""), 16));
uint8_t* BuildReadModemParaFrame(size_t *buf_len, uint8_t* buf);
uint8_t* BuildSetModemParaFrame(size_t *buf_len, uint8_t* buf, mixerGain, IFAmpGain, signalTh);
uint8_t* BuildSetReaderEnvModeFrame(size_t *buf_len, uint8_t* buf, (byte)cbxMode.SelectedIndex);
*/

#endif //COMMANDS_H
