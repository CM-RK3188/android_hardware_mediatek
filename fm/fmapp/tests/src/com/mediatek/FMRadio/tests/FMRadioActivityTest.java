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

import java.io.File;
import java.lang.reflect.Field;
import com.jayway.android.robotium.solo.Solo;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import android.app.ActionBar;
import android.app.AlertDialog;
import android.app.Instrumentation;
import android.app.Instrumentation.ActivityMonitor;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.media.AudioManager;
import android.media.AudioManager.OnAudioFocusChangeListener;
import android.net.Uri;
import android.os.Environment;
import android.test.ActivityInstrumentationTestCase2;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.app.Activity;
import com.mediatek.common.featureoption.FeatureOption;

import com.mediatek.FMRadio.FMRadioActivity;
import com.mediatek.FMRadio.FMRadioFavorite;
import com.mediatek.FMRadio.FMRadioUtils;
import com.mediatek.FMRadio.FMRadioStation;
import com.mediatek.FMRadio.FMRecorder;
import com.mediatek.FMRadio.FMRecordDialogFragment;
import com.mediatek.FMRadio.LogUtils;
import com.mediatek.FMRadio.R;
import com.mediatek.FMRadio.dialogs.SearchChannelsDialog;

public class FMRadioActivityTest extends ActivityInstrumentationTestCase2<FMRadioActivity> {
    private Instrumentation mInstrumentation = null;
    private Solo mSolo = null;
    private Context mContext = null;
    private ActivityMonitor mActivityMonitor = null;
    
    private ActionBar mActionBar = null;
    private FMRadioActivity mFMRadioActivity = null;
    private FMRadioFavorite mFMRadioFavorite = null;
    private AudioManager mAudioManager = null;
    private OnAudioFocusChangeListener mFMAudioFocusChangeListener = null;
    
    // bottom bar
    private ImageButton mButtonDecrease = null;
    private ImageButton mButtonPrevStation = null;
    private ImageButton mButtonNextStation = null;
    private ImageButton mButtonIncrease = null;
//    private ImageButton mButtonPlayStop = null;
    private MenuInflater mMenuInflater = null;
    // the star
    private ImageButton mButtonAddToFavorite = null;
    // record bar
    private ImageButton mButtonRecord = null;
    private ImageButton mButtonStop = null;
    private ImageButton mButtonPlayback = null;
    private AlertDialog mDialogRDSSetting = null;
    
    private Button mButtonRecordSave = null;
    private EditText mEditTextRecordName = null;
    
    private static final String CONTENTURI = "content://com.mediatek.FMRadio.FMRadioContentProvider/station";

    private TextView mTextViewFrequency = null;
    private static final int CHECK_TIME = 100;
    private static final int SHORT_TIME = 2000;
    private static final int ONE_SECOND = 1000;
    private static final int EXECUTE_TIME = 500;
    private static final int TIMEOUT = 5000;
    private static final int WAIT_TEAR_DOWN = 3000; 
    private static final int WAIT_MOUNT_UNMOUNT = 7000;
    private static final int RECORDING_FILE_TIME = 10000;
    
    private static final long SEARCH_TIME = FeatureOption.MTK_FM_50KHZ_SUPPORT ? 25000 : 20000;
    private static final int CONVERT_RATE = FeatureOption.MTK_FM_50KHZ_SUPPORT ? 100 : 10;
    private static final int TUNE_STEP = FeatureOption.MTK_FM_50KHZ_SUPPORT ? 5 : 1;
    private static final String TAG = "FMRadioFunctionTest";

    public FMRadioActivityTest() {
        super("com.mediatek.FMRadio", FMRadioActivity.class);
    }

    public FMRadioActivityTest(String pkg, Class<FMRadioActivity> activityClass) {
        super("com.mediatek.FMRadio", FMRadioActivity.class);
    }

    @Override
    protected void setUp() {
        try {
            super.setUp();
        } catch (Exception e) {
            e.printStackTrace();
        }
        setActivityInitialTouchMode(false);
        mInstrumentation = this.getInstrumentation();
        assertNotNull(mInstrumentation);
        mFMRadioActivity = getActivity();
        assertNotNull(mFMRadioActivity);
        mSolo = new Solo(getInstrumentation(), mFMRadioActivity);
        mContext = mFMRadioActivity.getApplicationContext();
        mAudioManager = 
            (AudioManager) mFMRadioActivity.getSystemService(mContext.AUDIO_SERVICE);
        waitForPowerupWithTimeout(TIMEOUT);
        waitForInitedWithTimeout(TIMEOUT);
        
        mButtonDecrease = (ImageButton) mFMRadioActivity.findViewById(R.id.button_decrease);
        mButtonPrevStation = (ImageButton) mFMRadioActivity.findViewById(R.id.button_prevstation);
        mButtonNextStation = (ImageButton) mFMRadioActivity.findViewById(R.id.button_nextstation);
        mButtonIncrease = (ImageButton) mFMRadioActivity.findViewById(R.id.button_increase);

        mButtonAddToFavorite = (ImageButton) mFMRadioActivity
                .findViewById(R.id.button_add_to_favorite);

        mButtonRecord = (ImageButton) mFMRadioActivity.findViewById(R.id.btn_record);
        mButtonStop = (ImageButton) mFMRadioActivity.findViewById(R.id.btn_stop);
        mButtonPlayback = (ImageButton) mFMRadioActivity.findViewById(R.id.btn_playback);

        mTextViewFrequency = (TextView) mFMRadioActivity
                .findViewById(R.id.station_value);
    }

    // click play/stop button.
    public void testCase01_PowerUpAndPowerDown() {
        boolean isPlaying = false;
        // test FM power down
        makeFMPowerUp();
        mInstrumentation.waitForIdleSync();
        FMRadioTestCaseUtil.requestFocusGain(mAudioManager);
        mSolo.sleep(SHORT_TIME);
        checkFMStopState();
        
        // test FM power up
        mInstrumentation
                .invokeMenuActionSync(mFMRadioActivity, R.id.fm_power, 0);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        checkFMPlayingState();
    }

    // click speaker/earphone menu item.
    public void testCase02_SwitchSpeakerAndEarphone() {
        if (!mButtonDecrease.isEnabled()) {
            makeFMPowerUp();
        }
        String earphone = mFMRadioActivity.getString(R.string.optmenu_earphone);
        String speaker = mFMRadioActivity.getString(R.string.optmenu_speaker);
        // test FM switch to speaker
        switchSpeaker();
        mSolo.sleep(EXECUTE_TIME);
        mInstrumentation
        .invokeMenuActionSync(mFMRadioActivity, R.id.fm_menu, 0);
        assertTrue(mSolo.searchText(earphone));
        mSolo.goBack();
        // test FM switch to earphone
        switchEarphone();
        mSolo.sleep(EXECUTE_TIME);
        mInstrumentation
        .invokeMenuActionSync(mFMRadioActivity, R.id.fm_menu, 0);
        assertTrue(mSolo.searchText(speaker));
        mSolo.goBack();
    }

    // click favorite star.
    public void testCase03_AddDeleteFavoriteChannels() {
        int station = 0;
        // test add channel as favorite
        station = FMRadioTestCaseUtil.getStationFromUI(mTextViewFrequency);
        deleteChannelFromFavorite(station);
        clickView(mButtonAddToFavorite);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        assertTrue(FMRadioStation.isFavoriteStation(mFMRadioActivity, station));

        // test delete channel from favorite
        station = FMRadioTestCaseUtil.getStationFromUI(mTextViewFrequency);
        addChannelAsFavorite(station);
        clickView(mButtonAddToFavorite);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        assertFalse(FMRadioStation.isFavoriteStation(mFMRadioActivity, station));
    }

    // click tune frequency.
    public void testCase04_TuneFrequency() {
        int tuneStation = 0, currentStation = 0;
        if (!mButtonDecrease.isEnabled()) {
            makeFMPowerUp();
        }
        // test decrease 0.1 MHZ
        tuneStation = FMRadioTestCaseUtil.getStationFromUI(mTextViewFrequency);
        clickView(mButtonDecrease);
        mInstrumentation.waitForIdleSync();
        tuneStation -= TUNE_STEP;
        if (tuneStation < FMRadioUtils.LOWEST_STATION) {
            tuneStation = FMRadioUtils.HIGHEST_STATION;
        }
        // add sleep time, because instrumentation waitForIdleSync just wait for
        // UI thread end, must wait for all hardware and database operation end
        mSolo.sleep(SHORT_TIME);
        currentStation = FMRadioStation.getCurrentStation(mFMRadioActivity);
        assertEquals(tuneStation, currentStation);
        
        // add to favorite
        testCase03_AddDeleteFavoriteChannels();

        // test increase 0.1 MHZ
        tuneStation = FMRadioTestCaseUtil.getStationFromUI(mTextViewFrequency);
        clickView(mButtonIncrease);
        mInstrumentation.waitForIdleSync();
        tuneStation += TUNE_STEP;
        if (tuneStation > FMRadioUtils.HIGHEST_STATION) {
            tuneStation = FMRadioUtils.LOWEST_STATION;
        }
        mSolo.sleep(SHORT_TIME);
        currentStation = FMRadioStation.getCurrentStation(mFMRadioActivity);
        assertEquals(tuneStation, currentStation);
    }

    // click tune station
    public void testCase05_SwitchChannel() {
        int oldStation = 0, stationFromUI = 0, stationFromDB = 0;
        if (!mButtonPrevStation.isEnabled()) {
            makeFMPowerUp();
        }
        
        // test seek previous station
        oldStation = FMRadioTestCaseUtil.getStationFromUI(mTextViewFrequency);
        clickView(mButtonPrevStation);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(TIMEOUT);
        stationFromUI = FMRadioTestCaseUtil
                .getStationFromUI(mTextViewFrequency);
        // check station change
        assertTrue(oldStation != stationFromUI);

        // test seek next station
        oldStation = FMRadioTestCaseUtil.getStationFromUI(mTextViewFrequency);
        clickView(mButtonNextStation);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(TIMEOUT);
        stationFromUI = FMRadioTestCaseUtil
                .getStationFromUI(mTextViewFrequency);
        // check station change
        assertTrue(oldStation != stationFromUI);
    }

    // click cancel when search channel. click back key when search channel.
    public void testCase06_CancelChannelSearch() {
        // check cancel scan when power up
        startScanWhenPowerUp();
        checkCancelScan();
        checkFMPlayingState();
        
    }

    // search channel and tune to one station.
    public void testCase07_SearchChannels() {
        // check scan when power up
        startScanWhenPowerUp();
        checkFinishScan();
    }

    // enter channel list activity
    public void testCase08_EnterChannelList() {
        if (!mButtonDecrease.isEnabled()) {
            makeFMPowerUp();
        }
        mInstrumentation.invokeMenuActionSync(mFMRadioActivity,
                R.id.fm_channel_list, 0);
        mActivityMonitor = new ActivityMonitor(
                "com.mediatek.FMRadio.FMRadioFavorite", null, false);
        mInstrumentation.addMonitor(mActivityMonitor);
        mFMRadioFavorite = (FMRadioFavorite) mActivityMonitor
                .waitForActivityWithTimeout(TIMEOUT);
        assertNotNull(mFMRadioFavorite);
        Uri uri = Uri.parse(CONTENTURI);
        Cursor c = mFMRadioFavorite.getContentResolver().query(
                uri,
                new String[] { FMRadioStation.Station._ID,
                        FMRadioStation.Station.COLUMN_STATION_FREQ,
                        FMRadioStation.Station.COLUMN_STATION_TYPE }, null,
                null, null);
        assertTrue(c != null && c.getCount() > 0);
        try {
            if (c != null) {
                c.moveToFirst();
                while (!c.isAfterLast()) {
                    int stationFreq = c.getInt(1);
                    int stationType = c.getInt(2);
                    if ((FMRadioStation.STATION_TYPE_FAVORITE == stationType)
                            || (FMRadioStation.STATION_TYPE_SEARCHED == stationType)) {
                        assertTrue(FMRadioTestCaseUtil.isExistInChannelList(
                                mFMRadioFavorite, stationFreq));
                    }
                    c.moveToNext();
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (c != null) {
                c.close();
            }
        }
        mInstrumentation.sendKeyDownUpSync(KeyEvent.KEYCODE_BACK);
    }

    // when play fm, audio focus loss, and stop fm
    public void testCase09_CheckFocusLoss() {
        mInstrumentation.waitForIdleSync();
        makeFMPowerDown();
        
        // check when power up, music get focus, fm power down
        //mInstrumentation
        //        .invokeMenuActionSync(mFMRadioActivity, R.id.fm_power, 0);
        //mInstrumentation.waitForIdleSync();
        mSolo.sleep(EXECUTE_TIME);
        //FMRadioTestCaseUtil.requestFocusGain(mAudioManager);
        //mInstrumentation.waitForIdleSync();
        checkFMStopState();
        //mSolo.sleep(SHORT_TIME);

        mSolo.sleep(SHORT_TIME);
        // fm is power up, check when fm is searching, focus is loss, fm power down
        startScanWhenPowerUp();
        mSolo.sleep(SHORT_TIME);
        FMRadioTestCaseUtil.requestFocusGain(mAudioManager);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(TIMEOUT);
        // check music is active, fm is power down
        checkFMStopState();
    }

    public void testCase10_EnterRecordingMode() {
        enterRecordingMode();
        mSolo.sleep(SHORT_TIME);
        checkRecordNotSaveIdle();
        mInstrumentation.sendKeyDownUpSync(KeyEvent.KEYCODE_BACK);
        mInstrumentation.waitForIdleSync();
        checkFMPlayingState();
    }

    // recording, discard, save.
    public void testCase11_RecordingAndSave() {
        enterRecordingMode();
        mSolo.sleep(SHORT_TIME);
        // judge whether enter recording mode
        checkRecordNotSaveIdle();
        
        // test recording
        clickView(mButtonRecord);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        checkRecordingState();
        // make recording time have 3 seconds
        mSolo.sleep(SHORT_TIME);
        // test stop reocording and save recording file
        clickView(mButtonStop);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        checkRecordingStopState();
        
        // enter some illegal characters
        mEditTextRecordName = (EditText) mSolo.getView(R.id.fm_recording_text);
        String fileName = mEditTextRecordName.getText().toString();
        mSolo.clearEditText(mEditTextRecordName);
        mSolo.enterText(mEditTextRecordName, "test:");
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        mButtonRecordSave = (Button) mSolo.getView(R.id.fm_recording_btn_save);
        assertFalse(mButtonRecordSave.isEnabled());
        // enter some legal characters
        mSolo.clearEditText(mEditTextRecordName);
        mSolo.enterText(mEditTextRecordName, fileName);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        assertTrue(mButtonRecordSave.isEnabled());

        // click discard not save file
        mSolo.clickOnButton(mContext.getString(R.string.btn_discard_recording));
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        checkRecordNotSaveIdle();

        // recording and make focus at OK button of save recording dialog
        clickView(mButtonRecord);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        checkRecordingState();
        // make recording time have 3 seconds
        mSolo.sleep(SHORT_TIME);
        // test stop reocording and save recording file
        clickView(mButtonStop);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        checkRecordingStopState();

        // click OK button of save recording dialog
        mSolo.clickOnButton(mContext.getString(R.string.btn_save_recording));
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        checkRecordHasSavedIdle();
        mInstrumentation.sendKeyDownUpSync(KeyEvent.KEYCODE_BACK);
        mInstrumentation.waitForIdleSync();
    }
    
    // recording and playback.
    public void testCase12_RecordingAndPlayBack() {
        enterRecordingMode();
        mSolo.sleep(SHORT_TIME);
        // judge whether enter recording mode
        checkRecordNotSaveIdle();
        
        // test recording
        clickView(mButtonRecord);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        checkRecordingState();
        // make recording time have 3 seconds
        mSolo.sleep(SHORT_TIME);
        // test stop reocording and save recording file
        clickView(mButtonStop);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        checkRecordingStopState();
        
        // click OK button of save recording dialog
        mSolo.clickOnButton(mContext.getString(R.string.btn_save_recording));
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        checkRecordHasSavedIdle();
        
       // test playback recording file
        clickView(mButtonPlayback);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(EXECUTE_TIME);
        checkPlayingBackState();
        mSolo.sleep(TIMEOUT);
        checkPlayingBackFinished();

        // test stop playback recording file
        clickView(mButtonPlayback);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        clickView(mButtonStop);
        mInstrumentation.waitForIdleSync();
        checkPlayingBackFinished();
        mInstrumentation.sendKeyDownUpSync(KeyEvent.KEYCODE_BACK);
        mInstrumentation.waitForIdleSync();
    }
    
    // test fm recreate when scan, and fm is powerup state.
    public void testCase14_relaunchFmWhenCancelScan() {
        int oldStation = FMRadioTestCaseUtil.getStationFromUI(mTextViewFrequency);
        String cancel = mFMRadioActivity.getString(R.string.btn_cancel);
        // test cancel scan
        checkRelaunchWhenScan();
        mInstrumentation.waitForIdleSync();
        mSolo.clickOnButton(cancel);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        checkFMPlayingState();
        
        int stationFromUI = FMRadioTestCaseUtil
                .getStationFromUI(mTextViewFrequency);
        // check station not change
        assertTrue(oldStation == stationFromUI);
        
        checkRelaunchWhenScan();
        checkFinishScan();
        
    }
    
    // mount, unmount sd card when recording
    public void testCase15_mountUnmountWhenRecording() {
        FMRadioTestCaseUtil.mountSDCard(mContext);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(WAIT_MOUNT_UNMOUNT);
        mSolo.sleep(SHORT_TIME);
        enterRecordingMode();
        mSolo.sleep(SHORT_TIME);
        checkRecordNotSaveIdle();
        String toastStartRecord = mContext.getString(R.string.toast_start_recording);
        String toastSdCardMiss = mContext.getString(R.string.toast_sdcard_missing);
        String save = mFMRadioActivity.getString(R.string.btn_save_recording);
        String discard = mFMRadioActivity.getString(R.string.btn_discard_recording);
        // unmount sd card when recording
        
        clickView(mButtonRecord);
        assertTrue(mSolo.waitForText(toastStartRecord, 0, SHORT_TIME));
        mSolo.sleep(SHORT_TIME);
        FMRadioTestCaseUtil.unmountSDCard(mContext);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(WAIT_MOUNT_UNMOUNT);
        assertFalse(mSolo.searchButton(save, true));
        checkRecordNotSaveIdle();

        // click record after unmount
        clickView(mButtonRecord);
        assertTrue(mSolo.waitForText(toastSdCardMiss));
        checkRecordNotSaveIdle();
        
        // check mount sd card and stop recording, popup save dialog, then unmount sd card
        FMRadioTestCaseUtil.mountSDCard(mContext);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(WAIT_MOUNT_UNMOUNT);
        clickView(mButtonRecord);
        assertTrue(mSolo.waitForText(toastStartRecord, 0, SHORT_TIME));
        mSolo.sleep(SHORT_TIME);
        clickView(mButtonStop);
        mInstrumentation.waitForIdleSync();
        assertTrue(mSolo.searchButton(save, true));
        FMRadioTestCaseUtil.unmountSDCard(mContext);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(WAIT_MOUNT_UNMOUNT);
        assertFalse(mSolo.searchButton(save, true));
        
        // mount sdcard finally
        FMRadioTestCaseUtil.mountSDCard(mContext);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(WAIT_MOUNT_UNMOUNT);
        mInstrumentation.sendKeyDownUpSync(KeyEvent.KEYCODE_BACK);
        mInstrumentation.waitForIdleSync();
    }
    
    // mount, unmount sd card when recording
    public void testCase16_mountUnmountWhenPlayback() {
        // mount sdcard first
        FMRadioTestCaseUtil.mountSDCard(mContext);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(WAIT_MOUNT_UNMOUNT);
        
        // record some and playback
        String save = mContext.getString(R.string.btn_save_recording);
        String error = mContext.getString(R.string.toast_player_internal_error);
        mSolo.sleep(SHORT_TIME);
        enterRecordingMode();
        mSolo.sleep(SHORT_TIME);
        
        clickView(mButtonRecord);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(RECORDING_FILE_TIME);
        clickView(mButtonStop);
        mInstrumentation.waitForIdleSync();
        assertTrue(mSolo.searchButton(save, true));
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        mSolo.clickOnButton(save);
        mSolo.sleep(SHORT_TIME);
        clickView(mButtonPlayback);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        
        // unmount sd card
        FMRadioTestCaseUtil.unmountSDCard(mContext);
        mInstrumentation.waitForIdleSync();
        assertTrue(mSolo.waitForText(error, 0, WAIT_MOUNT_UNMOUNT));
        mSolo.sleep(WAIT_MOUNT_UNMOUNT);
        checkRecordNotSaveIdle();
        mSolo.sleep(SHORT_TIME);
        
        // mount sdcard finally
        FMRadioTestCaseUtil.mountSDCard(mContext);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(WAIT_MOUNT_UNMOUNT);
    }
    
    // check fm exit when power up state
    public void testCase17_FmExit() {
        boolean isPlaying = false;
        // test FM power down
        makeFMPowerUp();
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        
        // test FM power up
        mInstrumentation
                .invokeMenuActionSync(mFMRadioActivity, R.id.fm_power, 0);
        mSolo.sleep(SHORT_TIME);
    }
    
    // check fm back key when power down state
    public void testCase18_FmBackKeyExit() {
        boolean isPlaying = false;
        makeFMPowerUp();
        mInstrumentation.waitForIdleSync();
        FMRadioTestCaseUtil.requestFocusGain(mAudioManager);
        mSolo.sleep(SHORT_TIME);
        checkFMStopState();
        mSolo.goBack();
        mSolo.sleep(SHORT_TIME);
    }
    
    // check fm tune from channel list
    public void testCase19_relaunchFmTuneToStation() {
        boolean isPlaying = false;
        makeFMPowerUp();
        // add to favorite
        int station = 0;
        // test add channel as favorite
        station = FMRadioTestCaseUtil.getStationFromUI(mTextViewFrequency);
        deleteChannelFromFavorite(station);
        clickView(mButtonAddToFavorite);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        assertTrue(FMRadioStation.isFavoriteStation(mFMRadioActivity, station));
        
        // test FM power up
        mInstrumentation
                .invokeMenuActionSync(mFMRadioActivity, R.id.fm_channel_list, 0);
        mSolo.sleep(SHORT_TIME);
        mInstrumentation.waitForIdleSync();
        FMRadioTestCaseUtil.requestFocusGain(mAudioManager);
        mSolo.sleep(SHORT_TIME);
        
        relaunchFm();
        mFMRadioFavorite = (FMRadioFavorite) mSolo.getCurrentActivity();
        mSolo = new Solo(mInstrumentation, mFMRadioFavorite);
        mContext = mFMRadioFavorite.getApplicationContext();
        station = FMRadioTestCaseUtil.getStationFromUI(mTextViewFrequency);
        String pStation = String.valueOf((float) station / CONVERT_RATE);
        mSolo.clickOnText(pStation);
        mInstrumentation.waitForIdleSync();
        // delete favorite
        deleteChannelFromFavorite(station);
        mSolo.sleep(SHORT_TIME);
        checkFMPlayingState();
    }
    
    // recorder. save same name recorder file
    public void testCase20_saveRecorderWithSameName() {
        enterRecordingMode();
        File sdDir = Environment.getExternalStorageDirectory();
        File recordingFile = new File(sdDir.getPath() + "/FM Recording", "recorder_file");
        if (recordingFile.exists()) {
            recordingFile.delete();
        }
        mSolo.sleep(SHORT_TIME);
        
        // record some and playback
        String save = mContext.getString(R.string.btn_save_recording);
        String error = mContext.getString(R.string.toast_player_internal_error);
        
        // save recorder file
        clickView(mButtonRecord);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        clickView(mButtonStop);
        mInstrumentation.waitForIdleSync();
        assertTrue(mSolo.searchButton(save, true));
        mInstrumentation.waitForIdleSync();
        EditText editText = (EditText) mSolo.getView(R.id.fm_recording_text);
        mSolo.clearEditText(editText);
        mSolo.enterText(editText, "recorder_file");
        mInstrumentation.waitForIdleSync();
        InputMethodManager inputMethodManager = (InputMethodManager) mSolo.getCurrentActivity().getSystemService(Context.INPUT_METHOD_SERVICE);
        inputMethodManager.toggleSoftInput(0, 0);
        mSolo.clickOnButton(save);
        mSolo.sleep(SHORT_TIME);
        
        // save recorder file with same name
        clickView(mButtonRecord);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        clickView(mButtonStop);
        mInstrumentation.waitForIdleSync();
        assertTrue(mSolo.searchButton(save, true));
        mInstrumentation.waitForIdleSync();
        editText = (EditText) mSolo.getView(R.id.fm_recording_text);
        mSolo.clearEditText(editText);
        mSolo.enterText(editText, "recorder_file");
        mInstrumentation.waitForIdleSync();
        InputMethodManager inputMethodManager2 = (InputMethodManager) mSolo.getCurrentActivity().getSystemService(Context.INPUT_METHOD_SERVICE);
        inputMethodManager2.toggleSoftInput(0, 0);
        mSolo.clickOnButton(save);
        
        // check file has exist
        String hasFile = mContext.getString(R.string.already_exists);
        assertTrue(mSolo.waitForText(hasFile, 0, SHORT_TIME));
        
        // discard save record file
        String discard = mFMRadioActivity.getString(R.string.btn_discard_recording);
        mSolo.clickOnButton(discard);
        mInstrumentation.waitForIdleSync();
//        File sdDir = Environment.getExternalStorageDirectory();
//        File recordingFile = new File(sdDir.getPath() + "/FM Recording", "recorder_file");
        if (recordingFile.exists()) {
            recordingFile.delete();
        }
        
        checkRecordNotSaveIdle();
    }
    
    // check scan dialog, and fm is powerup sate when recreate
    private void checkRelaunchWhenScan() {
        startScanWhenPowerUp();
        //mSolo.sleep(ONE_SECOND);
        String cancel = mFMRadioActivity.getString(R.string.btn_cancel);
        boolean isVisible = mSolo.searchButton(cancel, true);
        assertTrue(isVisible);
        
        //relaunch after start scan
        relaunchFm();
        mInstrumentation.waitForIdleSync();
        initialRel();
        mSolo.sleep(ONE_SECOND);
        //check dialog is shown after relaunch
        isVisible = mSolo.searchButton(cancel, true);
        assertTrue(isVisible);
    }
    
    private void relaunchFm() {
        mSolo.getCurrentActivity().runOnUiThread(new Runnable() {

            @Override
            public void run() {
                mSolo.getCurrentActivity().recreate();

            }
        });
    }

    private void enterRecordingMode() {
        if (!mButtonDecrease.isEnabled()) {
            makeFMPowerUp();
        }
        mInstrumentation
                .invokeMenuActionSync(mFMRadioActivity, R.id.fm_menu, 0);
        mSolo.sleep(EXECUTE_TIME);
        String record = mFMRadioActivity.getString(R.string.optmenu_record);
        mSolo.clickOnText(record);
        mInstrumentation.waitForIdleSync();
    }

    // check ui and state when no recording file saved.
    public void checkRecordNotSaveIdle() {
        assertTrue(mButtonRecord.isEnabled());
        assertFalse(mButtonPlayback.isEnabled());
        assertFalse(mButtonStop.isEnabled());
        assertTrue(mButtonAddToFavorite.getVisibility() == View.GONE);
    }
    
    // check ui and state while recording.
    public void checkRecordingState() {
        assertFalse(mButtonRecord.isEnabled());
        assertFalse(mButtonPlayback.isEnabled());
        assertTrue(mButtonStop.isEnabled());
        assertTrue(mButtonAddToFavorite.getVisibility() == View.GONE);
    }
    
    // check ui and state when recording stop
    public void checkRecordingStopState() {
        mButtonRecordSave = (Button) mSolo.getView(R.id.fm_recording_btn_save);
        assertTrue(mButtonRecordSave.isEnabled());
        assertTrue(mButtonAddToFavorite.getVisibility() == View.GONE);
    }
    
    // check ui and state when save an recording file.
    public void checkRecordHasSavedIdle() {
        assertTrue(mButtonRecord.isEnabled());
        assertTrue(mButtonPlayback.isEnabled());
        assertFalse(mButtonStop.isEnabled());
        assertTrue(mButtonAddToFavorite.getVisibility() == View.GONE);
    }
    
    // check ui and state while playing back an recordfing file.
    public void checkPlayingBackState() {
        assertFalse(mButtonRecord.isEnabled());
        assertFalse(mButtonPlayback.isEnabled());
        assertTrue(mButtonStop.isEnabled());
        assertTrue(mButtonAddToFavorite.getVisibility() == View.GONE);
    }
    
    // check ui and state when play back an recording file finish.
    public void checkPlayingBackFinished() {
        assertTrue(mButtonRecord.isEnabled());
        assertTrue(mButtonPlayback.isEnabled());
        assertFalse(mButtonStop.isEnabled());
        assertTrue(mButtonAddToFavorite.getVisibility() == View.GONE);
    }
    
    // check state, if cancel scan
    public void checkCancelScan() {
        mSolo.sleep(SHORT_TIME);
        String cancel = mFMRadioActivity.getString(R.string.btn_cancel);
        mSolo.clickOnText(cancel);
        mInstrumentation.waitForIdleSync();
        int currentStation = (Integer) FMRadioTestCaseUtil
                .getVariableFromActivity(mFMRadioActivity, "mCurrentStation");
        assertEquals(currentStation,
                FMRadioTestCaseUtil.getStationFromUI(mTextViewFrequency));
        mSolo.sleep(SHORT_TIME);
    }
    
    // check state, if finish scan
    public void checkFinishScan() {
        mInstrumentation.waitForIdleSync();
        mActivityMonitor = mInstrumentation.addMonitor(
                FMRadioFavorite.class.getName(), null, false);
        mFMRadioFavorite = (FMRadioFavorite) mInstrumentation
                .waitForMonitorWithTimeout(mActivityMonitor, SEARCH_TIME);
        mSolo.sleep(SHORT_TIME);
        assertNotNull(mFMRadioFavorite);

        // test data in database.
        Uri uri = Uri.parse(CONTENTURI);
        Cursor c = mFMRadioFavorite.getContentResolver().query(
                uri,
                new String[] { FMRadioStation.Station._ID,
                        FMRadioStation.Station.COLUMN_STATION_FREQ,
                        FMRadioStation.Station.COLUMN_STATION_TYPE }, null,
                null, null);
        assertTrue(c != null && c.getCount() > 0);
        int stationFreq = 0;
        int stationType = 0;
        if (c != null) {
            c.moveToFirst();
            while (!c.isAfterLast()) {
                stationFreq = c.getInt(1);
                stationType = c.getInt(2);
                Log.d(TAG, "station =" + stationFreq);
                if (FMRadioStation.STATION_TYPE_SEARCHED == stationType) {
                    assertTrue(FMRadioTestCaseUtil.isExistInChannelList(
                            mFMRadioFavorite, stationFreq));
                }
                c.moveToNext();
            }
        }
        if (c != null) {
            c.close();
        }

        // click the last station in listview to play.
        String pStation = String.valueOf((float) stationFreq / CONVERT_RATE);
        mSolo.clickOnText(pStation);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
        assertEquals(stationFreq,
                FMRadioTestCaseUtil.getStationFromUI(mTextViewFrequency));
        
        checkFMPlayingState();
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

    private void deleteChannelFromFavorite(int station) {
        if (FMRadioStation.isFavoriteStation(mFMRadioActivity, station)) {
            clickView(mButtonAddToFavorite);
            mInstrumentation.waitForIdleSync();
            mSolo.sleep(SHORT_TIME);
        }
    }

    private void addChannelAsFavorite(int station) {
        if (!FMRadioStation.isFavoriteStation(mFMRadioActivity, station)) {
            clickView(mButtonAddToFavorite);
            mInstrumentation.waitForIdleSync();
            mSolo.sleep(SHORT_TIME);
        }

    }

    private void makeFMPowerUp() {
        boolean isPlaying = false;
        isPlaying = (Boolean) FMRadioTestCaseUtil.getVariableFromActivity(
                mFMRadioActivity, "mIsPlaying");
        if (!isPlaying) {
            mInstrumentation
            .invokeMenuActionSync(mFMRadioActivity, R.id.fm_power, 0);
        }
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(TIMEOUT);
    }

    private void makeFMPowerDown() {
        boolean isPlaying = true;
        isPlaying = (Boolean) FMRadioTestCaseUtil.getVariableFromActivity(
                mFMRadioActivity, "mIsPlaying");
        if (isPlaying) {
            FMRadioTestCaseUtil.requestFocusGain(mAudioManager);
        }
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(SHORT_TIME);
    }

    private void clickView(final View view) {
        try {
            runTestOnUiThread(new Runnable() {
                @Override
                public void run() {
                    view.performClick();
                }
            });
        } catch (Throwable t) {
            t.printStackTrace();
        }
    }

    private void waitForInitedWithTimeout(long timeOut) {
        long startTime = System.currentTimeMillis();
        boolean isInited = (Boolean) FMRadioTestCaseUtil
                .getVariableFromMethod(mFMRadioActivity, "isServiceInit");
        while (!isInited) {
            if (System.currentTimeMillis() - startTime > timeOut) {
                break;
            }
            mSolo.sleep(CHECK_TIME);
        }
    }

    private void waitForPowerupWithTimeout(long timeOut) {

        long startTime = System.currentTimeMillis();
        boolean isPlaying = (Boolean) FMRadioTestCaseUtil
                .getVariableFromActivity(mFMRadioActivity, "mIsPlaying");
        while (!isPlaying) {
            if (System.currentTimeMillis() - startTime > timeOut) {
                break;
            }
            mSolo.sleep(CHECK_TIME);
        }
    }
    
    // start scan when power up state
    private void startScanWhenPowerUp() {
        if (!mButtonDecrease.isEnabled()) {
            makeFMPowerUp();
        }
        mInstrumentation
                .invokeMenuActionSync(mFMRadioActivity, R.id.fm_menu, 0);
        mInstrumentation.waitForIdleSync();
        mSolo.sleep(ONE_SECOND);
        String search = mFMRadioActivity.getString(R.string.optmenu_search);
        mSolo.clickOnText(search);
    }

    // check play FM layout button enable
    public void checkFMPlayingState() {
        assertTrue(mButtonDecrease.isEnabled());
        assertTrue(mButtonIncrease.isEnabled());
        assertTrue(mButtonNextStation.isEnabled());
        assertTrue(mButtonPrevStation.isEnabled());

    }

    // check play FM layout button enable
    public void checkFMStopState() {
        assertFalse(mButtonDecrease.isEnabled());
        assertFalse(mButtonPrevStation.isEnabled());
        assertFalse(mButtonNextStation.isEnabled());
        assertFalse(mButtonIncrease.isEnabled());
    }
    
    public void initialRel() {
        mFMRadioActivity = (FMRadioActivity) mSolo.getCurrentActivity();
        mSolo = new Solo(mInstrumentation, mFMRadioActivity);
        mContext = mFMRadioActivity.getApplicationContext();
        assertNotNull(mFMRadioActivity);
        mButtonPrevStation = (ImageButton) mFMRadioActivity.findViewById(R.id.button_prevstation);
        mButtonNextStation = (ImageButton) mFMRadioActivity.findViewById(R.id.button_nextstation);
        mButtonDecrease = (ImageButton) mFMRadioActivity.findViewById(R.id.button_decrease);
        mButtonIncrease = (ImageButton) mFMRadioActivity.findViewById(R.id.button_increase);

        mButtonAddToFavorite = (ImageButton) mFMRadioActivity
                .findViewById(R.id.button_add_to_favorite);

        mTextViewFrequency = (TextView) mFMRadioActivity
        .findViewById(R.id.station_value);
    }

    @Override
    protected void tearDown() {
        mSolo.finishOpenedActivities();
        mInstrumentation.waitForIdleSync();
        FMRadioTestCaseUtil.sleep(WAIT_TEAR_DOWN);
        try {
            mSolo.finalize();
        } catch (Throwable t) {
            t.printStackTrace();
        }
        try {
            super.tearDown();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

}
