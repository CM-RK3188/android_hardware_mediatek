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

#include "custom/cust_bt.h"
#include "bt_kal.h"


/**************************************************************************
 *              G L O B A L   V A R I A B L E S                           *
***************************************************************************/

/* Structure for the sending command */
static HciCommand  hciCommand;
/* Opcode of the sending command */
static HciCommandType wOpCode;

static HANDLE  bt_comm_port;
static sBTInitVar_t btinit[1];
extern BOOL fInternalCfg;
extern int g_chipId;

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S                 *
***************************************************************************/

static BtStatus GORMcmd_HCC_Set_Local_BD_Addr(void);
static BtStatus GORMcmd_HCC_Set_CapID(void);
static BtStatus GORMcmd_HCC_Set_LinkKeyType(void);
static BtStatus GORMcmd_HCC_Set_UnitKey(void);
static BtStatus GORMcmd_HCC_Set_Encryption(void);
static BtStatus GORMcmd_HCC_Set_PinCodeType(void);
static BtStatus GORMcmd_HCC_Set_Voice(void);
static BtStatus GORMcmd_HCC_Set_PCM(void);
static BtStatus GORMcmd_HCC_Set_Radio(void);
static BtStatus GORMcmd_HCC_Set_TX_Power_Offset(void);
static BtStatus GORMcmd_HCC_Set_Sleep_Timeout(void);
static BtStatus GORMcmd_HCC_Set_BT_FTR(void);


static BtStatus GORMcmd_HCC_Set_OSC_Info(void);
static BtStatus GORMcmd_HCC_Set_LPO_Info(void);
static BtStatus GORMcmd_HCC_Set_PTA(void);
static BtStatus GORMcmd_HCC_Set_RF_Desense(void);
static BtStatus GORMcmd_HCC_RESET(void);
static BtStatus GORMcmd_HCC_Set_Sleep_Control_Reg(void);


static BtStatus GORMcmd_HCC_Set_BLEPTA(void);
static BtStatus GORMcmd_HCC_Set_Internal_PTA_1(void);
static BtStatus GORMcmd_HCC_Set_Internal_PTA_2(void);
static BtStatus GORMcmd_HCC_Set_SLP_LDOD_VCTRL_Reg(void);
static BtStatus GORMcmd_HCC_Set_RF_Reg_100(void);


static BtStatus GORMcmd_HCC_Read_Local_Version(void);
static BtStatus GORMcmd_HCC_Simulate_MT6612(void);
static BtStatus GORMcmd_HCC_Fix_UART_Escape(void);
static BtStatus GORMcmd_HCC_GetHwVersion(void);
static BtStatus GORMcmd_HCC_GetGormVersion(void);
static BtStatus GORMcmd_HCC_ChangeBaudRate(void);
static BtStatus GORMcmd_HCC_WritePatch(void);
static BtStatus GORMcmd_HCC_Set_Chip_Feature(void);
static BtStatus GORMcmd_HCC_Enable_PTA(void);
static BtStatus GORMcmd_HCC_Set_WiFi_Ch(void);
static BtStatus GORMcmd_HCC_Set_AFH_Mask(void);


static BOOL BT_Get_Local_BD_Addr(PUCHAR ucBDAddr);
static void GetRandomValue(UCHAR string[6]);


//===================================================================
#ifdef MTK_COMBO_SUPPORT
sHciScriptElement_t bt_init_script_6620[] = // Combo chip 6620
{    
    {  GORMcmd_HCC_Set_Local_BD_Addr       }, /*0xFC1A*/
    {  GORMcmd_HCC_Set_LinkKeyType         }, /*0xFC1B*/
    {  GORMcmd_HCC_Set_UnitKey             }, /*0xFC75*/
    {  GORMcmd_HCC_Set_Encryption          }, /*0xFC76*/
    {  GORMcmd_HCC_Set_PinCodeType         }, /*0x0C0A*/
    {  GORMcmd_HCC_Set_Voice               }, /*0x0C26*/
    {  GORMcmd_HCC_Set_PCM                 }, /*0xFC72*/
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

sHciScriptElement_t bt_init_script_6628[] = //Combo Chip 6628
{    
    {  GORMcmd_HCC_Set_Local_BD_Addr       }, /*0xFC1A*/
    {  GORMcmd_HCC_Set_LinkKeyType         }, /*0xFC1B*/
    {  GORMcmd_HCC_Set_UnitKey             }, /*0xFC75*/
    {  GORMcmd_HCC_Set_Encryption          }, /*0xFC76*/
    {  GORMcmd_HCC_Set_PinCodeType         }, /*0x0C0A*/
    {  GORMcmd_HCC_Set_Voice               }, /*0x0C26*/
    {  GORMcmd_HCC_Set_PCM                 }, /*0xFC72*/
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
#else// Standalone Chip MT6622
sHciScriptElement_t bt_init_script_6622[] = 
{
    {  GORMcmd_HCC_Read_Local_Version      }, /*0x1001*/
    {  GORMcmd_HCC_Simulate_MT6612         }, /*0xFCCC*/
    {  GORMcmd_HCC_Fix_UART_Escape         }, /*0xFCD0*/
    {  GORMcmd_HCC_GetHwVersion            }, /*0xFCD1*/
    {  GORMcmd_HCC_GetGormVersion          }, /*0xFCD1*/
    {  GORMcmd_HCC_ChangeBaudRate          }, /*0xFC77*/
    {  GORMcmd_HCC_WritePatch              }, /*0xFCC4*/
    {  GORMcmd_HCC_Set_Local_BD_Addr       }, /*0xFC1A*/
    {  GORMcmd_HCC_Set_CapID               }, /*0xFC7F*/
    {  GORMcmd_HCC_Set_LinkKeyType         }, /*0xFC1B*/
    {  GORMcmd_HCC_Set_UnitKey             }, /*0xFC75*/
    {  GORMcmd_HCC_Set_Encryption          }, /*0xFC76*/
    {  GORMcmd_HCC_Set_PinCodeType         }, /*0x0C0A*/
    {  GORMcmd_HCC_Set_Voice               }, /*0x0C26*/
    {  GORMcmd_HCC_Set_PCM                 }, /*0xFC72*/
    {  GORMcmd_HCC_Set_Radio               }, /*0xFC79*/
    {  GORMcmd_HCC_Set_Sleep_Timeout       }, /*0xFC7A*/
    {  GORMcmd_HCC_Set_BT_FTR              }, /*0xFC7D*/
    {  GORMcmd_HCC_Set_Chip_Feature        }, /*0xFC1E*/
    {  GORMcmd_HCC_Set_OSC_Info            }, /*0xFC7B*/
    {  GORMcmd_HCC_Set_LPO_Info            }, /*0xFC7C*/
    {  GORMcmd_HCC_Set_RF_Desense          }, /*0xFC20*/
    {  GORMcmd_HCC_Set_PTA                 }, /*0xFC74*/
    {  GORMcmd_HCC_RESET                   }, /*0x0C03*/
    {  GORMcmd_HCC_Enable_PTA              }, /*0xFCD2*/
    {  GORMcmd_HCC_Set_WiFi_Ch             }, /*0xFCD3*/
    {  GORMcmd_HCC_Set_AFH_Mask            }, /*0x0C3F*/
    {  GORMcmd_HCC_Set_Sleep_Control_Reg   }, /*0xFCD0*/
    {  0  },
};
#endif

/**************************************************************************
 *                         F U N C T I O N S                              *
***************************************************************************/

static BtStatus GORMcmd_HCC_Set_Local_BD_Addr(void)
{
    wOpCode = 0xFC1A;
	unsigned char sDefaultAddr[6];    
	
    LOG_DBG("GORMcmd_HCC_Set_Local_BD_Addr\n");
	
#ifdef MTK_COMBO_SUPPORT
	if(0x6620 == g_chipId){
		memcpy(sDefaultAddr, sDefaultCfg_6620.addr, 6);
	}
	else if(0x6628 == g_chipId){
		memcpy(sDefaultAddr, sDefaultCfg_6628.addr, 6);
	}
#else
	memcpy(sDefaultAddr, sDefaultCfg_6622.addr, 6);
#endif
	

    if (fInternalCfg){
        int file_fd = 0;
        ssize_t written;
        
        if (0 == memcmp(btinit->bt_conf.fields.addr, sDefaultAddr, 6))
        {
            LOG_WAN("BD address default value\n");
            
        #ifdef MTK_COMBO_SUPPORT
            /* Want to retrieve module eFUSE address on combo chip */
            BT_Get_Local_BD_Addr(btinit->bt_conf.fields.addr);
            
            if (0 == memcmp(btinit->bt_conf.fields.addr, sDefaultAddr, 6)){        
                LOG_WAN("eFUSE address default value\n");
            #ifdef BD_ADDR_AUTOGEN
                GetRandomValue(btinit->bt_conf.fields.addr);
            #endif
            }
            else{
                LOG_WAN("eFUSE address has valid value\n");
            }
        #else
            #ifdef BD_ADDR_AUTOGEN
            GetRandomValue(btinit->bt_conf.fields.addr);
            #endif
        #endif
        
            /* Update BD address in internal BT config file */
            file_fd = open(INTERNAL_BT_CFG_FILE, O_WRONLY);
            if (file_fd < 0){
                LOG_ERR("Failed to open %s!\n", INTERNAL_BT_CFG_FILE);
            }
            else{
                written = write(file_fd, btinit->bt_conf.fields.addr, 6);
                close(file_fd);
            }
        }
        else {
            /* BD address has valid value, use it directly */
        }
        
        /* Clear flag */
        fInternalCfg = FALSE;
    }
    
    hciCommand.parms[0] = btinit->bt_conf.fields.addr[5];
    hciCommand.parms[1] = btinit->bt_conf.fields.addr[4];
    hciCommand.parms[2] = btinit->bt_conf.fields.addr[3];
    hciCommand.parms[3] = btinit->bt_conf.fields.addr[2];
    hciCommand.parms[4] = btinit->bt_conf.fields.addr[1];
    hciCommand.parms[5] = btinit->bt_conf.fields.addr[0];
    
    LOG_WAN("Write BD address: %02x-%02x-%02x-%02x-%02x-%02x\n",
            hciCommand.parms[5], hciCommand.parms[4], hciCommand.parms[3],
            hciCommand.parms[2], hciCommand.parms[1], hciCommand.parms[0]);
    
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 6, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_LinkKeyType(void)
{
    wOpCode = 0xFC1B;

#ifdef MTK_COMBO_SUPPORT
    hciCommand.parms[0] = 0x01;	  /*00: unit key; 01: combination key*/
#else
    hciCommand.parms[0] = btinit->bt_conf.fields.link_key_type[0];
#endif

    LOG_DBG("GORMcmd_HCC_Set_LinkKeyType\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 1, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_UnitKey(void)
{
    wOpCode = 0xFC75;

#ifdef MTK_COMBO_SUPPORT
    hciCommand.parms[0]  = 0x00;
    hciCommand.parms[1]  = 0x00;
    hciCommand.parms[2]  = 0x00;
    hciCommand.parms[3]  = 0x00;
    hciCommand.parms[4]  = 0x00;
    hciCommand.parms[5]  = 0x00;
    hciCommand.parms[6]  = 0x00;
    hciCommand.parms[7]  = 0x00;
    hciCommand.parms[8]  = 0x00;
    hciCommand.parms[9]  = 0x00;
    hciCommand.parms[10] = 0x00;
    hciCommand.parms[11] = 0x00;
    hciCommand.parms[12] = 0x00;
    hciCommand.parms[13] = 0x00;
    hciCommand.parms[14] = 0x00;
    hciCommand.parms[15] = 0x00;
#else
    hciCommand.parms[0]  = btinit->bt_conf.fields.unit_key[0];
    hciCommand.parms[1]  = btinit->bt_conf.fields.unit_key[1];
    hciCommand.parms[2]  = btinit->bt_conf.fields.unit_key[2];
    hciCommand.parms[3]  = btinit->bt_conf.fields.unit_key[3];
    hciCommand.parms[4]  = btinit->bt_conf.fields.unit_key[4];
    hciCommand.parms[5]  = btinit->bt_conf.fields.unit_key[5];
    hciCommand.parms[6]  = btinit->bt_conf.fields.unit_key[6];
    hciCommand.parms[7]  = btinit->bt_conf.fields.unit_key[7];
    hciCommand.parms[8]  = btinit->bt_conf.fields.unit_key[8];
    hciCommand.parms[9]  = btinit->bt_conf.fields.unit_key[9];
    hciCommand.parms[10] = btinit->bt_conf.fields.unit_key[10];
    hciCommand.parms[11] = btinit->bt_conf.fields.unit_key[11];
    hciCommand.parms[12] = btinit->bt_conf.fields.unit_key[12];
    hciCommand.parms[13] = btinit->bt_conf.fields.unit_key[13];
    hciCommand.parms[14] = btinit->bt_conf.fields.unit_key[14];
    hciCommand.parms[15] = btinit->bt_conf.fields.unit_key[15];
#endif

    LOG_DBG("GORMcmd_HCC_Set_UnitKey\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 16, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_Encryption(void)
{
    wOpCode = 0xFC76;

#ifdef MTK_COMBO_SUPPORT
    hciCommand.parms[0] = 0x00;
    hciCommand.parms[1] = 0x02;
    hciCommand.parms[2] = 0x10;
#else
    hciCommand.parms[0] = btinit->bt_conf.fields.encryption[0];
    hciCommand.parms[1] = btinit->bt_conf.fields.encryption[1];
    hciCommand.parms[2] = btinit->bt_conf.fields.encryption[2];
#endif

    LOG_DBG("GORMcmd_HCC_Set_Encryption\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 3, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_PinCodeType(void)
{
    wOpCode = 0x0C0A;

#ifdef MTK_COMBO_SUPPORT
    hciCommand.parms[0] = 0x00;	  /*00: variable PIN; 01: Fixed PIN*/
#else
    hciCommand.parms[0] = btinit->bt_conf.fields.pin_code_type[0];
#endif

    LOG_DBG("GORMcmd_HCC_Set_PinCodeType\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 1, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_Voice(void)
{
    wOpCode = 0x0C26;
    
    hciCommand.parms[0] = btinit->bt_conf.fields.voice[0];
    hciCommand.parms[1] = btinit->bt_conf.fields.voice[1];
    
    LOG_DBG("GORMcmd_HCC_Set_Voice\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 2, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_PCM(void)
{
    wOpCode = 0xFC72;
    
    hciCommand.parms[0] = btinit->bt_conf.fields.codec[0];
    hciCommand.parms[1] = btinit->bt_conf.fields.codec[1];
    hciCommand.parms[2] = btinit->bt_conf.fields.codec[2];
    hciCommand.parms[3] = btinit->bt_conf.fields.codec[3];	
    
    LOG_DBG("GORMcmd_HCC_Set_PCM\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 4, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_Radio(void)
{
    wOpCode = 0xFC79;
    
    hciCommand.parms[0] = btinit->bt_conf.fields.radio[0];
    hciCommand.parms[1] = btinit->bt_conf.fields.radio[1];
    hciCommand.parms[2] = btinit->bt_conf.fields.radio[2];
    hciCommand.parms[3] = btinit->bt_conf.fields.radio[3];
    hciCommand.parms[4] = btinit->bt_conf.fields.radio[4];
    hciCommand.parms[5] = btinit->bt_conf.fields.radio[5];
    
    LOG_DBG("GORMcmd_HCC_Set_Radio\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 6, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_Sleep_Timeout(void)
{
    wOpCode = 0xFC7A;
    
    hciCommand.parms[0] = btinit->bt_conf.fields.sleep[0];
    hciCommand.parms[1] = btinit->bt_conf.fields.sleep[1];
    hciCommand.parms[2] = btinit->bt_conf.fields.sleep[2];
    hciCommand.parms[3] = btinit->bt_conf.fields.sleep[3];
    hciCommand.parms[4] = btinit->bt_conf.fields.sleep[4];
    hciCommand.parms[5] = btinit->bt_conf.fields.sleep[5];
    hciCommand.parms[6] = btinit->bt_conf.fields.sleep[6];
    
    LOG_DBG("GORMcmd_HCC_Set_Sleep_Timeout\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 7, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_BT_FTR(void)
{
    wOpCode = 0xFC7D;
    
    hciCommand.parms[0] = btinit->bt_conf.fields.feature[0];
    hciCommand.parms[1] = btinit->bt_conf.fields.feature[1];
    
    LOG_DBG("GORMcmd_HCC_Set_BT_FTR\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 2, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_OSC_Info(void)
{
    wOpCode = 0xFC7B;
    
    hciCommand.parms[0] = 0x01;    /* do not care on 6620 */
    hciCommand.parms[1] = 0x01;    /* do not care on 6620 */
    hciCommand.parms[2] = 0x14;    /* clock drift */
    hciCommand.parms[3] = 0x0A;    /* clock jitter */
#ifdef MTK_COMBO_SUPPORT
	if(g_chipId == 0x6620)
		hciCommand.parms[4] = 0x05;  /* OSC stable time */
	else if(g_chipId == 0x6628)
		hciCommand.parms[4] = 0x08;  /* OSC stable time */
#else
	hciCommand.parms[4] = 0x06; 	/* MT6622 OSC stable time */
#endif

    LOG_DBG("GORMcmd_HCC_Set_OSC_Info\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 5, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_LPO_Info(void)
{
    wOpCode = 0xFC7C;
    
    hciCommand.parms[0] = 0x01;    /* LPO source = external */
    hciCommand.parms[1] = 0xFA;    /* LPO clock drift = 250ppm */
    hciCommand.parms[2] = 0x0A;    /* LPO clock jitter = 10us */
    hciCommand.parms[3] = 0x02;    /* LPO calibration mode = manual mode */
    hciCommand.parms[4] = 0x00;    /* LPO calibration interval = 10 mins */
    hciCommand.parms[5] = 0xA6;
    hciCommand.parms[6] = 0x0E;
    hciCommand.parms[7] = 0x00;
    hciCommand.parms[8] = 0x40;    /* LPO calibration cycles = 64 */
    hciCommand.parms[9] = 0x00;
    
    LOG_DBG("GORMcmd_HCC_Set_LPO_Info\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 10, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_PTA(void)
{
    wOpCode = 0xFC74;

#ifdef MTK_COMBO_SUPPORT
    hciCommand.parms[0] = 0xC9;
    hciCommand.parms[1] = 0x8B;
    hciCommand.parms[2] = 0xBF;
    hciCommand.parms[3] = 0x00;
    hciCommand.parms[4] = 0x00;
    hciCommand.parms[5] = 0x52;
    hciCommand.parms[6] = 0x0E;
    hciCommand.parms[7] = 0x0E;
    hciCommand.parms[8] = 0x1F;
    hciCommand.parms[9] = 0x1B;
#else
    hciCommand.parms[0] = 0xCF;
    hciCommand.parms[1] = 0x8B;
    hciCommand.parms[2] = 0x1F;
    hciCommand.parms[3] = 0x04;
    hciCommand.parms[4] = 0x08;
    hciCommand.parms[5] = 0xA2;
    hciCommand.parms[6] = 0x62;
    hciCommand.parms[7] = 0x56;
    hciCommand.parms[8] = 0x07;
    hciCommand.parms[9] = 0x1B;
#endif

    LOG_DBG("GORMcmd_HCC_Set_PTA\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 10, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_RF_Desense(void)
{
    wOpCode = 0xFC20;
     
#ifdef MTK_COMBO_SUPPORT
    hciCommand.parms[0] = 0x01;    /* DWA is off at Channel 30, others still ON */
    hciCommand.parms[1] = 0x00;    /* Turn Off RX Current Boost Function */
    hciCommand.parms[2] = 0x00;    /* Turn Off 0.8889 interface switch Function */
    hciCommand.parms[3] = 0x00;    /* Turn Off MPLL Hopping Function */
    hciCommand.parms[4] = 0x00;    /* Turn Off DC_Notch Function */
    hciCommand.parms[5] = 0x00;    /* Turn Off EMI_Hopping Function */
#else
    hciCommand.parms[0] = 0x00;    /* DWA is off at Channel 30, others still ON */
    hciCommand.parms[1] = 0x00;    /* Turn On RX Current Boost Function */
    hciCommand.parms[2] = 0x01;    /* Turn Off 0.8889 interface switch Function */
    hciCommand.parms[3] = 0x00;    /* Turn Off MPLL Hopping Function */
    hciCommand.parms[4] = 0x01;    /* Turn Off DC_Notch Function */
    hciCommand.parms[5] = 0x00;    /* Turn Off EMI_Hopping Function */
#endif
    
    LOG_DBG("GORMcmd_HCC_Set_RF_Desense\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 6, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_RESET(void)
{
    wOpCode = 0x0C03;
    
    LOG_DBG("GORMcmd_HCC_RESET\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 0, NULL) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_Sleep_Control_Reg(void)
{
    DWORD addr, val;
    wOpCode = 0xFCD0;
    
    LOG_DBG("GORMcmd_HCC_Set_Sleep_Control_Reg\n");

#ifdef MTK_COMBO_SUPPORT
    addr = 0x81010074;
    val = 0x000029E2;
#else
    addr = 0x80090074;
    val = 0x00000A82;
#endif

    hciCommand.parms[0] = (UCHAR)((addr) & 0xFF);
    hciCommand.parms[1] = (UCHAR)((addr >> 8) & 0xFF);
    hciCommand.parms[2] = (UCHAR)((addr >> 16) & 0xFF);
    hciCommand.parms[3] = (UCHAR)((addr >> 24) & 0xFF);
    hciCommand.parms[4] = (UCHAR)((val) & 0xFF);
    hciCommand.parms[5] = (UCHAR)((val >> 8) & 0xFF);
    hciCommand.parms[6] = (UCHAR)((val >> 16) & 0xFF);
    hciCommand.parms[7] = (UCHAR)((val >> 24) & 0xFF);
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 8, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

#ifdef MTK_COMBO_SUPPORT
static BtStatus GORMcmd_HCC_Set_TX_Power_Offset(void)
{
    wOpCode = 0xFC93;
    
    hciCommand.parms[0] = btinit->bt_conf.fields.tx_pwr_offset[0];
    hciCommand.parms[1] = btinit->bt_conf.fields.tx_pwr_offset[1];
    hciCommand.parms[2] = btinit->bt_conf.fields.tx_pwr_offset[2];
    
    LOG_DBG("GORMcmd_HCC_Set_TX_Power_Offset\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 3, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_BLEPTA(void)
{
    wOpCode = 0xFCFC;
    
    hciCommand.parms[0] = 0x16;	
    hciCommand.parms[1] = 0x0E;	
    hciCommand.parms[2] = 0x0E;	
    hciCommand.parms[3] = 0x00; 
    hciCommand.parms[4] = 0x07; 
    
    LOG_DBG("GORMcmd_HCC_Set_BLEPTA\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 5, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_Internal_PTA_1(void)
{
    wOpCode = 0xFCFB;   
    
    hciCommand.parms[0] = 0x00;
    hciCommand.parms[1] = 0x01;    /* bt_pta_high_level_tx */
    hciCommand.parms[2] = 0x0F;    /* bt_pta_mid_level_tx */
    hciCommand.parms[3] = 0x0F;    /* bt_pta_low_level_tx */
    hciCommand.parms[4] = 0x01;    /* bt_pta_high_level_rx */
    hciCommand.parms[5] = 0x0F;    /* bt_pta_mid_level_rx */
    hciCommand.parms[6] = 0x0F;    /* bt_pta_low_level_rx */
    hciCommand.parms[7] = 0x01;    /* ble_pta_high_level_tx */
    hciCommand.parms[8] = 0x0F;    /* ble_pta_mid_level_tx */
    hciCommand.parms[9] = 0x0F;    /* ble_pta_low_level_tx */
    hciCommand.parms[10] = 0x01;   /* ble_pta_high_level_rx */
    hciCommand.parms[11] = 0x0F;   /* ble_pta_mid_level_rx */
    hciCommand.parms[12] = 0x0F;   /* ble_pta_low_level_rx */
    hciCommand.parms[13] = 0x02;   /* time_r2g */
    hciCommand.parms[14] = 0x01;   /* always  0x1 */
    
    LOG_DBG("GORMcmd_HCC_Set_Internal_PTA_1\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 15, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_Internal_PTA_2(void)
{
    wOpCode = 0xFCFB;   
     
    hciCommand.parms[0] = 0x01;		
    hciCommand.parms[1] = 0x19;    /* wifi20_hb */
    hciCommand.parms[2] = 0x19;    /* wifi20_hb */
    hciCommand.parms[3] = 0x07;    /* next_rssi_update_bt_slots */
    hciCommand.parms[4] = 0xD0;    /* next_rssi_update_bt_slots */
    hciCommand.parms[5] = 0x00;    /* stream_identify_by_host */
    hciCommand.parms[6] = 0x01;    /* enable auto AFH */
    
    LOG_DBG("GORMcmd_HCC_Set_Internal_PTA_2\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 7, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_SLP_LDOD_VCTRL_Reg(void)
{
    DWORD addr, val;
    wOpCode = 0xFCD0;
    
    LOG_DBG("GORMcmd_HCC_Set_SLP_LDOD_VCTRL_Reg\n");
    
    addr = 0x8102001C;
    val = 0x00000879;
    hciCommand.parms[0] = (UCHAR)((addr) & 0xFF);
    hciCommand.parms[1] = (UCHAR)((addr >> 8) & 0xFF);
    hciCommand.parms[2] = (UCHAR)((addr >> 16) & 0xFF);
    hciCommand.parms[3] = (UCHAR)((addr >> 24) & 0xFF);
    hciCommand.parms[4] = (UCHAR)((val) & 0xFF);
    hciCommand.parms[5] = (UCHAR)((val >> 8) & 0xFF);
    hciCommand.parms[6] = (UCHAR)((val >> 16) & 0xFF);
    hciCommand.parms[7] = (UCHAR)((val >> 24) & 0xFF);
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 8, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_RF_Reg_100(void)
{
    wOpCode = 0xFCB0;
    
    hciCommand.parms[0] = 0x64;	
    hciCommand.parms[1] = 0x01;
    hciCommand.parms[2] = 0x02;
    hciCommand.parms[3] = 0x00;
    hciCommand.parms[4] = 0x00;
    hciCommand.parms[5] = 0x00;
    
    LOG_DBG("GORMcmd_HCC_Set_RF_Reg_100\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 6, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}
#endif

#ifndef MTK_COMBO_SUPPORT
static BtStatus GORMcmd_HCC_Read_Local_Version(void)
{
    wOpCode = 0x1001;
    
    LOG_DBG("GORMcmd_HCC_Read_Local_Version\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 0, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Simulate_MT6612(void)
{
    wOpCode = 0xFCCC;
    hciCommand.parms[0] = 0x00;    /* disable */
    
    LOG_DBG("GORMcmd_HCC_Simulate_MT6612\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 1, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Fix_UART_Escape(void)
{
    DWORD addr, val;
    wOpCode = 0xFCD0;
    
    addr = 0x80060040;
    val = 0x00000077;
    hciCommand.parms[0] = (UCHAR)((addr) & 0xFF);
    hciCommand.parms[1] = (UCHAR)((addr >> 8) & 0xFF);
    hciCommand.parms[2] = (UCHAR)((addr >> 16) & 0xFF);
    hciCommand.parms[3] = (UCHAR)((addr >> 24) & 0xFF);
    hciCommand.parms[4] = (UCHAR)((val) & 0xFF);
    hciCommand.parms[5] = (UCHAR)((val >> 8) & 0xFF);
    hciCommand.parms[6] = (UCHAR)((val >> 16) & 0xFF);
    hciCommand.parms[7] = (UCHAR)((val >> 24) & 0xFF);
    
    LOG_DBG("GORMcmd_HCC_Fix_UART_Escape\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 8, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_GetHwVersion(void)
{
    DWORD addr;
    wOpCode = 0xFCD1;
    
    addr = 0x80000000;
    hciCommand.parms[0] = (UCHAR)((addr) & 0xFF);
    hciCommand.parms[1] = (UCHAR)((addr >> 8) & 0xFF);
    hciCommand.parms[2] = (UCHAR)((addr >> 16) & 0xFF);
    hciCommand.parms[3] = (UCHAR)((addr >> 24) & 0xFF);
    
    LOG_DBG("GORMcmd_HCC_GetHwVersion\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 4, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_GetGormVersion(void)
{
    DWORD addr;
    wOpCode = 0xFCD1;
    
    addr = 0x80000004;
    hciCommand.parms[0] = (UCHAR)((addr) & 0xFF);
    hciCommand.parms[1] = (UCHAR)((addr >> 8) & 0xFF);
    hciCommand.parms[2] = (UCHAR)((addr >> 16) & 0xFF);
    hciCommand.parms[3] = (UCHAR)((addr >> 24) & 0xFF);
    
    LOG_DBG("GORMcmd_HCC_GetGormVersion\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 4, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

/* Implement in BT_SetBaudRate */
static BtStatus GORMcmd_HCC_ChangeBaudRate(void)
{
    return BT_STATUS_NOT_SUPPORTED;
}

/* Implement in BT_DownPatch */
static BtStatus GORMcmd_HCC_WritePatch(void)
{
    return BT_STATUS_NOT_SUPPORTED;
}

static BtStatus GORMcmd_HCC_Set_CapID(void)
{
    wOpCode = 0xFC7F;
    hciCommand.parms[0] = btinit->bt_conf.fields.capId[0];
    
    LOG_DBG("GORMcmd_HCC_Set_CapID\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 1, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_Chip_Feature(void)
{
    wOpCode = 0xFC1E;
    
    hciCommand.parms[0] = 0xBF;
    hciCommand.parms[1] = 0xFE;
    hciCommand.parms[2] = 0x8D;
    hciCommand.parms[3] = 0xFE;
    hciCommand.parms[4] = 0x98;
    hciCommand.parms[5] = 0x1F;
    hciCommand.parms[6] = 0x59;
    hciCommand.parms[7] = 0x87;
    
    LOG_DBG("GORMcmd_HCC_Set_Chip_Feature\r\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 8, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Enable_PTA(void)
{
    wOpCode = 0xFCD2;
    hciCommand.parms[0] = 0x00;
    
    LOG_DBG("GORMcmd_HCC_Enable_PTA\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 1, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_WiFi_Ch(void)
{
    wOpCode = 0xFCD3;
    
    hciCommand.parms[0] = 0x00;
    hciCommand.parms[1] = 0x0A;
    
    LOG_DBG("GORMcmd_HCC_Set_WiFi_Ch\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 2, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static BtStatus GORMcmd_HCC_Set_AFH_Mask(void)
{
    wOpCode = 0x0C3F;
    
    hciCommand.parms[0] = 0xFF;
    hciCommand.parms[1] = 0xFF;
    hciCommand.parms[2] = 0xFF;
    hciCommand.parms[3] = 0xFF;
    hciCommand.parms[4] = 0xFF;
    hciCommand.parms[5] = 0xFF;
    hciCommand.parms[6] = 0xFF;
    hciCommand.parms[7] = 0xFF;
    hciCommand.parms[8] = 0xFF;
    hciCommand.parms[9] = 0x7F;
    
    LOG_DBG("GORMcmd_HCC_Set_AFH_Mask\n");
    
    if(BT_SendHciCommand(bt_comm_port, wOpCode, 10, &hciCommand) == TRUE){
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}
#endif


DWORD GORM_Init(
    HANDLE  hComPort,
    PUCHAR  pucPatchExtData,
    DWORD   dwPatchExtLen,
    PUCHAR  pucPatchData,
    DWORD   dwPatchLen,
    PBYTE   pucConfData,
    DWORD   dwBaud,
    DWORD   dwHostBaud,
    DWORD   dwFlowControl
    )
{
    int  i = 0;
    BtStatus btStatus = BT_STATUS_SUCCESS;
    UCHAR ucEventBuf[MAX_EVENT_SIZE];
    DWORD dwEventLen;
    
    LOG_DBG("GORM_Init\n");
    
    // Save comm port fd for GORMcmd use
    bt_comm_port = hComPort;
    
    // Copy configuration data
    memcpy(btinit->bt_conf.raw, pucConfData, sizeof(struct btradio_conf_data));

#ifdef MTK_COMBO_SUPPORT
	if(0x6620 == g_chipId){
		btinit->cur_script = bt_init_script_6620;
	}
	else if(0x6628 == g_chipId){
		btinit->cur_script = bt_init_script_6628;
	}
#else
	btinit->cur_script = bt_init_script_6622;
#endif
	
    if((btinit->cur_script) == NULL){
        LOG_ERR("No matching init script\n");
        return BT_STATUS_FAILED;
    }
    
    i = 0;
    while(btinit->cur_script[i].command_func){

#ifndef MTK_COMBO_SUPPORT
        if(GORMcmd_HCC_ChangeBaudRate == btinit->cur_script[i].command_func)
        {
            if(BT_SetBaudRate(hComPort, dwBaud, dwHostBaud, dwFlowControl) == FALSE){
                LOG_ERR("Change UART baudrate fails\n");
                return BT_STATUS_FAILED;
            }            
            i ++;
            continue;
        }
        
        if(GORMcmd_HCC_WritePatch == btinit->cur_script[i].command_func)
        {
            if((pucPatchData == NULL) || (dwPatchLen == 0)){
                LOG_ERR("No valid patch data\n");
            }
            else{
                if(BT_DownPatch(hComPort, pucPatchData, dwPatchLen) == FALSE){
                    LOG_ERR("Download patch fails\n");
                    return BT_STATUS_FAILED;
                }
            }
            i ++;
            continue;
        }
#endif

        btStatus = btinit->cur_script[i].command_func();
        
        if(btStatus == BT_STATUS_CANCELLED){
            i ++;
            continue;//skip
        }
        
        if(btStatus == BT_STATUS_FAILED){
            LOG_ERR("Send command %d fails\n", i);
            return btStatus;
        }
        
        if(BT_ReadExpectedEvent(
            hComPort, 
            ucEventBuf, 
            MAX_EVENT_SIZE, 
            0x0E, 
            &dwEventLen, 
            TRUE, 
            wOpCode, 
            TRUE, 
            0x00) == FALSE){
            
            LOG_ERR("Read event of command %d fails\n", i);
            btStatus = BT_STATUS_FAILED;
            return btStatus;
        }
        
        i ++;
    }
    
    return btStatus;
}


static BOOL BT_Get_Local_BD_Addr(PUCHAR ucBDAddr)
{
    LOG_DBG("BT_Get_Local_BD_Addr\n");
    
    USHORT  opCode = 0x1009;
    DWORD   dwReadLength = 0;
    UCHAR   pAckEvent[20];
    
    if(BT_SendHciCommand(bt_comm_port, opCode, 0, NULL) == FALSE){
        LOG_ERR("Send get BD address command fails\n");
        return FALSE;
    }
    
    // Read local BD addr in firmware
    if(BT_ReadExpectedEvent(
        bt_comm_port, 
        pAckEvent, 
        sizeof(pAckEvent),
        0x0E,
        &dwReadLength,
        TRUE,
        0x1009,
        TRUE,
        0x00) == FALSE){
        
        LOG_ERR("Read local BD address fails\n");
        return FALSE;
    }
    
    LOG_WAN("Local BD address: %02x-%02x-%02x-%02x-%02x-%02x\n",
            pAckEvent[12], pAckEvent[11], pAckEvent[10], 
            pAckEvent[9], pAckEvent[8], pAckEvent[7]);
    
    ucBDAddr[0] = pAckEvent[12];
    ucBDAddr[1] = pAckEvent[11];
    ucBDAddr[2] = pAckEvent[10];
    ucBDAddr[3] = pAckEvent[9];
    ucBDAddr[4] = pAckEvent[8];
    ucBDAddr[5] = pAckEvent[7];
    
    return TRUE;
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
