#include "parser.h"



/*
 * builds a command frame.
 */
uint8_t* build_cmd_frame(uint8_t* buf, uint8_t cmd, uint16_t len, uint8_t* payload)
{
    buf[0] = FRAME_BEGIN;
    buf[1] = FRAME_TYPE_CMD;
    buf[2] = cmd;
    buf[3] = (len >> 8) & 0xff;
    buf[4] = len & 0xff;
    memcpy(&buf[5], payload, len);
    buf[5+len] = calc_frame_checksum(buf);
    buf[6+len] = FRAME_END;
}


/*
 * calculates the checksum, assumes that the first byte is the FRAME_BEGIN header byte
 */
uint8_t calc_frame_checksum(uint8_t* buf, len)
{
    uint8_t cs = 0x00;
    for(int i=0; i<(4+len); i++)
    {
        cs += buf[1+i];
    }
    return cs;
}

/*
 * param field valid are 0,1,2 MODULE_*_FIELD
 */
BuildGetModuleInfoFrame(uint8_t* buf, uint8_t field)
{
    uint8_t cmd = CMD_GET_MODULE_INFO;
    uint16_t len = 1;
    return build_cmd_frame(buf, cmd, len, field);
}

BuildGetPaPowerFrame(uint8_t* buf)
{
    uint8_t cmd = CMD_GET_POWER;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildGetQueryFrame(uint8_t* buf)
{
    uint8_t cmd = CMD_GET_QUERY;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildGetRfChannelFrame(uint8_t* buf)
{
    uint8_t cmd = CMD_GET_RF_CHANNEL;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildGetSelectFrame(uint8_t* buf, )
{
    uint8_t cmd = CMD_GET_SELECT_PARA;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, );
}
BuildInsertRfChFrame(uint8_t* buf, channelNum, channelList)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, );
}
BuildIoControlFrame(uint8_t* buf, uint8_t* io_payload)
{
    uint8_t cmd = CMD_IO_CONTROL;
    uint16_t len = 3;
    return build_cmd_frame(buf, cmd, len, uint8_t* io_payload);
}
BuildKillFrame(uint8_t* buf, strKillPasswd, killRfu)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildLockFrame(uint8_t* buf, textBoxLockAccessPwd.Text, lockPayload)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildMonzaQTFrame(uint8_t* buf, strAccessPasswd, false, cbxMonzaQT_SR.Checked, cbxMonzaQT_MEM.Checked)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildNXPChangeConfigFrame(uint8_t* buf, strAccessPasswd, Convert.ToInt32(txtConfigData.Text.Replace(" ",""), 16))
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildNXPChangeEasFrame(uint8_t* buf, strAccessPasswd, cbxSetEas.Checked)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildNXPEasAlarmFrame(uint8_t* buf)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildNXPReadProtectFrame(uint8_t* buf, strAccessPasswd, cbxReadProtectReset.Checked)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildReadDataFrame(uint8_t* buf, strAccessPasswd, intMemBank, wordPtr, wordCnt)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildReadModemParaFrame(uint8_t* buf)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildReadMultiFrame(uint8_t* buf, loopCnt)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildReadSingleFrame(uint8_t* buf)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildSaveConfigToNvFrame(uint8_t* buf, NV_enable)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildScanJammerFrame(uint8_t* buf)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildScanRssiFrame(uint8_t* buf)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildSetBaudFrame(uint8_t* buf)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildSetCWFrame(uint8_t* buf, ConstCode.SET_ON)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildSetFhssFrame(uint8_t* buf, ConstCode.SET_ON)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildSetInventoryModeFrame(uint8_t* buf, ConstCode.INVENTORY_MODE1);  //INVENTORY_MODE
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildSetModemParaFrame(uint8_t* buf, mixerGain, IFAmpGain, signalTh)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildSetModuleSleepFrame(uint8_t* buf)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildSetPaPowerFrame(uint8_t* buf, (Int16)powerDBm)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildSetQueryFrame(uint8_t* buf, intDR, intM, intTRext, intSel, intSession, intTarget, intQ)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildSetReaderEnvModeFrame(uint8_t* buf, (byte)cbxMode.SelectedIndex)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildSetRegionFrame(uint8_t* buf, ConstCode.REGION_CODE_US)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildSetRfChannelFrame(uint8_t* buf, cbxChannel.SelectedIndex.ToString("X2"))
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildSetSelectFrame(uint8_t* buf, intSelTarget, intAction, intSelMemBank, intSelPointer, intMaskLen, intTruncate, txtSelMask.Text)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildStopReadFrame(uint8_t* buf)
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
BuildWriteDataFrame(uint8_t* buf, strAccessPasswd, intMemBan
{
    uint8_t cmd = ;
    uint16_t len = 0;
    return build_cmd_frame(buf, cmd, len, NULL);
}
genLockPayload((byte)cbxLockUserAction.SelectedIndex, 0x04);
