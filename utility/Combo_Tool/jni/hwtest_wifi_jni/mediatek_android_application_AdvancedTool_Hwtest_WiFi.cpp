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

/* //device/libs/android_runtime/android_media_MediaPlayer.cpp
 **
 ** Copyright 2007, The Android Open Source Project
 **
 ** Licensed under the Apache License, Version 2.0 (the "License");
 ** you may not use this file except in compliance with the License.
 ** You may obtain a copy of the License at
 **
 **     http://www.apache.org/licenses/LICENSE-2.0
 **
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 */

//#define HWTEST_LOG_NDEBUG 0
//#define HWTEST_LOG_TAG "emBtTest-JNI"
//#include "utils/Log.h"


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <utils/threads.h>
#include <dlfcn.h>
#include <string.h>
#include "jni.h"
#include "JNIHelp.h"
#include <errno.h>
#include "android_runtime/AndroidRuntime.h"
#include "utils/Errors.h"  // for status_t
#undef LOG_NDEBUG 
#undef NDEBUG

#include "hwtest_log.h"
#include "hwtest_wifi.h"

#ifdef HWTEST_LOG_TAG
#undef HWTEST_LOG_TAG
#define HWTEST_LOG_TAG "HWTest_WiFi_JNI"
#endif

#include <linux/capability.h>
#include <linux/prctl.h>
#include <cutils/properties.h>
#include <private/android_filesystem_config.h>
#ifdef HAVE_LIBC_SYSTEM_PROPERTIES
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#endif

//extern "C" {
//#include "libwifitest.h"
//}

//#include <cutils/xlog.h>
//#include <cutils/log.h>
#include <cutils/sockets.h>
//#include <cutils/properties.h>

 
// ----------------------------------------------------------------------------

using namespace android;
#define TX_DATA_RATE_COUNT 12
#define TX_MCS_RATE_COUNT  8

#define CONTINUOUS_PACKET_TX 0
#define TX_OUTOUT_POWER      1
#define CARRIER_SUPPRESSION  2
#define LOCAL_LEAKAGE        3
#define ENTER_POWER_OFF      4

struct fields_t {
	jfieldID ChannelIdx;
	jfieldID BandwidthIdx;
	jfieldID TxGain;
	jfieldID PadloadLen;
	jfieldID Count;
	jfieldID PreambleIdx;
	jfieldID RateIdx;
	jfieldID ModeIdx;
	jfieldID GITypeIdx;
	jfieldID FreqOffset;
};
static fields_t fields;

typedef struct tagWiFiTxParam
{
	int iChannelIdx;
	int iBandwidthIdx;
	int iTxGain;
	int iPadloadLen;
	int iCount;
	int iPreambleIdx;
	int iRateIdx;
	int iModeIdx;
	int iGITypeIdx;
	int iFreqOffset;
}WiFiTxParam;
WiFiTxParam g_WiFiTxParam;

#define INTERFACE_PROP_NAME "wifi.interface"

int g_ChipType = INIT_CHIP;

/***************************************************************************
 * 
 * For MT5931、MT6620、MT6628 WiFi Rx/Tx hwtest
 * 
 * *************************************************************************/
#define LIB_WIFITEST "libwifitest.so"
static void *glib_handle = NULL;

static int SIFS = 20;

int CHANEL_MAP[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
int BANDWIDTH_MAP[] = {WIFI_TEST_BW_20,
					 WIFI_TEST_BW_40,
					 WIFI_TEST_BW_20U,
					 WIFI_TEST_BW_20L};
					 
int MODE_MAP[]= {CONTINUOUS_PACKET_TX,
				TX_OUTOUT_POWER,
				CARRIER_SUPPRESSION,
				LOCAL_LEAKAGE
				};
		
static unsigned char macAddr[] = {0x00, 0x80, 0x12, 0x13, 0x14, 0x15};
int RATE_MAP[] = {WIFI_TEST_RATE_1MBPS,
				  WIFI_TEST_RATE_2MBPS,
				  WIFI_TEST_RATE_5_5MBPS,
				  WIFI_TEST_RATE_6MBPS,
				  WIFI_TEST_RATE_9MBPS,
				  WIFI_TEST_RATE_11MBPS,
				  WIFI_TEST_RATE_12MBPS,
				  WIFI_TEST_RATE_18MBPS,
				  WIFI_TEST_RATE_24MBPS,
				  WIFI_TEST_RATE_36MBPS,
				  WIFI_TEST_RATE_48MBPS,
				  WIFI_TEST_RATE_54MBPS,
				};

int GITYPE_MAP[] = {0, 1};
				  
int PREABLE11N_MAP[] = {WIFI_TEST_PREAMBLE_TYPE_MIXED_MODE,
					  WIFI_TEST_PREAMBLE_TYPE_GREENFIELD};
/***************************************************************************
 * 
 * For MT7601、MT5370 WiFi Rx/Tx hwtest
 * 
 * *************************************************************************/

ATE_CMD_st g_iwpriv_cmd_tx[] = 
{
	{ATE, "ATE", "ATESTART"},
	{ATEDA, "ATEDA", "00:11:22:33:44:55"},
	{ATESA, "ATESA", "00:aa:bb:cc:dd:ee"},
	{ATEBSSID, "ATEBSSID", "00:11:22:33:44:55"},
	{ATECHANNEL, "ATECHANNEL", "1"},
	{ATETXMODE, "ATETXMODE", "2"},
	{ATETXMSC, "ATETXMCS", "1"},
	{ATETXBW, "ATETXBW", "1"},
	{ATETXGI, "ATETXGI", "0"},
	{ATETXLEN, "ATETXLEN", "1024"},
//	{ATETXFREQOFFSET, "ATETXFREQOFFSET", "10"},
//	{ATETXANT, "ATETXANT", "1"},
	{ATETXPOW0, "ATETXPOW0", "15"},
//	{ATETXPOW1, "ATETXPOW1", "15"},
//	{ATETXPOW2, "ATETXPOW2", "15"},
	{ATETXCNT, "ATETXCNT", "100000"},
	{ATEMODETYPE, "ATE", "TXFRAME"},
	{ATE_CMD_ENUM_MAX, "", ""},
};

ATE_CMD_st g_iwpriv_cmd_rx[] = 
{
	{ATE, "ATE", "ATESTART"},
	{ATECOUNTRESET, "ResetCounter", "0"},
	{ATECHANNEL, "ATECHANNEL", "1"},
	{ATETXMODE, "ATETXMODE", "2"},
//	{ATETXMSC, "ATETXMSC", "7"},
	{ATETXBW, "ATETXBW", "1"},
	{ATETXGI, "ATETXGI", "0"},
//	{ATETXFREQOFFSET, "ATETXFREQOFFSET", "36"},
	{ATETXANT, "ATETXANT", "0"},
	{ATEMODETYPE, "ATE", "RXFRAME"},
	{ATE_CMD_ENUM_MAX, "", ""},
};
					  
// ----------------------------------------------------------------------------



static int InitChiperType(void)
{
	char szInterfaceName[16] = "";
	char szTestCmd[ATE_COMMAND_LENGTH] = {0};
	char MT76xxKeyword[] = "driverVer";
	char MT662xKeyword[] = "set_test_mode";

	FILE *fpOut = NULL;
	char szResult[126] = "";
	
	HWTEST_LOGD("Enter InitChiperType()...\n");
	
	snprintf(szTestCmd, ATE_COMMAND_LENGTH, "iwpriv %s", MT76xx_WLAN_INTERFACE);
				
	fpOut = popen(szTestCmd, "r");
	if (NULL == fpOut)
	{
		HWTEST_LOGE("%s: popen error!\n", __func__);
		return ERROR;
	}
		
	while (NULL != fgets(szResult, 126, fpOut))
	{
		if (NULL != strstr(szResult, MT76xxKeyword))
		{
			g_ChipType = MT76xx;
			HWTEST_LOGD("Chip type = MT76xx");
			break;
		}
		else if (NULL != strstr(szResult, MT662xKeyword))
		{
			g_ChipType = MT662x;
			HWTEST_LOGD("Chip type = MT662x");
			break;
		}
		
	}
	pclose(fpOut);
		
	HWTEST_LOGD("Leave InitChiperType()...\n");
	
	return 0;
}

/*
static int InitChiperType(void)
{
	char szInterfaceName[16] = "";
	
	if (property_get(INTERFACE_PROP_NAME, szInterfaceName, NULL)) 
	{
		HWTEST_LOGD("property_get %s=%s", INTERFACE_PROP_NAME, szInterfaceName);
		if (0 == strncmp(szInterfaceName, MT662x_WLAN_INTERFACE, strlen(szInterfaceName)))
		{
			g_ChipType = MT662x;
		}
		else if(0 == strncmp(szInterfaceName, MT76xx_WLAN_INTERFACE, strlen(szInterfaceName)))
		{
			g_ChipType = MT76xx;
		}
        return 0;
    }
	
	return -1;
}
*/

static void debug_init_param(JNIEnv *env, jobject thiz)
{
	HWTEST_LOGD("ChannelIdx    = %d ", (int) env->GetIntField(thiz, fields.ChannelIdx));
	HWTEST_LOGD("BandwidthIdx  = %d ", (int) env->GetIntField(thiz, fields.BandwidthIdx));
	HWTEST_LOGD("TxGain        = %d ", (int) env->GetIntField(thiz, fields.TxGain));
	HWTEST_LOGD("PadloadLen    = %d ", (int) env->GetIntField(thiz, fields.PadloadLen));
	HWTEST_LOGD("Count         = %d ", (int) env->GetIntField(thiz, fields.Count));
	HWTEST_LOGD("PreambleIdx   = %d ", (int) env->GetIntField(thiz, fields.PreambleIdx));
	HWTEST_LOGD("RateIdx       = %d ", (int) env->GetIntField(thiz, fields.RateIdx));
	HWTEST_LOGD("ModeIdx       = %d ", (int) env->GetIntField(thiz, fields.ModeIdx));
	HWTEST_LOGD("GITypeIdx     = %d ", (int) env->GetIntField(thiz, fields.GITypeIdx));
	HWTEST_LOGD("FreqOffset    = %d ", (int) env->GetIntField(thiz, fields.FreqOffset));
}

/*
int os_program_init(void)
{
//#ifdef ANDROID
	// We ignore errors here since errors are normal if we
	// are already running as non-root.
	 
	gid_t groups[] = { AID_INET, AID_WIFI, AID_KEYSTORE };
	setgroups(sizeof(groups)/sizeof(groups[0]), groups);

	prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0);

	if (-1 == setgid(AID_WIFI))
	{
		HWTEST_LOGE("setgid [%d] error", AID_WIFI);
	}
	if (-1 == setuid(AID_WIFI))
	{
		HWTEST_LOGE("setuid [%d] error", AID_WIFI);
	}

	struct __user_cap_header_struct header;
	struct __user_cap_data_struct cap;
	header.version = _LINUX_CAPABILITY_VERSION;
	header.pid = 0;
	cap.effective = cap.permitted =
		(1 << CAP_NET_ADMIN) | (1 << CAP_NET_RAW);
	cap.inheritable = 0;
	capset(&header, &cap);
	HWTEST_LOGE("os_program_init\n");
//#endif


	return 0;
}
*/


/***************************************************************************
 * 
 * For MT7601、MT5370 WiFi Rx/Tx hwtest
 * 
 * *************************************************************************/
#define BINAARY 	0x01
#define OCTAL 		0x02
#define DECIMAL 	0x03
#define HEXADECIMAL 0x04
static int Str2Int(char *pStr, int iSystem)
{
	int iResult = 0;
	char *pCur = NULL;
	int iCount = 0;
	int iPostion = 0;
	int iMulNum = 0;
	int iMulSum = 0;
	int iMulTmp = 0;
	
	if (NULL == pStr)
	{
		return -1;
	}
	
	iCount = strlen(pStr);
	//pCur pointer the last character of pStr
	pCur = pStr + iCount -1;
	printf("%d\n", iCount);
	
	switch (iSystem)
	{
		case BINAARY:
			break;
		case OCTAL:
			break;
		case DECIMAL:
			break;
		case HEXADECIMAL:
			iMulNum = 16;
			iMulSum = 1;
			while (iPostion < iCount)
			{
				if (0 != iPostion)
				{
					iMulSum = iMulSum * iMulNum;
				}
				
				if (*pCur >= 0x30 && *pCur <= 0x39)
				{
					iResult += (*pCur - 0x30) * iMulSum; 
				}
				else if (*pCur >= 0x61 && *pCur <= 0x66)
				{
					iResult += (*pCur - 0x61 + 10) * iMulSum;
				}
				else if (*pCur >= 0x41 && *pCur <= 0x46)
				{
					iResult += (*pCur - 0x41 + 10) * iMulSum;
				}
				else
				{
					return -1;
				}
				iPostion ++;
				pCur --;
			}
			break;
		default:
			break;
	}
	
	return iResult;
}

static char *Int2Str(int iNumber, char *pString, int iSystem)
{
	char szTmp[64] = {0};
	char *pRet = NULL;
	int sign = 0;
	int iNum = 0;
	int iRe = 0;
	int i = 0;
	
	iNum = iNumber;
	pRet = pString;

	if (0 == iNum)
	{
		*pRet = '0';
		return pString;
	}
	
	if (iNum < 0)
	{
		sign = 1;
		iNum = -iNumber;
		*pRet++ = '-';
	}

	while (iNum > 0)
	{
		iRe = iNum % iSystem;
		iNum = iNum / iSystem;
		if (iRe < 10)
		{
			szTmp[i] = '0' + iRe;
		}
		else
		{
			szTmp[i] = 'a' + iRe - 10;
		}
		i ++;
	}

	while (i > 0)
	{
		*pRet++ = szTmp[--i];
	}
	*pRet = 0;
	
	return pString;
}

static int GetEfuseValue(char *pAddress)
{
	char pATECmdBuf[ATE_COMMAND_LENGTH] = "";
	FILE *fpOut = NULL;
	char szBuf[126] = "";
	char szRst[16] = "";
	char szKey[] = "0x";
	int iResult = 0;
	
	char *pCur = NULL;
	int iCount = 0;
	int iNeedNum = 2;
	
	snprintf(pATECmdBuf, ATE_COMMAND_LENGTH, "/system/xbin/mysu /system/xbin/iwpriv %s e2p %s", MT76xx_WLAN_INTERFACE, pAddress);
	HWTEST_LOGD("Send ATE CMD : %s", pATECmdBuf);	
	fpOut = popen(pATECmdBuf, "r");
	
	if (NULL == fpOut)
	{
		HWTEST_LOGE("%s: popen error!\n", __func__);
		return ERROR;
	}
		
	while (NULL != fgets(szBuf, 126, fpOut))
	{
		if (NULL != strstr(szBuf, szKey))
		{
			iCount = strlen(szBuf);
			HWTEST_LOGD("%s", szBuf);
			
			pCur = strstr(szBuf, ":");
			//[0x003a]:0x014C
			strncpy(szRst, pCur + 5, iNeedNum);
		
			HWTEST_LOGD("0x%s = %s", pAddress, szRst);
			iResult = Str2Int(szRst, HEXADECIMAL);
		}
		memset(szBuf, 0, sizeof(szBuf));
	}
	
	pclose(fpOut);
	
	return iResult;
}

/*
static int Str2Int(char *pString)
{

}
*/
static int GetValue(char *pString)
{
	int iRst = 0;
	char *pStart = NULL;
	//pString : "Rx success              = 1230"
	pStart = strstr(pString, "=");
	if (NULL == pStart)
	{
		HWTEST_LOGE("strstr error!");
		return -1;
	}
	pStart = pStart + 2;

	iRst = atoi(pStart);
	
	return iRst;
}

int UpdateATECommand(ATE_CMD_st *ATE_CMD_List, ATE_CMD_INDEX KeyWordIndex, char *pValue)
{
	if (NULL == pValue || KeyWordIndex > ATE_CMD_ENUM_MAX)
	{
		HWTEST_LOGE("%s: Value is NULL or Index invalid", __func__);
		return ERROR;
	}
	
	int i = 0;
	while (TRUE)
	{
		if (ATE_CMD_ENUM_MAX == ATE_CMD_List[i].iIndex)
		{
			HWTEST_LOGD("%s: KeyWord Index %d can't find", __func__, KeyWordIndex);
			break;
		}
		
		if (KeyWordIndex == ATE_CMD_List[i].iIndex)
		{
			HWTEST_LOGD("%s: Before update %s = %s\n", __func__, ATE_CMD_List[i].szKeyword, 
											ATE_CMD_List[i].szValue);
			memset(ATE_CMD_List[i].szValue, '\0', ATE_VALUE_LENGTH);
			strncpy(ATE_CMD_List[i].szValue, pValue, strlen(pValue));
			HWTEST_LOGD("%s: After update %s = %s\n", __func__, ATE_CMD_List[i].szKeyword, 
											ATE_CMD_List[i].szValue);
			break;
		}

		i ++;
	}
	
	return SUCCESS;
} 

int SendATECommand(ATE_CMD_st *ATE_CMD_List)
{
	char pATECmdBuf[ATE_COMMAND_LENGTH] = "";
//	FILE *fpOut = NULL;
	char pResult[1024] = "";
	
	if (NULL == ATE_CMD_List)
	{
		HWTEST_LOGE("%s: ATE CMD List is NULL\n", __func__);
		return ERROR;
	}
	
	int i = 0;
	while (TRUE)
	{
		if (ATE_CMD_ENUM_MAX == ATE_CMD_List[i].iIndex)
		{
			HWTEST_LOGD("%s: ATE CMD Send done!\n", __func__);
			break;
		}
		memset(pATECmdBuf, '\0', ATE_COMMAND_LENGTH);
		snprintf(pATECmdBuf, ATE_COMMAND_LENGTH, "/system/xbin/mysu /system/xbin/iwpriv %s set %s=%s", MT76xx_WLAN_INTERFACE, 
							 ATE_CMD_List[i].szKeyword, ATE_CMD_List[i].szValue);
/*
		fpOut = popen(pATECmdBuf, "r");
		if (NULL == fpOut)
		{
			LOGE("%s: popen error!\n", __func__);
			return ERROR;
		}
		
		fgets(pResult, 1024, fpOut);
		printf("%s\n", pResult);
		pclose(fpOut);
*/		
		if (system(pATECmdBuf) < 0)
		{
			HWTEST_LOGE(" CMD: %s error", pATECmdBuf);
		}
		HWTEST_LOGD("[%d] %s\n", i, pATECmdBuf);
		i ++;
	}
	
	return SUCCESS;
}

static bool MT76xx_RxStart(int iChannelIndex, int iBWIndex)
{
	bool retval = false;
	HWTEST_LOGD("Enter MT76xx_RxStart()...\n");
	int iChannel = 0;
	int iBandwidth = 0;
	char szTmp[32] = {0};

	iChannel = CHANEL_MAP[iChannelIndex];
	iBandwidth = BANDWIDTH_MAP[iBWIndex];
		
	UpdateATECommand(g_iwpriv_cmd_rx, ATECHANNEL, Int2Str(iChannel, szTmp, 10));
	memset(szTmp, 0, sizeof(szTmp));
	UpdateATECommand(g_iwpriv_cmd_rx, ATETXBW, Int2Str(iBandwidth, szTmp, 10));

	if (ERROR == SendATECommand(g_iwpriv_cmd_rx))
	{
		HWTEST_LOGE("Send Rx ATE Command Error");
		return false;
	}
	
	HWTEST_LOGD("Leave MT76xx_RxStart()...\n");
	return true;
}

static bool MT76xx_RxStop(void)
{
	bool retval = false;
	HWTEST_LOGD("Enter MT76xx_RxStop()...\n");

	char pATECmdBuf[ATE_COMMAND_LENGTH] = "";

	snprintf(pATECmdBuf, ATE_COMMAND_LENGTH, "/system/xbin/mysu /system/xbin/iwpriv %s set ATE=ATESTART", MT76xx_WLAN_INTERFACE);

	if ((retval = system(pATECmdBuf)) < 0)
	{
		HWTEST_LOGE(" CMD: %s error", pATECmdBuf);
	}
	
	HWTEST_LOGD("Send CMD: %s", pATECmdBuf);	
	HWTEST_LOGD("Leave MT76xx_RxStop()...\n");
	return retval;
}



static bool MT76xx_GetRxResult(int *iRxResult)
{
	bool retval = false;
	HWTEST_LOGD("Enter MT76xx_GetRxResult()...\n");

	char szATERxReusltBuf[ATE_COMMAND_LENGTH] = "";
	char szRxOk[] = "Rx success";
	char szRxCRCError[] = "Rx with CRC";
	char szRxSSI[] = "RSSI-A";
	
	FILE *fpOut = NULL;
	char szResult[126] = "";

	snprintf(szATERxReusltBuf, ATE_COMMAND_LENGTH, "/system/xbin/mysu /system/xbin/iwpriv %s stat", MT76xx_WLAN_INTERFACE);
			
	fpOut = popen(szATERxReusltBuf, "r");
	if (NULL == fpOut)
	{
		HWTEST_LOGE("%s: popen error!\n", __func__);
		return ERROR;
	}
		
	while (NULL != fgets(szResult, 126, fpOut))
	{
		if (NULL != strstr(szResult, szRxOk))
		{
//			printf("%s\n", szResult);
			iRxResult[0] = GetValue(szResult);
			HWTEST_LOGD("%s", szResult);
			HWTEST_LOGD("Rx Ok = %d", iRxResult[0]);
		}
		else if (NULL != strstr(szResult, szRxCRCError))
		{
			iRxResult[1] = GetValue(szResult);
//			printf("%s\n", szResult);
			HWTEST_LOGD("%s", szResult);
			HWTEST_LOGD("CRC ERR = %d", iRxResult[1]);
		}
		else if (NULL != strstr(szResult, szRxSSI))
		{
			iRxResult[2] = GetValue(szResult);
//			printf("%s\n", szResult);
			HWTEST_LOGD("%s", szResult);
			HWTEST_LOGD("RSSI = %d", iRxResult[2]);
		}
	}
	HWTEST_LOGD("Send CMD: %s", szATERxReusltBuf);	
	pclose(fpOut);
	
	HWTEST_LOGD("Leave MT76xx_GetRxResult()...\n");
	return retval;
}

static bool MT76xx_TxStart(void)
{
	bool retval = false;
	char szTmp[64] = {0};

	int i11bRateEnd = 3;
	int i11gRateEnd = 12;
	int iRateMCS32 = 20;
	int i11bgModeEnd = 2;
	
	int iChannel = 0;
	int iBandwidth = 0;
	int iTxGain = 0;
	int iPadloadLen = 0;
	int iCount = 0;
	int iModeType = 0;
	int iGIType = 0;
	
	int iRateIdx = 0;
	int iRate = 0;
	int iPreambleIdx = 0;
	int iPreamble = 0;
	int iFreqOffset = 30;
	
	HWTEST_LOGD("Enter MT76xx_TxStart()...\n");
	
	iChannel = CHANEL_MAP[g_WiFiTxParam.iChannelIdx];
	iBandwidth = BANDWIDTH_MAP[g_WiFiTxParam.iBandwidthIdx];
	iModeType = MODE_MAP[g_WiFiTxParam.iModeIdx];	
	iGIType = GITYPE_MAP[g_WiFiTxParam.iGITypeIdx];
	
	iTxGain = g_WiFiTxParam.iTxGain;
	iPadloadLen = g_WiFiTxParam.iPadloadLen;
	iCount = g_WiFiTxParam.iCount;
	iFreqOffset = g_WiFiTxParam.iFreqOffset;

	if (iPadloadLen < 24 || iPadloadLen > 1500)
	{
		iPadloadLen = 1024;
	}

	if (iFreqOffset < 0 || iFreqOffset > 63)
	{
		iFreqOffset = 30;
	}

	iRateIdx = g_WiFiTxParam.iRateIdx;
	iPreambleIdx = g_WiFiTxParam.iPreambleIdx;	
	
	memset(szTmp, 0, sizeof(szTmp));
	UpdateATECommand(g_iwpriv_cmd_tx, ATECHANNEL, Int2Str(iChannel, szTmp, 10));
	memset(szTmp, 0, sizeof(szTmp));
	UpdateATECommand(g_iwpriv_cmd_tx, ATETXGI, Int2Str(iGIType, szTmp, 10));
	memset(szTmp, 0, sizeof(szTmp));
	UpdateATECommand(g_iwpriv_cmd_tx, ATETXLEN, Int2Str(iPadloadLen, szTmp, 10));	
	memset(szTmp, 0, sizeof(szTmp));
	UpdateATECommand(g_iwpriv_cmd_tx, ATETXPOW0, Int2Str(iTxGain, szTmp, 10));
	memset(szTmp, 0, sizeof(szTmp));
	
	UpdateATECommand(g_iwpriv_cmd_tx, ATETXCNT, Int2Str(iCount, szTmp, 10));
//	memset(szTmp, 0, sizeof(szTmp));
	
//	UpdateATECommand(g_iwpriv_cmd_tx, ATETXFREQOFFSET, Int2Str(iFreqOffset, szTmp, 10));

//	memset(szTmp, 0, sizeof(szTmp));
//	UpdateATECommand(g_iwpriv_cmd_tx, ATETXBW, Int2Str(iBandwidth, szTmp, 10));


	if (iRateIdx < i11bRateEnd) //iRateIdex :0-2
	{
		iRate = iRateIdx; //(MCS = 0, 1, 2)
		iPreamble = 0; //Mode = 0 (Rate = 1, 2, 5.5Mbps)
		iBandwidth = 0;
		memset(szTmp, 0, sizeof(szTmp));
		UpdateATECommand(g_iwpriv_cmd_tx, ATETXMODE, Int2Str(iPreamble, szTmp, 10));	
		memset(szTmp, 0, sizeof(szTmp));
		UpdateATECommand(g_iwpriv_cmd_tx, ATETXMSC, Int2Str(iRate, szTmp, 10));
		memset(szTmp, 0, sizeof(szTmp));
		UpdateATECommand(g_iwpriv_cmd_tx, ATETXBW, Int2Str(iBandwidth, szTmp, 10));
	}
	else if (iRateIdx < i11bRateEnd + 2)//iRateIdex :3-4
	{
		iRate = iRateIdx - i11bRateEnd; //(MCS = 0, 1)
		iPreamble = 1; //Mode = 1 (Rate = 6Mbps and 9Mbps)
		iBandwidth = 0;
		memset(szTmp, 0, sizeof(szTmp));
		UpdateATECommand(g_iwpriv_cmd_tx, ATETXMODE, Int2Str(iPreamble, szTmp, 10));	
		memset(szTmp, 0, sizeof(szTmp));
		UpdateATECommand(g_iwpriv_cmd_tx, ATETXMSC, Int2Str(iRate, szTmp, 10));
		memset(szTmp, 0, sizeof(szTmp));
		UpdateATECommand(g_iwpriv_cmd_tx, ATETXBW, Int2Str(iBandwidth, szTmp, 10));
	}
	else if (iRateIdx == i11bRateEnd + 2)//iRateIdex :5
	{
		iRate = 3; //(MCS = 3)
		iPreamble = 0; //Mode = 0 (Rate = 11Mbps)
		iBandwidth = 0;
		memset(szTmp, 0, sizeof(szTmp));
		UpdateATECommand(g_iwpriv_cmd_tx, ATETXMODE, Int2Str(iPreamble, szTmp, 10));	
		memset(szTmp, 0, sizeof(szTmp));
		UpdateATECommand(g_iwpriv_cmd_tx, ATETXMSC, Int2Str(iRate, szTmp, 10));
		memset(szTmp, 0, sizeof(szTmp));
		UpdateATECommand(g_iwpriv_cmd_tx, ATETXBW, Int2Str(iBandwidth, szTmp, 10));
	}
	else if (iRateIdx < i11gRateEnd)//iRateIdex :6 - 11
	{	
		iRate = iRateIdx - i11bRateEnd - 1;
		iPreamble = 1; //Mode = 1 (Rate = 12, 18, 24, 36, 48, 54Mbps)
		iBandwidth = 0;
		memset(szTmp, 0, sizeof(szTmp));
		UpdateATECommand(g_iwpriv_cmd_tx, ATETXMODE, Int2Str(iPreamble, szTmp, 10));	
		memset(szTmp, 0, sizeof(szTmp));
		UpdateATECommand(g_iwpriv_cmd_tx, ATETXMSC, Int2Str(iRate, szTmp, 10));
		memset(szTmp, 0, sizeof(szTmp));
		UpdateATECommand(g_iwpriv_cmd_tx, ATETXBW, Int2Str(iBandwidth, szTmp, 10));
	}
	else if (iRateIdx < iRateMCS32)//MCS0 - MCS7
	{	
		iRate = iRateIdx - i11gRateEnd;
		iPreamble = iPreambleIdx + i11bgModeEnd;
		memset(szTmp, 0, sizeof(szTmp));
		UpdateATECommand(g_iwpriv_cmd_tx, ATETXMODE, Int2Str(iPreamble, szTmp, 10));
		memset(szTmp, 0, sizeof(szTmp));
		UpdateATECommand(g_iwpriv_cmd_tx, ATETXMSC, Int2Str(iRate, szTmp, 10));
		memset(szTmp, 0, sizeof(szTmp));
		UpdateATECommand(g_iwpriv_cmd_tx, ATETXBW, Int2Str(iBandwidth, szTmp, 10));
	}
	else //MCS32
	{
		iRate = 32;
		iPreamble = iPreambleIdx + i11bgModeEnd;
		iBandwidth = 1;
		memset(szTmp, 0, sizeof(szTmp));
		UpdateATECommand(g_iwpriv_cmd_tx, ATETXMODE, Int2Str(iPreamble, szTmp, 10));
		memset(szTmp, 0, sizeof(szTmp));
		UpdateATECommand(g_iwpriv_cmd_tx, ATETXMSC, Int2Str(iRate, szTmp, 10));
		memset(szTmp, 0, sizeof(szTmp));
		UpdateATECommand(g_iwpriv_cmd_tx, ATETXBW, Int2Str(iBandwidth, szTmp, 10));
	}
	
	switch (iModeType)
	{
		case CONTINUOUS_PACKET_TX:
			UpdateATECommand(g_iwpriv_cmd_tx, ATEMODETYPE, (char *)"TXFRAME");
			break;
			
		case TX_OUTOUT_POWER:
			UpdateATECommand(g_iwpriv_cmd_tx, ATEMODETYPE, (char *)"TXCONT");
			break;
			
		case CARRIER_SUPPRESSION:
			UpdateATECommand(g_iwpriv_cmd_tx, ATEMODETYPE, (char *)"TXCARR");
			break;
			
		case LOCAL_LEAKAGE:
			//MT7601&MT5370 does not support Local leakage, use normal mode type
			UpdateATECommand(g_iwpriv_cmd_tx, ATEMODETYPE, (char *)"TXFRAME");
			break;
	}	


	if (ERROR == SendATECommand(g_iwpriv_cmd_tx))
	{
		HWTEST_LOGE("Send Tx ATE Command Error");
		return false;
	}
	
	return true;
}

static bool MT76xx_TxStop(void)
{
	bool retval = false;
	HWTEST_LOGD("Enter MT76xx_TxStop()...\n");

	char pATECmdBuf[ATE_COMMAND_LENGTH] = "";

	snprintf(pATECmdBuf, ATE_COMMAND_LENGTH, "/system/xbin/mysu /system/xbin/iwpriv %s set ATE=ATESTART", MT76xx_WLAN_INTERFACE);

	if ((retval = system(pATECmdBuf)) < 0)
	{
		HWTEST_LOGE(" CMD: %s error", pATECmdBuf);
	}

	HWTEST_LOGD("Send CMD: %s", pATECmdBuf);	
	HWTEST_LOGD("Leave MT76xx_TxStop()...\n");
	return retval;
}



/***************************************************************************
 * 
 * For MT5931、MT6620、MT6628 WiFi Rx/Tx hwtest
 * 
 * *************************************************************************/

static bool InitWifitestLib(void)
{
	bool bRst = false;
	const char *errstr;
	
	glib_handle = dlopen(LIB_WIFITEST, RTLD_LAZY);
    if (!glib_handle){
        HWTEST_LOGE("%s\n", dlerror());
        return bRst;
    }
	
	dlerror(); /* Clear any existing error */
	
	WIFI_TEST_OpenDUT = (bool (*)())dlsym(glib_handle, "WIFI_TEST_OpenDUT");
    WIFI_TEST_CloseDUT = (bool (*)())dlsym(glib_handle, "WIFI_TEST_CloseDUT");
    WIFI_TEST_SetPreamble = (bool (*)(PreambleType_t))dlsym(glib_handle, "WIFI_TEST_SetPreamble");
    WIFI_TEST_Channel = (bool (*)(int))dlsym(glib_handle, "WIFI_TEST_Channel");
    WIFI_TEST_TxDataRate = (bool (*)(int))dlsym(glib_handle, "WIFI_TEST_TxDataRate");
	WIFI_TEST_SetBandWidth = (bool (*)(int))dlsym(glib_handle, "WIFI_TEST_SetBandWidth");
    WIFI_TEST_TxGain = (bool (*)(int))dlsym(glib_handle, "WIFI_TEST_TxGain");
    WIFI_TEST_TxBurstInterval = (bool (*)(int))dlsym(glib_handle, "WIFI_TEST_TxBurstInterval");
    WIFI_TEST_TxPayloadLength = (bool (*)(int))dlsym(glib_handle, "WIFI_TEST_TxPayloadLength");
    WIFI_TEST_TxBurstFrames = (bool (*)(int))dlsym(glib_handle, "WIFI_TEST_TxBurstFrames");
    WIFI_TEST_TxDestAddress = (bool (*)(unsigned char *))dlsym(glib_handle, "WIFI_TEST_TxDestAddress");
	WIFI_TEST_TxStart = (bool (*)())dlsym(glib_handle, "WIFI_TEST_TxStart");
    WIFI_TEST_TxStop = (bool (*)())dlsym(glib_handle, "WIFI_TEST_TxStop");
    WIFI_TEST_RxStart = (bool (*)())dlsym(glib_handle, "WIFI_TEST_RxStart");
    WIFI_TEST_RxStop = (bool (*)())dlsym(glib_handle, "WIFI_TEST_RxStop");
    WIFI_TEST_FRError = (bool (*)(int *))dlsym(glib_handle, "WIFI_TEST_FRError");
    WIFI_TEST_FRGood = (bool (*)(int *))dlsym(glib_handle, "WIFI_TEST_FRGood");
    WIFI_TEST_RSSI = (bool (*)(signed char *))dlsym(glib_handle, "WIFI_TEST_RSSI");
    WIFI_TEST_IsRunning = (bool (*)())dlsym(glib_handle, "WIFI_TEST_IsRunning");
    WIFI_TEST_IsUp = (bool (*)())dlsym(glib_handle, "WIFI_TEST_IsUp");
    WIFI_TEST_TxDataRate11n = (bool (*)(ENUM_WIFI_TEST_MCS_RATE,ENUM_WIFI_TEST_PREAMBLE_TYPE,ENUM_WIFI_TEST_GI_TYPE))
							  dlsym(glib_handle, "WIFI_TEST_TxDataRate11n");
	WIFI_TEST_FrequencyAccuracy = (bool (*)(int))dlsym(glib_handle, "WIFI_TEST_FrequencyAccuracy");
    WIFI_TEST_FrequencyAccuracy_Stop = (bool (*)())dlsym(glib_handle, "WIFI_TEST_FrequencyAccuracy_Stop");
	WIFI_TEST_MCR_Read = (bool (*)(unsigned int, unsigned int *))dlsym(glib_handle, "WIFI_TEST_MCR_Read");
    WIFI_TEST_MCR_Write = (bool (*)(unsigned int, unsigned int))dlsym(glib_handle, "WIFI_TEST_MCR_Write");
    WIFI_TEST_EFUSE_Read = (bool (*)(unsigned int, unsigned int *))dlsym(glib_handle, "WIFI_TEST_EFUSE_Read");
    WIFI_TEST_EFUSE_Write = (bool (*)(unsigned int, unsigned int))dlsym(glib_handle, "WIFI_TEST_EFUSE_Write");
	WIFI_TEST_CW_MODE = (bool (*)(int))dlsym(glib_handle, "WIFI_TEST_CW_MODE");
    WIFI_TEST_Tx_Output_Power = (bool (*)())dlsym(glib_handle, "WIFI_TEST_Tx_Output_Power");
    WIFI_TEST_Local_Leakage = (bool (*)())dlsym(glib_handle, "WIFI_TEST_Local_Leakage");
	WIFI_TEST_Carrier_Suppression = (bool (*)())dlsym(glib_handle, "WIFI_TEST_Carrier_Suppression");
	
    if ((errstr = dlerror()) != NULL){
        HWTEST_LOGE("Can't find function symbols %s\n", errstr);
		dlclose(glib_handle);
        return bRst;
    }
	
	return true;
}

static int UninitWifitestLib(void)
{
	int iRst = 0;
	
	iRst = dlclose(glib_handle);

	return iRst;
}
 
static bool MT662x_RxStart(int iChannelIndex, int iBandWidthIndex)								
{
	bool iRst = true;
    bool ret[3];
	int iChannel = 1;
	int iBandWidth = 0;
	
	HWTEST_LOGD("Enter MT662x_RxStart.");
	iChannel = CHANEL_MAP[iChannelIndex];
	iBandWidth = BANDWIDTH_MAP[iBandWidthIndex];
		
    iRst = WIFI_TEST_OpenDUT();
	
	if (false == iRst)
	{
		HWTEST_LOGE("Leave MT662x_RxStart() due to WIFI_TEST_OpenDUT() failure...\n");
		return false;
	}

	iRst = WIFI_TEST_Channel(iChannel);
	if (false == iRst)
	{
		HWTEST_LOGE("Leave MT662x_RxStart() due to WIFI_TEST_Channel() failure...\n");
		WIFI_TEST_CloseDUT();
		return false;
	}
	iRst = WIFI_TEST_SetBandWidth(iBandWidth);
	if (false == iRst)
	{
		HWTEST_LOGE("Leave MT662x_RxStart() due to WIFI_TEST_SetBandWidth() failure...\n");
		WIFI_TEST_CloseDUT();
		return false;
	}
		
	iRst = WIFI_TEST_RxStart();
	if (false == iRst)
	{
		HWTEST_LOGE("Leave MT662x_RxStart() due to WIFI_TEST_RxStart() failure...\n");
		WIFI_TEST_CloseDUT();
		return false;
	}
	
	HWTEST_LOGD("Leave MT662x_RxStart.");
	return iRst;
}

static bool MT662x_GetRxResult(int *iRxResult)
{
	bool bRst = true;

	signed char rxRssiFinal;
	bool ret[3];
	
	HWTEST_LOGD("Enter MT662x_GetRxResult.");
	
//	os_program_init();
	
	ret[0] = WIFI_TEST_FRGood(&iRxResult[0]);
    ret[1] = WIFI_TEST_FRError(&iRxResult[1]);
    ret[2] = WIFI_TEST_RSSI(&rxRssiFinal);
	iRxResult[2] = (int)rxRssiFinal;
	
	HWTEST_LOGD("RX OK    = %d \n", iRxResult[0]);
	HWTEST_LOGD("RX Error = %d \n", iRxResult[1]);
	HWTEST_LOGD("RX RSSI  = %d \n", iRxResult[2]);
	
	HWTEST_LOGD("Leave MT662x_GetRxResult.");

	return bRst;
	
}

static bool MT662x_RxStop(void) 
{
	bool iRst = true;
    bool ret[3];
	
	HWTEST_LOGD("Enter MT662x_RxStop.");
    iRst = WIFI_TEST_RxStop();
	if (false == iRst)
	{
		HWTEST_LOGE("Leave MT662x_RxStop() due to WIFI_TEST_RxStop() failure...\n");
		return false;
	}

	
	iRst = WIFI_TEST_CloseDUT();
	if (false == iRst)
	{
		HWTEST_LOGE("Leave MT662x_RxStop() due to WIFI_TEST_CloseDUT() failure...\n");
		return false;
	}
	
	HWTEST_LOGD("Leave MT662x_RxStop.");
	return iRst;
}

static bool MT662x_TxStart(void) 
{
	bool retval = false;
	int iChannel = 0;
	int iBandwidth = 0;
	int iTxGain = 0;
	int iPadloadLen = 0;
	int iCount = 0;
	int iPreamble = 0;
	int iRate = 0;
	int iMode = 0;
	int iGIType = 0;
	
	int iRateIdx = 0;
	int iModeIdx = 0;
	bool bBGModeFlag = true;
	
	HWTEST_LOGD("Enter MT662x_TxStart()...\n");
	
	iChannel = CHANEL_MAP[g_WiFiTxParam.iChannelIdx];
	iBandwidth = BANDWIDTH_MAP[g_WiFiTxParam.iBandwidthIdx];
	iTxGain = g_WiFiTxParam.iTxGain;
	iPadloadLen = g_WiFiTxParam.iPadloadLen;
	iCount = g_WiFiTxParam.iCount;
	
	iGIType = g_WiFiTxParam.iGITypeIdx;
	iRateIdx = g_WiFiTxParam.iRateIdx;
	iPreamble = g_WiFiTxParam.iPreambleIdx;
	iModeIdx = g_WiFiTxParam.iModeIdx;
	
	iMode = MODE_MAP[iModeIdx];

	ENUM_WIFI_TEST_GI_TYPE GIType = WIFI_TEST_GI_TYPE_NORMAL_GI;
	ENUM_WIFI_TEST_PREAMBLE_TYPE Preamble11n = WIFI_TEST_PREAMBLE_TYPE_MIXED_MODE;
	PreambleType_t Preamble = WIFI_TEST_PREAMBLE_SHORT;
	ENUM_WIFI_TEST_MCS_RATE Rate11n = WIFI_TEST_MCS_RATE_0;
	
	if (iRateIdx >= TX_DATA_RATE_COUNT)
	{
		int iVal = iRateIdx - TX_DATA_RATE_COUNT;
		bBGModeFlag = false;
		switch (iVal)
		{
			case 0:
				Rate11n = WIFI_TEST_MCS_RATE_0;
			break;
			case 1:
				Rate11n = WIFI_TEST_MCS_RATE_1;
				break;
			case 2:
				Rate11n = WIFI_TEST_MCS_RATE_2;
				break;
			case 3:
				Rate11n = WIFI_TEST_MCS_RATE_3;
				break;
			case 4:
				Rate11n = WIFI_TEST_MCS_RATE_4;
				break;
			case 5:
				Rate11n = WIFI_TEST_MCS_RATE_5;
				break;
			case 6:
				Rate11n = WIFI_TEST_MCS_RATE_6;
				break;
			case 7:
				Rate11n = WIFI_TEST_MCS_RATE_7;
				break;	
			case 8:
				Rate11n = WIFI_TEST_MCS_RATE_32;
				break;
		}
	}
	else
	{
		bBGModeFlag = true;
		iRate = RATE_MAP[iRateIdx];
	}
	
	switch (iGIType)
	{
		case 0:
			GIType = WIFI_TEST_GI_TYPE_NORMAL_GI;
			break;
		case 1:
			GIType = WIFI_TEST_GI_TYPE_SHORT_GI;
			break;
	}
	
	switch (iPreamble)
	{
		case 0:
			if (true == bBGModeFlag)
			{
				Preamble = WIFI_TEST_PREAMBLE_LONG;
			}
			else
			{
				Preamble11n = WIFI_TEST_PREAMBLE_TYPE_MIXED_MODE;
			}
			break;
		case 1:
			if (true == bBGModeFlag)
			{
				Preamble = WIFI_TEST_PREAMBLE_SHORT;
			}
			else
			{
				Preamble11n = WIFI_TEST_PREAMBLE_TYPE_GREENFIELD;
			}
			break;
	}
	
	retval = WIFI_TEST_OpenDUT();
//	sleep(1);
	
	WIFI_TEST_TxDestAddress(macAddr);
	
	retval = WIFI_TEST_Channel(iChannel);
	
	retval = WIFI_TEST_SetBandWidth(iBandwidth);
	
	retval = WIFI_TEST_TxGain(iTxGain);
	
	retval = WIFI_TEST_TxPayloadLength(iPadloadLen);
	
	retval = WIFI_TEST_TxBurstInterval(SIFS);
	
	retval = WIFI_TEST_TxBurstFrames(iCount);
	
	if (true == bBGModeFlag)
	{
		HWTEST_LOGD("Set 11bg mode Tx Data:");
		retval = WIFI_TEST_TxDataRate(iRate);
		HWTEST_LOGD("Rate = %d", iRate);
		retval = WIFI_TEST_SetPreamble(Preamble);
	}
	else
	{
		HWTEST_LOGD("Set 11n mode Tx Data:");
		retval = WIFI_TEST_TxDataRate11n(Rate11n, Preamble11n, GIType);
		HWTEST_LOGD("MCSRate = %d", Rate11n);
		HWTEST_LOGD("Preamble11n = %d", Preamble11n);
		HWTEST_LOGD("iGIType = %d", GIType);
	}	

	switch (iMode)
	{
		case CONTINUOUS_PACKET_TX:
			retval = WIFI_TEST_TxStart();
			break;
		case TX_OUTOUT_POWER:
			retval = WIFI_TEST_Tx_Output_Power();
			break;
		case CARRIER_SUPPRESSION:
			retval = WIFI_TEST_Carrier_Suppression();
			break;
		case LOCAL_LEAKAGE:
			retval = WIFI_TEST_Local_Leakage();
			break;
		default :
			HWTEST_LOGE("Set Mode Error!");
	}
	HWTEST_LOGD("iMode = %d\n", iMode);

	HWTEST_LOGD("Leave MT662x_TxStart()... retval = %d\n", retval);
	return retval;
}

static bool MT662x_TxStop(void) 
{
	bool retval = false;
	HWTEST_LOGD("Enter MT662x_TxStop()...\n");
		
	retval = WIFI_TEST_TxStop();
    retval = WIFI_TEST_CloseDUT();
	
	HWTEST_LOGD("Leave MT662x_TxStop()...\n");
	return retval;
}

/***************************************************************************
 * 
 * JNI Code
 * 
 * *************************************************************************/

static bool InitJNIParam(JNIEnv *env, jobject thiz) {
	HWTEST_LOGE("Enter process_init_param()...\n");
	jclass clazz = env->GetObjectClass(thiz);
	if (clazz == NULL) {
		HWTEST_LOGE("Can't find mediatek/android/application/Advanced_Tool/WiFiHWTestNative");
		HWTEST_LOGE(
				"Leave process_init_param() due to Can't find mediatek/android/application/Advanced_Tool/WiFiHWTestNative...\n");
		jniThrowException(env, "java/lang/Exception", NULL);
		return false;
	}

	fields.ChannelIdx = env->GetFieldID(clazz, "mChannelIndex", "I");
	if (NULL == fields.ChannelIdx) {
		HWTEST_LOGE(
				"Leave process_init_param() due to Can't find WiFiHWTestNative.mChannelIndex...\n");
		jniThrowException(env, "java/lang/RuntimeException",
				"Can't find WiFiHWTestNative.mChannelIndex");
		return false;
	}

	fields.BandwidthIdx = env->GetFieldID(clazz, "mBandWidthIndex", "I");

	if (NULL == fields.BandwidthIdx) {
		HWTEST_LOGE(
				"Leave process_init_param() due to Can't find WiFiHWTestNative.mBandWidthIndex...\n");
		jniThrowException(env, "java/lang/RuntimeException",
				"Can't find WiFiHWTestNative.mBandWidthIndex");
		return false;
	}

	fields.TxGain = env->GetFieldID(clazz, "mTxGain", "I");
	if (NULL == fields.TxGain) {
		HWTEST_LOGE(
				"Leave process_init_param() due to Can't find WiFiHWTestNative.mTxGain...\n");
		jniThrowException(env, "java/lang/RuntimeException",
				"Can't find WiFiHWTestNative.mTxGain");
		return false;
	}

	fields.PadloadLen = env->GetFieldID(clazz, "mPadloadLen", "I");
	if (NULL == fields.PadloadLen) {
		HWTEST_LOGE(
				"Leave process_init_param() due to Can't find WiFiHWTestNative.mPadloadLen...\n");
		jniThrowException(env, "java/lang/RuntimeException",
				"Can't find WiFiHWTestNative.mPadloadLen");
		return false;
	}

	fields.Count = env->GetFieldID(clazz, "mCount", "I");
	if (NULL == fields.Count) {
		HWTEST_LOGE(
				"Leave process_init_param() due to Can't find WiFiHWTestNative.mCount...\n");
		jniThrowException(env, "java/lang/RuntimeException",
				"Can't find WiFiHWTestNative.mCount");
		return false;
	}
	
	fields.PreambleIdx = env->GetFieldID(clazz, "mPreambleIndex", "I");
	if (NULL == fields.PreambleIdx) {
		HWTEST_LOGE(
				"Leave process_init_param() due to Can't find WiFiHWTestNative.mPreambleIndex...\n");
		jniThrowException(env, "java/lang/RuntimeException",
				"Can't find WiFiHWTestNative.mPreambleIndex");
		return false;
	}
	
	fields.RateIdx = env->GetFieldID(clazz, "mRateIndex", "I");
	if (NULL == fields.RateIdx) {
		HWTEST_LOGE(
				"Leave process_init_param() due to Can't find WiFiHWTestNative.mRateIndex...\n");
		jniThrowException(env, "java/lang/RuntimeException",
				"Can't find WiFiHWTestNative.mRateIndex");
		return false;
	}
	
	fields.ModeIdx = env->GetFieldID(clazz, "mModeIndex", "I");
	if (NULL == fields.ModeIdx) {
		HWTEST_LOGE(
				"Leave process_init_param() due to Can't find WiFiHWTestNative.mModeIndex...\n");
		jniThrowException(env, "java/lang/RuntimeException",
				"Can't find WiFiHWTestNative.mModeIndex");
		return false;
	}
	
	fields.GITypeIdx = env->GetFieldID(clazz, "mGITypeIndex", "I");
	if (NULL == fields.GITypeIdx) {
		HWTEST_LOGE(
				"Leave process_init_param() due to Can't find WiFiHWTestNative.mGITypeIndex...\n");
		jniThrowException(env, "java/lang/RuntimeException",
				"Can't find WiFiHWTestNative.mGITypeIndex");
		return false;
	}

	fields.FreqOffset = env->GetFieldID(clazz, "mFreqOffset", "I");
	if (NULL == fields.FreqOffset) {
		HWTEST_LOGE(
				"Leave process_init_param() due to Can't find WiFiHWTestNative.mFreqOffset...\n");
		jniThrowException(env, "java/lang/RuntimeException",
				"Can't find WiFiHWTestNative.mFreqOffset");
		return false;
	}

	HWTEST_LOGE("Leave process_init_param()...\n");
	return true;
}

static bool InitWiFiTxParam(JNIEnv *env, jobject thiz)
{
	g_WiFiTxParam.iBandwidthIdx = (int) env->GetIntField(thiz, fields.BandwidthIdx);
	g_WiFiTxParam.iChannelIdx = (int)env->GetIntField(thiz, fields.ChannelIdx);
	g_WiFiTxParam.iTxGain = (int) env->GetIntField(thiz, fields.TxGain);
	g_WiFiTxParam.iPadloadLen = (int) env->GetIntField(thiz, fields.PadloadLen);
	g_WiFiTxParam.iCount = (int) env->GetIntField(thiz, fields.Count);
	g_WiFiTxParam.iGITypeIdx = (int) env->GetIntField(thiz, fields.GITypeIdx);
	g_WiFiTxParam.iRateIdx = (int) env->GetIntField(thiz, fields.RateIdx);
	g_WiFiTxParam.iPreambleIdx = (int) env->GetIntField(thiz, fields.PreambleIdx);
	g_WiFiTxParam.iModeIdx = (int) env->GetIntField(thiz, fields.ModeIdx);
	g_WiFiTxParam.iFreqOffset = (int) env->GetIntField(thiz, fields.FreqOffset);
	
	return true;
}

static jint WiFiTest_Init(JNIEnv *env, jobject thiz)
{
	InitChiperType();
	
	switch (g_ChipType)
	{
		case MT76xx:
			HWTEST_LOGD("MT76xx");
			break;
		case MT662x:
			HWTEST_LOGD("MT662x");
			if (false == InitWifitestLib())
			{
				HWTEST_LOGE("InitWifitestLib libwifitest.so error");
				return ERROR;
			}
			break;
		case INIT_CHIP:
			break;
		default :
			break;
	}
	
	return g_ChipType;
}

static jint WiFiTest_Uninit(JNIEnv *env, jobject thiz)
{
	switch (g_ChipType)
	{
		case MT76xx:
			
			break;
		case MT662x:
			if (false == UninitWifitestLib())
			{
				HWTEST_LOGE("Uninit %s error", LIB_WIFITEST);
				return ERROR;
			}
			break;
		case INIT_CHIP:
			break;
		default :
			break;
	}
	
	return SUCCESS;
}
#define TX_GAIN 	0x01
#define FREQ_OFFSET 0x02
static jint  WiFi_MT76xxGetEfuse(JNIEnv *env, jobject thiz ,
								jint nType, jint nChannelIdx)
{
	int iResult = 0;

	char szBuf[32] = "";
	int iFreqOffAddr = 0x3a;
	int iTxGainInit = 0x52;
	
	switch (nType)
	{
		case TX_GAIN:
			iResult = GetEfuseValue(Int2Str(iTxGainInit + nChannelIdx, szBuf, 16));
			break;
		case FREQ_OFFSET:
			iResult = GetEfuseValue(Int2Str(iFreqOffAddr, szBuf, 16));
			break;
		default :
			break;
	}

	return iResult;
}

static jboolean WiFiTest_RxStart(JNIEnv *env, jobject thiz ,
								jint nChannelIdx, jint nBandWidthIdx) 
{
	bool bRst = false;
	int iChannelIndex = nChannelIdx;
	int iBWIndex = nBandWidthIdx;
	
	switch (g_ChipType)
	{
		case MT76xx:
			bRst = MT76xx_RxStart(iChannelIndex, iBWIndex);
			break;
		case MT662x:
			bRst = MT662x_RxStart(iChannelIndex, iBWIndex);
			break;
		case INIT_CHIP:
			break;
		default :
			break;
	}
	
	return bRst;	
}

static jboolean WiFiTest_RxStop(JNIEnv *env, jobject thiz) 
{
	bool bRst = false;
	
	switch (g_ChipType)
	{
		case MT76xx:
			bRst = MT76xx_RxStop();
			break;
		case MT662x:
			bRst = MT662x_RxStop();
			break;
		case INIT_CHIP:
			break;
		default :
			break;
	}
	
	return bRst;	
}

static jintArray  WiFiTest_GetRxResult(JNIEnv *env, jobject thiz)
{
	bool bRst = false;
	int iRxResult[3] = {0};
	
	switch (g_ChipType)
	{
		case MT76xx:
			bRst = MT76xx_GetRxResult(iRxResult);
			break;
		case MT662x:
			bRst = MT662x_GetRxResult(iRxResult);
			break;
		case INIT_CHIP:
			break;
		default :
			break;
	}

	jintArray jniResult = env->NewIntArray(3);
	env->SetIntArrayRegion(jniResult, 0, 3, iRxResult);	
	
	return jniResult;
}

static jboolean WiFiTest_TxStart(JNIEnv *env, jobject thiz) 
{
	bool bRst = false;
		
	if (!InitJNIParam(env, thiz)) {
		HWTEST_LOGE("Leave WiFiTest_TxStart() due to process_init_param() failure...\n");
		return -1;
	}

	InitWiFiTxParam(env, thiz);

	debug_init_param(env, thiz);
	
	switch (g_ChipType)
	{
		case MT76xx:
			bRst = MT76xx_TxStart();
			break;
		case MT662x:
			bRst = MT662x_TxStart();
			break;
		case INIT_CHIP:
			break;
		default :
			break;
	}
	
	return bRst;
}

static jboolean WiFiTest_TxStop(JNIEnv *env, jobject thiz) 
{
	bool bRst = false;
	
	switch (g_ChipType)
	{
		case MT76xx:
			bRst = MT76xx_TxStop();
			break;
		case MT662x:
			bRst = MT662x_TxStop();
			break;
		case INIT_CHIP:
			break;
		default :
			break;
	}
	
	return bRst;
}

static JNINativeMethod mehods[] = {
	/*
		{ "getChipId", "()I",(void *) BtTest_getChipId },
		{ "isBLESupport", "()I",(void *) BtTest_isBLESupport },
		{ "init", "()I", (void *) BtTest_Init },
		{ "doBtTest", "(I)I",(void *) BtTest_doBtTest },
		{ "unInit", "()I",(void *) BtTest_UnInit },
		{ "hciCommandRun", "([CI)[C",(void *) BtTest_HCICommandRun },
		{ "hciReadEvent", "()[C",(void *) BtTest_HCIReadEvent },

		{ "noSigRxTestStart", "(IIII)Z", (void *) BtTest_StartNoSigRxTest },
		{ "noSigRxTestResult", "()[I", (void *) BtTest_EndNoSigRxTest },

		{"getChipIdInt", "()I", (void *) BtTest_GetChipIdInt },
		{"getChipEcoNum", "()I", (void *) BtTest_GetChipEcoNum },
		{"getPatchId", "()[C", (void *) BtTest_GetPatchId },
		{"getPatchLen", "()J", (void *) BtTest_GetPatchLen },
	
		{"relayerStart", "(II)I", (void *) BtTest_relayerStart },
		{"relayerExit", "()I", (void *) BtTest_relayerExit },
	*/			
		{ "WiFi_Init", "()I", (void *) WiFiTest_Init},
		{ "WiFi_Uninit", "()Z", (void *) WiFiTest_Uninit},
		
		{ "WiFi_RxStart", "(II)Z", (void *) WiFiTest_RxStart },
		{ "WiFi_RxStop", "()Z", (void *) WiFiTest_RxStop },
		{ "WiFi_GetRxResult", "()[I", (void *) WiFiTest_GetRxResult },
		
		{ "WiFi_TxStart", "()Z", (void *) WiFiTest_TxStart },
		{ "WiFi_TxStop", "()Z", (void *) WiFiTest_TxStop },
		{ "WiFi_MT76xxGetEfuse", "(II)I", (void *)WiFi_MT76xxGetEfuse},
	/*	
		{ "TxOnlyTestEnd", "()Z", (void *) BtTest_TxOnlyTest_End },
		{ "TxOnlyTestStart", "(IIIII)Z", (void *) BtTest_TxOnlyTest_Start },
		{ "noSigRxTestStart", "(IIII)Z", (void *) BtTest_NoSigRxTest_Start },
		{ "noSigRxTestResult", "()[I", (void *) BtTest_NoSigRxTest_End },
	*/	
		};
// This function only registers the native methods
static int register_combotool_advanced_wifi(JNIEnv *env) {
	HWTEST_LOGE("Register: register_combotool_advanced_wifi()...\n");
	/*
	 JNINativeMethod nm;
	 nm.name = "doBtTest";
	 nm.signature = "(I)I";
	 nm.fnPtr = (void *)BtTest_doBtTest;
	 */
	HWTEST_LOGE("register_combotool_advanced_wifi");
	//  return AndroidRuntime::registerNativeMethods(env, "mediatek/android/application/Advanced_Tool/EMBt", &nm, 1);
	return AndroidRuntime::registerNativeMethods(env,
			"mediatek/android/application/Advanced_Tool/WiFiHWTestNative", mehods, NELEM(mehods));
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	JNIEnv* env = NULL;
	jint result = -1;

	HWTEST_LOGD("Enter JNI_OnLoad()...\n");
	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		HWTEST_LOGE("ERROR: GetEnv failed\n");
		goto bail;
	}
	assert(env != NULL);

	//if(registerNatives(env) != JNI_TRUE)
	if (register_combotool_advanced_wifi(env) < 0) {
		HWTEST_LOGE("ERROR: Wifi native registration failed\n");
		goto bail;
	}

	/* success -- return valid version number */
	result = JNI_VERSION_1_4;

	HWTEST_LOGD("Leave JNI_OnLoad()...\n");
	bail: return result;
}
