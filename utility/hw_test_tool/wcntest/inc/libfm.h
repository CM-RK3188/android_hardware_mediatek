
#ifndef __LIBFM_H__
#define __LIBFM_H__

int fm_open_dev(const char *path);
int fm_close_dev(void);
int fm_power_up(int freq);
int fm_power_up_tx(int freq);
int fm_power_down(void);
int fm_rds_on_off(int onoff);
int fm_tune(int freq);
int fm_tune_tx(int freq);
int fm_rds_tx(int pi, int pty, const char *ps);
int fm_seek(int dir);
int fm_scan(void);
int fm_jammer_scan(void);
int fm_getcqi(int num);
int fm_scan_force_stop(void);
int fm_seek_all(void);
int fm_mute(void);
int fm_unmute(void);
int fm_setvol(int volume);
int fm_getvol(void);
int fm_switchana(int ana);
int fm_getrssi(int *pRssi);
int fm_32kppm_compensation(int age, int drift);
int fm_i2s_set(int onoff, int mode, int samplerate);
int fm_over_bt(int onoff);
int fm_read_reg(uint8_t addr);
int fm_print_reg(int out);
int fm_write_reg(uint8_t addr, uint16_t value);
int fm_mod_reg(uint8_t addr, uint16_t val, uint16_t mask);
int fm_delay(int sec, int microsec);

#endif //__LIBFM_H__
