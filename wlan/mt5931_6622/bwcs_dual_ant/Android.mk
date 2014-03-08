LOCAL_PATH:= $(call my-dir)

################## build iwlib ###################
include $(CLEAR_VARS)
LOCAL_SRC_FILES := iwlib.c
LOCAL_CFLAGS += -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wpointer-arith -Wcast-qual -Winline -MMD -fPIC
LOCAL_MODULE:= libiw_mtk
LOCAL_STATIC_LIBRARIES := libcutils libc libm
include $(BUILD_STATIC_LIBRARY)



################## build iwevent ###################
include $(CLEAR_VARS)
LOCAL_SRC_FILES := bwcs.c bwcs_sock.c iwevent.c iwlib.h
LOCAL_CFLAGS += -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wpointer-arith -Wcast-qual -Winline -MMD -fPIC
LOCAL_MODULE:= bwcs_dual_ant
LOCAL_STATIC_LIBRARIES := libcutils libc libm libiw_mtk
LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES) # install to system/xbin
include $(BUILD_EXECUTABLE)
