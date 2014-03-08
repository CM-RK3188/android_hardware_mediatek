
$(warning [gps]BOARD_MEDIATEK_USES_GPS=$(BOARD_MEDIATEK_USES_GPS))
$(warning [gps]BOARD_CONNECTIVITY_MODULE=$(BOARD_CONNECTIVITY_MODULE))

ifeq ($(strip $(BOARD_MEDIATEK_USES_GPS)),true)
ifeq ($(strip $(BOARD_CONNECTIVITY_MODULE)),combo_mt66xx)

    $(warning [gps]starting to build gps native part...)
    LOCAL_PATH:=$(call my-dir)
    include $(LOCAL_PATH)/combo_mt66xx/Android.mk
    
endif
endif
