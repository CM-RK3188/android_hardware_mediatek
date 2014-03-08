
#ifndef __FM_WRAPPER_H__
#define __FM_WRAPPER_H__
#include "wcn_cmd.h"

int fmw_open_dev(struct wcn_cmd *cmd);
int fmw_close_dev(struct wcn_cmd *cmd);
int fmw_power_up(struct wcn_cmd *cmd);
int fmw_power_up_tx(struct wcn_cmd *cmd);
int fmw_power_down(struct wcn_cmd *cmd);
int fmw_tune(struct wcn_cmd *cmd);
int fmw_tune_tx(struct wcn_cmd *cmd);
int fmw_seek(struct wcn_cmd *cmd);
int fmw_scan(struct wcn_cmd *cmd);
int fmw_jammer_scan(struct wcn_cmd *cmd);
int fmw_getcqi(struct wcn_cmd *cmd);
int fmw_getrssi(struct wcn_cmd *cmd);
int fmw_switchana(struct wcn_cmd *cmd); 
int fmw_mute(struct wcn_cmd *cmd);
int fmw_unmute(struct wcn_cmd *cmd);
int fmw_scan_force_stop(struct wcn_cmd *cmd);
int fmw_setvol(struct wcn_cmd *cmd);
int fmw_getvol(struct wcn_cmd *cmd);
int fmw_over_bt(struct wcn_cmd *cmd);
int fmw_32kppm_compensation(struct wcn_cmd *cmd);
int fmw_i2s_set(struct wcn_cmd *cmd);
int fmw_rds_on_off(struct wcn_cmd *cmd);
int fmw_rds_tx(struct wcn_cmd *cmd);
int fmw_read_reg(struct wcn_cmd *cmd);
int fmw_write_reg(struct wcn_cmd *cmd);
int fmw_mod_reg(struct wcn_cmd *cmd);
int fmw_print_reg(struct wcn_cmd *cmd);
int fmw_bye(struct wcn_cmd *cmd);
int fmw_delay(struct wcn_cmd *cmd);

#define FM_TEST_TBL \
    {"fmopen", 0, fmw_open_dev, {NULL, NULL, NULL}, "eg: open, desc: open dev/fm"}, \
    {"fmclose", 0, fmw_close_dev, {NULL, NULL, NULL}, "eg: close, desc: close dev/fm"}, \
    {"fmpwron", 0, fmw_power_up, {NULL, NULL, NULL}, "eg: pwron, desc: power on fm rx subsystem"}, \
    {"fmtxpwron", 0, fmw_power_up_tx, {NULL, NULL, NULL}, "eg: txpwron, desc: power on fm tx subsystem"}, \
    {"fmpwroff", 0, fmw_power_down, {NULL, NULL, NULL}, "eg: pwroff, desc: power off fm subsystem"}, \
    {"fmtune", 1, fmw_tune, {NULL, NULL, NULL}, "eg: tune 876, desc: tune to a channel"}, \
    {"fmtxtune", 1, fmw_tune_tx, {NULL, NULL, NULL}, "eg: txtune 876, desc: tune tx to a channel"}, \
    {"fmseek", 1, fmw_seek, {NULL, NULL, NULL}, "eg: seek up, desc: seek to another channel"}, \
    {"fmscan", 0, fmw_scan, {NULL, NULL, NULL}, "eg: scan, desc: auto scan"}, \
    {"fmjamscan", 0, fmw_jammer_scan, {NULL, NULL, NULL}, "eg: jamscan, desc: auto jammer scan"}, \
    {"fmcqi", 1, fmw_getcqi, {NULL, NULL, NULL}, "eg: cqi, desc: get channel CQI"}, \
    {"fmrssi", 0, fmw_getrssi, {NULL, NULL, NULL}, "eg: rssi, desc: get current channel RSSI"}, \
    {"fmmute", 0, fmw_mute, {NULL, NULL, NULL}, "eg: mute, desc: mute audio"}, \
    {"fmunmute", 0, fmw_unmute, {NULL, NULL, NULL}, "eg: unmute, desc: unmute audio"}, \
    {"fmsstop", 0, fmw_scan_force_stop, {NULL, NULL, NULL}, "eg: sstop, desc: stop scan flow"}, \
    {"fmsetvol", 1, fmw_setvol, {NULL, NULL, NULL}, "eg: setvol 14, desc: set audio volume(rang:0~14)"}, \
    {"fmgetvol", 0, fmw_getvol, {NULL, NULL, NULL}, "eg: getvol, desc: get audio volume"}, \
    {"fmviabt", 1, fmw_over_bt, {NULL, NULL, NULL}, "eg: viabt on, desc: set fm over bt on/off"}, \
    {"fmgpsrtc", 2, fmw_32kppm_compensation, {NULL, NULL, NULL}, "eg: gpsrtc 10 20, desc: gps rtc setting"}, \
    {"fmseti2s", 3, fmw_i2s_set, {NULL, NULL, NULL}, "eg: seti2s on slave 32k , desc: i2s setting"}, \
    {"fmana", 1, fmw_switchana, {NULL, NULL, NULL}, "eg: fmana long, desc: switch antenna"}, \
    {"fmrds", 1, fmw_rds_on_off, {NULL, NULL, NULL}, "eg: rds on, desc: rds rx on/off"}, \
    {"fmtxrds", 3, fmw_rds_tx, {NULL, NULL, NULL}, "eg: txrds 0x1234 0 0 , desc: rds tx"}, \
    {"fmdelay", 2, fmw_delay, {NULL, NULL, NULL}, "eg: delay 100 ms, desc: delay n s/ms/us"}, \
    {"fmrd", 1, fmw_read_reg, {NULL, NULL, NULL}, "eg: rd 0x62, desc: read a register"}, \
    {"fmread", 1, fmw_read_reg, {NULL, NULL, NULL}, "eg: read 0x62, desc: read a register"}, \
    {"fmwr", 2, fmw_write_reg, {NULL, NULL, NULL}, "eg: wr 0x62 0xFF15, desc: write a register"}, \
    {"fmwrite", 2, fmw_write_reg, {NULL, NULL, NULL}, "eg: write 0x62 0xFF15, desc: write a register"}, \
    {"fmmod", 3, fmw_mod_reg, {NULL, NULL, NULL}, "eg: mod 0x62 0xFFFE 0x0001, desc: modify a register"}, \
    {"fmmodify", 3, fmw_mod_reg, {NULL, NULL, NULL}, "eg: modify 0x62 0xFFFE 0x0001, desc: modify a register"}, \
    {"fmshowreg", 1, fmw_print_reg, {NULL, NULL, NULL}, "eg: showreg, desc: print all register"}

#endif //__FM_WRAPPER_H__