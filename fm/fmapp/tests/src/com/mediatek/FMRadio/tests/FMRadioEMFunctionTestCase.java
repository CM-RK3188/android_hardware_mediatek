/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

package com.mediatek.FMRadio.tests;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import android.app.Activity;
import android.app.Instrumentation;
import android.test.ActivityInstrumentationTestCase2;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.RadioButton;
import android.widget.TextView;
import android.content.Context;
import com.mediatek.common.featureoption.FeatureOption;

import com.jayway.android.robotium.solo.Solo;
import com.mediatek.FMRadio.FMRadioActivity;
import com.mediatek.FMRadio.FMRadioEMActivity;
import com.mediatek.FMRadio.FMRadioFavorite;
import com.mediatek.FMRadio.FMRadioStation;
import com.mediatek.FMRadio.FMRadioUtils;
import com.mediatek.FMRadio.R;


public class FMRadioEMFunctionTestCase extends
        ActivityInstrumentationTestCase2<FMRadioEMActivity> {
    private Solo mSolo = null;
    private TextView mTextViewFrequency = null;
    private FMRadioEMActivity mFMRadioEMActivity = null;
    private Context mContext = null; 
    private Instrumentation mInstrumentation = null;
    private Button mButtonTune = null;
    private RadioButton mRdAntennaS = null;
    private RadioButton mRdAntennaL = null;
    
    private EditText mEditFreq = null;
    
    private TextView mTextStereoMono = null;
    private TextView mTextRssi = null;
    private TextView mTextCapArray = null;
    private TextView mTextRdsBler = null;
    private TextView mTextRdsPS = null;
    private TextView mTextRdsRT = null;
    private TextView mTextChipID = null;
    private TextView mTextECOVersion = null;
    private TextView mTextPatchVersion = null;
    private TextView mTextDSPVersion = null;
    private static final int CONVERT_RATE = FeatureOption.MTK_FM_50KHZ_SUPPORT ? 100 : 10;
    private static final int CHECK_TIME = 200;
    private static final int EXECUTE_TIME = 1000;
    private static final int POWER_UP_TIME = 2000;
    
    public FMRadioEMFunctionTestCase() {
        super("com.mediatek.FMRadio", FMRadioEMActivity.class);
    }
    @Override
    protected void setUp() throws Exception {
        super.setUp();
        mSolo = new Solo(getInstrumentation(), getActivity());
        mInstrumentation = getInstrumentation();
        mFMRadioEMActivity = (FMRadioEMActivity)getActivity();
        mContext = mFMRadioEMActivity.getApplicationContext();
        mButtonTune = (Button) mFMRadioEMActivity.findViewById(R.id.FMR_Freq_tune);
        mRdAntennaS = (RadioButton) mFMRadioEMActivity.findViewById(R.id.FMR_Antenna_short);
        mRdAntennaL = (RadioButton) mFMRadioEMActivity.findViewById(R.id.FMR_Antenna_long);
        mTextStereoMono = (TextView) mFMRadioEMActivity.findViewById(R.id.FMR_Status_Stereomono);
        mTextRssi = (TextView) mFMRadioEMActivity.findViewById(R.id.FMR_Status_RSSI);
        mTextCapArray = (TextView) mFMRadioEMActivity.findViewById(R.id.FMR_Status_Caparray);
        mTextRdsBler = (TextView) mFMRadioEMActivity.findViewById(R.id.FMR_RDS_Ratio);
        mTextRdsPS = (TextView) mFMRadioEMActivity.findViewById(R.id.FMR_RDS_PS);
        mTextRdsRT = (TextView) mFMRadioEMActivity.findViewById(R.id.FMR_RDS_RT);
        mTextChipID = (TextView) mFMRadioEMActivity.findViewById(R.id.FMR_Chip_ID);
        mTextECOVersion = (TextView) mFMRadioEMActivity.findViewById(R.id.FMR_ECO_Version);
        mTextPatchVersion = (TextView) mFMRadioEMActivity.findViewById(R.id.FMR_Patch_Version);
        mTextDSPVersion = (TextView) mFMRadioEMActivity.findViewById(R.id.FMR_DSP_Version);
//        mEditFreq = (EditText) mFMRadioEMActivity.findViewById(R.id.FMR_Freq_edit);
    }
    
    public void testCase01_Tune() {
        mInstrumentation.waitForIdleSync();
        mEditFreq = (EditText) mSolo.getView(R.id.FMR_Freq_edit);
        mSolo.clearEditText(mEditFreq);
        float cStation = 91.4f;
        mSolo.enterText(mEditFreq, String.valueOf(cStation));
        mInstrumentation.waitForIdleSync();
        InputMethodManager inputMethodManager = (InputMethodManager) mSolo.getCurrentActivity().getSystemService(Context.INPUT_METHOD_SERVICE);
        inputMethodManager.toggleSoftInput(0, 0);
        mSolo.clickOnButton(mContext.getString(R.string.FMR_Freq_tune));
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(EXECUTE_TIME);
        assertEquals((int) (cStation * CONVERT_RATE), FMRadioStation.getCurrentStation(mFMRadioEMActivity));
        mSolo.sleep(CHECK_TIME);
        checkUIParams();
    }
    
    public void testCase02_AntennaType() {
        mSolo.clickOnButton(mContext.getString(R.string.FMR_Antenna_short));
        mInstrumentation.waitForIdleSync();
        checkUIParams();
        mSolo.clickOnButton(mContext.getString(R.string.FMR_Antenna_long));
        mInstrumentation.waitForIdleSync();
        checkUIParams();
    }
    
    public void testCase03_ForceSet() {
        mSolo.clickOnButton(mContext.getString(R.string.FMR_Stereomono_mono));
        mInstrumentation.waitForIdleSync();
        checkUIParams();
        mSolo.clickOnButton(mContext.getString(R.string.FMR_Stereomono_stereo));
        mInstrumentation.waitForIdleSync();
        checkUIParams();
    }
    
    public void checkUIParams() {
        try {
            Thread.sleep(200);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        String chipStr = getStringFromMethod(mFMRadioEMActivity, "getChipId");
        assertEquals(chipStr, mTextChipID.getText());
        String eocStr = getStringFromMethod(mFMRadioEMActivity, "getEcoVersion");
        assertEquals(eocStr, mTextECOVersion.getText());
        String patchStr = getStringFromMethod(mFMRadioEMActivity, "getPatchVersion");
        assertEquals(patchStr, mTextPatchVersion.getText());
        String dspStr = getStringFromMethod(mFMRadioEMActivity, "getDspVersion");
        assertEquals(dspStr, mTextDSPVersion.getText());
    }
    
    private void sleep(long milliseconds) {
        try {
            Thread.sleep(milliseconds);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
    
    private int getIntFromMethod(Activity activity, String method) {
        int value = -1;
        Class c = mFMRadioEMActivity.getClass();
        try {
            Method m = (Method) c.getDeclaredMethod(method, new Class[] {});
            m.setAccessible(true);
            value = (Integer) m.invoke(activity, new Object[] {});
        } catch (SecurityException e) {
            e.printStackTrace();
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        }
        return value;
    }
    
    private boolean getBooleanFromMethod(Activity activity, String method) {
        boolean value = false;
        Class c = mFMRadioEMActivity.getClass();
        try {
            Method m = (Method) c.getDeclaredMethod(method, new Class[] {});
            m.setAccessible(true);
            value = (Boolean) m.invoke(activity, new Object[] {});
        } catch (SecurityException e) {
            e.printStackTrace();
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        }
        return value;
    }
    
    private String getStringFromMethod(Activity activity, String method) {
        String value = "";
        Class c = mFMRadioEMActivity.getClass();
        try {
            Method m = (Method) c.getDeclaredMethod(method, new Class[] {});
            m.setAccessible(true);
            value = (String) m.invoke(activity, new Object[] {});
        } catch (SecurityException e) {
            e.printStackTrace();
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        }
        return value;
    }
    
    private String getStringFromVariable(Activity activity, String variable) {
        Field field = null;
        String value = "";
        try {
            field = FMRadioEMActivity.class.getDeclaredField(variable);
            field.setAccessible(true);
            value = (String) field.get(activity);
        } catch (SecurityException e) {
            e.printStackTrace();
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        }
        return value;
    }
    
    @Override
    protected void tearDown() throws Exception {
        try {
            mSolo.finalize();
        } catch (Throwable t) {
            t.printStackTrace();
        }
        super.tearDown();
    }
    

}
