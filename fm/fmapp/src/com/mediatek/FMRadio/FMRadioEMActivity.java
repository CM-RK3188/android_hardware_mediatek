/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
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

package com.mediatek.FMRadio;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.media.AudioManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

import com.mediatek.FMRadio.FMRadioFeatureOption;

import java.util.Arrays;

/**
 * This class is used to provide engineer mode function for hardware test
 *
 */
public class FMRadioEMActivity extends Activity {
    public static final String TAG = "FmRx/EM";
    
    private static final String TYPE_MSGID = "MSGID";
    private static final int MSGID_OK = 1;
    private static final int MSGID_UPDATE_RDS = 2;
    private static final int MSGID_UPDATE_CURRENT_STATION = 3;

    private static final int MSGID_TICK_EVENT = 6;
    private static final int MSGID_INIT_OK = 7;
    private static final int MSGID_POWERUP = 8;
    private static final int MSGID_TUNE_FNISHED = 9;
    
    private boolean mIsServiceStarted = false;
    private boolean mIsServiceBinded = false;
    private FMRadioService mService = null;
    
    private boolean mIsPlaying = false; // When start, the radio is not playing.
    
    // Record whether we are destroying.
    private boolean mIsDestroying = false;
    // Strings shown in RDS text view.
    private String mPSString = "";
    private String mLRTextString = "";
    
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
    private EditText mEditFreq = null;    
    private Button mButtonTune = null;
    private RadioButton mRdAntennaS = null;
    private RadioButton mRdAntennaL = null;
    private RadioGroup mRgAntenna = null;
    private RadioButton mRdStereo = null;
    private RadioButton mRdMono = null;
    private Context mContext = null;

    // Can not use float to record the station. Because there will be inaccuracy when increase/decrease 0.1
    private int mCurrentStation = FMRadioUtils.DEFAULT_STATION;
    private AudioManager mAudioManager = null;
    private HeadsetConnectionReceiver mHeadsetConnectionReceiver = null;

    /**
     * use to show update antenna UI
     *
     */
    private class HeadsetConnectionReceiver extends BroadcastReceiver {
         public void onReceive(Context context, Intent intent) {    
            if (intent.getIntExtra("state", 0) == 0) { // unpluged
                if (mIsPlaying) {
                    mRgAntenna.check(R.id.FMR_Antenna_short);
                }
            } else if (intent.getIntExtra("state", 0) == 1) { // pluged
                if (mIsPlaying) {
                    mRgAntenna.check(R.id.FMR_Antenna_long);
                }
            }
                 
        }  
    };     
    
    /**
     * click listener
     */
    private View.OnClickListener mButtonClickListener = new View.OnClickListener() {
        /**
         * handle click event 
         * @param v clicked view
         */
        public void onClick(View v) {
            switch (v.getId()) {
            case R.id.FMR_Antenna_short:
                switchAntenna(1);
                break;
            case R.id.FMR_Antenna_long:
                switchAntenna(0);
                break;
            case R.id.FMR_Stereomono_stereo:
//                if (!setStereoMono(false)) {
//                    Toast.makeText(mContext, "Set Stereo Mono failed.", Toast.LENGTH_SHORT).show();
//                }
                setStereoMono(false);
                break;
            case R.id.FMR_Stereomono_mono:
//                if (!setStereoMono(true)) {
//                    Toast.makeText(mContext, "Set Stereo Mono failed.", Toast.LENGTH_SHORT).show();
//                }
                setStereoMono(true);
                break;
            case R.id.FMR_Freq_tune:
                String s = mEditFreq.getText().toString();
                float freq = 0;
                try {
                    freq = Float.valueOf(s);
                } catch (NumberFormatException e) {
                    Toast.makeText(FMRadioEMActivity.this, "bad float format.", Toast.LENGTH_SHORT).show();
                    mEditFreq.setText(FMRadioUtils.formatStation(mCurrentStation));
                    return;
                }
                
                // disable button before it finished tune.
                mButtonTune.setEnabled(false);
                tuneToStation(FMRadioUtils.computeStation(freq));
                break;
            default :
                LogUtils.d(TAG, "invalid view id");
            }
        }
    };
    
    /**
     * FM Radio listener
     */
    private FMRadioListener mFMRadioListener = new FMRadioListener() {
        
        /**
         * call back method from service
         */
        public void onCallBack(Bundle bundle) {
            Message msg = mHandler.obtainMessage(bundle.getInt(FMRadioListener.CALLBACK_FLAG));
            msg.setData(bundle);
            mHandler.sendMessage(msg);
        }
    };
    
    /**
     *  Called when the activity is first created. 
     */
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        LogUtils.d(TAG, "begin FMRadioEMActivity.onCreate");
        // Bind the activity to FM audio stream.
        //setVolumeControlStream(AudioManager.STREAM_FM);
        setContentView(R.layout.fm_rx_em);
        // Init FM database
        mContext = getApplicationContext();
        FMRadioStation.initFMDatabase(mContext);
        
        mTextStereoMono = (TextView) findViewById(R.id.FMR_Status_Stereomono);
        mTextRssi = (TextView) findViewById(R.id.FMR_Status_RSSI);
        mTextCapArray = (TextView) findViewById(R.id.FMR_Status_Caparray);
        mTextRdsBler = (TextView) findViewById(R.id.FMR_RDS_Ratio);
        mTextRdsPS = (TextView) findViewById(R.id.FMR_RDS_PS);
        mTextRdsRT = (TextView) findViewById(R.id.FMR_RDS_RT);
        mTextChipID = (TextView) findViewById(R.id.FMR_Chip_ID);
        mTextECOVersion = (TextView) findViewById(R.id.FMR_ECO_Version);
        mTextPatchVersion = (TextView) findViewById(R.id.FMR_Patch_Version);
        mTextDSPVersion = (TextView) findViewById(R.id.FMR_DSP_Version);
        mEditFreq = (EditText) findViewById(R.id.FMR_Freq_edit);    
        mButtonTune = (Button) findViewById(R.id.FMR_Freq_tune);
        mRdAntennaS = (RadioButton) findViewById(R.id.FMR_Antenna_short);
        mRdAntennaL = (RadioButton) findViewById(R.id.FMR_Antenna_long);
        mRgAntenna = (RadioGroup) findViewById(R.id.FMR_Antenna_type);        
        mRdStereo = (RadioButton) findViewById(R.id.FMR_Stereomono_stereo);
        mRdMono = (RadioButton) findViewById(R.id.FMR_Stereomono_mono);
        
        mRdAntennaS.setOnClickListener(mButtonClickListener);
        mRdAntennaL.setOnClickListener(mButtonClickListener);
        mRdStereo.setOnClickListener(mButtonClickListener);
        mRdMono.setOnClickListener(mButtonClickListener);
        mButtonTune.setOnClickListener(mButtonClickListener);
        
        // Should start FM service first.
        ComponentName cn = startService(new Intent(FMRadioEMActivity.this, FMRadioService.class));
        if (null == cn) {
            LogUtils.e(TAG, "Error: Cannot start FM service");
        } else {
            LogUtils.d(TAG, "Start FM service successfully.");
            mIsServiceStarted = true;
            mIsServiceBinded = bindService(
                new Intent(FMRadioEMActivity.this, FMRadioService.class),
                mServiceConnection,
                Context.BIND_AUTO_CREATE);
        }
        if (!mIsServiceBinded) {
            LogUtils.e(TAG, "Error: Cannot bind FM service");
            finish();
            return;
        } else {
            LogUtils.d(TAG, "Bind FM service successfully.");
        }
        
        
        IntentFilter filterHeadset = new IntentFilter();
        filterHeadset.addAction(Intent.ACTION_HEADSET_PLUG);
        mHeadsetConnectionReceiver = new HeadsetConnectionReceiver();
        LogUtils.d(TAG, "Register HeadsetConnectionReceiver");
        registerReceiver(mHeadsetConnectionReceiver, filterHeadset);

        // Get all the views and set their actions.
        mCurrentStation = FMRadioStation.getCurrentStation(mContext);
        mAudioManager = (AudioManager)getSystemService(Context.AUDIO_SERVICE);
        refreshTextStatus(mIsPlaying);
        LogUtils.d(TAG, "end FMRadioEMActivity.onCreate");
    }
    
    /**
     * called when bind service
     */
    private ServiceConnection mServiceConnection = new ServiceConnection() {
        
        /**
         * called when bind service
         * @param className service name
         * @param service service
         */
        public void onServiceConnected(ComponentName className, IBinder service) {
            LogUtils.d(TAG, "begin FMRadioEMActivity.onServiceConnected");
            mService = ((FMRadioService.ServiceBinder)service).getService();
            if (null == mService) {
                LogUtils.e(TAG, "Error: null interface");
                finish();
            } else {
                mService.registerFMRadioListener(mFMRadioListener);
                if (!isServiceInit()) {
                    LogUtils.d(TAG, "FM service is not init.");
                    initService(mCurrentStation);
                    refreshTextStatus(false);
//                    InitialThread thread = new InitialThread();
//                    thread.start();
//                    mService.openDeviceAsync();
                    LogUtils.d(TAG, "onService connect.mCurrentStation: " + mCurrentStation);
                    mService.powerUpAsync(FMRadioUtils.computeFrequency(mCurrentStation));
                    
                } else {
                    LogUtils.d(TAG, "FM service is already init.");
                    if (isDeviceOpen()) {
                        // Get the current frequency in service and save it into database.
                        int iFreq = getFrequency();
                        if (FMRadioUtils.isValidStation(iFreq)) {
                            if (mCurrentStation != iFreq) {
                                LogUtils.d(TAG, "The frequency in FM service is not same as in database.");
                                mCurrentStation = iFreq;
                                // Save the current station frequency into data base.
                                FMRadioStation.setCurrentStation(mContext, mCurrentStation);
                            } else {
                                LogUtils.d(TAG, "The frequency in FM service is same as in database.");
                            }
                        } else {
                            LogUtils.e(TAG, "Error: invalid frequency in service.");
                        }
                        
                        mIsPlaying = isPowerUp();


                        if (mIsPlaying) {
                            LogUtils.d(TAG, "FM is already power up.");
                            refreshTextStatus(true);
                        }

                        if (mIsPlaying) {
                            // Update the RDS text view.
                            mPSString = getPS();
                            mLRTextString = getLRText();
                            mHandler.sendEmptyMessage(MSGID_UPDATE_RDS);
                        }

                    } else {
                        // This is theoretically never happen.
                        LogUtils.e(TAG, "Error: FM device is not open");
                    }
                    mHandler.sendEmptyMessage(MSGID_INIT_OK);
                }
            }
            LogUtils.d(TAG, "<<< FMRadioEMActivity.onServiceConnected");
        }

        public void onServiceDisconnected(ComponentName className) {
            LogUtils.d(TAG, ">>> FMRadioEMActivity.onServiceDisconnected");
            mService = null;
            LogUtils.d(TAG, "<<< FMRadioEMActivity.onServiceDisconnected");
        }
    };
    private Handler mHandler = new Handler() {
        public void handleMessage(Message msg) {
            LogUtils.d(TAG, ">>> handleMessage what: " + msg.what);
            if (mIsDestroying) {
                LogUtils.d(TAG, "Warning: app is being destroyed.");
                LogUtils.d(TAG, "<<< handleMessage");
                return;
            }
            Bundle bundle;
            switch (msg.what) {
            case MSGID_INIT_OK:
                mTextChipID.setText(getChipId());
                mTextECOVersion.setText(getEcoVersion());
                mTextPatchVersion.setText(getPatchVersion());
                mTextDSPVersion.setText(getDspVersion());
                break;
            case MSGID_TICK_EVENT:
                if (isDeviceOpen()) {
                    mTextRdsBler.setText(String.format("%d%%", readRdsBler()));
                    mTextRssi.setText(String.format("%d", readRssi()));
                    boolean stereo = getStereoMono();
                    mTextStereoMono.setText(stereo ? "Stereo" : "Mono");
                    if (mRdAntennaL.isChecked()) {
                        mTextCapArray.setText("N/A");
                    } else {
                        mTextCapArray.setText(formatCapArray(readCapArray()));
                    }
                }
                final int interval = 300;
                sendEmptyMessageDelayed(MSGID_TICK_EVENT, interval);
                break;
                
            case FMRadioListener.MSGID_POWERUP_FINISHED:
                if (isSpeakerUsed()) {
//                    switchAntenna(0);
                    mRgAntenna.check(R.id.FMR_Antenna_short);
                } else {
//                    switchAntenna(1);
                    mRgAntenna.check(R.id.FMR_Antenna_long);
                }
                bundle = msg.getData();
                mIsPlaying = bundle.getBoolean(FMRadioListener.KEY_IS_POWER_UP);
                mHandler.sendEmptyMessage(MSGID_INIT_OK);
                refreshTextStatus(true);
                break;
                
            case FMRadioListener.MSGID_SWITCH_ANNTENNA:
                bundle = msg.getData();
                boolean isSwitch = bundle.getBoolean(FMRadioListener.KEY_IS_SWITCH_ANNTENNA);
                if (isSwitch) {
//                    mService.powerUpAsync(FMRadioUtils.computeFrequency(mCurrentStation));
                    LogUtils.d(TAG, "mHandler. switch anntenna ok");
                }
                break;
            case FMRadioListener.MSGID_TUNE_FINISHED:
                bundle = msg.getData();
                float frequency = bundle.getFloat(FMRadioListener.KEY_TUNE_TO_STATION);
                mCurrentStation = FMRadioUtils.computeStation(frequency);
                // Save the current station frequency into data base.
//                FMRadioStation.setCurrentStation(mContext, mCurrentStation);
                refreshTextStatus(true);
                break;
                
            case FMRadioListener.LISTEN_PS_CHANGED:
            case FMRadioListener.LISTEN_RT_CHANGED:
                bundle = msg.getData();
                mPSString = bundle.getString(FMRadioListener.KEY_PS_INFO);
                mLRTextString = bundle.getString(FMRadioListener.KEY_RT_INFO);
                showRDS();
                break;
            case FMRadioListener.LISTEN_RDSSTATION_CHANGED:
                bundle = msg.getData();
                mCurrentStation = bundle.getInt(FMRadioListener.KEY_RDS_STATION);
                break;
            default:
                LogUtils.d(TAG, "invalid view id");
            }
            LogUtils.d(TAG, "<<< handleMessage");
        }
        
    };
    
    private String formatCapArray(int raw) {
        final float[] pF = { 0.166f, 0.332f, 0.664f, 1.33f, 2.66f, 5.31f, 10.6f, 18.6f };

        float sum = 0.0f;
        for (int i = 0; i < pF.length; i++) {
            final int base = 6;
            sum += (((raw >> (base + i)) & 0x1) == 1 ? pF[i] : 0.0);
        }
        return String.format("%.2f", sum);
    }
    
    /**
     * called when activity resume
     */
    public void onResume() {
        super.onResume();
        final int oneSecond = 1000;
        mHandler.sendEmptyMessageDelayed(MSGID_TICK_EVENT, oneSecond);
    }
    
    /**
     * called when activity pause
     */
    public void onPause() {
        mHandler.removeMessages(MSGID_TICK_EVENT);
        super.onPause();
    }
    
    /**
     * called when activity destroy
     */
    public void onDestroy() {
        LogUtils.d(TAG, ">>> FMRadioEMActivity.onDestroy");
        mIsDestroying = true;
        
        if (null != mHeadsetConnectionReceiver) {
            LogUtils.d(TAG, "Unregister headset broadcast receiver.");
            unregisterReceiver(mHeadsetConnectionReceiver);
            mHeadsetConnectionReceiver = null;
        }
        // Should powerdown FM.
        if (mIsPlaying) {
            LogUtils.d(TAG, "FM is Playing. So stop it.");
//            powerDown();
            mService.powerDownAsync();
            mIsPlaying = false;
        }

        // Unbind the FM service.
        if (mIsServiceBinded) {
            unbindService(mServiceConnection);
            mIsServiceBinded = false;
        }
        if (null != mService) {
            mService.unregisterFMRadioListener(mFMRadioListener);
        }
        mService = null;
        mFMRadioListener = null;
        
        if (mIsServiceStarted) {
            boolean bRes = stopService(new Intent(FMRadioEMActivity.this, FMRadioService.class));
            if (!bRes) {
                LogUtils.d(TAG, "Error: Cannot stop the FM service.");
            }
            mIsServiceStarted = false;
        }
        
        LogUtils.d(TAG, "<<< FMRadioEMActivity.onDestroy");
        super.onDestroy();
    }
    
    private boolean isAntennaAvailable() {
        return FMRadioFeatureOption.MTK_MT519X_FM_SUPPORT ? true : mAudioManager.isWiredHeadsetOn();
    }    

    private void tuneToStation(final int iStation) {
        mService.tuneStationAsync(FMRadioUtils.computeFrequency(iStation));
    }
    
    
    private void refreshTextStatus(boolean on) {
        if (!on) {
            mTextStereoMono.setText("X");
            mTextRssi.setText("X");
            mTextCapArray.setText("X");
            mTextRdsBler.setText("X");
            mTextRdsPS.setText("X");
            mTextRdsRT.setText("X");
            mEditFreq.setText(FMRadioUtils.formatStation(mCurrentStation));
            mRgAntenna.clearCheck();
            mButtonTune.setEnabled(false);
        } else {
            mButtonTune.setEnabled(true);
        }
    }
    
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
    }
    
    /*private int rdsset(boolean rdson) {
        return mIsRDSSupported ? setRDS(rdson) : -1;
    }*/

    private void showRDS() {
        LogUtils.v(TAG, "FMRadioEMActivity.showRDS PS : " + mPSString + "RT :" + mLRTextString);        
        mTextRdsPS.setText(mPSString);
        mTextRdsRT.setText(mLRTextString);
    }
    
    /**
     * get chip id
     * @return chip id 
     */
    public String getChipId() {
        int[] hardwareVersion = getHardwareVersion();
        int chipId = 0;
        if (null != hardwareVersion) {
            chipId = hardwareVersion[0];
        }
        return getStringValue(chipId);
    }
    
    /**
     * get ECO version
     * @return ECO version
     */
    public String getEcoVersion() {
        int[] hardwareVersion = getHardwareVersion();
        int ecoVersion = 0;
        if (null != hardwareVersion) {
            ecoVersion = hardwareVersion[1];
        }
        return "E" + getStringValue(ecoVersion);
    }
    
    /**
     * get patch version
     * @return patch version
     */
    public String getPatchVersion() {
        int[] hardwareVersion = getHardwareVersion();
        int patchVersion = 0;
        if (null != hardwareVersion) {
            final int patchVersionPosition = 3;
            patchVersion = hardwareVersion[patchVersionPosition];
        }
        String patchStr = getStringValue(patchVersion);
        float patch = 0;
        if (null != patchStr) {
            try {
                final int hundred = 100;
                patch = Float.parseFloat(patchStr) / hundred;
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }
        return Float.toString(patch);
    }
    
    /**
     * get DSP version
     * @return DSP version
     */
    public String getDspVersion() {
        int[] hardwareVersion = getHardwareVersion();
        int dspVersion = 0;
        if (null != hardwareVersion) {
            dspVersion = hardwareVersion[2];
        }
        return "V" + getStringValue(dspVersion);
    }

    private boolean isDeviceOpen() {
        if (null != mService) {
            return mService.isDeviceOpen();
        }
        return false;
    }

    private boolean isPowerUp() {
        boolean bRet = false;
        if (null != mService) {
            return mService.isPowerUp();
        }
        return false;
    }

    private String getPS() {
        if (null != mService) {
            return mService.getPS();
        }
        return null;
    }

    private String getLRText() {
        if (null != mService) {
            return mService.getLRText();
        }
        return null;
    }

    private boolean isRDSSupported() {
        if (null != mService) {
            return mService.isRDSSupported();
        }
        return false;
    }

    private boolean isSpeakerUsed() {
        if (null != mService) {
            return mService.isSpeakerUsed();
        }
        return true;
    }

    private void initService(int iCurrentStation) {
        if (null != mService) {
            mService.initService(iCurrentStation);
        }
    }

    private boolean isServiceInit() {
        if (null != mService) {
            return mService.isServiceInit();
        }
        return false;
    }

    private int getFrequency() {
        if (null != mService) {
            return mService.getFrequency();
        }
        return 0;
    }

    /**
     * read cap array
     * @return cap array
     */
    public int readCapArray() {
        if (null != mService) {
            return mService.getCapArray();
        }
        return 0;
    }

    /**
     * get rssi value
     * @return rssi value
     */
    public int readRssi() {
        if (null != mService) {
            return mService.getRssi();
        }
        return 0;
    }

    /**
     * get stereo or mono
     * @return true is stereo, false mono
     */
    public boolean getStereoMono() {
        if (null != mService) {
            return mService.getStereoMono();
        }
        return false;
    }

    /**
     * set stereo or mono
     * @param isMono mono or not
     * @return whether success
     */
    public void setStereoMono(boolean isMono) {
//        if (null != mService) {
//            return mService.setStereoMono(isMono);
//        }
//        return false;
        mService.setStereoMono(isMono);
    }

    /**
     * switch antenna
     * 
     * @param antenna
     *            antenna (0, long antenna, 1 short antenna)
     * @return (0, success; 1 failed; 2 not support)
     */
    public void switchAntenna(int type) {
        /*if (null != mService) {
            return mService.switchAntenna(type);
        }
        return 2;*/
        mService.switchAntennaAsync(type);
    }

    /**
     * read rds bler
     * 
     * @return rds bler value
     */
    public int readRdsBler() {
        if (null != mService) {
            return mService.getRdsBler();
        }
        return 0;
    }

    /**
     * get hardware version
     * @return hardware version information array(0, ChipId; 1, EcoVersion; 2, PatchVersion; 3,
     *         DSPVersion)
     */
    public int[] getHardwareVersion() {
        int[] hardwareVersion = null;
        if (null != mService) {
            hardwareVersion = mService.getHardwareVersion();
        }
        LogUtils.v(TAG, "getHardwareversion: " + Arrays.toString(hardwareVersion));
        return hardwareVersion;
    }

    /**
     * get actual version string
     * @param convertData version data
     * @return actual version string
     */
    public String getStringValue(int convertData) {
        StringBuilder temp = new StringBuilder();
        int quotient = convertData;
        int remainder = 0;
        while (quotient > 0) {
            final int hexadecimal = 16;
            remainder = quotient % hexadecimal;
            quotient = quotient / hexadecimal;
            temp = temp.append(remainder);
        }
        return temp.reverse().toString();
    }
}
