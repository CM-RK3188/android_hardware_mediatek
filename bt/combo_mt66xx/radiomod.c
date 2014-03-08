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
#include <fcntl.h>
#include <pthread.h>
#include <time.h>

#include "bt_hci_bdroid.h"
#include "bt_vendor_lib.h"
#include "utils.h"
#include "bt_mtk.h"


/**************************************************************************
 *                 G L O B A L   V A R I A B L E S                        *
***************************************************************************/

sBTInitVar_t btinit[1];
sBTInitCB_t  btinit_ctrl;

static uint16_t wOpCode;
static HciCmdStatus_t cmd_status;
extern bt_vendor_callbacks_t *bt_vnd_cbacks;

extern BOOL fInternalCfg;
static BOOL fgetEFUSE = FALSE;
extern int g_chipId;


/**************************************************************************
 *             F U N C T I O N   D E C L A R A T I O N S                  *
***************************************************************************/

static BOOL GORMcmd_HCC_Get_Local_BD_Addr(HC_BT_HDR *);
static void GORMevt_HCE_Get_Local_BD_Addr(void *);
static BOOL GORMcmd_HCC_Set_Local_BD_Addr(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_LinkKeyType(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_UnitKey(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_Encryption(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_PinCodeType(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_Voice(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_PCM(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_Radio(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_TX_Power_Offset(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_Sleep_Timeout(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_BT_FTR(HC_BT_HDR *);


static BOOL GORMcmd_HCC_Set_OSC_Info(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_LPO_Info(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_PTA(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_BLEPTA(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_RF_Desense(HC_BT_HDR *);
static BOOL GORMcmd_HCC_RESET(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_Internal_PTA_1(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_Internal_PTA_2(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_Sleep_Control_Reg(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_SLP_LDOD_VCTRL_Reg(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_RF_Reg_100(HC_BT_HDR *);


static void GORMevt_HCE_Common_Complete(void *);
static void GetRandomValue(UCHAR *);


//===================================================================
// Combo Chip MT6620
sHciScriptElement_t bt_init_preload_script_6620[] = 
{
    {  GORMcmd_HCC_Get_Local_BD_Addr       }, /*0x1009*/
    {  GORMcmd_HCC_Set_Local_BD_Addr       }, /*0xFC1A*/
    {  GORMcmd_HCC_Set_LinkKeyType         }, /*0xFC1B*/
    {  GORMcmd_HCC_Set_UnitKey             }, /*0xFC75*/
    {  GORMcmd_HCC_Set_Encryption          }, /*0xFC76*/
    {  GORMcmd_HCC_Set_PinCodeType         }, /*0x0C0A*/
    {  GORMcmd_HCC_Set_Radio               }, /*0xFC79*/
    {  GORMcmd_HCC_Set_TX_Power_Offset     }, /*0xFC93*/
    {  GORMcmd_HCC_Set_Sleep_Timeout       }, /*0xFC7A*/
    {  GORMcmd_HCC_Set_BT_FTR              }, /*0xFC7D*/
    {  GORMcmd_HCC_Set_OSC_Info            }, /*0xFC7B*/
    {  GORMcmd_HCC_Set_LPO_Info            }, /*0xFC7C*/
    {  GORMcmd_HCC_Set_PTA                 }, /*0xFC74*/
    {  GORMcmd_HCC_Set_BLEPTA              }, /*0xFCFC*/
    {  GORMcmd_HCC_Set_RF_Desense          }, /*0xFC20*/
    {  GORMcmd_HCC_RESET                   }, /*0x0C03*/
    {  GORMcmd_HCC_Set_Internal_PTA_1      }, /*0xFCFB*/
    {  GORMcmd_HCC_Set_Internal_PTA_2      }, /*0xFCFB*/
    {  GORMcmd_HCC_Set_Sleep_Control_Reg   }, /*0xFCD0*/
    {  GORMcmd_HCC_Set_SLP_LDOD_VCTRL_Reg  }, /*0xFCD0*/
    {  GORMcmd_HCC_Set_RF_Reg_100          }, /*0xFCB0*/
    {  0  },
};
//===================================================================
// Combo Chip  MT6628
sHciScriptElement_t bt_init_preload_script_6628[] = 
{
    {  GORMcmd_HCC_Get_Local_BD_Addr       }, /*0x1009*/
    {  GORMcmd_HCC_Set_Local_BD_Addr       }, /*0xFC1A*/
    {  GORMcmd_HCC_Set_LinkKeyType         }, /*0xFC1B*/
    {  GORMcmd_HCC_Set_UnitKey             }, /*0xFC75*/
    {  GORMcmd_HCC_Set_Encryption          }, /*0xFC76*/
    {  GORMcmd_HCC_Set_PinCodeType         }, /*0x0C0A*/
    {  GORMcmd_HCC_Set_Radio               }, /*0xFC79*/
    {  GORMcmd_HCC_Set_TX_Power_Offset     }, /*0xFC93*/
    {  GORMcmd_HCC_Set_Sleep_Timeout       }, /*0xFC7A*/
    {  GORMcmd_HCC_Set_BT_FTR              }, /*0xFC7D*/
    {  GORMcmd_HCC_Set_OSC_Info            }, /*0xFC7B*/
    {  GORMcmd_HCC_Set_LPO_Info            }, /*0xFC7C*/
    {  GORMcmd_HCC_Set_PTA                 }, /*0xFC74*/
    {  GORMcmd_HCC_Set_BLEPTA              }, /*0xFCFC*/
    {  GORMcmd_HCC_RESET                   }, /*0x0C03*/
    {  GORMcmd_HCC_Set_Internal_PTA_1      }, /*0xFCFB*/
    {  GORMcmd_HCC_Set_Internal_PTA_2      }, /*0xFCFB*/
    {  GORMcmd_HCC_Set_RF_Reg_100          }, /*0xFCB0*/
    {  0  },
};

sHciScriptElement_t bt_init_postload_script[] = 
{    
    {  GORMcmd_HCC_Set_Voice               }, /*0x0C26*/
    {  GORMcmd_HCC_Set_PCM                 }, /*0xFC72*/
    {  0  },
};

/**************************************************************************
 *                       F U N C T I O N S                                *
***************************************************************************/

static BOOL GORMcmd_HCC_Get_Local_BD_Addr(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0x1009;
    
    p_cmd->len = 3;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 0;
    
    LOG_DBG("GORMcmd_HCC_Get_Local_BD_Addr\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Get_Local_BD_Addr);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static void GORMevt_HCE_Get_Local_BD_Addr(void *p_evt)
{
    HC_BT_HDR *p_buf = (HC_BT_HDR *)p_evt;
    uint8_t *p;
    uint8_t event, status;
    uint16_t opcode;
    BOOL success;
    
    UCHAR ucBDAddr[6];
    
    LOG_DBG("GORMevt_HCE_Get_Local_BD_Addr\n");
    
    p = (uint8_t *)(p_buf + 1);
    event = *p;
    p += 3;
    STREAM_TO_UINT16(opcode, p)
    status = *p++;
    
    if ((event == 0x0E) && /* Command Complete Event */
        (opcode == 0x1009) && /* OpCode correct */
        (status == 0)) /* Success */
    {
        success = TRUE;
    }
    else{
        success = FALSE;
    }
    
    ucBDAddr[5] = *p++;
    ucBDAddr[4] = *p++;
    ucBDAddr[3] = *p++;
    ucBDAddr[2] = *p++;
    ucBDAddr[1] = *p++;
    ucBDAddr[0] = *p++;
    
    LOG_WAN("Retrieve eFUSE address: %02x-%02x-%02x-%02x-%02x-%02x\n",
            ucBDAddr[0], ucBDAddr[1], ucBDAddr[2], ucBDAddr[3], ucBDAddr[4], ucBDAddr[5]);
            
    memcpy(btinit->bt_conf.fields.addr, ucBDAddr, 6);
    
    if (bt_vnd_cbacks){
        bt_vnd_cbacks->dealloc(p_buf);
    }
    
    pthread_mutex_lock(&btinit_ctrl.mutex);
    cmd_status = success ? CMD_SUCCESS : CMD_FAIL;
    // Wake up command tx thread
    pthread_cond_signal(&btinit_ctrl.cond);
    pthread_mutex_unlock(&btinit_ctrl.mutex);
    
    return;
}

static BOOL GORMcmd_HCC_Set_Local_BD_Addr(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC1A;

	unsigned char sDefaultAddr[6];
	
	if(0x6620 == g_chipId){
		memcpy(sDefaultAddr, sDefaultCfg_6620.addr, 6);
	}
	else if(0x6628 == g_chipId){
		memcpy(sDefaultAddr, sDefaultCfg_6628.addr, 6);
	}
	
    if (fInternalCfg)
    {
        int file_fd = 0;
        ssize_t written;
		
        if (0 == memcmp(btinit->bt_conf.fields.addr, sDefaultAddr, 6))
        {
            if (fgetEFUSE){
                LOG_WAN("eFUSE address default value\n");
            }
            else{
                LOG_WAN("BD address default value\n");
            }
            
        #ifdef BD_ADDR_AUTOGEN
            GetRandomValue(btinit->bt_conf.fields.addr);
        #endif
            
            /* Update BD address in internal BT config file */
            file_fd = open(INTERNAL_BT_CFG_FILE, O_WRONLY);
            if (file_fd < 0){
                LOG_ERR("Failed to open %s!error code:%s\n", INTERNAL_BT_CFG_FILE, strerror(errno));
            }
            else{
                written = write(file_fd, btinit->bt_conf.fields.addr, 6);
                close(file_fd);
            }
        }
        else{
            if (fgetEFUSE){
                LOG_WAN("eFUSE address has valid value\n");
                /* Update BD address in internal BT config file */
                file_fd = open(INTERNAL_BT_CFG_FILE, O_WRONLY);
                if (file_fd < 0){
                    LOG_ERR("Failed to open %s!\n", INTERNAL_BT_CFG_FILE);//todo
                }
                else{
                    written = write(file_fd, btinit->bt_conf.fields.addr, 6);
                    close(file_fd);
                }
            }
            else{
                /* BD address has valid value, use it directly */
            }
        }
        /* Clear flag */
        fgetEFUSE = FALSE;
        fInternalCfg = FALSE;
    }

    p_cmd->len = 9;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 6;
    
    //params
    *p++ = btinit->bt_conf.fields.addr[5];
    *p++ = btinit->bt_conf.fields.addr[4];
    *p++ = btinit->bt_conf.fields.addr[3];
    *p++ = btinit->bt_conf.fields.addr[2];
    *p++ = btinit->bt_conf.fields.addr[1];
    *p++ = btinit->bt_conf.fields.addr[0];
    
    LOG_DBG("GORMcmd_HCC_Set_Local_BD_Addr\n");
    
    LOG_WAN("Write BD address: %02x-%02x-%02x-%02x-%02x-%02x\n",
            btinit->bt_conf.fields.addr[5], btinit->bt_conf.fields.addr[4], 
            btinit->bt_conf.fields.addr[3], btinit->bt_conf.fields.addr[2], 
            btinit->bt_conf.fields.addr[1], btinit->bt_conf.fields.addr[0]);
    
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_Set_LinkKeyType(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC1B;
    
    p_cmd->len = 4;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 1;
    
    //params
    *p++ = 0x01;	    /*00: unit key; 01: combination key*/
    
    LOG_DBG("GORMcmd_HCC_Set_LinkKeyType\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_Set_UnitKey(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC75;
    
    p_cmd->len = 19;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 16;
    
    //params
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    
    LOG_DBG("GORMcmd_HCC_Set_UnitKey\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_Set_Encryption(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC76;
    
    p_cmd->len = 6;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 3;
    
    //params
    *p++ = 0x00;
    *p++ = 0x02;
    *p++ = 0x10;
    
    LOG_DBG("GORMcmd_HCC_Set_Encryption\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_Set_PinCodeType(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0x0C0A;
    
    p_cmd->len = 4;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 1;
    
    //params
    *p++ = 0x00;	  /*00: variable PIN; 01: Fixed PIN*/
    
    LOG_DBG("GORMcmd_HCC_Set_PinCodeType\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_Set_Voice(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0x0C26;
    
    p_cmd->len = 5;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 2;
    
    //params
    *p++ = btinit->bt_conf.fields.voice[0];
    *p++ = btinit->bt_conf.fields.voice[1];
    
    LOG_DBG("GORMcmd_HCC_Set_Voice\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_Set_PCM(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC72;
    
    p_cmd->len = 7;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 4;
    
    //params
    *p++ = btinit->bt_conf.fields.codec[0];
    *p++ = btinit->bt_conf.fields.codec[1];
    *p++ = btinit->bt_conf.fields.codec[2];
    *p++ = btinit->bt_conf.fields.codec[3];	
    
    LOG_DBG("GORMcmd_HCC_Set_PCM\r\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_Set_Radio(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC79;
    
    p_cmd->len = 9;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 6;
    
    //params
    *p++ = btinit->bt_conf.fields.radio[0];
    *p++ = btinit->bt_conf.fields.radio[1];
    *p++ = btinit->bt_conf.fields.radio[2];
    *p++ = btinit->bt_conf.fields.radio[3];
    *p++ = btinit->bt_conf.fields.radio[4];
    *p++ = btinit->bt_conf.fields.radio[5];
    
    LOG_DBG("GORMcmd_HCC_Set_Radio\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_Set_TX_Power_Offset(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC93;
    
    p_cmd->len = 6;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 3;
    
    //params
    *p++ = btinit->bt_conf.fields.tx_pwr_offset[0];
    *p++ = btinit->bt_conf.fields.tx_pwr_offset[1];
    *p++ = btinit->bt_conf.fields.tx_pwr_offset[2];
    
    LOG_DBG("GORMcmd_HCC_Set_TX_Power_Offset\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_Set_Sleep_Timeout(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC7A;
    
    p_cmd->len = 10;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 7;
    
    //params
    *p++ = btinit->bt_conf.fields.sleep[0];
    *p++ = btinit->bt_conf.fields.sleep[1];
    *p++ = btinit->bt_conf.fields.sleep[2];
    *p++ = btinit->bt_conf.fields.sleep[3];
    *p++ = btinit->bt_conf.fields.sleep[4];
    *p++ = btinit->bt_conf.fields.sleep[5];
    *p++ = btinit->bt_conf.fields.sleep[6];
    
    LOG_DBG("GORMcmd_HCC_Set_Sleep_Timeout\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_Set_BT_FTR(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC7D;
    
    p_cmd->len = 5;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 2;
    
    //params
    *p++ = btinit->bt_conf.fields.feature[0];
    *p++ = btinit->bt_conf.fields.feature[1];
    
    LOG_DBG("GORMcmd_HCC_Set_BT_FTR\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_Set_OSC_Info(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC7B;
    
    p_cmd->len = 8;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 5;
    
    //params
    *p++ = 0x01;     /* do not care on 6620 */
    *p++ = 0x01;     /* do not care on 6620 */
    *p++ = 0x14;     /* clock drift */
    *p++ = 0x0A;     /* clock jitter */

	if(g_chipId == 0x6620)
		*p++ = 0x05;	 /* OSC stable time */
	else if(g_chipId == 0x6628)
		*p++ = 0x08;	 /* OSC stable time */
    
    LOG_DBG("GORMcmd_HCC_Set_OSC_Info\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_Set_LPO_Info(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC7C;
    
    p_cmd->len = 13;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 10;
    
    //params
    *p++  = 0x01;     /* LPO source = external */
    *p++  = 0xFA;     /* LPO clock drift = 250ppm */
    *p++  = 0x0A;     /* LPO clock jitter = 10us */
    *p++  = 0x02;     /* LPO calibration mode = manual mode */
    *p++  = 0x00;     /* LPO calibration interval = 10 mins */
    *p++  = 0xA6;
    *p++  = 0x0E;
    *p++  = 0x00;
    *p++  = 0x40;     /* LPO calibration cycles = 64 */
    *p++  = 0x00;
    
    LOG_DBG("GORMcmd_HCC_Set_LPO_Info\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_Set_PTA(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC74;
    
    p_cmd->len = 13;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 10;
    
    //params
    *p++ = 0xC9;
    *p++ = 0x8B;
    *p++ = 0xBF;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x52;
    *p++ = 0x0E;
    *p++ = 0x0E;
    *p++ = 0x1F;
    *p++ = 0x1B;
    
    LOG_DBG("GORMcmd_HCC_Set_PTA\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_Set_BLEPTA(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFCFC;
    
    p_cmd->len = 8;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 5;
    
    //params
    *p++ = 0x16;
    *p++ = 0x0E;
    *p++ = 0x0E;
    *p++ = 0x00;
    *p++ = 0x07;
    
    LOG_DBG("GORMcmd_HCC_Set_BLEPTA\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_Set_RF_Desense(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC20;
    
    p_cmd->len = 9;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 6;
    
    //params
    *p++ = 0x01;    /* DWA is off at Channel 30, others still ON */
    *p++ = 0x00;    /* Turn Off RX Current Boost Function */
    *p++ = 0x00;    /* Turn Off 0.8889 interface switch Function */
    *p++ = 0x00;    /* Turn Off MPLL Hopping Function */
    *p++ = 0x00;    /* Turn Off DC_Notch Function */
    *p++ = 0x00;    /* Turn Off EMI_Hopping Function */
    
    LOG_DBG("GORMcmd_HCC_Set_RF_Desense\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_RESET(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0x0C03;
    
    p_cmd->len = 3;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 0;
    
    LOG_DBG("GORMcmd_HCC_RESET\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_Set_Internal_PTA_1(HC_BT_HDR *p_cmd)
{   
    uint8_t *p, ret;
    wOpCode = 0xFCFB;
    
    p_cmd->len = 18;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 15;
    
    //params
    *p++ = 0x00;
    *p++ = 0x01;     /* bt_pta_high_level_tx */
    *p++ = 0x0F;     /* bt_pta_mid_level_tx */
    *p++ = 0x0F;     /* bt_pta_low_level_tx */
    *p++ = 0x01;     /* bt_pta_high_level_rx */
    *p++ = 0x0F;     /* bt_pta_mid_level_rx */
    *p++ = 0x0F;     /* bt_pta_low_level_rx */
    *p++ = 0x01;     /* ble_pta_high_level_tx */
    *p++ = 0x0F;     /* ble_pta_mid_level_tx */
    *p++ = 0x0F;     /* ble_pta_low_level_tx */
    *p++ = 0x01;     /* ble_pta_high_level_rx */
    *p++ = 0x0F;     /* ble_pta_mid_level_rx */
    *p++ = 0x0F;     /* ble_pta_low_level_rx */
    *p++ = 0x02;     /* time_r2g */
    *p++ = 0x01;     /* always  0x1 */
    
    LOG_DBG("GORMcmd_HCC_Set_Internal_PTA_1\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_Set_Internal_PTA_2(HC_BT_HDR *p_cmd)
{   
    uint8_t *p, ret;
    wOpCode = 0xFCFB;
    
    p_cmd->len = 10;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 7;
    
    //params
    *p++ = 0x01;		
    *p++ = 0x19;     /* wifi20_hb */
    *p++ = 0x19;     /* wifi20_hb */
    *p++ = 0x07;     /* next_rssi_update_bt_slots */
    *p++ = 0xD0;     /* next_rssi_update_bt_slots */
    *p++ = 0x00;     /* stream_identify_by_host */
    *p++ = 0x01;     /* enable auto AFH */
    
    LOG_DBG("GORMcmd_HCC_Set_Internal_PTA_2\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_Set_Sleep_Control_Reg(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    uint32_t addr, val;
    wOpCode = 0xFCD0;
    
    p_cmd->len = 11;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 8;
    
    addr = 0x81010074;
    val = 0x000029E2;
    //params
    *p++ = (uint8_t)((addr) & 0xFF);
    *p++ = (uint8_t)((addr >> 8) & 0xFF);
    *p++ = (uint8_t)((addr >> 16) & 0xFF);
    *p++ = (uint8_t)((addr >> 24) & 0xFF);
    *p++ = (uint8_t)((val) & 0xFF);
    *p++ = (uint8_t)((val >> 8) & 0xFF);
    *p++ = (uint8_t)((val >> 16) & 0xFF);
    *p++ = (uint8_t)((val >> 24) & 0xFF);
    
    LOG_DBG("GORMcmd_HCC_Set_Sleep_Control_Reg\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_Set_SLP_LDOD_VCTRL_Reg(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    uint32_t addr, val;
    wOpCode = 0xFCD0;
    
    p_cmd->len = 11;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 8;
    
    addr = 0x8102001C;
    val = 0x00000879;
    //params
    *p++ = (uint8_t)((addr) & 0xFF);
    *p++ = (uint8_t)((addr >> 8) & 0xFF);
    *p++ = (uint8_t)((addr >> 16) & 0xFF);
    *p++ = (uint8_t)((addr >> 24) & 0xFF);
    *p++ = (uint8_t)((val) & 0xFF);
    *p++ = (uint8_t)((val >> 8) & 0xFF);
    *p++ = (uint8_t)((val >> 16) & 0xFF);
    *p++ = (uint8_t)((val >> 24) & 0xFF);
    
    LOG_DBG("GORMcmd_HCC_Set_SLP_LDOD_VCTRL_Reg\r\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static BOOL GORMcmd_HCC_Set_RF_Reg_100(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFCB0;
    
    p_cmd->len = 9;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 6;
    
    //params
    *p++ = 0x64;	
    *p++ = 0x01;
    *p++ = 0x02;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    
    LOG_DBG("GORMcmd_HCC_Set_RF_Reg_100\n");
    
    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }
    
    return ret;
}

static void GORMevt_HCE_Common_Complete(void *p_evt)
{
    HC_BT_HDR *p_buf = (HC_BT_HDR *)p_evt;
    uint8_t *p;
    uint8_t event, status;
    uint16_t opcode;
    BOOL success;
    
    LOG_DBG("GORMevt_HCE_Common_Complete\n");
    
    p = (uint8_t *)(p_buf + 1);
    event = *p;
    p += 3;
    STREAM_TO_UINT16(opcode, p);
    status = *p;
    
    if ((event == 0x0E) && /* Command Complete Event */
        (opcode == wOpCode) && /* OpCode correct */
        (status == 0)) /* Success */
    {
        success = TRUE;
    }
    else{
        success = FALSE;
    }
    
    if (bt_vnd_cbacks){
        bt_vnd_cbacks->dealloc(p_buf);
    }
    
    pthread_mutex_lock(&btinit_ctrl.mutex);
    cmd_status = success ? CMD_SUCCESS : CMD_FAIL;
    // Wake up command tx thread
    pthread_cond_signal(&btinit_ctrl.cond);
    pthread_mutex_unlock(&btinit_ctrl.mutex);
    
    return;
}


void *GORM_FW_Init_Thread(void *ptr)
{
    int i = 0;
    HC_BT_HDR  *p_buf = NULL;
    bt_vendor_op_result_t ret = BT_VND_OP_RESULT_FAIL;
    
    LOG_DBG("FW init thread starts\n");
    
    pthread_mutex_init(&btinit_ctrl.mutex, NULL);
    pthread_cond_init(&btinit_ctrl.cond, NULL);
   
	// preload init script	
    if(0x6620 == g_chipId){
		btinit->cur_script = bt_init_preload_script_6620;
	}
	else if(0x6628 == g_chipId){
		btinit->cur_script = bt_init_preload_script_6628;
	}
	
    // Can not find matching script. Simply skip
    if((btinit->cur_script) == NULL){
        LOG_ERR("No matching init script\n");
        goto exit;
    }
    
    if (fInternalCfg)
    {
        if(0x6620 == g_chipId){
			if (0 == memcmp(btinit->bt_conf.fields.addr, sDefaultCfg_6620.addr, 6))
	        {
	            /* BD address default value, want to retrieve module eFUSE on combo chip */
	            fgetEFUSE = TRUE;
	        }	
		}
		else if(0x6628 == g_chipId){
			if (0 == memcmp(btinit->bt_conf.fields.addr, sDefaultCfg_6628.addr, 6))
	        {
	            /* BD address default value, want to retrieve module eFUSE on combo chip */
	            fgetEFUSE = TRUE;
	        }			
		}
    }
    
    if (fgetEFUSE) //perform GORMcmd_HCC_Get_Local_BD_Addr
        i = 0;
    else //skip GORMcmd_HCC_Get_Local_BD_Addr
        i = 1;
    
    while(btinit->cur_script[i].command_func)
    {
        p_buf = NULL;
        
        if (bt_vnd_cbacks){
            p_buf = (HC_BT_HDR *) bt_vnd_cbacks->alloc(BT_HC_HDR_SIZE + \
                                                           HCI_CMD_MAX_SIZE);
        }
        else{
            LOG_ERR("No libbt-hci callbacks!\n");
        }
        
        if (p_buf)
        {
            p_buf->event = MSG_STACK_TO_HC_HCI_CMD;
            p_buf->offset = 0;
            p_buf->layer_specific = 0;
            
            cmd_status = CMD_PENDING;
            
            if(btinit->cur_script[i].command_func(p_buf) == FALSE){
                LOG_ERR("Send command %d fails\n", i);
                if (bt_vnd_cbacks){
                    bt_vnd_cbacks->dealloc(p_buf);
                }
                goto exit;
            }
        }
        else{
            LOG_ERR("Alloc command %d buffer fails\n", i);
            goto exit;
        }
        
        // Wait for event returned
        pthread_mutex_lock(&btinit_ctrl.mutex);
        while (cmd_status == CMD_PENDING){
            pthread_cond_wait(&btinit_ctrl.cond, &btinit_ctrl.mutex);
        }
        
        if (cmd_status == CMD_FAIL){
            LOG_ERR("The event of command %d error\n", i);
            pthread_mutex_unlock(&btinit_ctrl.mutex);
            goto exit;
        }
        else{
            LOG_DBG("The event of command %d success\n", i);
            pthread_mutex_unlock(&btinit_ctrl.mutex);
        }
        
        i ++;
    }
    
    ret = BT_VND_OP_RESULT_SUCCESS;

exit:    
    pthread_mutex_destroy(&btinit_ctrl.mutex);
    pthread_cond_destroy(&btinit_ctrl.cond);
    
    if (bt_vnd_cbacks){
        bt_vnd_cbacks->fwcfg_cb(ret);
    }
    
    return NULL;
}

void *GORM_SCO_Init_Thread(void *ptr)
{
    int i = 0;
    HC_BT_HDR  *p_buf = NULL;
    bt_vendor_op_result_t ret = BT_VND_OP_RESULT_FAIL;
    
    LOG_DBG("SCO init thread starts\n");
    
    pthread_mutex_init(&btinit_ctrl.mutex, NULL);
    pthread_cond_init(&btinit_ctrl.cond, NULL);
    
    // postload init script
    btinit->cur_script = bt_init_postload_script;
    
    // Can not find matching script. Simply skip
    if((btinit->cur_script) == NULL){
        LOG_ERR("No matching init script\n");
        goto exit;
    }
    
    while(btinit->cur_script[i].command_func)
    {
        p_buf = NULL;
        
        if (bt_vnd_cbacks){
            p_buf = (HC_BT_HDR *) bt_vnd_cbacks->alloc(BT_HC_HDR_SIZE + \
                                                           HCI_CMD_MAX_SIZE);
        }
        else{
            LOG_ERR("No libbt-hci callbacks!\n");
        }
        
        if (p_buf)
        {
            p_buf->event = MSG_STACK_TO_HC_HCI_CMD;
            p_buf->offset = 0;
            p_buf->layer_specific = 0;
            
            cmd_status = CMD_PENDING;
            
            if(btinit->cur_script[i].command_func(p_buf) == FALSE){
                LOG_ERR("Send command %d fails\n", i);
                if (bt_vnd_cbacks){
                    bt_vnd_cbacks->dealloc(p_buf);
                }
                goto exit;
            }
        }
        else{
            LOG_ERR("Alloc command %d buffer fails\n", i);
            goto exit;
        }
        
        // Wait for event returned
        pthread_mutex_lock(&btinit_ctrl.mutex);
        while (cmd_status == CMD_PENDING){
            pthread_cond_wait(&btinit_ctrl.cond, &btinit_ctrl.mutex);
        }
        
        if (cmd_status == CMD_FAIL){
            LOG_ERR("The event of command %d error\n", i);
            pthread_mutex_unlock(&btinit_ctrl.mutex);
            goto exit;
        }
        else{
            LOG_DBG("The event of command %d success\n", i);
            pthread_mutex_unlock(&btinit_ctrl.mutex);
        }
        
        i ++;
    }
    
    ret = BT_VND_OP_RESULT_SUCCESS;

exit:    
    pthread_mutex_destroy(&btinit_ctrl.mutex);
    pthread_cond_destroy(&btinit_ctrl.cond);
    
    if (bt_vnd_cbacks){
        bt_vnd_cbacks->scocfg_cb(ret);
    }
    
    return NULL;
}


static void GetRandomValue(UCHAR string[6])
{
    int iRandom = 0;
    int fd = 0;
    unsigned long seed;
    
    LOG_WAN("Enable random generation\n");
    
    /* initialize random seed */
    srand (time(NULL));
    iRandom = rand();
    LOG_WAN("iRandom = [%d]", iRandom);
    string[0] = (((iRandom>>24|iRandom>>16) & (0xFE)) | (0x02));
    
    /* second seed */
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand (tv.tv_usec);
    iRandom = rand();
    LOG_WAN("iRandom = [%d]", iRandom);
    string[1] = ((iRandom>>8) & 0xFF);
    
    /* third seed */
    fd = open("/dev/urandom", O_RDONLY);
    if (fd > 0){
        if (read(fd, &seed, sizeof(unsigned long)) > 0){
            srand (seed);
            iRandom = rand();
        }
        close(fd);
    }
    
    LOG_WAN("iRandom = [%d]", iRandom);
    string[5] = (iRandom & 0xFF);
    
    return;
}
