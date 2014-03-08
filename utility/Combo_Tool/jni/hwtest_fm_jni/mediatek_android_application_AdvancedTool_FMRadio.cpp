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
#undef HWTEST_LOG_NDEBUG 
#undef NDEBUG

#include "hwtest_log.h"

#ifdef HWTEST_LOG_TAG
#undef HWTEST_LOG_TAG
#define HWTEST_LOG_TAG "HWTest_FM-JNI"
#endif
extern "C" {
#include "libfm.h"
}
//#include <cutils/xlog.h>

#include <cutils/sockets.h>
#include <cutils/properties.h>
//#include <cutils/log.h>


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
const char *FMRadioPath = "/dev/fm";	

static jboolean FMRadio_Opendev(JNIEnv *env, jobject thiz) 
{
	HWTEST_LOGD("Enter FMRadio_Opendev.");
	
	if (0 != fm_open_dev(FMRadioPath)) {
		HWTEST_LOGE("Leave FMRadio_Opendev() due to fm_open_dev() failure...\n");
		//jniThrowException(env, "BT init failed", NULL);
		return false;
	}
	
	HWTEST_LOGD("Leave FMRadio_Opendev.");
	return true;
}

static jboolean FMRadio_Closedev(JNIEnv *env, jobject thiz) 
{	
	HWTEST_LOGD("Enter FMRadio_Closedev.");
	
	if (0 != fm_close_dev()) {
		HWTEST_LOGE("Leave FMRadio_Closedev() due to fm_close_dev() failure...\n");
		//jniThrowException(env, "BT init failed", NULL);
		return false;
	}
	
	HWTEST_LOGD("Leave FMRadio_Closedev.");
	return true;
}

static jboolean FMRadio_Powerup(JNIEnv *env, jobject thiz, jint nFreq) 
{
	int iFreq = 0;

	HWTEST_LOGD("Enter FMRadio_Powerup.");
	iFreq = nFreq;
	HWTEST_LOGD("Freq = %d", iFreq);
	
	if (0 != fm_power_up(iFreq)) {
		HWTEST_LOGE("Leave FMRadio_Powerup() due to fm_power_up() failure...\n");
		//jniThrowException(env, "BT init failed", NULL);
		return false;
	}
	
	HWTEST_LOGD("Leave FMRadio_Powerup.");
	return true;
}

static jboolean FMRadio_Powerdown(JNIEnv *env, jobject thiz, jint nType) 
{	
	HWTEST_LOGD("Enter FMRadio_Powerdown.");
	
	if (0 == nType)
	{
		if (0 != fm_power_down())
		{
			HWTEST_LOGE("Leave FMRadio_Powerdown() due to fm_power_down() failure...\n");
			//jniThrowException(env, "BT init failed", NULL);
			return false;
		}
	}
		
	HWTEST_LOGD("Leave FMRadio_Powerdown.");
	return true;
}


static jboolean FMRadio_Tune(JNIEnv *env, jobject thiz, jint nFreq) 
{	
	int iFreq = 0;
	HWTEST_LOGD("Enter FMRadio_Tune.");
	iFreq = nFreq;
	HWTEST_LOGD("Tune on Freq = %d", iFreq);
	if (0 != fm_tune(iFreq))
	{
		HWTEST_LOGE("Leave FMRadio_Tune() due to fm_tune() failure...\n");
		//jniThrowException(env, "BT init failed", NULL);
		return false;
	}
		
	HWTEST_LOGD("Leave FMRadio_Tune.");
	return true;
}

static jint FMRadio_readRssi(JNIEnv *env, jobject thiz) 
{	
	int iRssi = 0;
	int iRst = 0;
	HWTEST_LOGD("Enter FMRadio_readRssi.");
	
//	sleep(5);
	iRst = fm_getrssi(&iRssi);
	HWTEST_LOGD("RSSI = %d", iRssi);
	
	HWTEST_LOGD("Leave FMRadio_readRssi.");
	return iRssi;
}

static jint FMRadio_switchAntenna(JNIEnv *env, jobject thiz,  jint nAntenna) 
{	
	int iAntenna = 0;
	int iRst = 0;
	HWTEST_LOGD("Enter FMRadio_switchAntenna.");
	iAntenna = nAntenna;
//	sleep(5);
	iRst = fm_switchana(iAntenna);
	HWTEST_LOGD("iAntenna = %d", iAntenna);
	
	HWTEST_LOGD("Leave FMRadio_switchAntenna.");
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
		{ "opendev", "()Z", (void *) FMRadio_Opendev },
		{ "closedev", "()Z", (void *) FMRadio_Closedev },
		
		{ "powerup", "(I)Z", (void *) FMRadio_Powerup },
		{ "powerdown", "(I)Z", (void *) FMRadio_Powerdown },
		
		{ "tune", "(I)Z", (void *) FMRadio_Tune },		
		{ "readRssi", "()I", (void *) FMRadio_readRssi },	
		{ "switchAntenna", "(I)I", (void *) FMRadio_switchAntenna },	
		
		};
// This function only registers the native methods
static int register_combotool_advanced_fm(JNIEnv *env) {
	HWTEST_LOGE("Register: register_combotool_advanced_fm()...\n");
	/*
	 JNINativeMethod nm;
	 nm.name = "doBtTest";
	 nm.signature = "(I)I";
	 nm.fnPtr = (void *)BtTest_doBtTest;
	 */
	HWTEST_LOGE("register_combotool_advanced_fm");
	//  return AndroidRuntime::registerNativeMethods(env, "mediatek/android/application/Advanced_Tool/FMRadioNative", &nm, 1);
	return AndroidRuntime::registerNativeMethods(env,
			"mediatek/android/application/Advanced_Tool/FMRadioNative", mehods, NELEM(mehods));
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
	if (register_combotool_advanced_fm(env) < 0) {
		HWTEST_LOGE("ERROR: FM native registration failed\n");
		goto bail;
	}

	/* success -- return valid version number */
	result = JNI_VERSION_1_4;

	HWTEST_LOGD("Leave JNI_OnLoad()...\n");
	bail: return result;
}
