
LOCAL_PATH:=$(call my-dir)

#For MT66XX combo chip
ifeq ($(strip $(BOARD_CONNECTIVITY_MODULE)),combo_mt66xx)
    include $(LOCAL_PATH)/$(strip $(BOARD_CONNECTIVITY_MODULE))/Android.mk
endif

#For MT6622 + MT5931 Solution
ifeq ($(strip $(BOARD_CONNECTIVITY_MODULE)),mt5931_6622)
    include $(LOCAL_PATH)/$(strip $(BOARD_CONNECTIVITY_MODULE))/Android.mk
endif

#For MT6622 Solution Add by Yuebing
ifeq ($(strip $(BOARD_CONNECTIVITY_MODULE)),mt6622)
    include $(LOCAL_PATH)/mt5931_6622/Android.mk
endif
