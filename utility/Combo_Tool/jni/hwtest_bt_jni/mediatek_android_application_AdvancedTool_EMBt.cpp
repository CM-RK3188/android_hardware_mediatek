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
#include <errno.h>
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
#include "android_runtime/AndroidRuntime.h"
#include "utils/Errors.h"  // for status_t
#undef LOG_NDEBUG 
#undef NDEBUG

#include "hwtest_log.h"

#ifdef HWTEST_LOG_TAG
#undef HWTEST_LOG_TAG
#define HWTEST_LOG_TAG "EM_BT_EX-JNI"
#endif

#include "bt_em.h"

//#include <cutils/xlog.h>
//#include <cutils/log.h>
#include <cutils/sockets.h>
#include <cutils/properties.h>
//typedef int BOOL;

/*
extern "C" void RELAYER_exit(void);
 extern "C" BOOL RELAYER_start(int serial_port, int serial_speed);
*/
/*
extern "C" void EM_BT_deinit(void);
extern "C" BOOL EM_BT_init(void);

extern "C" BOOL EM_BT_read(unsigned char *peer_buf, int peer_len,int* piResultLen);

extern "C" BOOL EM_BT_write(unsigned char *peer_buf, int peer_len);


 typedef BOOL (*bt_init)(void);
 typedef void (*bt_deinit)(void);
 typedef BOOL (*bt_read)(
 unsigned char *peer_buf,
 int  peer_len,
 int* piResultLen);
 typedef BOOL (*bt_write)(
 unsigned char *peer_buf,
 int peer_len);

 static bt_init EM_BT_init = NULL;
 static bt_deinit EM_BT_deinit = NULL;
 static bt_read EM_BT_read = NULL;
 static bt_write EM_BT_write = NULL;
 */

// ----------------------------------------------------------------------------

using namespace android;

// ----------------------------------------------------------------------------

static unsigned char PatternMap[]= {0x01, 0x02, 0x03, 0x04, 0x09};
static unsigned char RxPacketTypeMap[]= {0x03, 0x04, 0x0A, 0x0B, 0x0E, 0x0F,
										0x24, 0x28, 0x2A, 0x2B, 0x2E, 0x2F};
static unsigned char TxPacketTypeMap[]= { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
		0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0E, 0x0F, 0x17, 0x1C, 0x1D, 0x24,
		0x28, 0x2A, 0x2B, 0x2E, 0x2F, 0x36, 0x37, 0x3C, 0x3D };
static unsigned char TxChannelMap[]= {0x00, 0x01};

static jint BtTest_Init(JNIEnv *env, jobject thiz)
{
	if (false == EM_BT_init())
	{
		return -1;
	}
	
	return 0;
}

static jint BtTest_Deinit(JNIEnv *env, jobject thiz)
{
	EM_BT_deinit();
	
	return 0;
}

static jboolean BtTest_TxOnlyTest_End(JNIEnv *env, jobject thiz)
{
	EM_BT_TxOnlyTest_end();
	
	EM_BT_deinit();
	
	return true;
}

static jboolean BtTest_TxOnlyTest_Start(JNIEnv *env, jobject thiz ,
				jint nPatternIdx, jint nChannelIdx, jint nFreq, jint nPacketTypeIdx, jint nPacketLen)
{
	BOOL bRst = true;
	unsigned char ucPattern = 0;
	unsigned char ucChannel = 0; 
	unsigned char ucPacketType = 0;
    int iFreq = 0;
	int iPacketLength = 0;
	
	HWTEST_LOGD("Enter BtTest_TxOnlyTest.");
	
	ucPattern = PatternMap[nPatternIdx];
	ucPacketType = TxPacketTypeMap[nPacketTypeIdx];
	ucChannel = TxChannelMap[nChannelIdx];
	iFreq = nFreq;
	iPacketLength = nPacketLen;
	
	if (EM_BT_init() == false) {
		HWTEST_LOGE("Leave BtTest_TxOnlyTest() due to EM_BT_init() failure...\n");
		//jniThrowException(env, "BT init failed", NULL);
		return false;
	}
	
	HWTEST_LOGD("ucPattern      = %d \n", ucPattern);
	HWTEST_LOGD("ucChannel      = %d \n", ucChannel);
	HWTEST_LOGD("iFreq          = %d \n", iFreq);
	HWTEST_LOGD("ucPacketType   = %d \n", ucPacketType);
	HWTEST_LOGD("iPacketLength  = %d \n", iPacketLength);
	
	bRst = EM_BT_TxOnlyTest_start(
                   ucPattern, 
                   ucChannel, 
                   iFreq, 
                   ucPacketType, 
                   iPacketLength);
	if (false == bRst)
	{
		HWTEST_LOGE("Leave BtTest_TxOnlyTest() due to EM_BT_TxOnlyTest() failure...\n");
		EM_BT_deinit();
		return false;
	}
	
//	EM_BT_deinit();	
	HWTEST_LOGD("Leave BtTest_TxOnlyTest.");
	return bRst;
}

static jintArray BtTest_NoSigRxTest_End(JNIEnv *env, jobject thiz)
{
	int iResult[4] = {0};
	unsigned long ulPktCount = 0;
    unsigned long ulByteCount = 0;
    float fPER = 0.0;
	float fBER = 0.0;
	BOOL bRst = true;
	
	HWTEST_LOGD("Enter BtTest_NoSigRxTest_End.");

	bRst = EM_BT_NonSignalRx_end(
                   &ulPktCount,
                   &fPER,
                   &ulByteCount,
                   &fBER);
	
	if (false == bRst)
	{
		HWTEST_LOGE("Leave BtTest_NoSigRxTest_Start() due to EM_BT_NonSignalRx_end() failure...\n");
		EM_BT_deinit();
		return 0;
	}
	else
	{
		HWTEST_LOGD("packet count      = %ld \n", ulPktCount);
		HWTEST_LOGD("packet error rate = %f \n", fPER);
		HWTEST_LOGD("byte count        = %ld \n", ulByteCount);
		HWTEST_LOGD("bit error rate    = %f \n", fBER);
	}
	
	iResult[0] = (int)ulPktCount;
	iResult[1] = (int)(fPER * 100000);
	iResult[2] = (int)ulByteCount;
	iResult[3] = (int)(fBER * 100000);
	
	jintArray jniResult = env->NewIntArray(4);
	env->SetIntArrayRegion(jniResult, 0, 4, iResult);	
	
	EM_BT_deinit();
	HWTEST_LOGD("Leave BtTest_NoSigRxTest_End.");
	
	return jniResult;
}
static jboolean BtTest_NoSigRxTest_Start(JNIEnv *env, jobject thiz, 
             jint nPatternIdx, jint nPacketTypeIdx, jint nFreq, jint nAddress) {

	unsigned char ucRxPattern = 0;
    int iFreq = 0;
    unsigned char ucPacketType = 0;
    unsigned long ulTesterAddr = 0;
	
	BOOL bRst = true;
	
	HWTEST_LOGD("Enter BtTest_NoSigRxTest.");
	
	ucRxPattern = (unsigned char)PatternMap[nPatternIdx];
	ucPacketType = (unsigned char)RxPacketTypeMap[nPacketTypeIdx];
	iFreq = nFreq;
	ulTesterAddr = (unsigned long)nAddress;
	
	if (EM_BT_init() == false) {
		HWTEST_LOGE("Leave BtTest_NoSigRxTest_Start() due to EM_BT_init() failure...\n");
		//jniThrowException(env, "BT init failed", NULL);
		return false;
	}
	
	bRst = EM_BT_NonSignalRx_start(
                   ucRxPattern, 
                   iFreq, 
                   ucPacketType, 
                   ulTesterAddr);
	if (false == bRst)
	{
		HWTEST_LOGE("Leave BtTest_NoSigRxTest_Start() due to EM_BT_NonSignalRx_start() failure...\n");
		EM_BT_deinit();
		return false;
	}
	
	HWTEST_LOGD("Leave BtTest_NoSigRxTest.");
	
	return bRst;
}

static jboolean BtTest_TestMode_Enable(JNIEnv *env, jobject thiz, jint nPowerLevel) 
{
	int iPowerLevel = 0;
	bool iRst = true;
	
	HWTEST_LOGD("Enter BtTest_TestMode_Enable.");
	
	iPowerLevel = nPowerLevel;
	if (iPowerLevel < 0 || iPowerLevel > 7)
	{
		HWTEST_LOGW("iPowerLevel = %d, PowerLevel should be [0, 7], User default value 5", iPowerLevel);
		iPowerLevel = 5;
	}
	
	if (EM_BT_init() == false) {
		HWTEST_LOGE("Leave BtTest_TestMode_Enable() due to EM_BT_init() failure...\n");
		//jniThrowException(env, "BT init failed", NULL);
		return false;
	}
	
	iRst = EM_BT_TestMode_enter(iPowerLevel);
	if (false == iRst)
	{
		HWTEST_LOGE("Leave BtTest_TestMode_Enable() due to EM_BT_TestMode_enter() failure...\n");
		EM_BT_deinit();
		return false;
	}
	
	HWTEST_LOGD("Leave BtTest_TestMode_Enable.");
	return iRst;
}

static jboolean BtTest_TestMode_Disable(JNIEnv *env, jobject thiz) 
{
	bool iRst = true;
	
	HWTEST_LOGD("Enter BtTest_TestMode_Disable.");
	
	iRst = EM_BT_TestMode_exit();
	if (false == iRst)
	{
		HWTEST_LOGE("Leave BtTest_TestMode_Disable() due to EM_BT_TestMode_exit() failure...\n");
		EM_BT_deinit();
		return false;
	}
	
	EM_BT_deinit();
	
	HWTEST_LOGD("Leave BtTest_TestMode_Disable.");
	return iRst;
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
		{ "Bt_init", "()I", (void *) BtTest_Init },
		{ "Bt_deinit", "()I", (void *) BtTest_Deinit },
		
		{ "TestModeEnable", "(I)Z", (void *) BtTest_TestMode_Enable },
		{ "TestModeDisable", "()Z", (void *) BtTest_TestMode_Disable },
		
		{ "TxOnlyTestEnd", "()Z", (void *) BtTest_TxOnlyTest_End },
		{ "TxOnlyTestStart", "(IIIII)Z", (void *) BtTest_TxOnlyTest_Start },
		{ "noSigRxTestStart", "(IIII)Z", (void *) BtTest_NoSigRxTest_Start },
		{ "noSigRxTestResult", "()[I", (void *) BtTest_NoSigRxTest_End },
		
		};
// This function only registers the native methods
static int register_combotool_advanced_bt(JNIEnv *env) {
	HWTEST_LOGE("Register: register_combotool_advanced_bt()...\n");
	/*
	 JNINativeMethod nm;
	 nm.name = "doBtTest";
	 nm.signature = "(I)I";
	 nm.fnPtr = (void *)BtTest_doBtTest;
	 */
	HWTEST_LOGE("register_combotool_advanced_bt");
	//  return AndroidRuntime::registerNativeMethods(env, "mediatek/android/application/Advanced_Tool/EMBt", &nm, 1);
	return AndroidRuntime::registerNativeMethods(env,
			"mediatek/android/application/Advanced_Tool/EMBt", mehods, NELEM(mehods));
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
	if (register_combotool_advanced_bt(env) < 0) {
		HWTEST_LOGE("ERROR: Bluetooth native registration failed\n");
		goto bail;
	}

	/* success -- return valid version number */
	result = JNI_VERSION_1_4;

	HWTEST_LOGD("Leave JNI_OnLoad()...\n");
	bail: return result;
}
