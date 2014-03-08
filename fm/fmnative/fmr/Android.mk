#ifeq ($(MTK_FM_SUPPORT), yes)

FM_LIB_BUILD_AR1000 := yes
FM_LIB_BUILD_MT6616 := yes
FM_LIB_BUILD_MT6620 := yes
FM_LIB_BUILD_MT6626 := yes
FM_LIB_BUILD_MT6628 := yes
FM_LIB_BUILD_MT519X := no

LOCAL_PATH := $(call my-dir)
###############################################################################
# Define MTK FM Radio Chip solution
###############################################################################

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	fmr_core.cpp \
	fmr_err.cpp \
	libfm_jni.cpp 
	
LOCAL_C_INCLUDES := $(JNI_H_INCLUDE) \
	frameworks/base/include/media

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libdl \
	libmedia \

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libfmjni
include $(BUILD_SHARED_LIBRARY)

########################
ifeq ($(FM_LIB_BUILD_MT6616), yes)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	mt6616.cpp \
  common.cpp \
  bt_ctrl.cpp
	
LOCAL_C_INCLUDES := $(JNI_H_INCLUDE) \
	frameworks/base/include/media

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libdl \
	libmedia

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libfmmt6616
include $(BUILD_SHARED_LIBRARY)
endif

########################
ifeq ($(FM_LIB_BUILD_MT6626), yes)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	mt6626.cpp \
  common.cpp \
  bt_ctrl.cpp
	
LOCAL_C_INCLUDES := $(JNI_H_INCLUDE) \
	frameworks/base/include/media

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libdl \
	libmedia

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libfmmt6626
include $(BUILD_SHARED_LIBRARY)
endif

########################
ifeq ($(FM_LIB_BUILD_MT6620), yes)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	mt6620.cpp \
  common.cpp
  
LOCAL_C_INCLUDES := $(JNI_H_INCLUDE) \
	frameworks/base/include/media

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libdl \
	libmedia

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libfmmt6620
include $(BUILD_SHARED_LIBRARY)
endif

########################
ifeq ($(FM_LIB_BUILD_MT6628), yes)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	mt6628.cpp \
	common.cpp 
	
LOCAL_C_INCLUDES := $(JNI_H_INCLUDE) \
	frameworks/base/include/media \

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libdl \
	libmedia

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libfmmt6628
include $(BUILD_SHARED_LIBRARY)
endif

########################
ifeq ($(FM_LIB_BUILD_AR1000), yes)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	ar1000.cpp \
  common.cpp
  
LOCAL_C_INCLUDES := $(JNI_H_INCLUDE) \
	frameworks/base/include/media

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libdl \
	libmedia

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libfmar1000
include $(BUILD_SHARED_LIBRARY)
endif

########################
ifeq ($(FM_LIB_BUILD_MT519X), yes)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	mt519x.cpp \
  common.cpp
  
LOCAL_C_INCLUDES := $(JNI_H_INCLUDE) \
	frameworks/base/include/media

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libdl \
	libmedia

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libfmmt519x
include $(BUILD_SHARED_LIBRARY)
endif

########################

#endif
