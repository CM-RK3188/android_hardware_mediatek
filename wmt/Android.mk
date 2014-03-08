# Copyright Statement:
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. and/or its licensors.
# Without the prior written permission of MediaTek inc. and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.
#
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


# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Configuration

LOCAL_PATH := $(call my-dir)
#DST_FOLDER = external/combo_tool
#ANDROID_MK_DIR = external/combo_tool
ifeq ($(strip $(BOARD_CONNECTIVITY_MODULE)), combo_mt66xx)

LINUX_SERIAL_H_PATH := $(shell find $(KERNEL_HEADERS_COMMON) -type f -name "serial.h" | grep -F linux/serial.h)
$(info linux serial.h path: $(LINUX_SERIAL_H_PATH))
ifeq ($(LINUX_SERIAL_H_PATH),)
BUILD_HAVE_SERIAL_H := false
else
BUILD_HAVE_SERIAL_H := true
endif

# applied to MT6620

BUILD_LAUNCHER  := true
BUILD_WMT_LPBK  := true
BUILD_STP_DUMP  := true
BUILD_WMT_CONCURRENCY  := true
BUILD_WMT_CFG := true
BUILD_PATCH := true
BUILD_WLAN_FW := true
BUILD_WLAN_MAC_ADDRESS := true
BUILD_WLAN_5GHZ_BAND := false

ifeq ($(BUILD_LAUNCHER), true)
include $(CLEAR_VARS)
LOCAL_MODULE := wmt_launcher
LOCAL_CFLAGS := -DWMT_PLAT_APEX
ifeq ($(BUILD_HAVE_SERIAL_H), true)
LOCAL_CFLAGS += -DCFG_HAVE_SERIAL_H
endif
LOCAL_MODULE_TAGS := optional
#LOCAL_MODULE_CLASS := EXECUTABLES
#LOCAL_MODULE_PATH := $(TARGET_OUT)/bin
LOCAL_SRC_FILES := stp_uart_launcher.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_SHARED_LIBRARIES := libcutils
include $(BUILD_EXECUTABLE)
endif

ifeq ($(BUILD_WMT_LPBK), true)
include $(CLEAR_VARS)
LOCAL_MODULE := wmt_lpbk
LOCAL_MODULE_TAGS := optional
#LOCAL_MODULE_CLASS := EXECUTABLES
#LOCAL_MODULE_PATH := $(TARGET_OUT)/bin
LOCAL_SRC_FILES := wmt_loopback.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)
include $(BUILD_EXECUTABLE)
endif

ifeq ($(BUILD_STP_DUMP), true)
include $(CLEAR_VARS)
LOCAL_SRC_FILES :=                                      \
		  stp_dump/stp_dump.c			\
		  stp_dump/eloop.c	\
		  stp_dump/os_linux.c	
LOCAL_SHARED_LIBRARIES := libc libcutils
LOCAL_MODULE := stp_dump3
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/bin
include $(BUILD_EXECUTABLE)
endif

ifeq ($(BUILD_WMT_CONCURRENCY), true)
include $(CLEAR_VARS)
LOCAL_SRC_FILES  := wmt_concurrency.c
#LOCAL_C_INCLUDES := $(LOCAL_PATH)/custom
LOCAL_MODULE := wmt_concurrency
LOCAL_MODULE_TAGS := eng
include $(BUILD_EXECUTABLE)
endif

ifeq ($(BUILD_WMT_CFG), true)
include $(CLEAR_VARS)
LOCAL_MODULE := WMT.cfg
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/firmware
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)
endif



#MTK_MULTI_PATCH_SUPPORT
ifeq ($(BUILD_PATCH), true)
    include $(CLEAR_VARS)
ifeq ($(MTK_COMBO_CHIP), MT6620E2)
    LOCAL_MODULE := mt6620_patch_hdr.bin
else
    LOCAL_MODULE := mt6620_patch_e3_0_hdr.bin
endif
    LOCAL_MODULE_TAGS := optional
    LOCAL_MODULE_CLASS := ETC
    LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/firmware
    LOCAL_SRC_FILES := $(LOCAL_MODULE)
    include $(BUILD_PREBUILT)
endif
ifeq ($(BUILD_PATCH), true)
    include $(CLEAR_VARS)
ifeq ($(MTK_COMBO_CHIP), MT6620E2)
    LOCAL_MODULE := mt6620_patch_hdr.bin
else
    LOCAL_MODULE := mt6620_patch_e3_1_hdr.bin
endif
    LOCAL_MODULE_TAGS := optional
    LOCAL_MODULE_CLASS := ETC
    LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/firmware
    LOCAL_SRC_FILES := $(LOCAL_MODULE)
    include $(BUILD_PREBUILT)
endif
ifeq ($(BUILD_PATCH), true)
    include $(CLEAR_VARS)
ifeq ($(MTK_COMBO_CHIP), MT6620E2)
    LOCAL_MODULE := mt6620_patch_hdr.bin
else
    LOCAL_MODULE := mt6620_patch_e3_2_hdr.bin
endif
    LOCAL_MODULE_TAGS := optional
    LOCAL_MODULE_CLASS := ETC
    LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/firmware
    LOCAL_SRC_FILES := $(LOCAL_MODULE)
    include $(BUILD_PREBUILT)
endif
ifeq ($(BUILD_PATCH), true)
    include $(CLEAR_VARS)
ifeq ($(MTK_COMBO_CHIP), MT6620E2)
    LOCAL_MODULE := mt6620_patch_hdr.bin
else
    LOCAL_MODULE := mt6620_patch_e3_3_hdr.bin
endif
    LOCAL_MODULE_TAGS := optional
    LOCAL_MODULE_CLASS := ETC
    LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/firmware
    LOCAL_SRC_FILES := $(LOCAL_MODULE)
    include $(BUILD_PREBUILT)
endif

ifeq ($(BUILD_PATCH), true)
$(warning building mt6628_patch_e1_hdr) 
    include $(CLEAR_VARS)
    LOCAL_MODULE := mt6628_patch_e1_hdr.bin
    LOCAL_MODULE_TAGS := optional
    LOCAL_MODULE_CLASS := ETC
    LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/firmware
    LOCAL_SRC_FILES := $(LOCAL_MODULE)
    include $(BUILD_PREBUILT)
    
$(warning building mt6628_patch_e2_hdr) 
    include $(CLEAR_VARS)
    LOCAL_MODULE := mt6628_patch_e2_0_hdr.bin
    LOCAL_MODULE_TAGS := optional
    LOCAL_MODULE_CLASS := ETC
    LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/firmware
    LOCAL_SRC_FILES := $(LOCAL_MODULE)
    include $(BUILD_PREBUILT)

    include $(CLEAR_VARS)
    LOCAL_MODULE := mt6628_patch_e2_1_hdr.bin
    LOCAL_MODULE_TAGS := optional
    LOCAL_MODULE_CLASS := ETC
    LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/firmware
    LOCAL_SRC_FILES := $(LOCAL_MODULE)
    include $(BUILD_PREBUILT)
endif

endif
