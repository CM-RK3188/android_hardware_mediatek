/*
package mediatek.android.application.Advanced_Tool;

import mediatek.android.application.R;
import mediatek.android.application.FileParse;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.text.TextUtils;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Toast;
import android.util.Log;

import java.util.Arrays;

public class Advanced_WiFi_Tx_for_Porting_OK extends Activity implements OnClickListener {
    private static final String TAG = "Advanced_WiFi_Tx";
    private static final long MAX_VALUE = 0xFFFFFFFF;
    private static final int MIN_VALUE = 0x0;
    private static final int HANDLER_EVENT_GO = 1;
    private static final int HANDLER_EVENT_STOP = 2;
    private static final int HANDLER_EVENT_TIMER = 3;
    private static final int HANDLER_EVENT_FINISH = 4;
    private static final int DIALOG_WIFI_FAIL = 2;
    private static final int DIALOG_WIFI_ERROR = 3;
    private static final int MAX_LOWER_RATE_NUMBER = 12;
    private static final int MAX_HIGH_RATE_NUMBER = 21;
    private static final int CCK_RATE_NUMBER = 4;
    private static final int DEFAULT_PKT_CNT = 3000;
    private static final int DEFAULT_PKT_LEN = 1024;
    private static final int DEFAULT_TX_GAIN = 0;
    private static final int ONE_SENCOND = 1000;
    private static final int BIT_8_MASK = 0xFF;
    private static final int LENGTH_3 = 3;
    private static final int BANDWIDTH_40MHZ_MASK = 0x8000;
    private static final int BANDWIDTH_INDEX_40 = 1;
    private static final int CHANNEL_0 = 0;
    private static final int CHANNEL_1 = 1;
    private static final int CHANNEL_11 = 11;
    private static final int CHANNEL_12 = 12;
    private static final int CHANNEL_13 = 13;
    private static final int CHANNEL_14 = 14;
    private static final int ATPARAM_INDEX_COMMAND = 1;
    private static final int ATPARAM_INDEX_POWER = 2;
    private static final int ATPARAM_INDEX_RATE = 3;
    private static final int ATPARAM_INDEX_PREAMBLE = 4;
    private static final int ATPARAM_INDEX_ANTENNA = 5;
    private static final int ATPARAM_INDEX_PACKLENGTH = 6;
    private static final int ATPARAM_INDEX_PACKCOUNT = 7;
    private static final int ATPARAM_INDEX_PACKINTERVAL = 8;
    private static final int ATPARAM_INDEX_TEMP_COMPENSATION = 9;
    private static final int ATPARAM_INDEX_TXOP_LIMIT = 10;
    private static final int ATPARAM_INDEX_PACKCONTENT = 12;
    private static final int ATPARAM_INDEX_RETRY_LIMIT = 13;
    private static final int ATPARAM_INDEX_QOS_QUEUE = 14;
    private static final int ATPARAM_INDEX_BANDWIDTH = 15;
    private static final int ATPARAM_INDEX_GI = 16;
    private static final int ATPARAM_INDEX_CWMODE = 65;
    private static final int ATPARAM_INDEX_TRANSMITCOUNT = 32;
    private static final int COMMAND_INDEX_OUTPUTPOWER = 4;
    private static final int COMMAND_INDEX_STOPTEST = 0;
    private static final int COMMAND_INDEX_STARTTX = 1;
    private static final int COMMAND_INDEX_LOCALFREQ = 5;
    private static final int COMMAND_INDEX_CARRIER = 6;
    private static final int COMMAND_INDEX_CARRIER_NEW = 10;
    private static final int RATE_MODE_MASK = 31;
    private static final int RATE_MCS_INDEX = 0x20;
    private static final int RATE_NOT_MCS_INDEX = 0x07;
    private static final int CWMODE_CCKPI = 5;
    private static final int CWMODE_OFDMLTF = 2;
    private static final int TXOP_LIMIT_VALUE = 0x00020000;
    private static final int TEST_MODE_TX = 0;
    private static final int TEST_MODE_DUTY = 1;
    private static final int TEST_MODE_CARRIER = 2;
    private static final int TEST_MODE_LEAKAGE = 3;
    private static final int TEST_MODE_POWEROFF = 4;
    public static final long[] PACKCONTENT_BUFFER = { 0xff220004, 0x33440006,
            0x55660008, 0x55550019, 0xaaaa001b, 0xbbbb001d };
    private boolean mHighRateSelected = false;
    private boolean mCCKRateSelected = true;
    private long mChipID = 0x0;
    private int mLastRateGroup = 0;
    private int mLastBandwidth = 0;
    private Spinner mChannelSpinner = null;
    private Spinner mGuardIntervalSpinner = null;
    private Spinner mBandwidthSpinner = null;
    private Spinner mPreambleSpinner = null;
    private EditText mEtPkt = null;
    private EditText mEtPktCnt = null;
    private EditText mEtTxGain = null;
    private Spinner mRateSpinner = null;
    private Spinner mModeSpinner = null;
    // private EditText mXTEdit = null;
    // private Button mWriteBtn = null;
    // private Button mReadBtn = null;
    // private CheckBox mALCCheck = null;
    private Button mBtnGo = null;
    private Button mBtnStop = null;
    private ArrayAdapter<String> mChannelAdapter = null;
    private ArrayAdapter<String> mRateAdapter = null;
    private ArrayAdapter<String> mModeAdapter = null;
    private ArrayAdapter<String> mPreambleAdapter = null;
    private ArrayAdapter<String> mGuardIntervalAdapter = null;
    private ArrayAdapter<String> mBandwidthAdapter = null;
    private int mModeIndex = 0;
    private int mPreambleIndex = 0;
    private int mBandwidthIndex = 0;
    private int mGuardIntervalIndex = 0;
    private int mAntenna = 0;

    private String PacketLength;
	private String PacketCount;
	private String Power;
	private String Channel;
	private String Bandwidth;
	private String Rate;
	private String Mode;
	private String Interval;
	private String Preamble;
    
    private RateInfo mRate = null;
    private ChannelInfo mChannel = null;
    private long mPktLenNum = DEFAULT_PKT_LEN;
    private long mCntNum = DEFAULT_PKT_CNT;
    private long mTxGainVal = DEFAULT_TX_GAIN;
    private boolean mTestInPorcess = false;
    private WiFiStateManager mWiFiStateManager = null;
    private HandlerThread mTestThread = null;
    FileParse fileparse = new FileParse("ComboTool.ini");
    
    private final Handler mHandler = new Handler() {
    
        @Override
        public void handleMessage(Message msg) {
            if (HANDLER_EVENT_FINISH == msg.what) {
                Log.v(TAG, "receive HANDLER_EVENT_FINISH");
                setViewEnabled(true);
            }
        }
    };
    private Handler mEventHandler = null;

    String[] mMode = { "continuous packet tx", "100% duty cycle",
            "carrier suppression", "local leakage", "enter power off" };
    String[] mPreamble = { "Normal", "CCK short", "802.11n mixed mode",
            "802.11n green field", };
    String[] mBandwidth = { "20MHz", "40MHz", "U20MHz", "L20MHz", };
    String[] mGuardInterval = { "800ns", "400ns", };

    class RateInfo {
        private static final short EEPROM_RATE_GROUP_CCK = 0;
        private static final short EEPROM_RATE_GROUP_OFDM_6_9M = 1;
        private static final short EEPROM_RATE_GROUP_OFDM_12_18M = 2;
        private static final short EEPROM_RATE_GROUP_OFDM_24_36M = 3;
        private static final short EEPROM_RATE_GROUP_OFDM_48_54M = 4;
        private static final short EEPROM_RATE_GROUP_OFDM_MCS0_32 = 5;

        int mRateIndex = 0;

        private final short[] mUcRateGroupEep = { EEPROM_RATE_GROUP_CCK,
                EEPROM_RATE_GROUP_CCK, EEPROM_RATE_GROUP_CCK,
                EEPROM_RATE_GROUP_CCK, EEPROM_RATE_GROUP_OFDM_6_9M,
                EEPROM_RATE_GROUP_OFDM_6_9M, EEPROM_RATE_GROUP_OFDM_12_18M,
                EEPROM_RATE_GROUP_OFDM_12_18M, EEPROM_RATE_GROUP_OFDM_24_36M,
                EEPROM_RATE_GROUP_OFDM_24_36M, EEPROM_RATE_GROUP_OFDM_48_54M,
                EEPROM_RATE_GROUP_OFDM_48_54M,
                // for future use
                EEPROM_RATE_GROUP_OFDM_MCS0_32, EEPROM_RATE_GROUP_OFDM_MCS0_32,
                EEPROM_RATE_GROUP_OFDM_MCS0_32, EEPROM_RATE_GROUP_OFDM_MCS0_32,
                EEPROM_RATE_GROUP_OFDM_MCS0_32, EEPROM_RATE_GROUP_OFDM_MCS0_32,
                EEPROM_RATE_GROUP_OFDM_MCS0_32, EEPROM_RATE_GROUP_OFDM_MCS0_32,
                EEPROM_RATE_GROUP_OFDM_MCS0_32, };
        private final String[] mPszRate = { "1M", "2M", "5.5M", "11M", "6M",
                "9M", "12M", "18M", "24M", "36M", "48M", "54M",
                // for future use 
                "MCS0", "MCS1", "MCS2", "MCS3", "MCS4", "MCS5", "MCS6", "MCS7",
                "MCS32",

        };

        private final int[] mRateCfg = { 2, 4, 11, 22, 12, 18, 24, 36, 48, 72,
                96, 108,
                // here we need to add cfg data for MCS
                22, 12, 18, 24, 36, 48, 72, 96, 108 };

       
        int getRateNumber() {
            return mPszRate.length;
        }

        
        String getRate() {
            return mPszRate[mRateIndex];
        }

        
        int getRateCfg() {
            return mRateCfg[mRateIndex];
        }

       
        int getUcRateGroupEep() {
            return mUcRateGroupEep[mRateIndex];
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        View contentView = LayoutInflater.from(this.getParent()).inflate(R.layout.advanced_wifi_tx, null);
		setContentView(contentView);
        
        mChannelSpinner = (Spinner) findViewById(R.id.WiFi_Tx_Channel_Spinner);
        mPreambleSpinner = (Spinner) findViewById(R.id.WiFi_Tx_Preamble_Spinner);
        mEtPkt = (EditText) findViewById(R.id.WiFi_Tx_PktLength_Edit);
        mEtPktCnt = (EditText) findViewById(R.id.WiFi_Tx_Pktcnt_Edit);
        mEtTxGain = (EditText) findViewById(R.id.WiFi_Tx_Power_Edit);// Tx gain
        mRateSpinner = (Spinner) findViewById(R.id.WiFi_Tx_Rate_Spinner);
        mModeSpinner = (Spinner) findViewById(R.id.WiFi_Tx_Mode_Spinner);
        // mXTEdit = (EditText) findViewById(R.id.WiFi_XtalTrim_Edit);
        // mWriteBtn = (Button) findViewById(R.id.WiFi_XtalTrim_Write);
        // mReadBtn = (Button) findViewById(R.id.WiFi_XtalTrim_Read);
        // mALCCheck = (CheckBox) findViewById(R.id.WiFi_ALC);
        mBtnGo = (Button) findViewById(R.id.WiFi_Tx_Start);
        mBtnStop = (Button) findViewById(R.id.WiFi_Tx_Stop);
        mBandwidthSpinner = (Spinner) findViewById(R.id.WiFi_TX_Bandwidth_Spinner);
        mGuardIntervalSpinner = (Spinner) findViewById(R.id.WiFi_Tx_GuardInterval_Spinner);

        mTestThread = new HandlerThread("Advanced Wifi Tx Test");
        mTestThread.start();
        mEventHandler = new EventHandler(mTestThread.getLooper());
        mBtnGo.setOnClickListener(this);
        mBtnStop.setOnClickListener(this);
        mChannel = new ChannelInfo();
        mRate = new RateInfo();
        
        if (mWiFiStateManager == null) {
            mWiFiStateManager = new WiFiStateManager(Advanced_WiFi_Tx_for_Porting_OK.this);
        }
        mChipID = mWiFiStateManager.checkState(Advanced_WiFi_Tx_for_Porting_OK.this);
        mChannel.getSupportChannels();
        
		PacketLength = fileparse.getValueByKey("[advance-wifi]", "TX_PACKET_LENGTH");
		PacketCount = fileparse.getValueByKey("[advance-wifi]", "TX_PACKET_COUNT");
		Power = fileparse.getValueByKey("[advance-wifi]", "TX_POWER");
		Channel = fileparse.getValueByKey("[advance-wifi]", "TX_CHANNEL");
		Bandwidth = fileparse.getValueByKey("[advance-wifi]", "TX_BANDWIDTH");
		Rate = fileparse.getValueByKey("[advance-wifi]", "TX_RATE");
		Mode = fileparse.getValueByKey("[advance-wifi]", "TX_MODE");
		Interval = fileparse.getValueByKey("[advance-wifi]", "TX_GUARD_INTERVAL");
		Preamble = fileparse.getValueByKey("[advance-wifi]", "TX_PREAMBLE");
        
	    mEtPkt.setText(PacketLength);
	    mEtPktCnt.setText(PacketCount);
	    mEtTxGain.setText(Power);// Tx gain
		
        mEtPktCnt.setOnKeyListener(new View.OnKeyListener() {
            public boolean onKey(View v, int keyCode, KeyEvent event) {
                CharSequence inputVal = mEtPktCnt.getText();
                if (TextUtils.equals(inputVal, "0")) {
                    Toast.makeText(Advanced_WiFi_Tx_for_Porting_OK.this,
                            R.string.wifi_toast_packet_error,
                            Toast.LENGTH_SHORT).show();
                    mEtPktCnt.setText(String.valueOf(DEFAULT_PKT_CNT));
                }
                return false;
            }
        });
        mChannelAdapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_spinner_item);
        mChannelAdapter
                .setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        for (int i = 1; i <= ChannelInfo.sChannels[0]; i++) {
            if (ChannelInfo.sChannels[i] > ChannelInfo.CHANNEL_NUMBER_14) {
                break;
            }
            for (String s : mChannel.mFullChannelName) {
                if (s.startsWith("Channel " + ChannelInfo.sChannels[i] + " ")) {
                    mChannelAdapter.add(s);
                    break;
                }
            }
        }
        mChannelSpinner.setAdapter(mChannelAdapter);
        mChannelSpinner.setOnItemSelectedListener(new OnItemSelectedListener() {

            public void onItemSelected(AdapterView<?> arg0, View arg1,
                    int arg2, long arg3) {
                if (WiFiHWTestNative.sIsInitialed) {
                    mChannel.mChannelSelect = mChannelAdapter.getItem(arg2);
                    Log.i(TAG, "The mChannelSelect is : "
                            + mChannel.mChannelSelect);
                    int number = mChannel.getChannelFreq();
                    WiFiHWTestNative.setChannel(number);
                    Log.i(TAG, "The channel freq =" + number);
                    uiUpdateTxPower();
                } else {
                    showDialog(DIALOG_WIFI_ERROR);
                }
            }

            public void onNothingSelected(AdapterView<?> arg0) {
                Log.v(TAG, "onNothingSelected");
            }
        });
        mRateAdapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_spinner_item);
        mRateAdapter
                .setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        for (int i = 0; i < mRate.getRateNumber(); i++) {
            mRateAdapter.add(mRate.mPszRate[i]);
        }
        mRateSpinner.setAdapter(mRateAdapter);
        mRateSpinner.setOnItemSelectedListener(new OnItemSelectedListener() {

            public void onItemSelected(AdapterView<?> arg0, View arg1,
                    int arg2, long arg3) {
                if (!WiFiHWTestNative.sIsInitialed) {
                    showDialog(DIALOG_WIFI_ERROR);
                    return;
                }
                mRate.mRateIndex = arg2;

                // set Tx Rate
                Log.i(TAG, "The mRateIndex is : " + arg2);
                // judge if high rate item selected MCS0~MCS7 MCS32
                mHighRateSelected = arg2 >= MAX_LOWER_RATE_NUMBER ? true
                        : false;
                int delta = mHighRateSelected ? 2 : 0;
                mPreambleAdapter.clear();
                mPreambleAdapter.add(mPreamble[0 + delta]);
                mPreambleAdapter.add(mPreamble[1 + delta]);
                mPreambleIndex = delta;
                uiUpdateTxPower();

                if (arg2 >= CCK_RATE_NUMBER) {
                    if (mCCKRateSelected) {
                        mCCKRateSelected = false;
                        mModeAdapter.remove(mMode[2]);
                        mModeSpinner.setSelection(0);
                    }
                } else {
                    if (!mCCKRateSelected) {
                        mCCKRateSelected = true;
                        mModeAdapter.insert(mMode[2], 2);
                        mModeSpinner.setSelection(0);
                    }
                }
                updateChannels();
                mLastRateGroup = mRate.getUcRateGroupEep();
            }

            public void onNothingSelected(AdapterView<?> arg0) {
                Log.v(TAG, "onNothingSelected");
            }
        });
        mModeAdapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_spinner_item);
        mModeAdapter
                .setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        for (int i = 0; i < mMode.length; i++) {
            mModeAdapter.add(mMode[i]);
        }
        mModeSpinner.setAdapter(mModeAdapter);
        mModeSpinner.setOnItemSelectedListener(new OnItemSelectedListener() {

            public void onItemSelected(AdapterView<?> arg0, View arg1,
                    int arg2, long arg3) {
                mModeIndex = arg2;
                Log.i(TAG, "The mModeIndex is : " + arg2);
                if (!mCCKRateSelected) {
                    if (arg2 >= 2) {
                        mModeIndex++;
                    }
                }
            }

            public void onNothingSelected(AdapterView<?> arg0) {
                Log.v(TAG, "onNothingSelected");
            }
        });
        // 802.11n select seetings
        mPreambleAdapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_spinner_item);
        mPreambleAdapter
                .setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        for (int i = 0; i < mPreamble.length; i++) {
            mPreambleAdapter.add(mPreamble[i]);
        }
        mPreambleSpinner.setAdapter(mPreambleAdapter);
        mPreambleSpinner
                .setOnItemSelectedListener(new OnItemSelectedListener() {

                    public void onItemSelected(AdapterView<?> arg0, View arg1,
                            int arg2, long arg3) {
                        mPreambleIndex = arg2 + (mHighRateSelected ? 2 : 0);

                    }

                    public void onNothingSelected(AdapterView<?> arg0) {
                        Log.v(TAG, "onNothingSelected");
                    }
                });
        // Bandwidth seetings
        mBandwidthAdapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_spinner_item);
        mBandwidthAdapter
                .setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        for (int i = 0; i < mBandwidth.length; i++) {
            mBandwidthAdapter.add(mBandwidth[i]);
        }
        mBandwidthSpinner.setAdapter(mBandwidthAdapter);
        mBandwidthSpinner
                .setOnItemSelectedListener(new OnItemSelectedListener() {

                    public void onItemSelected(AdapterView<?> arg0, View arg1,
                            int arg2, long arg3) {
                        // max Bandwidth setting value is 4
                        mBandwidthIndex = arg2 < mBandwidth.length ? arg2
                                : mBandwidthIndex;
                        updateChannels();
                        mLastBandwidth = mBandwidthIndex;
                    }

                    public void onNothingSelected(AdapterView<?> arg0) {
                        Log.v(TAG, "onNothingSelected");
                    }
                });
        // Guard Interval seetings
        mGuardIntervalAdapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_spinner_item);
        mGuardIntervalAdapter
                .setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        for (int i = 0; i < mGuardInterval.length; i++) {
            mGuardIntervalAdapter.add(mGuardInterval[i]);
        }
        mGuardIntervalSpinner.setAdapter(mGuardIntervalAdapter);
        mGuardIntervalSpinner
                .setOnItemSelectedListener(new OnItemSelectedListener() {
                    public void onItemSelected(AdapterView<?> arg0, View arg1,
                            int arg2, long arg3) {
                        mGuardIntervalIndex = arg2 < 2 ? arg2
                                : mGuardIntervalIndex;
                    }

                    public void onNothingSelected(AdapterView<?> arg0) {
                        Log.v(TAG, "onNothingSelected");
                    }
                });
        setViewEnabled(true);
    }

    @Override
    protected void onStart() {
        super.onStart();
        checkWiFiChipState();
    }

    public void onClick(View view) {
        if (!WiFiHWTestNative.sIsInitialed) {
            showDialog(DIALOG_WIFI_ERROR);
            return;
        }
        Log.d(TAG, "view_id = " + view.getId());
        if (view == mBtnGo) {
            onClickBtnTxGo();
        } else if (view == mBtnStop) {
            onClickBtnTxStop();
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        if (mEventHandler != null) {
            mEventHandler.removeMessages(HANDLER_EVENT_TIMER);
            if (mTestInPorcess) {
                if (WiFiHWTestNative.sIsInitialed) {
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_COMMAND,
                            COMMAND_INDEX_STOPTEST);
                }
                mTestInPorcess = false;
            }
        }
        mTestThread.quit();
        super.onDestroy();
    }

    private void uiUpdateTxPower() {
        short ucGain = 0;
        long i4TxPwrGain = 0;
        // long i4OutputPower = 0;
        // long i4targetAlc = 0;
        long[] gain = new long[LENGTH_3];
        int comboChannelIndex = mChannel.getChannelIndex();
        // 40MHz 0x8000 | mChannel.mChannelIndex else mChannel.mChannelIndex
        comboChannelIndex |= ((mBandwidthIndex == BANDWIDTH_INDEX_40) ? BANDWIDTH_40MHZ_MASK
                : 0);
        // may change to array[3];
        Log.w(TAG, "channelIdx " + comboChannelIndex + " rateIdx "
                + mRate.mRateIndex + " gain " + Arrays.toString(gain) + " Len "
                + LENGTH_3);
        if (0 == WiFiHWTestNative.readTxPowerFromEEPromEx(comboChannelIndex,
                mRate.mRateIndex, gain, LENGTH_3)) {
            i4TxPwrGain = gain[0];
            // i4OutputPower = gain[1];
            // i4targetAlc = gain[2];
            Log.i(TAG, "i4TxPwrGain from uiUpdateTxPower is " + i4TxPwrGain);
            ucGain = (short) (i4TxPwrGain & BIT_8_MASK);
        }
        
        mEtTxGain.setText(String.format(
                getString(R.string.wifi_tx_gain_format), ucGain / 2.0));
        // mTxGainEdit.setText(Long.toHexString(ucGain));
    }

    private void updateChannels() {
        boolean bUpdateWifiChannel = false;
        if (ChannelInfo.sHas14Ch) {
            if (mLastRateGroup != mRate.getUcRateGroupEep()) {
                if (RateInfo.EEPROM_RATE_GROUP_CCK == mRate.getUcRateGroupEep()) {
                    if (ChannelInfo.sHasUpper14Ch) {
                        int index = mChannelAdapter
                                .getPosition(mChannel.mFullChannelName[CHANNEL_14]);
                        if (-1 == index) {
                            mChannelAdapter
                                    .add(mChannel.mFullChannelName[CHANNEL_13]);
                            bUpdateWifiChannel = true;
                        } else {
                            mChannelAdapter.insert(
                                    mChannel.mFullChannelName[CHANNEL_13],
                                    index);
                            bUpdateWifiChannel = true;
                        }
                    } else {
                        mChannelAdapter
                                .add(mChannel.mFullChannelName[CHANNEL_13]);
                        bUpdateWifiChannel = true;
                    }
                } else if (RateInfo.EEPROM_RATE_GROUP_CCK == mLastRateGroup) {
                    mChannelAdapter
                            .remove(mChannel.mFullChannelName[CHANNEL_13]);
                    bUpdateWifiChannel = true;
                }
            }
        }
        if (ChannelInfo.sHasUpper14Ch) {
            if (mLastRateGroup != mRate.getUcRateGroupEep()) {
                if (RateInfo.EEPROM_RATE_GROUP_CCK == mLastRateGroup) {
                    for (int i = 1; i <= ChannelInfo.sChannels[0]; i++) {
                        if (ChannelInfo.sChannels[i] > ChannelInfo.CHANNEL_NUMBER_14) {
                            for (String s : mChannel.mFullChannelName) {
                                if (s.startsWith("Channel "
                                        + ChannelInfo.sChannels[i] + " ")) {
                                    mChannelAdapter.add(s);
                                    bUpdateWifiChannel = true;
                                    break;
                                }
                            }
                        }
                    }
                } else if (RateInfo.EEPROM_RATE_GROUP_CCK == mRate
                        .getUcRateGroupEep()) {
                    for (int i = ChannelInfo.CHANNEL_NUMBER_14; 
                    i < mChannel.mFullChannelName.length; i++) {
                        mChannelAdapter.remove(mChannel.mFullChannelName[i]);
                        bUpdateWifiChannel = true;
                    }
                }
            }
        }
        if (mLastBandwidth != mBandwidthIndex) {
            if (mBandwidthIndex == BANDWIDTH_INDEX_40) {
                mChannelAdapter.remove(mChannel.mFullChannelName[CHANNEL_0]);
                mChannelAdapter.remove(mChannel.mFullChannelName[CHANNEL_1]);
                mChannelAdapter.remove(mChannel.mFullChannelName[CHANNEL_11]);
                mChannelAdapter.remove(mChannel.mFullChannelName[CHANNEL_12]);
                bUpdateWifiChannel = true;
            }
            if (mLastBandwidth == BANDWIDTH_INDEX_40) {
                mChannelAdapter.insert(mChannel.mFullChannelName[CHANNEL_0],
                        CHANNEL_0);
                mChannelAdapter.insert(mChannel.mFullChannelName[CHANNEL_1],
                        CHANNEL_1);
                if (mChannel.isContains(CHANNEL_12)) {
                    mChannelAdapter.insert(
                            mChannel.mFullChannelName[CHANNEL_11], CHANNEL_11);
                }
                if (mChannel.isContains(CHANNEL_13)) {
                    mChannelAdapter.insert(
                            mChannel.mFullChannelName[CHANNEL_12], CHANNEL_12);
                }
                bUpdateWifiChannel = true;
            }
        }
        if (bUpdateWifiChannel) {
            updateWifiChannel();
        }
    }

    private void updateWifiChannel() {
        if (!WiFiHWTestNative.sIsInitialed) {
            Log.w(TAG, "Wifi is not initialized");
            showDialog(DIALOG_WIFI_ERROR);
            return;
        }
        if (0 == mChannel.getChannelIndex()) {
            mChannel.mChannelSelect = mChannelAdapter.getItem(0);
            int number = mChannel.getChannelFreq();
            WiFiHWTestNative.setChannel(number);
            Log.i(TAG, "The channel freq =" + number);
            uiUpdateTxPower();
        } else {
            mChannelSpinner.setSelection(0);
        }
    }

    
    private void onClickBtnTxGo() {
        long u4TxGainVal = 0;
        int i = 0;
        long pktNum;
        long cntNum;
        CharSequence inputVal;
        try {
            float pwrVal = Float.parseFloat(mEtTxGain.getText().toString());
            u4TxGainVal = (long) (pwrVal * 2);
            mEtTxGain
                    .setText(String.format(
                            getString(R.string.wifi_tx_gain_format),
                            u4TxGainVal / 2.0));
            // u4TxGainVal = Long.parseLong(mTxGainEdit.getText().toString(),
            // 16);
        } catch (NumberFormatException e) {
            Toast.makeText(Advanced_WiFi_Tx_for_Porting_OK.this, "invalid input value",
                    Toast.LENGTH_SHORT).show();
            return;
        }
        mTxGainVal = u4TxGainVal;
        mTxGainVal = mTxGainVal > BIT_8_MASK ? BIT_8_MASK : mTxGainVal;
        mTxGainVal = mTxGainVal < 0 ? 0 : mTxGainVal;
        Log.i(TAG, "Wifi Tx Test : " + mMode[mModeIndex]);
        switch (mModeIndex) {
        case TEST_MODE_TX:
            try {
                pktNum = Long.parseLong(mEtPkt.getText().toString());
                cntNum = Long.parseLong(mEtPktCnt.getText().toString());
            } catch (NumberFormatException e) {
                Toast.makeText(Advanced_WiFi_Tx_for_Porting_OK.this, "invalid input value",
                        Toast.LENGTH_SHORT).show();
                return;
            }
            mPktLenNum = pktNum;
            mCntNum = cntNum;
            break;
        case TEST_MODE_DUTY:
            // EMWifi.setOutputPower(mRate.getRateCfg(), u4TxGainVal,
            // u4Antenna);//for mt5921
            // set output power
            // setp 1:set rate
            // setp 2:set Tx gain
            // setp 3:set Antenna
            // setp 4:start output power test
            break;

        case TEST_MODE_CARRIER:
           
            // setp 1:set EEPROMRate Info
            // setp 2:set Tx gain
            // setp 3:set Antenna
            // step 4:start RF Carriar Suppression Test
            break;
        case TEST_MODE_LEAKAGE:
            // EMWifi.setLocalFrequecy(u4TxGainVal, u4Antenna);//for mt5921
            // setp 1:set Tx gain
            // setp 2:set Antenna
            // step 3:start Local Frequency Test

            break;
        case TEST_MODE_POWEROFF:
            // EMWifi.setNormalMode();
            // EMWifi.setOutputPin(20, 0);
            // EMWifi.setPnpPower(4);
            break;
        default:
            break;
        }
        if (mEventHandler == null) {
            Log.w(TAG, "eventHandler = null");
        } else {
            mEventHandler.sendEmptyMessage(HANDLER_EVENT_GO);
            // mGoBtn.setEnabled(false);
            setViewEnabled(false);
        }
    }

    private void setViewEnabled(boolean state) {
        mChannelSpinner.setEnabled(state);
        mGuardIntervalSpinner.setEnabled(state);
        mBandwidthSpinner.setEnabled(state);
        mPreambleSpinner.setEnabled(state);
        mEtPkt.setEnabled(state);
        mEtPktCnt.setEnabled(state);
        mEtTxGain.setEnabled(state);
        mRateSpinner.setEnabled(state);
        mModeSpinner.setEnabled(state);
        // mXTEdit.setEnabled(state);
        // mWriteBtn.setEnabled(state);
        // mReadBtn.setEnabled(state);
        // mALCCheck.setEnabled(state);
        mBtnGo.setEnabled(state);
        mBtnStop.setEnabled(!state);
    }

    private void onClickBtnTxStop() {
        if (mEventHandler == null) {
            Log.w(TAG, "eventHandler = null");
        } else {
            mEventHandler.sendEmptyMessage(HANDLER_EVENT_STOP);
        }
        switch (mModeIndex) {
        case TEST_MODE_TX:
            break;
        case TEST_MODE_POWEROFF:
            WiFiHWTestNative.setPnpPower(1);
            WiFiHWTestNative.setTestMode();
            WiFiHWTestNative.setChannel(mChannel.getChannelFreq());
            uiUpdateTxPower();
            // mGoBtn.setEnabled(true);
            break;
        default:
            WiFiHWTestNative.setStandBy();
            // mGoBtn.setEnabled(true);
            break;
        }
    }

    private void checkWiFiChipState() {
        int result = 0x0;
        if (mWiFiStateManager == null) {
            mWiFiStateManager = new WiFiStateManager(this);
        }
        result = mWiFiStateManager.checkState(this);
        switch (result) {
        case WiFiStateManager.ENABLE_WIFI_FAIL:
            showDialog(DIALOG_WIFI_FAIL);
            break;
        case WiFiStateManager.INVALID_CHIP_ID:
        case WiFiStateManager.SET_TEST_MODE_FAIL:
            showDialog(DIALOG_WIFI_ERROR);
            break;
        case WiFiStateManager.CHIP_READY:
        case WiFiStateManager.CHIP_ID_6620:
        case WiFiStateManager.CHIP_ID_5921:
            break;
        default:
            break;
        }
    }

    @Override
    protected Dialog onCreateDialog(int id) {
        Dialog dialog = null;
        AlertDialog.Builder builder = null;
        switch (id) {
        case DIALOG_WIFI_FAIL:
            builder = new AlertDialog.Builder(this);
            builder.setTitle(R.string.wifi_dialog_fail);
            builder.setCancelable(false);
            builder.setMessage(getString(R.string.wifi_dialog_fail_message));
            builder.setPositiveButton(R.string.wifi_ok,
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            mWiFiStateManager.disableWiFi();
                            finish();
                        }
                    });
            dialog = builder.create();
            break;
        case DIALOG_WIFI_ERROR:
            builder = new AlertDialog.Builder(this);
            builder.setTitle(R.string.wifi_dialog_error);
            builder.setCancelable(false);
            builder.setMessage(getString(R.string.wifi_dialog_error_message));
            builder.setPositiveButton(R.string.wifi_ok,
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            mWiFiStateManager.disableWiFi();
                            finish();
                        }
                    });
            dialog = builder.create();
            break;
        default:
            Log.d(TAG, "error dialog ID");
            break;
        }
        return dialog;
    }

    class EventHandler extends Handler {

        public EventHandler(Looper looper) {
            super(looper);
        }

        public void handleMessage(Message msg) {
            if (!WiFiHWTestNative.sIsInitialed) {
                showDialog(DIALOG_WIFI_ERROR);
                return;
            }
            Log.d(TAG, "new msg");
            // long i = 0;
            int rateIndex;
            long[] u4Value = new long[1];
            switch (msg.what) {
            case HANDLER_EVENT_GO:
                switch (mModeIndex) {
                case TEST_MODE_TX:
                    // EMWifi.setChannel(mChannel.getChannelFreq());
                    // set Tx gain of RF
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_POWER, mTxGainVal);
                    Log.i(TAG, "The mPreambleIndex is : " + mPreambleIndex);
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_PREAMBLE, mPreambleIndex);
                    // u4Antenna = 0, never be changed since first
                    // valued
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_ANTENNA, mAntenna);
                    // set package length, is there a maximum packet
                    // length? mtk80758-2010-11-2
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_PACKLENGTH, mPktLenNum);
                    // set package length, is there a maximum packet
                    // length? mtk80758-2010-11-2
                    // if cntNum = 0, send continious unless stop button
                    // is pressed
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_PACKCOUNT, mCntNum);
                    // package interval in unit of us, no need to allow
                    // user to set this value
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_PACKINTERVAL, 0);
                    // if (mALCCheck.isChecked() == false) {
                    // i = 0;
                    // } else {
                    // i = 1;
                    // }
                    // 9, means temperature conpensation
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_TEMP_COMPENSATION, 0);
                    // TX enable enable ? what does this mean
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_TXOP_LIMIT,
                            TXOP_LIMIT_VALUE);
                    // set Tx content
                    for (int i = 0; i < PACKCONTENT_BUFFER.length; i++) {
                        WiFiHWTestNative.setATParam(ATPARAM_INDEX_PACKCONTENT,
                                PACKCONTENT_BUFFER[i]);
                    }
                    // packet retry limit
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_RETRY_LIMIT, 1);
                    // QoS queue -AC2
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_QOS_QUEUE, 2);
                    Log.i(TAG, "The mGuardIntervalIndex is : "
                            + mGuardIntervalIndex);
                    // GuardInterval setting
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_GI, mGuardIntervalIndex);
                    Log.i(TAG, "The mBandwidthIndex is : " + mBandwidthIndex);
                    // Bandwidth setting
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_BANDWIDTH, mBandwidthIndex);
                    rateIndex = mRate.mRateIndex;
                    if (mHighRateSelected) {
                        rateIndex -= MAX_LOWER_RATE_NUMBER;
                        if (rateIndex > RATE_NOT_MCS_INDEX) {
                            rateIndex = RATE_MCS_INDEX; // for MCS32
                        }
                        rateIndex |= (1 << RATE_MODE_MASK);
                    }
                    // rateIndex |= (1 << 31);
                    Log.i(TAG, String.format("TXX rate index = 0x%08x",
                            rateIndex));
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_RATE, rateIndex);
                    int number = mChannel.getChannelFreq();
                    WiFiHWTestNative.setChannel(number);
                    Log.i(TAG, "target channel freq ="
                            + mChannel.getChannelFreq());
                    // start tx test
                    if (0 == WiFiHWTestNative.setATParam(ATPARAM_INDEX_COMMAND,
                            COMMAND_INDEX_STARTTX)) {
                        mTestInPorcess = true;
                    }
                    sendEmptyMessageDelayed(HANDLER_EVENT_TIMER, ONE_SENCOND);
                    break;
                case TEST_MODE_DUTY:
                    // EMWifi.setOutputPower(mRate.getRateCfg(),
                    // u4TxGainVal, u4Antenna);//for mt5921
                    // set output power
                    // setp 1:set rate
                    // setp 2:set Tx gain
                    // setp 3:set Antenna
                    // setp 4:start output power test
                    rateIndex = mRate.mRateIndex;
                    if (mHighRateSelected) {
                        rateIndex -= MAX_LOWER_RATE_NUMBER;
                        if (rateIndex > RATE_NOT_MCS_INDEX) {
                            rateIndex = RATE_MCS_INDEX; // for MCS32
                        }
                        rateIndex |= (1 << RATE_MODE_MASK);
                    }
                    Log.i(TAG, String.format("Tx rate index = 0x%08x",
                            rateIndex));
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_RATE, rateIndex);
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_POWER, mTxGainVal);
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_ANTENNA, mAntenna);
                    // start output power test
                    if (0 == WiFiHWTestNative.setATParam(ATPARAM_INDEX_COMMAND,
                            COMMAND_INDEX_OUTPUTPOWER)) {
                        mTestInPorcess = true;
                    }
                    break;
                case TEST_MODE_CARRIER:
                    // setp 1:set EEPROMRate Info
                    // setp 2:set Tx gain
                    // setp 3:set Antenna
                    // step 4:start RF Carriar Suppression Test
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_POWER, mTxGainVal);
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_ANTENNA, mAntenna);
                    // start carriar suppression test
                    if (ChipSupport.getChip() == ChipSupport.MTK_6573_SUPPORT) {
                        if (0 == WiFiHWTestNative.setATParam(ATPARAM_INDEX_COMMAND,
                                COMMAND_INDEX_CARRIER)) {
                            mTestInPorcess = true;
                        }
                    } else {
                        if (mCCKRateSelected) {
                            WiFiHWTestNative.setATParam(ATPARAM_INDEX_CWMODE,
                                    CWMODE_CCKPI);
                        } else {
                            WiFiHWTestNative.setATParam(ATPARAM_INDEX_CWMODE,
                                    CWMODE_OFDMLTF);
                        }
                        if (0 == WiFiHWTestNative.setATParam(ATPARAM_INDEX_COMMAND,
                                COMMAND_INDEX_CARRIER_NEW)) {
                            mTestInPorcess = true;
                        }
                    }
                    break;
                case TEST_MODE_LEAKAGE:
                    // Wifi.setLocalFrequecy(u4TxGainVal, u4Antenna);
                    // setp 1:set Tx gain
                    // setp 2:set Antenna
                    // step 3:start Local Frequency Test
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_POWER, mTxGainVal);
                    WiFiHWTestNative.setATParam(ATPARAM_INDEX_ANTENNA, mAntenna);
                    // start carriar suppression test
                    if (0 == WiFiHWTestNative.setATParam(ATPARAM_INDEX_COMMAND,
                            COMMAND_INDEX_LOCALFREQ)) {
                        mTestInPorcess = true;
                    }
                    break;
                case TEST_MODE_POWEROFF:
                    // Wifi.setNormalMode();
                    // Wifi.setOutputPin(20, 0);
                    // Wifi.setPnpPower(4);
                    break;
                default:
                    break;
                }
                break;
            case HANDLER_EVENT_STOP:
                Log.i(TAG, "The Handle event is : HANDLER_EVENT_STOP");
                if (mTestInPorcess) {
                    u4Value[0] = WiFiHWTestNative.setATParam(ATPARAM_INDEX_COMMAND,
                            COMMAND_INDEX_STOPTEST);
                    mTestInPorcess = false;
                }
                // driver does not support query operation on
                // functionIndex = 1 , we can only judge whether this
                // operation is processed successfully through the
                // return value
                if (mEventHandler != null) {
                    mEventHandler.removeMessages(HANDLER_EVENT_TIMER);
                }
                mHandler.sendEmptyMessage(HANDLER_EVENT_FINISH);
                break;
            case HANDLER_EVENT_TIMER:
                u4Value[0] = 0;
                long pktCnt = 0;
                Log.i(TAG, "The Handle event is : HANDLER_EVENT_TIMER");
                try {
                    pktCnt = Long.parseLong(mEtPktCnt.getText().toString());
                } catch (NumberFormatException e) {
                    Toast.makeText(Advanced_WiFi_Tx_for_Porting_OK.this, "invalid input value",
                            Toast.LENGTH_SHORT).show();
                    return;
                }
                // here we need to judge whether target number packet is
                // finished sent or not
                if (0 == WiFiHWTestNative
                        .getATParam(ATPARAM_INDEX_TRANSMITCOUNT, u4Value)) {
                    Log.d(TAG,
                            "query Transmitted packet count succeed, count = "
                                    + u4Value[0] + " target count = " + pktCnt);
                    if (u4Value[0] == pktCnt) {
                        removeMessages(HANDLER_EVENT_TIMER);
                        mHandler.sendEmptyMessage(HANDLER_EVENT_FINISH);
                        break;
                    }
                } else {
                    Log.w(TAG, "query Transmitted packet count failed");
                }
                sendEmptyMessageDelayed(HANDLER_EVENT_TIMER, ONE_SENCOND);
                break;
            default:
                break;
            }
        }
    }
}*/