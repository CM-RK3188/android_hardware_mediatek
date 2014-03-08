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
#	wlan_mt6620.ko \
#	wlan_mt6628.ko

#PRODUCT_COPY_FILES += \
#	kernel/drivers/mediatek/combo_mt66xx/wmt/mtk_wmt_wifi.ko:system/lib/modules/mtk_wmt_wifi.ko\
#	kernel/drivers/mediatek/combo_mt66xx/wlan/mt6620/wlan/wlan_mt6620.ko:system/lib/modules/wlan_mt6620.ko\
#	kernel/drivers/mediatek/combo_mt66xx/wlan/mt6628/wlan/wlan_mt6628.ko:system/lib/modules/wlan_mt6628.ko

PRODUCT_COPY_FILES +=  \
frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml

# Common
PRODUCT_PROPERTY_OVERRIDES += persist.radio.mediatek.chipid=-1
PRODUCT_PACKAGES += \
	init.connectivity.rc \
        wmt_launcher \
        wmt_lpbk \
        wmt_concurrency \
        stp_dump3 \
        WMT.cfg \
        mt6620_patch_e3_0_hdr.bin \
        mt6620_patch_e3_1_hdr.bin \
        mt6620_patch_e3_2_hdr.bin \
        mt6620_patch_e3_3_hdr.bin \
        mt6620_patch_e6_hdr.bin \
	mt6628_patch_e1_hdr.bin \
        mt6628_patch_e2_hdr.bin \
        mt6628_patch_e2_0_hdr.bin \
        mt6628_patch_e2_1_hdr.bin 


PRODUCT_PACKAGES += \
	iwconfig \
	iwlist \
	iwpriv


# BT
PRODUCT_PACKAGES += \
        libbluetooth_mtk \
        libbluetooth_mtk_pure \
        libbluetooth_em \
        autobt

# GPS
PRODUCT_PACKAGES += \
        gps.default\
	libmnlp \
	mnld \
        libmnlp_mt6628 \
        libmnlp_mt6620 \
        libmnlp_mt3332 \
        libmnl.a \
        libsupl.a \
        libhotstill.a \
        libagent.a

#AGPS
#PRODUCT_PACKAGES += \
#        mtk_agpsd \
#        libagpssupl \
#        libssladp

# FM
PRODUCT_PACKAGES += \
    libfmjni \
    libfmmt6616 \
    libfmmt6626 \
    libfmmt6620 \
    libfmmt6628 \
    libfmar1000 \
    libfmcust \
    fm_cust.cfg \
    mt6620_fm_cust.cfg \
    mt6628_fm_rom.bin \
    mt6628_fm_v1_patch.bin \
    mt6628_fm_v1_coeff.bin \
    mt6628_fm_v2_patch.bin \
    mt6628_fm_v2_coeff.bin \
    mt6628_fm_v3_patch.bin \
    mt6628_fm_v3_coeff.bin \
    mt6628_fm_v4_patch.bin \
    mt6628_fm_v4_coeff.bin \
    mt6628_fm_v5_patch.bin \
    mt6628_fm_v5_coeff.bin \
    fmtest \
    FMRadio \
    FMTransmitter

PRODUCT_PROPERTY_OVERRIDES += \
    fmradio.driver.chip=1

# WLAN
PRODUCT_PACKAGES += \
    wlan_loader \
	hald \
	hdc \
	libwifitest \
	wifitest \
	wpa_supplicant \
	wpa_supplicant.conf \
	wpa_cli \
	p2p_supplicant.conf \
	WIFI_RAM_CODE \
	WIFI_RAM_CODE_E6 \
	WIFI_RAM_CODE_MT6628 \
	WIFI_6628 \
	WIFI \
	dhcpcd.conf

# HW test tool
PRODUCT_PACKAGES += \
    wcntest \
    wcntestd \
    libfmr \
    libautogps 

#Combo_Tool APK
PRODUCT_PACKAGES += \
    libhwtest_bt_jni \
    libhwtest_fm_jni \
    libhwtest_wifi_jni \
    mysu
    
