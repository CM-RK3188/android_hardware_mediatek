LOCAL_PATH:= $(call my-dir)
    include $(CLEAR_VARS)
	LOCAL_MODULE_TAGS := optional
    LOCAL_SRC_FILES := libwifitest.c iwlib.c lib.c
    LOCAL_SHARED_LIBRARIES := liblog
    LOCAL_PRELINK_MODULE := false
    LOCAL_MODULE := libwifitest
    include $(BUILD_SHARED_LIBRARY)

    include $(CLEAR_VARS)
    LOCAL_MODULE := wifitest
    LOCAL_MODULE_TAGS := eng
    LOCAL_SHARED_LIBRARIES := libwifitest
    LOCAL_CFLAGS += -Wall
    LOCAL_SRC_FILES := main.c
    include $(BUILD_EXECUTABLE)

