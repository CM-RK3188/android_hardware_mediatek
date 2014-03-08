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
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dlfcn.h>
#include <pthread.h>

#include "bt_em.h"


typedef unsigned long DWORD;
typedef unsigned char UCHAR;

#define TRUE      1
#define FALSE     0

/* LOG_TAG must be defined before log.h */
#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG         "BT_EM "
#include <cutils/log.h>

#define BT_EM_DEBUG     1
#define ERR(f, ...)     ALOGE("%s: " f, __func__, ##__VA_ARGS__)
#define WAN(f, ...)     ALOGW("%s: " f, __func__, ##__VA_ARGS__)
#if BT_EM_DEBUG
#define DBG(f, ...)     ALOGD("%s: " f, __func__, ##__VA_ARGS__)
#define TRC(f)          ALOGW("%s #%d", __func__, __LINE__)
#else
#define DBG(...)        ((void)0)
#define TRC(f)          ((void)0)
#endif

#ifndef MTK_COMBO_SUPPORT
#define BTHWCTL_DEV_NAME            "/dev/bthwctl"
#define BTHWCTL_IOC_MAGIC           0xf6
#define BTHWCTL_IOCTL_SET_POWER     _IOWR(BTHWCTL_IOC_MAGIC, 0, uint32_t)
#endif

/**************************************************************************
 *                 G L O B A L   V A R I A B L E S                        *
***************************************************************************/

static int bt_fd = -1;

/* Used to read serial port */
static pthread_t rxThread;
static pthread_t inqThread;
static BOOL bKillThread = FALSE;

// mtk bt library
static void *glib_handle = NULL;
typedef int (*INIT)(void);
typedef int (*UNINIT)(int fd);
typedef int (*WRITE)(int fd, unsigned char *buf, unsigned long len);
typedef int (*READ)(int fd, unsigned char *buf, unsigned long len);

INIT    mtk = NULL;
UNINIT  bt_restore = NULL;
WRITE   bt_send_data = NULL;
READ    bt_receive_data = NULL;

/**************************************************************************
  *                         F U N C T I O N S                             *
***************************************************************************/

static BOOL BT_DisableSleepMode(void);

#ifndef MTK_COMBO_SUPPORT
static int bt_set_power(int on) 
{
    int sz;
    int fd = -1;
    int ret = -1;
    const uint32_t buf = (on ? 1 : 0);
    
    TRC();
    
    fd = open(BTHWCTL_DEV_NAME, O_RDWR);
    if (fd < 0) {
        ERR("Open %s to set BT power fails: %s(%d)", BTHWCTL_DEV_NAME,
            strerror(errno), errno);
        goto out;
    }
    
    ret = ioctl(fd, BTHWCTL_IOCTL_SET_POWER, &buf);
    if(ret < 0) {
        ERR("Set BT power %d fails: %s(%d)\n", buf, 
            strerror(errno), errno);
        goto out;
    }

out:
    if (fd >= 0) close(fd);
    return ret;
}
#endif

BOOL EM_BT_init(void)
{
    const char *errstr;
    
    TRC();

#ifndef MTK_COMBO_SUPPORT
    /* In case BT is powered on before test */
    bt_set_power(0);
    
    if(bt_set_power(1) < 0) {
        ERR("BT power on fails\n");
        return -1;
    }
#endif

    glib_handle = dlopen("libbluetooth_mtk_pure.so", RTLD_LAZY);
    if (!glib_handle){
        ERR("%s\n", dlerror());
        goto error;
    }
    
    dlerror(); /* Clear any existing error */
    
    mtk = dlsym(glib_handle, "mtk");
    bt_restore = dlsym(glib_handle, "bt_restore");
    bt_send_data = dlsym(glib_handle, "bt_send_data");
    bt_receive_data = dlsym(glib_handle, "bt_receive_data");
    
    if ((errstr = dlerror()) != NULL){
        ERR("Can't find function symbols %s\n", errstr);
        goto error;
    }
    
    bt_fd = mtk();
    if (bt_fd < 0)
        goto error;

    DBG("BT is enabled success\n");

#ifndef MTK_COMBO_SUPPORT
    /* BT EM driver DONOT handle sleep mode and EINT,
       so disable Host and Controller sleep in Engineer Mode (HW test)
       on discrete BT chip;
       on combo chip, THIS IS NO NEED.
       */
    BT_DisableSleepMode();
#endif

    return TRUE;

error:
    if (glib_handle){
        dlclose(glib_handle);
        glib_handle = NULL;
    }

#ifndef MTK_COMBO_SUPPORT
    bt_set_power(0);
#endif

    return FALSE;
}

void EM_BT_deinit(void)
{
    TRC();
    
    if (!glib_handle){
        ERR("mtk bt library is unloaded!\n");
    }
    else{
        if (bt_fd < 0){
            ERR("bt driver fd is invalid!\n");
        }
        else{
            bt_restore(bt_fd);
            bt_fd = -1;
        }
        dlclose(glib_handle);
        glib_handle = NULL;
    }

#ifndef MTK_COMBO_SUPPORT
    bt_set_power(0); /* shutdown BT */
#endif

    return;
}

BOOL EM_BT_reset(void)
{
    int i;
    UCHAR HCI_RESET[] = {0x01, 0x03, 0x0C, 0x0};
    UCHAR pAckEvent[7];
    // Expected to receive event
    UCHAR ucEvent[] = {0x04, 0x0E, 0x04, 0x01, 0x03, 0x0C, 0x00};
    
    TRC();
    
    if (!glib_handle){
        ERR("mtk bt library is unloaded!\n");
        return FALSE;
    }
    if (bt_fd < 0){
        ERR("bt driver fd is invalid!\n");
        return FALSE;
    }
    
    if(bt_send_data(bt_fd, HCI_RESET, sizeof(HCI_RESET)) < 0){
        ERR("Send HCI reset command fails errno %d\n", errno);
        return FALSE;
    }
    
    DBG("write:\n");
    for (i = 0; i < sizeof(HCI_RESET); i++) {
        DBG("%02x\n", HCI_RESET[i]);
    }
    
    if(bt_receive_data(bt_fd, pAckEvent, sizeof(pAckEvent)) < 0){
        ERR("Receive command complete event fails errno %d\n", errno);
        return FALSE;
    }
    
    DBG("read:\n");
    for (i = 0; i < sizeof(pAckEvent); i++) {
        DBG("%02x\n", pAckEvent[i]);
    }
    
    if(memcmp(pAckEvent, ucEvent, sizeof(ucEvent))){
        ERR("Receive unexpected event\n");
        return FALSE;
    }
    
    return TRUE;
}

BOOL EM_BT_TxOnlyTest_start(
    UCHAR tx_pattern,
    UCHAR channel,
    int freq,
    UCHAR packet_type,
    int packet_len
    )
{
    int i;
    UCHAR HCI_VS_TX_TEST[] = {0x01, 0x0D, 0xFC, 0x17, 0x00, 
                              0x00, 
                              0x00, //Tx pattern
                              0x00, //Channel (Single frequency or 79 channels hopping)
                              0x00, //Tx frequency
                              0x00, 
                              0x00, 0x01, 
                              0x00, //Packet type
                              0x00, 0x00, //Packet length
                              0x02, 0x00, 0x01, 0x00, 
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                              0x00, 0x00};
    UCHAR pAckEvent1[14];
    UCHAR pAckEvent2[7];
    // Expected to receive event
    UCHAR ucEvent1[] = {0x04, 0x03, 0x0B, 0x00}; //Connection complete
    UCHAR ucEvent2[] = {0x04, 0x0E, 0x04, 0x01, 0x0D, 0xFC, 0x00}; //Command complete
    
    TRC();
    
    if (!glib_handle){
        ERR("mtk bt library is unloaded!\n");
        return FALSE;
    }
    if (bt_fd < 0){
        ERR("bt driver fd is invalid!\n");
        return FALSE;
    }
    
    // Prepare Tx test command
    HCI_VS_TX_TEST[6] = tx_pattern;
    HCI_VS_TX_TEST[7] = channel;
    HCI_VS_TX_TEST[8] = (UCHAR)freq;
    HCI_VS_TX_TEST[12] = packet_type;
    HCI_VS_TX_TEST[13] = (UCHAR)(packet_len & 0xFF);
    HCI_VS_TX_TEST[14] = (UCHAR)((packet_len >> 8) & 0xFF);
    
    if(bt_send_data(bt_fd, HCI_VS_TX_TEST, sizeof(HCI_VS_TX_TEST)) < 0){
        ERR("Send tx test command fails errno %d\n", errno);
        return FALSE;
    }
    
    DBG("write:\n");
    for (i = 0; i < sizeof(HCI_VS_TX_TEST); i++) {
        DBG("%02x\n", HCI_VS_TX_TEST[i]);
    }
    
    if(tx_pattern != 0x0A){
        // Receive connection complete event
        if(bt_receive_data(bt_fd, pAckEvent1, sizeof(pAckEvent1)) < 0){
            ERR("Receive connection complete event fails errno %d\n", errno);
            return FALSE;
        }
        
        DBG("read:\n");
        for (i = 0; i < sizeof(pAckEvent1); i++) {
            DBG("%02x\n", pAckEvent1[i]);
        }
        
        if(memcmp(pAckEvent1, ucEvent1, sizeof(ucEvent1))){
            ERR("Receive unexpected event\n");
            return FALSE;
        }
    }
    
    // Receive command complete event
    if(bt_receive_data(bt_fd, pAckEvent2, sizeof(pAckEvent2)) < 0){
        ERR("Receive command complete event fails errno %d\n", errno);
        return FALSE;
    }
    
    DBG("read:\n");
    for (i = 0; i < sizeof(pAckEvent2); i++) {
        DBG("%02x\n", pAckEvent2[i]);
    }
    
    if(memcmp(pAckEvent2, ucEvent2, sizeof(ucEvent2))){
        ERR("Receive unexpected event\n");
        return FALSE;
    }
    
    return TRUE;
}

BOOL EM_BT_TxOnlyTest_end(void)
{    
    //return EM_BT_reset();
    return TRUE;
}

BOOL EM_BT_NonSignalRx_start(
    UCHAR rx_pattern,
    int freq,
    UCHAR packet_type,
    DWORD tester_addr
    )
{
    int i;
    UCHAR HCI_VS_NSR_START[] = {0x01, 0x0D, 0xFC, 0x17, 0x00, 
                                0x00, //Rx pattern
                                0x0B, //Rx test mode
                                0x00, 
                                0x00, 
                                0x00, //Rx frequency
                                0x00, 0x01, 
                                0x00, //Packet type
                                0x00, 0x00, 
                                0x02, 0x00, 0x01, 0x00, 
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //Tester address
                                0x00, 0x00};
    UCHAR pAckEvent1[14];
    UCHAR pAckEvent2[7];
    // Expected to receive event
    UCHAR ucEvent1[] = {0x04, 0x03, 0x0B, 0x00}; //Connection complete
    UCHAR ucEvent2[] = {0x04, 0x0E, 0x04, 0x01, 0x0D, 0xFC, 0x00}; //Command complete
    
    TRC();
    
    if (!glib_handle){
        ERR("mtk bt library is unloaded!\n");
        return FALSE;
    }
    if (bt_fd < 0){
        ERR("bt driver fd is invalid!\n");
        return FALSE;
    }
    
    // Prepare Non-Signal-Rx test start command
    HCI_VS_NSR_START[5] = rx_pattern;
    HCI_VS_NSR_START[9] = (UCHAR)freq;
    HCI_VS_NSR_START[12] = packet_type;
    HCI_VS_NSR_START[21] = (UCHAR)((tester_addr >> 24) & 0xFF);
    HCI_VS_NSR_START[22] = (UCHAR)((tester_addr >> 16) & 0xFF);
    HCI_VS_NSR_START[23] = (UCHAR)((tester_addr >> 8) & 0xFF);
    HCI_VS_NSR_START[24] = (UCHAR)(tester_addr & 0xFF);
    
    if(bt_send_data(bt_fd, HCI_VS_NSR_START, sizeof(HCI_VS_NSR_START)) < 0){
        ERR("Send Non-Signal-Rx start command fails errno %d\n", errno);
        return FALSE;
    }
    
    DBG("write:\n");
    for (i = 0; i < sizeof(HCI_VS_NSR_START); i++) {
        DBG("%02x\n", HCI_VS_NSR_START[i]);
    }
    
    // Receive connection complete event
    if(bt_receive_data(bt_fd, pAckEvent1, sizeof(pAckEvent1)) < 0){
        ERR("Receive connection complete event fails errno %d\n", errno);
        return FALSE;
    }
    
    DBG("read:\n");
    for (i = 0; i < sizeof(pAckEvent1); i++) {
        DBG("%02x\n", pAckEvent1[i]);
    }
    
    if(memcmp(pAckEvent1, ucEvent1, sizeof(ucEvent1))){
        ERR("Receive unexpected event\n");
        return FALSE;
    }
    
    // Receive command complete event
    if(bt_receive_data(bt_fd, pAckEvent2, sizeof(pAckEvent2)) < 0){
        ERR("Receive command complete event fails errno %d\n", errno);
        return FALSE;
    }
    
    DBG("read:\n");
    for (i = 0; i < sizeof(pAckEvent2); i++) {
        DBG("%02x\n", pAckEvent2[i]);
    }
    
    if(memcmp(pAckEvent2, ucEvent2, sizeof(ucEvent2))){
        ERR("Receive unexpected event\n");
        return FALSE;
    }
    
    return TRUE;
}

BOOL EM_BT_NonSignalRx_end(
    DWORD *pRxPktCount,
    float *pPktErrRate,
    DWORD *pRxByteCount,
    float *pBitErrRate
    )
{
    int i;
    UCHAR HCI_VS_NSR_END[] = {0x01, 0x0D, 0xFC, 0x17, 0x00, 
                              0x00,
                              0xFF, //test end
                              0x00, 
                              0x00, 
                              0x00, 
                              0x00, 0x01, 
                              0x00, 
                              0x00, 0x00, 
                              0x02, 0x00, 0x01, 0x00, 
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                              0x00, 0x00};
    UCHAR pAckEvent[23];
    // Expected to receive event
    UCHAR ucEvent[] = {0x04, 0x0E, 0x14, 0x01, 0x0D, 0xFC, 0x00}; //Non-Signal-Rx complete
    
    TRC();
    
    if (!glib_handle){
        ERR("mtk bt library is unloaded!\n");
        return FALSE;
    }
    if (bt_fd < 0){
        ERR("bt driver fd is invalid!\n");
        return FALSE;
    }
    
    // Non-Signal-Rx test end command
    if(bt_send_data(bt_fd, HCI_VS_NSR_END, sizeof(HCI_VS_NSR_END)) < 0){
        ERR("Send Non-Signal-Rx end command fails errno %d\n", errno);
        return FALSE;
    }
    
    DBG("write:\n");
    for (i = 0; i < sizeof(HCI_VS_NSR_END); i++) {
        DBG("%02x\n", HCI_VS_NSR_END[i]);
    }
    
    // Receive Non-Signal-Rx complete event
    if(bt_receive_data(bt_fd, pAckEvent, sizeof(pAckEvent)) < 0){
        ERR("Receive Non-Signal-Rx complete event fails errno %d\n", errno);
        return FALSE;
    }
    
    DBG("read:\n");
    for (i = 0; i < sizeof(pAckEvent); i++) {
        DBG("%02x\n", pAckEvent[i]);
    }
    
    if(memcmp(pAckEvent, ucEvent, sizeof(ucEvent))){
        ERR("Receive unexpected event\n");
        return FALSE;
    }
    else{
        *pRxPktCount = *((unsigned long*)&pAckEvent[7]);
        *pPktErrRate = (float)(*((unsigned long*)&pAckEvent[11]))/1000000;
        *pRxByteCount = *((unsigned long*)&pAckEvent[15]);
        *pBitErrRate = (float)(*((unsigned long*)&pAckEvent[19]))/1000000;
    }
    
    return TRUE;
}


static void *BT_Rx_Thread(void *ptr);

BOOL EM_BT_TestMode_enter(int power)
{
    int i;
    
    UCHAR HCI_VS_SET_RADIO[] = 
        {0x01, 0x79, 0xFC, 0x06, 0x07, 0x80, 0x00, 0x06, 0x03, 0x07};
    UCHAR HCI_TEST_MODE_ENABLE[] = 
        {0x01, 0x03, 0x18, 0x00};
    UCHAR HCI_WRITE_SCAN_ENABLE[] = 
        {0x01, 0x1A, 0x0C, 0x01, 0x03};
    UCHAR HCI_SET_EVENT_FILTER[] = 
        {0x01, 0x05, 0x0C, 0x03, 0x02, 0x00, 0x02};
    
    UCHAR pAckEvent[7];
    // Expected to receive event
    UCHAR ucEvent1[] = {0x04, 0x0E, 0x04, 0x01, 0x79, 0xFC, 0x00};
    UCHAR ucEvent2[] = {0x04, 0x0E, 0x04, 0x01, 0x03, 0x18, 0x00};
    UCHAR ucEvent3[] = {0x04, 0x0E, 0x04, 0x01, 0x1A, 0x0C, 0x00};
    UCHAR ucEvent4[] = {0x04, 0x0E, 0x04, 0x01, 0x05, 0x0C, 0x00};
    
    TRC();
    
    if (!glib_handle){
        ERR("mtk bt library is unloaded!\n");
        return FALSE;
    }
    if (bt_fd < 0){
        ERR("bt driver fd is invalid!\n");
        return FALSE;
    }
    
    ////////// First command: Set Tx power
    
    if (power >= 0 && power <= 7){
        HCI_VS_SET_RADIO[4] = (UCHAR)power;
        HCI_VS_SET_RADIO[9] = (UCHAR)power;
    }
    
    if(bt_send_data(bt_fd, HCI_VS_SET_RADIO, sizeof(HCI_VS_SET_RADIO)) < 0){
        ERR("Send set Tx power command fails errno %d\n", errno);
        return FALSE;
    }
    
    DBG("write:\n");
    for (i = 0; i < sizeof(HCI_VS_SET_RADIO); i++) {
        DBG("%02x\n", HCI_VS_SET_RADIO[i]);
    }
    
    if(bt_receive_data(bt_fd, pAckEvent, sizeof(pAckEvent)) < 0){
        ERR("Receive command complete event fails errno %d\n", errno);
        return FALSE;
    }
    
    DBG("read:\n");
    for (i = 0; i < sizeof(pAckEvent); i++) {
        DBG("%02x\n", pAckEvent[i]);
    }
    
    if(memcmp(pAckEvent, ucEvent1, sizeof(ucEvent1))){
        ERR("Receive unexpected event\n");
        return FALSE;
    }
    
    ////////// Second command: HCI_Enable_Device_Under_Test_Mode
    
    if(bt_send_data(bt_fd, HCI_TEST_MODE_ENABLE, sizeof(HCI_TEST_MODE_ENABLE)) < 0){
        ERR("Send test mode enable command fails errno %d\n", errno);
        return FALSE;
    }
    
    DBG("write:\n");
    for (i = 0; i < sizeof(HCI_TEST_MODE_ENABLE); i++) {
        DBG("%02x\n", HCI_TEST_MODE_ENABLE[i]);
    }
    
    if(bt_receive_data(bt_fd, pAckEvent, sizeof(pAckEvent)) < 0){
        ERR("Receive command complete event fails errno %d\n", errno);
        return FALSE;
    }
    
    DBG("read:\n");
    for (i = 0; i < sizeof(pAckEvent); i++) {
        DBG("%02x\n", pAckEvent[i]);
    }
    
    if(memcmp(pAckEvent, ucEvent2, sizeof(ucEvent2))){
        ERR("Receive unexpected event\n");
        return FALSE;
    }
    
    ////////// Third command: HCI_Write_Scan_Enable
    
    if(bt_send_data(bt_fd, HCI_WRITE_SCAN_ENABLE, sizeof(HCI_WRITE_SCAN_ENABLE)) < 0){
        ERR("Send write scan enable command fails errno %d\n", errno);
        return FALSE;
    }
    
    DBG("write:\n");
    for (i = 0; i < sizeof(HCI_WRITE_SCAN_ENABLE); i++) {
        DBG("%02x\n", HCI_WRITE_SCAN_ENABLE[i]);
    }
    
    if(bt_receive_data(bt_fd, pAckEvent, sizeof(pAckEvent)) < 0){
        ERR("Receive command complete event fails errno %d\n", errno);
        return FALSE;
    }
    
    DBG("read:\n");
    for (i = 0; i < sizeof(pAckEvent); i++) {
        DBG("%02x\n", pAckEvent[i]);
    }
    
    if(memcmp(pAckEvent, ucEvent3, sizeof(ucEvent3))){
        ERR("Receive unexpected event\n");
        return FALSE;
    }
    
    ////////// Fourth command: HCI_Set_Event_Filter
    
    if(bt_send_data(bt_fd, HCI_SET_EVENT_FILTER, sizeof(HCI_SET_EVENT_FILTER)) < 0){
        ERR("Send set event filter command fails errno %d\n", errno);
        return FALSE;
    }
    
    DBG("write:\n");
    for (i = 0; i < sizeof(HCI_SET_EVENT_FILTER); i++) {
        DBG("%02x\n", HCI_SET_EVENT_FILTER[i]);
    }
    
    if(bt_receive_data(bt_fd, pAckEvent, sizeof(pAckEvent)) < 0){
        ERR("Receive command complete event fails errno %d\n", errno);
        return FALSE;
    }
    
    DBG("read:\n");
    for (i = 0; i < sizeof(pAckEvent); i++) {
        DBG("%02x\n", pAckEvent[i]);
    }
    
    if(memcmp(pAckEvent, ucEvent4, sizeof(ucEvent4))){
        ERR("Receive unexpected event\n");
        return FALSE;
    }
    
    //
    // Create Rx thread to receive event from Controller during test
    //
    bKillThread = FALSE;
    pthread_create(&rxThread, NULL, BT_Rx_Thread, NULL);
    
    return TRUE;
}

BOOL EM_BT_TestMode_exit(void)
{
    bKillThread = TRUE;
    /* Wait until thread exist */
    pthread_join(rxThread, NULL);
    
    //return EM_BT_reset();
    return TRUE;
}

static void *BT_Rx_Thread(void *ptr)
{
    UCHAR ucRxBuf[512];
    UCHAR ucHeader = 0;
    int iLen = 0, pkt_len = 0;
    int i;
	  
    TRC();
    
    while(!bKillThread){
        
        if (!glib_handle){
            ERR("mtk bt library is unloaded!\n");
            goto CleanUp;
        }
        if (bt_fd < 0){
            ERR("bt driver fd is invalid!\n");
            goto CleanUp;
        }
        
        if(bt_receive_data(bt_fd, &ucHeader, sizeof(ucHeader)) < 0){
            ERR("Zero byte read\n");
            continue;
        }
        
        memset(ucRxBuf, 0, sizeof(ucRxBuf));
        ucRxBuf[0] = ucHeader;
        iLen = 1;
        
        switch (ucHeader)
        {
            case 0x04:
                DBG("Receive HCI event\n");
                if(bt_receive_data(bt_fd, &ucRxBuf[1], 2) < 0){
                    ERR("Read event header fails\n");
                    goto CleanUp;
                }
                
                iLen += 2;
                pkt_len = (int)ucRxBuf[2];
                if((iLen + pkt_len) > sizeof(ucRxBuf)){
                    ERR("Read buffer overflow! packet len %d\n", iLen + pkt_len);
                    goto CleanUp;
                }
                
                if(bt_receive_data(bt_fd, &ucRxBuf[3], pkt_len) < 0){
                    ERR("Read event param fails\n");
                    goto CleanUp;
                }
                
                iLen += pkt_len;
                break;
                
            case 0x02:
                DBG("Receive ACL data\n");
                if(bt_receive_data(bt_fd, &ucRxBuf[1], 4) < 0){
                    ERR("Read ACL header fails\n");
                    goto CleanUp;
                }
                
                iLen += 4;
                pkt_len = (((int)ucRxBuf[4]) << 8);
                pkt_len += ucRxBuf[3];//little endian
                if((iLen + pkt_len) > sizeof(ucRxBuf)){
                    ERR("Read buffer overflow! packet len %d\n", iLen + pkt_len);
                    goto CleanUp;
                }
                
                if(bt_receive_data(bt_fd, &ucRxBuf[5], pkt_len) < 0){
                    ERR("Read ACL data fails\n");
                    goto CleanUp;
                }
                
                iLen += pkt_len;
                break;
                
            case 0x03:
                DBG("Receive SCO data\n");
                if(bt_receive_data(bt_fd, &ucRxBuf[1], 3) < 0){
                    ERR("Read SCO header fails\n");
                    goto CleanUp;
                }
                
                iLen += 3;
                pkt_len = (int)ucRxBuf[3];
                if((iLen + pkt_len) > sizeof(ucRxBuf)){
                    ERR("Read buffer overflow! packet len %d\n", iLen + pkt_len);
                    goto CleanUp;
                }
                
                if(bt_receive_data(bt_fd, &ucRxBuf[4], pkt_len) < 0){
                    ERR("Read SCO data fails\n");
                    goto CleanUp;
                }
                
                iLen += pkt_len;
                break;
                
            default:
                ERR("Unexpected BT packet header %02x\n", ucHeader);
                goto CleanUp;
        }
        
        // Dump rx packet
        DBG("read:\n");
        for (i = 0; i < iLen; i++) {
            DBG("%02x\n", ucRxBuf[i]);
        }
    }

CleanUp:
    return 0;
}

static BOOL BT_Inquiry(void)
{
    int i;
    
    UCHAR HCI_INQUIRY[] = {0x01, 0x01, 0x04, 0x05, 0x33, 0x8B, 0x9E, 0x05, 0x0A};
    UCHAR pAckEvent[7];
    // Expected to receive status event
    UCHAR ucEvent[] = {0x04, 0x0F, 0x04, 0x00, 0x01, 0x01, 0x04};
    
    TRC();
    
    if (!glib_handle){
        ERR("mtk bt library is unloaded!\n");
        return FALSE;
    }
    if (bt_fd < 0){
        ERR("bt driver fd is invalid!\n");
        return FALSE;
    }
    
    if(bt_send_data(bt_fd, HCI_INQUIRY, sizeof(HCI_INQUIRY)) < 0){
        ERR("Send inquiry command fails errno %d\n", errno);
        return FALSE;
    }
    
    DBG("write:\n");
    for (i = 0; i < sizeof(HCI_INQUIRY); i++) {
        DBG("%02x\n", HCI_INQUIRY[i]);
    }
    
    if(bt_receive_data(bt_fd, pAckEvent, sizeof(pAckEvent)) < 0){
        ERR("Receive event fails errno %d\n", errno);
        return FALSE;
    }
    
    DBG("read:\n");
    for (i = 0; i < sizeof(pAckEvent); i++) {
        DBG("%02x\n", pAckEvent[i]);
    }
    
    if(memcmp(pAckEvent, ucEvent, sizeof(ucEvent))){
        ERR("Receive unexpected event\n");
        return FALSE;
    }
    
    return TRUE;
}

static void *BT_Inquiry_Thread(void *ptr)
{
    UCHAR ucRxBuf[256];
    UCHAR ucHeader = 0;
    int iLen = 0, pkt_len = 0;
    int i;
    FUNC_CB info_cb = (FUNC_CB)ptr;
    UCHAR btaddr[6];
    char  str[30];
	  
    TRC();
    
    while(!bKillThread){
        
        if (!glib_handle){
            ERR("mtk bt library is unloaded!\n");
            goto CleanUp;
        }
        if (bt_fd < 0){
            ERR("bt driver fd is invalid!\n");
            goto CleanUp;
        }
        
        if(bt_receive_data(bt_fd, &ucHeader, sizeof(ucHeader)) < 0){
            ERR("Zero byte read\n");
            continue;
        }
        
        memset(ucRxBuf, 0, sizeof(ucRxBuf));
        ucRxBuf[0] = ucHeader;
        iLen = 1;
        
        switch (ucHeader)
        {
            case 0x04:
                DBG("Receive HCI event\n");
                if(bt_receive_data(bt_fd, &ucRxBuf[1], 2) < 0){
                    ERR("Read event header fails\n");
                    goto CleanUp;
                }
                
                iLen += 2;
                pkt_len = (int)ucRxBuf[2];                
                if(bt_receive_data(bt_fd, &ucRxBuf[3], pkt_len) < 0){
                    ERR("Read event param fails\n");
                    goto CleanUp;
                }
                
                iLen += pkt_len;
                
                // Dump rx packet
                DBG("read:\n");
                for (i = 0; i < iLen; i++) {
                    DBG("%02x\n", ucRxBuf[i]);
                }
                
                if(ucRxBuf[1] == 0x02){
                    /* Inquiry result event */
                    if(pkt_len != 0x0F){
                        ERR("Unexpected inquiry result len %d", pkt_len);
                        goto CleanUp;
                    }
                    
                    /* Retrieve BD addr */
                    btaddr[0] = ucRxBuf[9];
                    btaddr[1] = ucRxBuf[8];
                    btaddr[2] = ucRxBuf[7];
                    btaddr[3] = ucRxBuf[6];
                    btaddr[4] = ucRxBuf[5];
                    btaddr[5] = ucRxBuf[4];
                    
                    /* Inquiry result callback */
                    memset(str, 0, sizeof(str));
                    sprintf(str, "    %02x:%02x:%02x:%02x:%02x:%02x\n",
                        btaddr[0], btaddr[1], btaddr[2], btaddr[3], btaddr[4], btaddr[5]);
                    info_cb(str);
                }
                else if(ucRxBuf[1] == 0x01){
                    /* Inquiry complete event */
                    if(pkt_len != 0x01){
                        ERR("Unexpected inquiry complete len %d", pkt_len);
                        goto CleanUp;
                    }
                    
                    if(ucRxBuf[3] != 0x00){
                        ERR("Unexpected inquiry complete status %d", (int)ucRxBuf[3]);
                        goto CleanUp;
                    }
                    
                    bKillThread = TRUE;
                    info_cb("---Inquiry completed---\n");
                }
                else{
                    /* simply ignore it? */
                    DBG("Unexpected event 0x%2x%d\n", ucRxBuf[1]);
                }
                break;
                
            default:
                ERR("Unexpected BT packet header %02x\n", ucHeader);
                goto CleanUp;
        }
    }

CleanUp:
    return 0;
}

BOOL EM_BT_Inquiry(FUNC_CB info_cb)
{
    if (BT_Inquiry()){
        //
        // Create thread to receive events during inquiry procedure
        //
        bKillThread = FALSE;
        pthread_create(&inqThread, NULL, BT_Inquiry_Thread, (void*)info_cb);
        return TRUE;
    }
    else{
        return FALSE;
    }
}


static BOOL BT_DisableSleepMode(void)
{
    UCHAR HCI_VS_SLEEP[] = 
        {0x01, 0x7A, 0xFC, 0x07, 0x00, 0x40, 0x1F, 0x00, 0x00, 0x00, 0x04};
    UCHAR pAckEvent[7];
    UCHAR ucEvent[] = {0x04, 0x0E, 0x04, 0x01, 0x7A, 0xFC, 0x00};
    
    TRC();
    
    if (!glib_handle){
        ERR("mtk bt library is unloaded!\n");
        return FALSE;
    }
    if (bt_fd < 0){
        ERR("bt driver fd is invalid!\n");
        return FALSE;
    }
    
    if(bt_send_data(bt_fd, HCI_VS_SLEEP, sizeof(HCI_VS_SLEEP)) < 0){
        ERR("Send disable sleep mode command fails errno %d\n", errno);
        return FALSE;
    }
    
    if(bt_receive_data(bt_fd, pAckEvent, sizeof(pAckEvent)) < 0){
        ERR("Receive event fails errno %d\n", errno);
        return FALSE;
    }
    
    if(memcmp(pAckEvent, ucEvent, sizeof(ucEvent))){
        ERR("Receive unexpected event\n");
        return FALSE;
    }
    
    return TRUE;
}
