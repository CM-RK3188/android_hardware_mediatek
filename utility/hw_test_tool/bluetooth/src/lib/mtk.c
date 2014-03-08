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
#include <termios.h>
#include <fcntl.h>
#include <cutils/properties.h>

#include "custom/cust_bt.h"
#include "custom/CFG_BT_File.h"
#include "bt_kal.h"


#ifndef MTK_COMBO_SUPPORT
#define FLOW_CTL_HW     0x0001
#define FLOW_CTL_SW     0x0002
#define FLOW_CTL_NONE   0x0000
#define FLOW_CTL_MASK   0x0003
#endif

BOOL fInternalCfg = FALSE;

#ifdef MTK_COMBO_SUPPORT
int g_chipId = 0;
#endif

/**************************************************************************
  *                         F U N C T I O N S                             *
***************************************************************************/

extern
BOOL BT_InitDevice(
    HANDLE  hComPort,
    PBYTE   pucConfData,
    DWORD   dwBaud,
    DWORD   dwHostBaud,
    DWORD   dwFlowControl,
    SETUP_UART_PARAM  setup_uart_param
);

extern
BOOL BT_DeinitDevice(
    HANDLE  hComPort
);

/* Initialize UART driver */
static int init_uart(char *dev)
{
    struct termios ti;
    int fd, i;
    
    LOG_TRC();
    
    fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        LOG_ERR("Can't open serial port\n");
        return -1;
    }

#ifndef MTK_COMBO_SUPPORT
    tcflush(fd, TCIOFLUSH);
	  
    if (tcgetattr(fd, &ti) < 0) {
        LOG_ERR("Can't get UART port setting");
        close(fd);
        return -1;
    }
    
    cfmakeraw(&ti);
    
    ti.c_cflag |= CLOCAL;
    ti.c_lflag = 0;
    
    ti.c_cflag &= ~CRTSCTS;
    ti.c_iflag &= ~(IXON | IXOFF | IXANY | 0x80000000);
    
    if (tcsetattr(fd, TCSANOW, &ti) < 0) {
        LOG_ERR("Can't set UART port setting");
        close(fd);
        return -1;
    }
    
    /* Set initial baudrate */
    cfsetospeed(&ti, B115200);
    cfsetispeed(&ti, B115200);
    
    if (tcsetattr(fd, TCSANOW, &ti) < 0) {
        LOG_ERR("Can't set UART port setting");
        close(fd);
        return -1;
    }
    
    tcflush(fd, TCIOFLUSH);
#endif

    return fd;
}

#ifdef MTK_COMBO_SUPPORT
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
#endif

#ifndef MTK_COMBO_SUPPORT
static int uart_speed(int s)
{
    switch (s) {
    case 9600:
         return B9600;
    case 19200:
         return B19200;
    case 38400:
         return B38400;
    case 57600:
         return B57600;
    case 115200:
         return B115200;
    case 230400:
         return B230400;
    case 460800:
         return B460800;
    case 500000:
         return B500000;
    case 576000:
         return B576000;
    case 921600:
         return B921600;
    case 1000000:
         return B1000000;
    case 1152000:
         return B1152000;
    case 1500000:
         return B1500000;
    case 2000000:
         return B2000000;
#ifdef B2500000
    case 2500000:
         return B2500000;
#endif
#ifdef B3000000
    case 3000000:
         return B3000000;
#endif
#ifdef B3500000
    case 3500000:
         return B3500000;
#endif
#ifdef B4000000
    case 4000000:
         return B4000000;
#endif
    default:
         return B57600;
    }
}

static int set_speed(int fd, struct termios *ti, int speed)
{
    int baudenum = uart_speed(speed);

    if ((baudenum == B57600) && (speed != 57600)) {
        LOG_DBG("non-standard baudrate: %d\n", speed);
        /* Non-standard baudrate can be supported? */
        return 0;
    } 
    else {
        LOG_DBG("standard baudrate: %d -> 0x%08x\n", speed, baudenum);
        cfsetospeed(ti, baudenum);
        cfsetispeed(ti, baudenum);
        return tcsetattr(fd, TCSANOW, ti);
    }
}

/* Host uart param configuration callback */
int setup_uart_param(
    unsigned long hComPort, 
    int iBaudrate, 
    int iFlowControl)
{
    struct termios ti;
    int fd;
    
    LOG_DBG("%d %d\n", iBaudrate, iFlowControl);
    
    fd = (int)hComPort;
    if (fd < 0) {
        LOG_ERR("Invalid serial port\n");
        return -1;
    }
    
    tcflush(fd, TCIOFLUSH);
    
    if (tcgetattr(fd, &ti) < 0) {
        LOG_ERR("Can't get port settings\n");
        return -1;
    }
    
    cfmakeraw(&ti);
    
    ti.c_cflag |= CLOCAL;
    ti.c_cflag &= ~CRTSCTS;
    ti.c_iflag &= ~(IXON | IXOFF | IXANY | 0x80000000);
    
    /* HW flow control */
    if (iFlowControl == 1){
        ti.c_cflag |= CRTSCTS;
    }
    else if (iFlowControl == 2){
        /* MTK SW flow control */
        //ti.c_iflag |= (IXON | IXOFF | IXANY);
        ti.c_iflag |= 0x80000000;
    }
    
    if (tcsetattr(fd, TCSANOW, &ti) < 0) {
        LOG_ERR("Can't set port settings\n");
        return -1;
    }
    
    /* Set baudrate */
    if (set_speed(fd, &ti, iBaudrate) < 0) {
        LOG_ERR("Can't set initial baud rate\n");
        return -1;
    }
    
    tcflush(fd, TCIOFLUSH);
    
    return 0;
}
#endif

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
#ifdef MTK_COMBO_SUPPORT		
		if(0x6620 == g_chipId){
			memcpy(&temp, &sDefaultCfg_6620, sizeof(struct btradio_conf_data));
		}
		else if(0x6628 == g_chipId){
			memcpy(&temp, &sDefaultCfg_6628, sizeof(struct btradio_conf_data));
		}
#else
		memcpy(&temp, &sDefaultCfg_6622, sizeof(struct btradio_conf_data));
#endif

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

int mtk(void)
{
    struct btradio_conf_data cfg;
    int speed = 0, iUseFlowControl = 0;
    SETUP_UART_PARAM uart_setup_callback = NULL;
	int fd = -1;
	
#ifdef MTK_COMBO_SUPPORT
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
#else
	memcpy(&cfg, &sDefaultCfg_6622, sizeof(struct btradio_conf_data));
#endif

    
    LOG_DBG("%s %d\n", __FILE__, __LINE__);
    
    fd = init_uart(CUST_BT_SERIAL_PORT);
    if (fd < 0){
        LOG_ERR("Can't initialize" CUST_BT_SERIAL_PORT);
        return -1;
    }

#ifndef MTK_COMBO_SUPPORT
    iUseFlowControl = FLOW_CTL_NONE;
    speed = CUST_BT_BAUD_RATE;
    uart_setup_callback = setup_uart_param;
#endif

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

#ifdef MTK_COMBO_SUPPORT
    LOG_DBG("[BD address %02x-%02x-%02x-%02x-%02x-%02x][voice %02x %02x][codec %02x %02x %02x %02x] \
            [radio %02x %02x %02x %02x %02x %02x][sleep %02x %02x %02x %02x %02x %02x %02x] \
            [feature %02x %02x][tx pwr offset %02x %02x %02x]\n",
            cfg.addr[0], cfg.addr[1], cfg.addr[2], cfg.addr[3], cfg.addr[4], cfg.addr[5],
            cfg.voice[0], cfg.voice[1], cfg.codec[0], cfg.codec[1], cfg.codec[2], cfg.codec[3],
            cfg.radio[0], cfg.radio[1], cfg.radio[2], cfg.radio[3], cfg.radio[4], cfg.radio[5],
            cfg.sleep[0], cfg.sleep[1], cfg.sleep[2], cfg.sleep[3], cfg.sleep[4], cfg.sleep[5], cfg.sleep[6],
            cfg.feature[0], cfg.feature[1],
            cfg.tx_pwr_offset[0], cfg.tx_pwr_offset[1], cfg.tx_pwr_offset[2]);
#else
    LOG_DBG("[BD address %02x-%02x-%02x-%02x-%02x-%02x][capId %02x][link key type %02x] \
            [unit key %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x] \
            [encryption %02x %02x %02x][pin code type %02x] \
            [voice %02x %02x][codec %02x %02x %02x %02x] \
            [radio %02x %02x %02x %02x %02x %02x][sleep %02x %02x %02x %02x %02x %02x %02x] \
            [feature %02x %02x][ECLK_SEL %02x]\n",
            cfg.addr[0], cfg.addr[1], cfg.addr[2], cfg.addr[3], cfg.addr[4], cfg.addr[5],
            cfg.capId[0], cfg.link_key_type[0],
            cfg.unit_key[0], cfg.unit_key[1], cfg.unit_key[2], cfg.unit_key[3], cfg.unit_key[4], cfg.unit_key[5], cfg.unit_key[6], cfg.unit_key[7],
            cfg.unit_key[8], cfg.unit_key[9], cfg.unit_key[10], cfg.unit_key[11], cfg.unit_key[12], cfg.unit_key[13], cfg.unit_key[14], cfg.unit_key[15],
            cfg.encryption[0], cfg.encryption[1], cfg.encryption[2], cfg.pin_code_type[0],
            cfg.voice[0], cfg.voice[1], cfg.codec[0], cfg.codec[1], cfg.codec[2], cfg.codec[3],
            cfg.radio[0], cfg.radio[1], cfg.radio[2], cfg.radio[3], cfg.radio[4], cfg.radio[5],
            cfg.sleep[0], cfg.sleep[1], cfg.sleep[2], cfg.sleep[3], cfg.sleep[4], cfg.sleep[5], cfg.sleep[6],
            cfg.feature[0], cfg.feature[1], cfg.ECLK_SEL[0]);
#endif


    if(BT_InitDevice(
        fd,
        (PBYTE)&cfg,
        speed,
        speed,
        iUseFlowControl,
        uart_setup_callback) == FALSE){

        LOG_ERR("Initialize BT device fails\n");        
        goto error;
    }

    LOG_WAN("mtk() success\n");
    return fd;

error:
    if (fd >= 0) close(fd);
    return -1;
}

int bt_restore(int fd)
{
    LOG_DBG("%s %d\n", __FILE__, __LINE__);
    
    BT_DeinitDevice(fd);
    close(fd);
    
    return 0; 
}

int write_comm_port(int fd, unsigned char *buf, unsigned long len)
{
    int nWritten = 0;
    int bytesToWrite = len;
    
    if (fd < 0){
        LOG_ERR("No available comm port\n");
        return -EIO;
    }
    
    while (bytesToWrite > 0){
        nWritten = write(fd, buf, bytesToWrite);
        if (nWritten < 0){
            if(errno == EINTR || errno == EAGAIN)
                break;
            else
                return -errno; // errno used for whole chip reset
        }
        bytesToWrite -= nWritten;
        buf += nWritten;
    }
    
    return (len - bytesToWrite);
}

int read_comm_port(int fd, unsigned char *buf, unsigned long len)
{
    int nRead = 0;
    int bytesToRead = len;
    
    if (fd < 0){
        LOG_ERR("No available comm port\n");
        return -EIO;
    }
    
    nRead = read(fd, buf, bytesToRead);
    if (nRead < 0){
        if(errno == EINTR || errno == EAGAIN)
            return 0;
        else
            return -errno; // errno used for whole chip reset
    }
	  
    return nRead;
}

int bt_send_data(int fd, unsigned char *buf, unsigned long len)
{
    int bytesWritten = 0;
    int bytesToWrite = len;
    
    // Send len bytes data in buffer
    while (bytesToWrite > 0){
        bytesWritten = write_comm_port(fd, buf, bytesToWrite);
        if (bytesWritten < 0){
            return -1;
        }
        bytesToWrite -= bytesWritten;
        buf += bytesWritten;
    }
    
    return 0;
}

int bt_receive_data(int fd, unsigned char *buf, unsigned long len)
{
    int bytesRead = 0;
    int bytesToRead = len;
    
    int ret = 0;
    struct timeval tv;
    fd_set readfd;
    
    tv.tv_sec = 5;  //SECOND
    tv.tv_usec = 0;   //USECOND
    FD_ZERO(&readfd);
    
    // Hope to receive len bytes
    while (bytesToRead > 0){
    
        FD_SET(fd, &readfd);
        ret = select(fd + 1, &readfd, NULL, NULL, &tv);
        
        if (ret > 0){
            bytesRead = read_comm_port(fd, buf, bytesToRead);
            if (bytesRead < 0){
                return -1;
            }
            else{
                bytesToRead -= bytesRead;
                buf += bytesRead;
            }
        }
        else if (ret == 0){
            LOG_ERR("Read comm port timeout = 5000ms!\n");
            return -1;
        }
        else if ((ret == -1) && (errno == EINTR)){
            LOG_ERR("select error EINTR\n");
        }
        else{
            LOG_ERR("select error %s(%d)!\n", strerror(errno), errno);
            return -1;
        }
    }
    
    return 0;
}
