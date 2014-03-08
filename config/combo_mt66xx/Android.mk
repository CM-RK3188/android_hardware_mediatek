

LOCAL_PATH := $(call my-dir)

# install init.mt662x.rc into out/target/product/$(project)/root folder, so that
# it can be import by the main init.rc or init.$(project).rc of the system.
include $(CLEAR_VARS)
LOCAL_MODULE := init.connectivity.rc
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT)
LOCAL_SRC_FILES := init.combo_mt66xx.rc
include $(BUILD_PREBUILT)

