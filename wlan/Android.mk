
LOCAL_PATH:=$(call my-dir)

#For MT66XX combo chip
ifeq ($(strip $(BOARD_CONNECTIVITY_MODULE)),combo_mt66xx)
    include $(LOCAL_PATH)/combo_mt66xx/Android.mk
endif
ifeq ($(strip $(BOARD_CONNECTIVITY_MODULE)),mt5931_6622)
    include $(LOCAL_PATH)/mt5931_6622/Android.mk
endif
