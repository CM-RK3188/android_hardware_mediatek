/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

package mediatek.android.application.Advanced_Tool;

public class WiFiHWTestNative {
 
    private int mTxGain;
    private int mPadloadLen;
    private int mCount;
    private int mFreqOffset;
    private int mPreambleIndex;
    private int mRateIndex;
    private int mModeIndex;
	private int mChannelIndex;
	private int mBandWidthIndex;
	private int mPreamble11nIndex;
	private int mGITypeIndex;
	 
    public  boolean bIsRxStart = false;
    public  boolean bIsTxStart = false;
    public  boolean bIs5GNeeded = true;

    public WiFiHWTestNative()
    {
    	mChannelIndex = 0;
        mBandWidthIndex = 0;
        mTxGain = 0;
        mPadloadLen = 0;
        mCount = 0;
        mRateIndex = 0;
        mModeIndex = 0;
        mGITypeIndex = 0;
        mFreqOffset = 30;
    }
    
    public native int WiFi_Init();
    public native boolean WiFi_Uninit();
    /**
     * Initial wifi
     * 
     * @return Chip ID, like 0x6620 or 0x5921
     */
    public native boolean WiFi_RxStart(int iChannelIdx, int iBandWidthIdx);

    /**
     * Uninitial wifi after test complete
     * 
     * @return ERROR_RFTEST_XXXX
     */
    public native boolean WiFi_RxStop();
    
    public native int[] WiFi_GetRxResult();
    
    public native boolean WiFi_TxStart();
    
    public native boolean WiFi_TxStop();
    
    public native int WiFi_MT76xxGetEfuse(int iType, int iChannel);
    
    public void setChannel(int val) {
    	mChannelIndex = val;
    }
    
    public void setBandWidth(int val) {
    	mBandWidthIndex = val;
    }
    
    public void setTxGain(int val) {
    	mTxGain = val;
    }
    
    public void setPadloadLen(int val) {
    	mPadloadLen = val;
    }
    
    public void setCount(int val) {
    	mCount = val;
    }
    
    public void setPreamble(int val) {
    	mPreambleIndex = val;
    }
    
    public void setRate(int val) {
    	mRateIndex = val;
    }
    
    public void setMode(int val) {
    	mModeIndex = val;
    }
    
    public void setGIType(int val) {
    	mGITypeIndex = val;
    }
    
    public void setFreqOffset(int val) {
    	mFreqOffset = val;
    }
    
    static {
        System.loadLibrary("hwtest_wifi_jni");
    }

}
