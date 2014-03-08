# Required global PRODUCT_* settings 
# This file is supposed to be included by the $(PRODUCT_MAKEFILES) file, which 
# is specified in device/$(vendor)/$(product)/AndroidProduct.mk. In most cases, 
# you can inherit this file from the subsidiary *.mk files inherted by 
# $(PRODUCT_MAKEFILES) file. 
# 
# Here is the example:
# ifeq ($(CONNECTIVITY_SOLUTION),MediaTek)
# $(call inherit-product-if-exists, hardware/mediatek/$(CONNECTIVITY_MODULE)/product_package.mk)
# endif

#$(info PRODUCT_PACKAGES include MT6620 )

# kernel drivers
#PRODUCT_PACKAGES += \
#	mtk_wmt_wifi.ko \
#	wlan_mt5931.ko


#PRODUCT_COPY_FILES += \
#	kernel/drivers/net/wireless/mt5931/drv_wlan/mtk_wmt_wifi.ko:system/lib/modules/mtk_wmt_wifi.ko \
#	kernel/drivers/net/wireless/mt5931/drv_wlan/wlan_mt5931.ko:system/lib/modules/wlan_mt5931.ko

PRODUCT_COPY_FILES +=  \
frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml

# Common
#PRODUCT_PROPERTY_OVERRIDES += persist.radio.mediatek.chipid=-1
PRODUCT_PACKAGES += \
	init.connectivity.rc


PRODUCT_PACKAGES += \
	iwconfig \
	iwlist \
	iwpriv


# BT
PRODUCT_PACKAGES += \
        libbluetooth_mtk \
	MTK_MT6622_E2_Patch.nb0 \
	bwcs

# WLAN
PRODUCT_PACKAGES += \
	wpa_supplicant \
	wpa_supplicant.conf \
	p2p_supplicant.conf \
	WIFI_RAM_CODE_MT5931 \
	WIFI_MT5931 \
	dhcpcd.conf


# HW test tool
#PRODUCT_PACKAGES += \
#    wcntest \
#    wcntestd \
#    libfmr \
#    libautogps 

#Combo_Tool APK
PRODUCT_PACKAGES += \
    libhwtest_bt_jni \
    libhwtest_fm_jni \
    libhwtest_wifi_jni \
    mysu
    
