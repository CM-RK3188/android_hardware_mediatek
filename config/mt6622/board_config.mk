# Collection of required global settings
# This file is supposed to be included from BoardConfig.mk of the platform,
# ex. device/$(vendor)/$(project)/BoardConfig.mk, and set both 
# $(BOARD_CONNECTIVITY_VENDOR) and $(BOARD_CONNECTIVITY_MODULE) properly

ifeq ($(strip $(BOARD_CONNECTIVITY_VENDOR)), MediaTek)

ifeq ($(strip $(BOARD_CONNECTIVITY_MODULE)), mt5931_6622)

# wlan
#WPA_SUPPLICANT_VERSION := VER_0_8_X_MEDIATEK
#P2P_SUPPLICANT_VERSION := VER_0_8_X_MEDIATEK
#BOARD_P2P_SUPPLICANT_DRIVER := NL80211

# bluetooth
#BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := device/generic/common/bluetooth

endif

ifeq ($(strip $(BOARD_CONNECTIVITY_MODULE)), mt6622)
# bluetooth
#BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := device/generic/common/bluetooth
endif


endif
