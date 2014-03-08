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
import android.content.Context;
import android.media.AudioManager;
import android.os.SystemClock;
import android.test.ActivityInstrumentationTestCase2;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.widget.ImageButton;
import android.widget.TextView;
import android.util.Log;
import android.widget.ListView;
import android.app.Instrumentation.ActivityMonitor;
import com.jayway.android.robotium.solo.Solo;

import com.mediatek.FMRadio.FMRadioActivity;
import com.mediatek.FMRadio.FMRadioFavorite;
import com.mediatek.FMRadio.FMRadioStation;
import com.mediatek.FMRadio.FMRadioUtils;
import com.mediatek.FMRadio.R;

public class FMRadioPerformanceTest extends ActivityInstrumentationTestCase2<FMRadioActivity> {

    public FMRadioPerformanceTest(Class<FMRadioActivity> activityClass) {
        super(activityClass);
    }
    
    public FMRadioPerformanceTest(){
        super(FMRadioActivity.class);
    }
    
    private AudioManager mAudioManager = null;
    
    // bottom bar
    private ImageButton mButtonDecrease = null;
    private ImageButton mButtonPrevStation = null;
    private ImageButton mButtonNextStation = null;
    private ImageButton mButtonIncrease = null;
//    private ImageButton mButtonPlayStop = null;
    
    private ImageButton mButtonAddToFavorite = null;
    
    private TextView mTextViewFrequency = null;
    
    private static final long TIMEOUT = 5000;
    private static final long CHECK_TIME = 100;
    private static final long SHORT_TIME = 3000;
    private static final long WAIT_UI_STATE_CHANGE = 10000;
    private static final long WAIT_SCAN_FINISH = 20000;
    private static final long WAIT_TEAR_DOWN = 3000;
    private static final int CONVERT_RATE = 10;
    private static final String TAG = "FMRadioPerformanceTest";

    private FMRadioActivity mFMRadioActivity = null;
    private Instrumentation mInstrumentation = null;
    private ActivityMonitor mActivityMonitor = null;
    private FMRadioFavorite mFMRadioFavorite = null;
    private Solo mSolo = null;

    @Override
    public void setUp() throws Exception {
        super.setUp();
        Log.i(TAG, "setUp");
        setActivityInitialTouchMode(false);
        mFMRadioActivity = getActivity();
        assertNotNull(mFMRadioActivity);
        mInstrumentation = getInstrumentation();
        assertNotNull(mInstrumentation);
        mSolo = new Solo(getInstrumentation(), mFMRadioActivity);
        Context mContext = mFMRadioActivity.getApplicationContext();
        mAudioManager = (AudioManager) mFMRadioActivity.getSystemService(mContext.AUDIO_SERVICE);
        // Makesure FMRadio is playing and initialed
        waitForPowerupWithTimeout(TIMEOUT);
        boolean isPlaying = getBooleanFromVariable(mFMRadioActivity, "mIsPlaying");
        assertTrue(isPlaying);
        waitForInitedWithTimeout(TIMEOUT);
        boolean isInited = getBooleanFromVariable(mFMRadioActivity, "mIsServiceBinded");
        assertTrue(isInited);
        mButtonDecrease = (ImageButton) mFMRadioActivity.findViewById(R.id.button_decrease);
        mButtonPrevStation = (ImageButton) mFMRadioActivity.findViewById(R.id.button_prevstation);
        mButtonNextStation = (ImageButton) mFMRadioActivity.findViewById(R.id.button_nextstation);
        mButtonIncrease = (ImageButton) mFMRadioActivity.findViewById(R.id.button_increase);
//        mButtonPlayStop = (ImageButton) mFMRadioActivity.findViewById(R.id.button_play_stop);
        
        mButtonAddToFavorite = (ImageButton) mFMRadioActivity.findViewById(R.id.button_add_to_favorite);
        mTextViewFrequency = (TextView) mFMRadioActivity.findViewById(R.id.station_value);
    }

    
    private void waitForInitedWithTimeout(long timeOut) {
        Log.i(TAG, ">>>waitForInitedWithTimeout");

        long startTime = System.currentTimeMillis();
        while (!getBooleanFromVariable(mFMRadioActivity, "mIsServiceBinded")) {
            if (System.currentTimeMillis() - startTime > timeOut) {
                break;
            }
            sleep(CHECK_TIME);
        }
        Log.i(TAG, "<<<waitForInitedWithTimeout");

    }

    private void waitForPowerupWithTimeout(long timeOut) {
        Log.i(TAG, ">>>waitForPowerupWithTimeout");

        long startTime = System.currentTimeMillis();
        while (!getBooleanFromVariable(mFMRadioActivity, "mIsPlaying")) {
            if (System.currentTimeMillis() - startTime > timeOut) {
                break;
            }
            sleep(CHECK_TIME);
        }
        Log.i(TAG, "<<<waitForPowerupWithTimeout");
    }
    
    /**
     * Test scan channel performance.
     */
    public void testCase00_ScanChannelPerformance() throws Exception {
        if (!mButtonDecrease.isEnabled()) {
            makeFMPowerUp();
        }
        mInstrumentation.invokeMenuActionSync(mFMRadioActivity, R.id.fm_menu, 0);
        mInstrumentation.waitForIdleSync();
        mInstrumentation.sendKeyDownUpSync(KeyEvent.KEYCODE_DPAD_DOWN);
        mInstrumentation.sendKeyDownUpSync(KeyEvent.KEYCODE_DPAD_UP);
        mInstrumentation.waitForIdleSync();
       long startTime = System.currentTimeMillis();
       Log.i(TAG, "[Performance test][FMRadio] scan channel start [" + startTime + "]");
       mInstrumentation.sendKeyDownUpSync(KeyEvent.KEYCODE_DPAD_CENTER);
       sleep(WAIT_SCAN_FINISH);
       mInstrumentation.sendKeyDownUpSync(KeyEvent.KEYCODE_BACK);
       sleep(SHORT_TIME);
    }

    /**
     * Test open channel performance.
     */
    /*public void testCase01_ChannelOpenPerformance() throws Exception {
        int station = getStationFromUI();
        addChannelAsFavorite(station);
        clickView(mButtonIncrease);
        sleep(SHORT_TIME);
        mInstrumentation.invokeMenuActionSync(mFMRadioActivity, R.id.fm_channel_list, 0);
        mActivityMonitor = new ActivityMonitor("com.mediatek.FMRadio.FMRadioFavorite", null, false);
        mInstrumentation.addMonitor(mActivityMonitor);
        mFMRadioFavorite = (FMRadioFavorite) mActivityMonitor.waitForActivityWithTimeout(TIMEOUT);
        assertNotNull(mFMRadioFavorite);
        ListView listView = (ListView) mFMRadioFavorite.findViewById(R.id.station_list);
        mSolo.clickOnView(listView.getChildAt(0));
        long startTime = System.currentTimeMillis();
        Log.i(TAG, "[Performance test][FMRadio] open channel start [" + startTime + "]");
        sleep(WAIT_UI_STATE_CHANGE);
        
    }*/
    
    /**
     * Test increase and decrease 0.1 MHz / 0.05 MHZ performance.
     */
    public void testCase02_FrequencyChangePerformance() throws Exception {
        
        // test decrease frequency performance
        long startTime = System.currentTimeMillis();
        Log.i(TAG, "[Performance test][FMRadio] decrease frequency start [" + startTime + "]");
        clickView(mButtonDecrease);
        sleep(WAIT_UI_STATE_CHANGE);
        
        // test increase frequency performance
        startTime = System.currentTimeMillis();
        Log.i(TAG, "[Performance test][FMRadio] increase frequency start [" + startTime + "]");
        clickView(mButtonIncrease);
        sleep(WAIT_UI_STATE_CHANGE);
                
    } 

    /**
     * Test seek channel performance.
     */
    public void testCase03_SeekChannelPerformance() throws Exception {
        // test seek previous station performance
        int stationGap = 0;
        int currentStation = FMRadioStation.getCurrentStation(mFMRadioActivity);
        long startTime = System.currentTimeMillis();
        Log.i(TAG, "[Performance test][FMRadio] seek previous channel start [" + startTime + "]");
        clickView(mButtonPrevStation);
        sleep(WAIT_UI_STATE_CHANGE);
        int searchStation = FMRadioStation.getCurrentStation(mFMRadioActivity);
        if (searchStation > currentStation) {
            stationGap = searchStation - currentStation;
            Log.i(TAG,"[Performance test][FMRadio] Test FMRadio seek time stationStep ["+ (float)stationGap / CONVERT_RATE  +"]" );
        } else if (searchStation < currentStation) {
            stationGap = FMRadioUtils.HIGHEST_STATION - currentStation + searchStation - FMRadioUtils.LOWEST_STATION;
            Log.i(TAG,"[Performance test][FMRadio] Test FMRadio seek time stationStep ["+ (float)stationGap / CONVERT_RATE  +"]" );
        } else {
            Log.e(TAG, "SearchStation Unchanged");
        }
        // test seek next station frequency performance
        currentStation = FMRadioStation.getCurrentStation(mFMRadioActivity);
        startTime = System.currentTimeMillis();
        Log.i(TAG, "[Performance test][FMRadio] seek next channel start [" + startTime + "]");
        clickView(mButtonNextStation);
        sleep(WAIT_UI_STATE_CHANGE);
        if (searchStation > currentStation) {
            stationGap = searchStation - currentStation;
            Log.i(TAG,"[Performance test][FMRadio] Test FMRadio seek time stationStep ["+ (float)stationGap / CONVERT_RATE +"]" );
        } else if (searchStation < currentStation) {
            stationGap = FMRadioUtils.HIGHEST_STATION - currentStation + searchStation - FMRadioUtils.LOWEST_STATION;
            Log.i(TAG,"[Performance test][FMRadio] Test FMRadio seek time stationStep ["+ (float)stationGap / CONVERT_RATE +"]" );
        } else {
            Log.e(TAG, "SearchStation Unchanged");
        }
    }
    
    /**
     * Test switch earphone and speaker performance.
     */
    public void testCase04_SwitchSpeakerEarphone() throws Exception {
        if (!mButtonDecrease.isEnabled()) {
            makeFMPowerUp();
        }
        String earphone = mFMRadioActivity.getString(R.string.optmenu_earphone);
        String speaker = mFMRadioActivity.getString(R.string.optmenu_speaker);

        // test switch speaker performance
        mInstrumentation.invokeMenuActionSync(mFMRadioActivity, R.id.fm_menu, 0);
        mSolo.clickOnText(speaker);
        sleep(SHORT_TIME);
        mInstrumentation.invokeMenuActionSync(mFMRadioActivity, R.id.fm_menu, 0);
        assertTrue(mSolo.searchText(earphone));
        mInstrumentation.waitForIdleSync();

        // test switch earphone performance
        mInstrumentation.invokeMenuActionSync(mFMRadioActivity, R.id.fm_menu, 0);
        mSolo.clickOnText(earphone);
        sleep(SHORT_TIME);
        mInstrumentation.invokeMenuActionSync(mFMRadioActivity, R.id.fm_menu, 0);
        assertTrue(mSolo.searchText(speaker));
    }

    /**
     * Test power up performance.
     */
    public void testCase05_PowerUpPerformance() throws Exception {
        if (mButtonDecrease.isEnabled()) {
            makeFMPowerDown();
        }
        // test power up performance
        long startTime = System.currentTimeMillis();
        Log.i(TAG, "[Performance test][FMRadio] power up start [" + startTime + "]");
//        clickView(mButtonPlayStop);
        mInstrumentation.invokeMenuActionSync(mFMRadioActivity, R.id.fm_power, 0);
        sleep(WAIT_UI_STATE_CHANGE);
    }
    
    public void testCase06_PowerDownPerformance() throws Exception {
        if (!mButtonDecrease.isEnabled()) {
            makeFMPowerUp();
        }
        // test power down performance
        long startTime = System.currentTimeMillis();
        Log.i(TAG, "[Performance test][FMRadio] power down start [" + startTime + "]");
        FMRadioTestCaseUtil.requestFocusGain(mAudioManager);
        sleep(WAIT_UI_STATE_CHANGE);
    }
    

    private void makeFMPowerUp() {
        boolean isPlaying = false;
        isPlaying = getBooleanFromVariable(mFMRadioActivity, "mIsPlaying");
        if (!isPlaying) {
            mInstrumentation
            .invokeMenuActionSync(mFMRadioActivity, R.id.fm_power, 0);
        }
        sleep(WAIT_UI_STATE_CHANGE);
       
    }

    private void makeFMPowerDown() {
        boolean isPlaying = true;
        isPlaying = getBooleanFromVariable(mFMRadioActivity, "mIsPlaying");
        if (isPlaying) {
            FMRadioTestCaseUtil.requestFocusGain(mAudioManager);
        }
        sleep(WAIT_UI_STATE_CHANGE);
        
    }
    
    private void switchEarphone() {
        if (!mButtonDecrease.isEnabled()) {
            makeFMPowerUp();
        }
        mInstrumentation
                .invokeMenuActionSync(mFMRadioActivity, R.id.fm_menu, 0);
        String earphone = mFMRadioActivity.getString(R.string.optmenu_earphone);
        mSolo.clickOnText(earphone);
        mInstrumentation.waitForIdleSync();
    }

    private void switchSpeaker() {
        if (!mButtonDecrease.isEnabled()) {
            makeFMPowerUp();
        }
        mInstrumentation
                .invokeMenuActionSync(mFMRadioActivity, R.id.fm_menu, 0);
        String speaker = mFMRadioActivity.getString(R.string.optmenu_speaker);
        mSolo.clickOnText(speaker);
        mInstrumentation.waitForIdleSync();

    }
    
    private boolean getBooleanFromVariable(Activity activity, String variable) {
        Field field = null;
        boolean value = false;
        try {
            field = FMRadioActivity.class.getDeclaredField(variable);
            field.setAccessible(true);
            value = ((Boolean) field.get(activity)).booleanValue();
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
    
    private boolean getBooleanFromMethod(Activity activity, String method) {

        boolean value = false;
        Class c = mFMRadioActivity.getClass();
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
    
    private void clickView(final View view) {
        try {
            runTestOnUiThread(new Runnable() {
                public void run() {
                    view.performClick();

                }
            });
        } catch (Throwable t) {
            t.printStackTrace();
        }
    }

    private void sleep(long milliseconds) {
        try {
            Thread.sleep(milliseconds);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    private void addChannelAsFavorite(int station) {
        if (!FMRadioStation.isFavoriteStation(mFMRadioActivity, station)) {
            clickView(mButtonAddToFavorite);
            mInstrumentation.waitForIdleSync();
            sleep(SHORT_TIME);
        }

    }
    
    private int getStationFromUI() {
        int station = 0;
        float frequency = 0;
        mTextViewFrequency = (TextView) mFMRadioActivity.findViewById(R.id.station_value);
        String frequencyStr = mTextViewFrequency.getText().toString();
        try {
            frequency = Float.parseFloat(frequencyStr);
        } catch (NumberFormatException e) {
            e.printStackTrace();
        }
        station = (int) (frequency * CONVERT_RATE);
        return station;

    }
    
    @Override
    public void tearDown() throws Exception {
        Log.i(TAG, "tearDown");
        try {
            mSolo.finalize();
        } catch (Throwable t) {
            t.printStackTrace();
        }
        mSolo.finishOpenedActivities();
        sleep(WAIT_TEAR_DOWN);
        super.tearDown();
    }


}
