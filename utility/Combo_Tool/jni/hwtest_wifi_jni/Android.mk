# Copyright Statement:
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. and/or its licensors.
# Without the prior written permission of MediaTek inc. and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.

# MediaTek Inc. (C) 2010. All rights reserved.
#
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
# RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
# AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
# NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
# SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
# CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
# CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
# AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
# MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
#
# The following software/firmware and/or related documentation ("MediaTek Software")
# have been modified by MediaTek Inc. All revisions are subject to any receiver's
# applicable license agreements with MediaTek Inc.

#ifeq ($(MTK_ENGINEERMODE_APP), yes)
#ifeq ($(MTK_BT_SUPPORT), yes)

#ifneq ($(BUILD_WITHOUT_PV),true)
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

MEDIATEK_HWTEST_PATH := hardware/mediatek/utility

LOCAL_SRC_FILES:= \
    mediatek_android_application_AdvancedTool_Hwtest_WiFi.cpp 


LOCAL_SHARED_LIBRARIES := \
    libdl \
	  libnativehelper \
    libandroid_runtime \
	  libutils \
	  libcutils \
	  libui \
          liblog


LOCAL_STATIC_LIBRARIES := 


LOCAL_PRELINK_MODULE :=false

LOCAL_C_INCLUDES += \
    external/tremor/Tremor \
    $(MEDIATEK_HWTEST_PATH)/hw_test_tool/LIBWIFITEST \
    frameworks/base/core/jni \
    $(PV_INCLUDES) \
    $(JNI_H_INCLUDE) \
    $(call include-path-for, corecg graphics)

LOCAL_CFLAGS +=

LOCAL_LDLIBS := -lpthread

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= libhwtest_wifi_jni

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
local_target_dir := $(TARGET_OUT)/xbin
LOCAL_SRC_FILES:= mysu.c 
LOCAL_MODULE_PATH := $(local_target_dir)
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= mysu

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
local_target_dir := $(TARGET_OUT)/xbin
LOCAL_SRC_FILES:= iwpriv 
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= iwpriv
LOCAL_MODULE_PATH := $(local_target_dir)
include $(BUILD_PREBUILT)
#endif

#endif
#endif
