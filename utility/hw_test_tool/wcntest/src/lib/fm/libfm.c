/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/* 
 *
 * (C) Copyright 2011 
 * MediaTek <www.MediaTek.com>
 * Hongcheng Xia <Hongcheng.Xia@MediaTek.com>
 *
 */

//for fm auto test and debug

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/resource.h>

#include "fm_main.h"
#include "fm_rds.h"
#include "fm_ioctl.h"
#include "fm_cust_cfg.h"

static int g_fm_fd = -1;
static uint16_t g_freq = 967;
static int type = -1;

int fm_mod_reg(uint8_t addr, uint16_t val, uint16_t mask);

//fm funtion APIs
int fm_open_dev(const char *path)
{
    int fd;
    const char *dev_path = FM_DEVICE_NAME;

    if (path) {
        dev_path = path;
        printf("use new dev path %s\n", dev_path);
    }
    fd = open(dev_path, O_RDWR);
    if (fd == -1) {
        printf("FAIL open %s failed\n", dev_path);
        return -1;
    }
    printf("OK\n");
    g_fm_fd = fd;

    return 0;
}

int fm_close_dev(void)
{
    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    if (0 != close(g_fm_fd)) {
        return -1;
    }

    printf("OK\n");
    g_fm_fd = -1;
    return 0;
}

int fm_power_up(int freq)
{
    int ret;
    struct fm_tune_parm parm;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    bzero(&parm, sizeof(struct fm_tune_parm));
    parm.band = FM_BAND_UE;
    if ((760 <= freq) && (freq <= 1080)) {
        parm.freq = freq;
    } else {
        parm.freq = 974;
    }
    parm.hilo = FM_AUTO_HILO_OFF;
    parm.space = FM_SPACE_100K;

    ret = ioctl(g_fm_fd, FM_IOCTL_POWERUP, &parm);
    if (ret) {
        printf("FAIL:%d:%d\n", ret, parm.err);
        return -1;
    }
    printf("OK\n");

    type = FM_RX;
    return 0;
}

int fm_power_up_tx(int freq)
{
    int ret;
    struct fm_tune_parm parm;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    bzero(&parm, sizeof(struct fm_tune_parm));
    parm.band = FM_BAND_UE;
    if ((760 <= freq) && (freq <= 1080)) {
        parm.freq = freq;
    } else {
        parm.freq = 974;
    }
    parm.hilo = FM_AUTO_HILO_OFF;
    parm.space = FM_SPACE_100K;

    ret = ioctl(g_fm_fd, FM_IOCTL_POWERUP_TX, &parm);
    if (ret) {
        printf("FAIL:%d:%d\n", ret, parm.err);
        return -1;
    }
    printf("OK\n");

    type = FM_TX;
    return 0;
}

int fm_power_down(void)
{
    int ret;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    ret = ioctl(g_fm_fd, FM_IOCTL_POWERDOWN, &type);
    if (ret) {
        printf("FAIL:%d\n", ret);
        return -1;
    }
    
    printf("OK\n");
    return 0;
}

int fm_rds_on_off(int onoff)
{
    int ret;
    uint16_t on_off;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    ret = ioctl(g_fm_fd, FM_IOCTL_RDS_ONOFF, &on_off);
    if (ret) {
        printf("FAIL:%d\n", ret);
        return -1;
    }
    
    printf("OK\n");
    return 0;
}

int fm_tune(int freq)
{
    int ret;
    struct fm_tune_parm parm;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    bzero(&parm, sizeof(struct fm_tune_parm));
    parm.band = FM_BAND_UE;
    parm.freq = freq;
    parm.hilo = FM_AUTO_HILO_OFF;
    parm.space = FM_SPACE_100K;

    ret = ioctl(g_fm_fd, FM_IOCTL_TUNE, &parm);
    if (ret) {
        printf("FAIL:%d:%d\n", ret, parm.err);
        return -1;
    }
    printf("OK:%d\n", parm.freq);
    g_freq = parm.freq;

    return 0;
}

int fm_tune_tx(int freq)
{
    int ret;
    struct fm_tune_parm parm;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    bzero(&parm, sizeof(struct fm_tune_parm));

    parm.band = FM_BAND_UE;
    parm.freq = freq;
    parm.hilo = FM_AUTO_HILO_OFF;
    parm.space = FM_SPACE_100K;

    ret = ioctl(g_fm_fd, FM_IOCTL_TUNE_TX, &parm);
    if (ret) {
        printf("FAIL:%d:%d\n", ret, parm.err);
        return -1;
    }
    printf("OK:%d\n", parm.freq);
    g_freq = parm.freq;

    return 0;
}

int fm_rds_tx(int pi, int pty, const char *ps_name)
{
    int ret;
    struct fm_rds_tx_parm parm;
    uint16_t ps[12] = {0x3132, 0x3334, 0x3536,
                       0x3132, 0x3334, 0x3536,
                       0x3132, 0x3334, 0x3536,
                       0x3132, 0x3334, 0x3536};
    int i;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    bzero(&parm, sizeof(struct fm_rds_tx_parm));
    parm.pi = (uint16_t)pi;
    for(i = 0; i < 12; i++){
        parm.ps[i] = ps[i];
    }
    parm.other_rds_cnt = 0;

    ret = ioctl(g_fm_fd, FM_IOCTL_RDS_TX, &parm);
    if (ret) {
        printf("FAIL:%d:%d\n", ret, parm.err);
        return -1;
    }
    printf("OK\n");

    return 0;
}

int fm_seek(int dir)
{
    int ret;
    struct fm_seek_parm parm;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }
    
    bzero(&parm, sizeof(struct fm_tune_parm));
    parm.band = FM_BAND_UE;
    parm.freq = g_freq;
    parm.hilo = FM_AUTO_HILO_OFF;
    parm.space = FM_SPACE_100K;
    parm.seekdir = dir;
    parm.seekth = FM_SEEKTH_LEVEL_DEFAULT;
    ret = ioctl(g_fm_fd, FM_IOCTL_SEEK, &parm);
    if (ret) {
        printf("FAIL:%d:%d\n", ret, parm.err);
        return -1;
    }
    printf("OK:%d\n",parm.freq);
    g_freq = parm.freq;

    return 0;
}

int fm_scan(void)
{
    int ret;
    int seekdir;
    struct fm_scan_parm parm;
    uint16_t CH_Data[256], tmp_val, ch_offset, MASK_CH;
    int LOFREQ = 875;    
    int i = 0, step = 0, ch_cnt = 0;
    
    if(g_fm_fd < 0){
        printf("WARN fd unavailable\n");
        return -2;
    }

    bzero(&parm, sizeof(struct fm_scan_parm));
    parm.band = FM_BAND_UE;
    parm.space = FM_SPACE_100K;
    parm.hilo = FM_AUTO_HILO_OFF;
    parm.freq = g_freq;
    parm.ScanTBLSize = sizeof(parm.ScanTBL)/sizeof(uint16_t);

    ret = ioctl(g_fm_fd, FM_IOCTL_SCAN, &parm);
    if (ret) {
        printf("FAIL:%d:%d\n", ret, parm.err);
        return -1;
    }
    printf("OK\n");
    g_freq = parm.freq;
    
    //parm.ScanTBLSize will return valid scan table size. 
    printf("Scan channel:");   
    for (ch_offset = 0; ch_offset < parm.ScanTBLSize; ch_offset++) {
		MASK_CH = 0x0001; 
		tmp_val = parm.ScanTBL[ch_offset];
		for (step = 0; step < 16; step++) {
			MASK_CH = 1 << step;
			if ((MASK_CH & tmp_val)!=0) {
				*(CH_Data + ch_cnt) = LOFREQ  + (ch_offset * 16 + step) * (parm.space); //100KHz
                if (*(CH_Data + ch_cnt) <= 1080) {
				    printf("%d ", *(CH_Data + ch_cnt)); 
				    ch_cnt++;
			    }
		    } 
	    } 
	} 
	printf("\nScan channel num:%d\n", ch_cnt); 

    return 0;
}

int fm_jammer_scan(void)
{
    int ret;
    int tmp;
    int seekdir;
    struct fm_scan_parm parm;
    uint16_t CH_Data[256], tmp_val, ch_offset, MASK_CH;
    int LOFREQ = 875;    
    int i = 0, step = 0, ch_cnt = 0;
    struct fm_cqi_req cqi_req;
    struct fm_cqi *cqi = NULL;
    
    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    ret = fm_mod_reg(0x61, 0x2000, 0xDFFF);
    if (ret) {
        return ret;
    }
    
    bzero(&parm, sizeof(struct fm_scan_parm));
    parm.band = FM_BAND_UE;
    parm.space = FM_SPACE_100K;
    parm.hilo = FM_AUTO_HILO_OFF;
    parm.freq = g_freq;
    parm.ScanTBLSize = sizeof(parm.ScanTBL)/sizeof(uint16_t);

    ret = ioctl(g_fm_fd, FM_IOCTL_SCAN, &parm);
    if (ret) {
        printf("FAIL:%d:%d\n", ret, parm.err);
        return -1;
    }
    printf("OK\n");
    g_freq = parm.freq;
    
    //parm.ScanTBLSize will return valid scan table size. 
    printf("Scan channel:");   
    for (ch_offset = 0; ch_offset < parm.ScanTBLSize; ch_offset++) {
		MASK_CH = 0x0001; 
		tmp_val = parm.ScanTBL[ch_offset];
		for (step = 0; step < 16; step++) {
			MASK_CH = 1 << step;
			if ((MASK_CH & tmp_val)!=0) {
				*(CH_Data + ch_cnt) = LOFREQ  + (ch_offset * 16 + step) * (parm.space); //100KHz
				printf("%d ", *(CH_Data + ch_cnt)); 
				ch_cnt++;
			}
		} 
	} 
	printf("\nScan channel num:%d\n", ch_cnt); 

    cqi_req.ch_num = (uint16_t)ch_cnt;
    cqi_req.ch_num = (cqi_req.ch_num > 255) ? 255 : cqi_req.ch_num;
    cqi_req.buf_size = cqi_req.ch_num*sizeof(struct fm_cqi);
    cqi_req.cqi_buf = malloc(cqi_req.buf_size + 1);
    if (!cqi_req.cqi_buf) {
        printf("no mem\n");
        return -1;
    }

    ret = ioctl(g_fm_fd, FM_IOCTL_CQI_GET, &cqi_req);
    if (ret) {
        free(cqi_req.cqi_buf);
        printf("FAIL:%d\n", ret);
        return -1;
    }
    cqi = (struct fm_cqi*)cqi_req.cqi_buf;
    tmp = 0;
    for (tmp = 0; tmp < cqi_req.ch_num; tmp++) {
        printf("freq %d, jam_freq 0x%04x, reserve 0x%04x\n", (cqi[tmp].ch/2 + 640), cqi[tmp].rssi, cqi[tmp].reserve);
    }
    free(cqi_req.cqi_buf);
    printf("OK\n");
    
    ret = fm_mod_reg(0x61, 0x0000, 0xDFFF);
    if (ret) {
        return ret;
    }

    return 0;
}

int fm_getcqi(int num)
{
    int ret;
    int tmp;
    struct fm_cqi_req cqi_req;
    struct fm_cqi *cqi = NULL;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    cqi_req.ch_num = (uint16_t)num;
    cqi_req.ch_num = (cqi_req.ch_num > 255) ? 255 : cqi_req.ch_num;
    cqi_req.buf_size = cqi_req.ch_num*sizeof(struct fm_cqi);
    cqi_req.cqi_buf = malloc(cqi_req.buf_size + 1);
    if (!cqi_req.cqi_buf) {
        printf("no mem\n");
        return -1;
    }

    ret = ioctl(g_fm_fd, FM_IOCTL_CQI_GET, &cqi_req);
    if (ret) {
        free(cqi_req.cqi_buf);
        printf("FAIL:%d\n", ret);
        return -1;
    }
    cqi = (struct fm_cqi*)cqi_req.cqi_buf;
    tmp = 0;
    for (tmp = 0; tmp < cqi_req.ch_num; tmp++) {
        printf("freq %d, rssi 0x%04x, reserve 0x%04x\n", cqi[tmp].ch/2 + 640, cqi[tmp].rssi, cqi[tmp].reserve);
    }
    free(cqi_req.cqi_buf);
    printf("OK\n");

    return 0;
}

int fm_scan_force_stop(void)
{
    int stop_scan = 1;
    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }
    
    if (lseek(g_fm_fd, 0, SEEK_END) != -1) {
        printf("OK\n");
        return 0;
    } else {
        printf("FAIL\n");
        return -1;    
    }
    
}

//scan test.
int fm_seek_all(void)
{
    int ret;
    int start_freq = 1080;

    struct fm_seek_parm parm;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    g_freq = start_freq;

    do{
        bzero(&parm, sizeof(struct fm_tune_parm));
        parm.band = FM_BAND_UE;
        parm.freq = g_freq;
        parm.hilo = FM_AUTO_HILO_OFF;
        parm.space = FM_SPACE_100K;
        parm.seekdir = FM_SEEK_UP;
        parm.seekth = FM_SEEKTH_LEVEL_DEFAULT;
        
        ret = ioctl(g_fm_fd, FM_IOCTL_SEEK, &parm);
        if (ret) {
            printf("FAIL:%d:%d\n", ret, parm.err);
            return -1;
        }
        printf("OK:%d:%d:%d\n", ret, parm.err, parm.freq);
        g_freq = parm.freq;
    } while (parm.err == FM_SUCCESS);
    
    return 0;
}

int fm_mute(void)
{
    int ret;
    uint32_t mute = 1;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    ret = ioctl(g_fm_fd, FM_IOCTL_MUTE, &mute);
    if (ret) {
        printf("FAIL:%d\n", ret);
        return -1;
    }

    printf("OK\n");
    return 0;
}

int fm_unmute(void)
{
    int ret;
    uint32_t mute = 0;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    ret = ioctl(g_fm_fd, FM_IOCTL_MUTE, &mute);
    if (ret) {
        printf("FAIL:%d\n", ret);
        return -1;
    }

    printf("OK\n");
    return 0;
}

int fm_setvol(int volume)
{
    int ret;
    uint32_t vol;
    int tmp;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    vol = (uint32_t)volume;
    ret = ioctl(g_fm_fd, FM_IOCTL_SETVOL, &vol);
    if (ret) {
        printf("FAIL:%d\n", ret);
        return -1;
    }

    printf("OK\n");
    return 0;
}

int fm_getvol(void)
{
    int ret;
    uint32_t vol;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    ret = ioctl(g_fm_fd, FM_IOCTL_GETVOL, &vol);
    if (ret) {
        printf("FAIL:%d\n", ret);
        return -1;
    }
    printf("OK\n");
    printf("volume:%d\n", (int)vol);

    return vol;
}


/* fm_switchana - switch FM anatenna between long and short
 *
 * @ana -antenna type, 0: long antenna; 1: short antenna 
 * return 0 if success, else error code
 */
int fm_switchana(int ana)
{
    int ret;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    ret = ioctl(g_fm_fd, FM_IOCTL_ANA_SWITCH, &ana);
    if (ret) {
        printf("FAIL:%d\n", ret);
        return -1;
    }

    printf("OK\n");
    return 0;
}


int fm_getrssi(int *pRssi)
{
    int ret;
    int rssi;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    ret = ioctl(g_fm_fd, FM_IOCTL_GETRSSI, &rssi);
    if (ret) {
        printf("FAIL:%d\n", ret);
        return -1;
    }
    printf("OK\n");
    printf("RSSI:%d\n", rssi);

    *pRssi = rssi;	

    return 0;
}

int fm_32kppm_compensation(int age, int drift)
{
    int ret;
    struct fm_gps_rtc_info rtcInfo;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    bzero(&rtcInfo, sizeof(struct fm_gps_rtc_info));
    rtcInfo.ageThd = 2;
    rtcInfo.driftThd = 30;
    rtcInfo.retryCnt = 2;
    rtcInfo.tvThd.tv_sec = 10;
    rtcInfo.age = age;
    rtcInfo.drift = drift;
    ret = ioctl(g_fm_fd, FM_IOCTL_GPS_RTC_DRIFT, &rtcInfo);
    if (ret) {
        printf("FAIL:%d\n", ret);
        return -1;
    }
    printf("OK\n");
    return 0;
}


/* fm_i2s_set - FM I2S setting 
 *
 * @onoff - 0: on; 1: off
 * @mode - 0: master; 1:slave
 * @rate - 0, 32K; 1, 44K; 2, 48K
 * return 0 if success, else error code
 */
int fm_i2s_set(int onoff, int mode, int samplerate)
{
    int ret;
    int tmp = -1;
    static struct fm_i2s_setting i2s_cfg;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    bzero(&i2s_cfg, sizeof(struct fm_i2s_setting));
    i2s_cfg.onoff = FM_I2S_ON;
    i2s_cfg.mode = FM_I2S_SLAVE;
    i2s_cfg.sample = FM_I2S_48K;
    if (onoff == 1) {
        i2s_cfg.onoff = FM_I2S_ON; 
    } else if (onoff == 0){
        i2s_cfg.onoff = FM_I2S_OFF;
    } else {
        return -1;
    }

    if (mode == 1) {
        i2s_cfg.mode = FM_I2S_MASTER; 
    } else if (mode == 0){
        i2s_cfg.mode = FM_I2S_SLAVE;
    } else {
        return -1;
    }

    if (samplerate == 0) {
        i2s_cfg.sample = FM_I2S_32K; 
    } else if (samplerate == 1){
        i2s_cfg.mode = FM_I2S_44K;
    } else if (samplerate == 2){
        i2s_cfg.mode = FM_I2S_48K;
    } else {
        return -1;
    }

    ret = ioctl(g_fm_fd, FM_IOCTL_I2S_SETTING, &i2s_cfg);
    if (ret) {
        printf("FAIL:%d\n", ret);
        return -1;
    }
    printf("OK\n");
    return 0;
}

int fm_over_bt(int onoff)
{
    int ret;
    uint32_t viabt = onoff;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    ret = ioctl(g_fm_fd, FM_IOCTL_OVER_BT_ENABLE, &viabt);
    if (ret) {
        printf("FAIL:%d\n", ret);
        return -1;
    }
    printf("OK\n");
    return 0;
}

int fm_read_reg(uint8_t addr)
{
    int ret;
    struct fm_ctl_parm parm;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    bzero(&parm, sizeof(struct fm_ctl_parm));
    parm.addr = addr;
    parm.rw_flag = 1;

    ret = ioctl(g_fm_fd, FM_IOCTL_RW_REG, &parm);
    if (ret) {
        printf("FAIL:%d:%d\n", ret, parm.err);
        return -1;
    }
    printf("OK: 0x%02x = 0x%04x\n", parm.addr, parm.val);

    return parm.val;
}

int print_page(uint16_t page, int mode)
{
    int ret;
    uint8_t addr = 0x00;
    int i = 0;
    int log_fd = -1;
    int std_out = -1;
    struct fm_ctl_parm parm;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }
    if (mode == 0) {
        ;//        
    } else if (mode == 1){
        std_out = dup(1);
        if (std_out < 0) {
            ;//dup std out error
        }
        close(1);
        log_fd = open("/data/data/fmlog", O_RDWR | O_CREAT | O_APPEND, 00660);
        if (log_fd < 0) {
            ;//error
        }
    }
    
    //setup page
    printf("page %d.\n", (int)page);
    bzero(&parm, sizeof(struct fm_ctl_parm));
    parm.addr = 0x9F;  //page addr
    parm.val = 0x0000; //page 0
    parm.rw_flag = 0; //write
    ret = ioctl(g_fm_fd, FM_IOCTL_RW_REG, &parm);
    if (ret) {
        printf("FAIL:%d:%d\n", ret, parm.err);
        return -1;
    }
    
start:    
    bzero(&parm, sizeof(struct fm_ctl_parm));
    parm.addr = addr;
    printf("0x%02x ", parm.addr);
    parm.rw_flag = 1;
    ret = ioctl(g_fm_fd, FM_IOCTL_RW_REG, &parm);
    if (ret) {
        printf("FAIL:%d:%d\n", ret, parm.err);
        return -1;
    }
    printf("0x%04x\n", parm.val);
    addr++;
    if (++i < 256)
        goto start;

    if (mode == 0) {
        ;//        
    } else if (mode == 1){
        close(log_fd);
        std_out = dup(std_out);
        printf("stdout [fd=%d]\n", std_out);
    }
    return 0;
}

int fm_print_reg(int out)
{
    int ret = 0;
    
    ret = print_page(0x03, out);
    ret = print_page(0x02, out);
    ret = print_page(0x01, out);
    ret = print_page(0x00, out);
    printf("OK\n");

    return 0;
}

int fm_write_reg(uint8_t addr, uint16_t value)
{
    int ret;
    struct fm_ctl_parm parm;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }

    bzero(&parm, sizeof(struct fm_ctl_parm));
    parm.addr = addr;
    parm.val = value;
    parm.rw_flag = 0;

    ret = ioctl(g_fm_fd, FM_IOCTL_RW_REG, &parm);
    if (ret) {
        printf("FAIL:%d:%d\n", ret, parm.err);
        return -1;
    }
    printf("OK: 0x%02x = 0x%04x\n", parm.addr, parm.val);

    return 0;
}

/*
 * fm_mod_reg
 * mod addr mask val -> mod 0x60 0xFFFE 0x0001
 */
int fm_mod_reg(uint8_t addr, uint16_t val, uint16_t mask)
{
    int ret;
    struct fm_ctl_parm parm;

    if (g_fm_fd < 0) {
        printf("WARN fd unavailable\n");
        return -2;
    }
    bzero(&parm, sizeof(struct fm_ctl_parm));
    parm.addr = addr;
    parm.rw_flag = 1;

    ret = ioctl(g_fm_fd, FM_IOCTL_RW_REG, &parm);
    if (ret) {
        printf("FAIL:%d:%d\n", ret, parm.err);
        return -1;
    }
    
    parm.val = (parm.val & mask) | (val & (~mask));
    parm.rw_flag = 0;
    ret = ioctl(g_fm_fd, FM_IOCTL_RW_REG, &parm);
    if (ret) {
        printf("FAIL:%d:%d\n", ret, parm.err);
        return -1;
    }
    printf("OK: 0x%02x = 0x%04x\n", parm.addr, parm.val);

    return 0;
}

int fm_delay(int sec, int microsec)
{
    struct timeval tval;

    tval.tv_sec = sec;
    tval.tv_usec = microsec;
    printf("OK: %ds,%dus \n", (int)tval.tv_sec, (int)tval.tv_usec);
    select(0, NULL, NULL, NULL, &tval);

    return 0;
}
