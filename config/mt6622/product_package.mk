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
#	mtk_hif_sdio.ko \
#	mtk_stp_wmt.ko \
#	mtk_stp_gps.ko \
#	mtk_stp_uart.ko \
#	mtk_wmt_wifi.ko \
#	hci_stp.ko \
#	mtk_gps.ko \
#	mtk_fm_drv.ko \
#	wlan_mt6620.ko \
#	wlan_mt6628.ko \
#	bt_uhid.ko

#PRODUCT_COPY_FILES += \
#	kernel/drivers/mtk_wcn_combo/common/mtk_hif_sdio.ko:system/lib/modules/mtk_hif_sdio.ko \
#	kernel/drivers/mtk_wcn_combo/common/mtk_stp_gps.ko:system/lib/modules/mtk_stp_gps.ko \
#	kernel/drivers/mtk_wcn_combo/common/mtk_stp_uart.ko:system/lib/modules/mtk_stp_uart.ko \
#	kernel/drivers/mtk_wcn_combo/common/mtk_stp_sdio.ko:system/lib/modules/mtk_stp_sdio.ko \
#	kernel/drivers/mtk_wcn_combo/common/mtk_stp_wmt.ko:system/lib/modules/mtk_stp_wmt.ko \
#	kernel/drivers/mtk_wcn_combo/common/mtk_stp_bt.ko:system/lib/modules/mtk_stp_bt.ko \
#	kernel/drivers/mtk_wcn_combo/common/mtk_wmt_wifi.ko:system/lib/modules/mtk_wmt_wifi.ko\
#	kernel/drivers/mtk_wcn_combo/drv_wlan/mt6620/wlan/wlan_mt6620.ko:system/lib/modules/wlan_mt6620.ko\
#	kernel/drivers/mtk_wcn_combo/drv_wlan/mt6628/wlan/wlan_mt6628.ko:system/lib/modules/wlan_mt6628.ko\
#	kernel/drivers/mtk_wcn_combo/drv_fm/mtk_fm_drv.ko:system/lib/modules/mtk_fm_drv.ko \
#	kernel/drivers/mtk_wcn_combo/drv_bt_uhid/bt_uhid.ko:system/lib/modules/bt_uhid.ko

#PRODUCT_COPY_FILES +=  \
#frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml

# Common
#PRODUCT_PROPERTY_OVERRIDES += persist.radio.mediatek.chipid=-1
PRODUCT_PACKAGES += \
	init.connectivity.rc \


#PRODUCT_PACKAGES += \
#	iwconfig \
#	iwlist \
#	iwpriv


# BT
PRODUCT_PACKAGES += \
        libbluetooth_mtk \
	MTK_MT6622_E2_Patch.nb0 \
	bwcs

# WLAN
#PRODUCT_PACKAGES += \
#   wlan_loader \
#	hald \
#	hdc \
#	libwifitest \
#	wifitest \
#	wpa_supplicant \
#	wpa_supplicant.conf \
#	wpa_cli \
#	p2p_supplicant.conf \
#	WIFI_RAM_CODE \
#	WIFI_RAM_CODE_E6 \
#	WIFI_RAM_CODE_MT6628 \
#	WIFI_6628 \
#	WIFI \
#	dhcpcd.conf

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
    
