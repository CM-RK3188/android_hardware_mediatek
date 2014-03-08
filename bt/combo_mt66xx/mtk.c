/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <cutils/properties.h>
//#include "cutils/properties.h"

#include "custom/cust_bt.h"
#include "bt_mtk.h"


/**************************************************************************
 *                 G L O B A L   V A R I A B L E S                        *
***************************************************************************/

bt_vendor_callbacks_t *bt_vnd_cbacks = NULL;
BOOL fInternalCfg = FALSE;
static int  bt_fd = -1;
int g_chipId = 0;

/**************************************************************************
  *                       F U N C T I O N S                               *
***************************************************************************/

extern
BOOL BT_InitDevice(
    PBYTE   ucConfData,
    DWORD   dwBaud,
    DWORD   dwHostBaud,
    DWORD   dwFlowControl,
    SETUP_UART_PARAM  setup_uart_param
);

extern
BOOL BT_InitSCO(
);


/* Register callback functions to libbt-hci.so */
void set_callbacks(const bt_vendor_callbacks_t* p_cb)
{
     bt_vnd_cbacks = p_cb;
}

/* Cleanup callback functions previously registered */
void clean_callbacks()
{
     bt_vnd_cbacks = NULL;
}

/* Initialize UART port */
int init_uart()
{
    struct termios ti;
    
    LOG_TRC();
    
    bt_fd = open(CUST_BT_SERIAL_PORT, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (bt_fd < 0) {
        LOG_ERR("Can't open serial port,error str:%s\n", strerror(errno));
        return -1;
    }
    
    return bt_fd;
}

/* Close UART port previously opened */
void close_uart()
{
    if (bt_fd >= 0) close(bt_fd);
    bt_fd = -1;
}

int bt_get_combo_id(int *pChipId)
{
    int  chipId_ready_retry = 0;
    char chipId_val[PROPERTY_VALUE_MAX];
    
    do {
        property_get("persist.radio.mediatek.chipid", chipId_val, NULL);
        if(0 == strcmp(chipId_val, "0x6620")){
            *pChipId = 0x6620;
            break;
        }
        else if(0 == strcmp(chipId_val, "0x6628")){
            *pChipId = 0x6628;
            break;
        }
        else {
            chipId_ready_retry ++;
            usleep(200000);
        }
    } while(chipId_ready_retry < 10);
    
    LOG_DBG("Get combo chip id retry %d\n", chipId_ready_retry);
    if (chipId_ready_retry >= 10){
        LOG_DBG("Get combo chip id fails!\n");
        return -1;
    }
    else{
        LOG_DBG("Combo chip id %x\n", *pChipId);
        return 0;
    }
}

static int load_custom_bt_conf(struct btradio_conf_data *cfg)
{
   /*
    This method depends on the configuration data store machenism
    on customer's platform. 
    Customer may use NVRAM, data file, or other patterns.
    Here RECOMMEND and GIVE AN EXAMPLE to push configuration data
    under /data/BT.cfg
    */

    struct btradio_conf_data temp;
    int file_fd = 0;
    ssize_t retLen;

    file_fd = open(CUSTOM_BT_CFG_FILE, O_RDONLY);
    if (file_fd < 0){
        LOG_WAN("Failed to open %s!\n", CUSTOM_BT_CFG_FILE);
        return -1;
    }
    
    retLen = read(file_fd, &temp, sizeof(temp));
    close(file_fd);
    if (retLen < 0){        
        return -1;
    }
    else if(retLen < sizeof(temp)){
        LOG_ERR("File read error len: %d\n", retLen);
        return -1;
    }
    else{
        memcpy(cfg, &temp, retLen);
        return 0;
    }   
}

static int load_internal_bt_conf(struct btradio_conf_data *cfg)
{
    struct btradio_conf_data temp;
    int file_fd = 0;
    ssize_t retLen;
    ssize_t written;

    file_fd = open(INTERNAL_BT_CFG_FILE, O_RDONLY);
    
    if(file_fd < 0){
        LOG_WAN("No internal BT config, generate from default value\n");
		
		if(0x6620 == g_chipId){
			memcpy(&temp, &sDefaultCfg_6620, sizeof(struct btradio_conf_data));
		}
		else if(0x6628 == g_chipId){
			memcpy(&temp, &sDefaultCfg_6628, sizeof(struct btradio_conf_data));
		}
        
        // Generate internal BT config file
        file_fd = open(INTERNAL_BT_CFG_FILE, O_WRONLY | O_CREAT, 0644);
        if (file_fd < 0){
            LOG_ERR("Try to create internal BT config, error\n");
            return -1;
        }
        
        written = write(file_fd, &temp, sizeof(temp));
        close(file_fd);
        if (written < 0){
            return -1;
        }
        else if(written < sizeof(temp)){
            LOG_ERR("File write error len: %d\n", written);
        }
        else{
            LOG_WAN("Internal BT config generated\n");
        }
        
        memcpy(cfg, &temp, sizeof(temp));
        return 0;
    }
    else{
        retLen = read(file_fd, &temp, sizeof(temp));
        close(file_fd);
        if (retLen < 0){
            return -1;
        }
        else if(retLen < sizeof(temp)){
            LOG_ERR("File read error len: %d\n", retLen);
        }
        
        memcpy(cfg, &temp, retLen);
        return 0;
    }
}

/* MTK specific chip initialization */
int mtk_fw_cfg(void)
{
    struct btradio_conf_data cfg;
    int speed = 0, iUseFlowControl = 0;
    SETUP_UART_PARAM uart_setup_callback = NULL;
    int ret;

	/* Get combo chip id first */
    if(bt_get_combo_id(&g_chipId) < 0){
        LOG_ERR("Unknown combo chip id\n");
		return -1;
    }

	LOG_DBG("%s %d, g_chipId=%x\n", __FILE__, __LINE__, g_chipId);

	if(0x6620 == g_chipId){
		memcpy(&cfg, &sDefaultCfg_6620, sizeof(struct btradio_conf_data));
	}
	else if(0x6628 == g_chipId){
		memcpy(&cfg, &sDefaultCfg_6628, sizeof(struct btradio_conf_data));
	}
    
    LOG_DBG("%s %d\n", __FILE__, __LINE__);
    
    /* Load BT config data */
    if (load_custom_bt_conf(&cfg) < 0){
        LOG_WAN("No custom BT config\n");
        
        if (load_internal_bt_conf(&cfg) < 0){
            LOG_ERR("Load internal BT config failed!\n");
        }
        else{
            LOG_WAN("Load internal BT config success\n");
            fInternalCfg = TRUE;
        }
    }
    else{
        LOG_WAN("Load custom BT config success\n");
    }
    
    LOG_DBG("[BD address %02x-%02x-%02x-%02x-%02x-%02x][voice %02x %02x][codec %02x %02x %02x %02x] \
            [radio %02x %02x %02x %02x %02x %02x][sleep %02x %02x %02x %02x %02x %02x %02x] \
            [feature %02x %02x][tx pwr offset %02x %02x %02x]\n",
            cfg.addr[0], cfg.addr[1], cfg.addr[2], cfg.addr[3], cfg.addr[4], cfg.addr[5],
            cfg.voice[0], cfg.voice[1], cfg.codec[0], cfg.codec[1], cfg.codec[2], cfg.codec[3],
            cfg.radio[0], cfg.radio[1], cfg.radio[2], cfg.radio[3], cfg.radio[4], cfg.radio[5],
            cfg.sleep[0], cfg.sleep[1], cfg.sleep[2], cfg.sleep[3], cfg.sleep[4], cfg.sleep[5], cfg.sleep[6],
            cfg.feature[0], cfg.feature[1],
            cfg.tx_pwr_offset[0], cfg.tx_pwr_offset[1], cfg.tx_pwr_offset[2]);


    ret = BT_InitDevice(
             (PBYTE)&cfg,
             speed,
             speed,
             iUseFlowControl,
             uart_setup_callback) == TRUE ? 0 : -1;

    return ret;
}

/* MTK specific SCO/PCM configuration */
int mtk_sco_cfg(void)
{
    return (BT_InitSCO() == TRUE ? 0 : -1);
}
