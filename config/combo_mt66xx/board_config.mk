# Collection of required global settings
# This file is supposed to be included from BoardConfig.mk of the platform,
# ex. device/$(vendor)/$(project)/BoardConfig.mk, and set both 
# $(BOARD_CONNECTIVITY_VENDOR) and $(BOARD_CONNECTIVITY_MODULE) properly

ifeq ($(strip $(BOARD_CONNECTIVITY_VENDOR)), MediaTek)

ifeq ($(strip $(BOARD_CONNECTIVITY_MODULE)), combo_mt66xx)

# wlan
WPA_SUPPLICANT_VERSION := VER_0_8_X
#P2P_SUPPLICANT_VERSION := VER_0_8_X
#BOARD_P2P_SUPPLICANT_DRIVER := NL80211
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_HOSTAPD_DRIVER := NL80211
BOARD_WLAN_DEVICE := mediatek
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_mt66xx
BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_mt66xx
#WIFI_DRIVER_MODULE_NAME:="/system/lib/modules/wlan_mt6620.ko"
WIFI_DRIVER_FW_PATH_PARAM:="/dev/wmtWifi"
WIFI_DRIVER_FW_PATH_STA:=STA
WIFI_DRIVER_FW_PATH_AP:=AP
WIFI_DRIVER_FW_PATH_STA:=P2P

# bluetooth
#BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := device/generic/common/bluetooth

# gps
BOARD_GPS_LIBRARIES := libgps_mtk

#fm

endif
endif
