package mediatek.android.application.Advanced_Tool;

import java.io.IOException;

import mediatek.android.application.R;
import mediatek.android.application.FileParse;

import android.app.Activity;
import android.content.Context;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.SystemClock;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

public class Advanced_WiFi_Tx extends Activity {
	
	private Button buttonStart;
	private Button buttonStop;
	
	private Spinner channelSpinner;
	private Spinner bandwidthSpinner;
	private Spinner rateSpinner;
	private Spinner modeSpinner;
	private Spinner intervalSpinner;
	private Spinner mPreambleSpinner;
	
	private TextView mTextViewTxGain; //for RT5370 & MT7601 Tx Gain
	private EditText editTextPktCnt;
	private EditText editTextPktLength;
	private EditText editTextTxPower;
//	private EditText editTextFreqOffset;
	
	private String PacketLength;
	private String PacketCount;
	private String Power;
	private String Channel;
	private String Bandwidth;
	private String Rate;
	private String Mode;
	private String Interval;
	private String Preamble;
//	private String FreqOffset;
	
	private int mWifiInitStat = WifiManager.WIFI_STATE_DISABLED;
	FileParse fileparse = new FileParse("ComboTool.ini");
	private ArrayAdapter<String>	m_adapter;
	private WiFiHWTestNative mWifiHWTest = new WiFiHWTestNative();
	private WifiManager mWifiManager = null;
	private static final long DEFAULT_WAIT_TIME = 100;
	private int CHIP_MT76xx = 0x02;
	private int MT76xx_TxGain = 0x01;
	private int MT76xx_Freq_Offset = 0x02;
	private int mTxGain = 0;
	private int mFreqOffset = 0;
	
	private static final String TAG = "Advanced_WiFi_Tx";
	private int iBGNRateFlag = 0;
	private int BGN_RATE_FLAG = 12;
	private boolean isMT76xx = false;
	private int ChipID = 0;
	
	final  String[] mChannelName = {
			"Channel 1 [2412MHz]", "Channel 2 [2417MHz]",
            "Channel 3 [2422MHz]", "Channel 4 [2427MHz]",
            "Channel 5 [2432MHz]", "Channel 6 [2437MHz]",
            "Channel 7 [2442MHz]", "Channel 8 [2447MHz]",
            "Channel 9 [2452MHz]", "Channel 10 [2457MHz]",
            "Channel 11 [2462MHz]", "Channel 12 [2467MHz]",
            "Channel 13 [2472MHz]", "Channel 14 [2484MHz]", };
	
	final  String[] mBandwidth = {
            "20MHz", "40MHz",
 //           "U20MHz", "L20MHz",
            };
	
	final  String[] mRate = {
            "1M", "2M",
            "5.5M", "6M",
            "9M",  "11M",
            "12M", "18M",
            "24M", "36M",
            "48M", "54M",
            "MCS0", "MCS1",
            "MCS2", "MCS3",
            "MCS4", "MCS5",
            "MCS6", "MCS7",
            "MCS32",};
	final  String[] mBW40MRate = {
            "MCS8", "MCS9",
            "MCS10", "MCS11",
            "MCS12", "MCS13",
            "MCS14", "MCS15",
            };
	
	final  String[] mMode = {
            "continuous packet tx", "tx output power",
            "carrier suppression", "local leakage",
  //          "enter power off",
            };
	
	final String[] mPreamble = { "Normal", "CCK short", };
	
	final String[] mPreamble11n = { "802.11n mixed mode",
            "802.11n green field", };
	
	final  String[] mInterval = {
            "800ns", "400ns",};
	//for MT76xx Tx Gain
	final String mMT76xxTxGainNmae = "PA DAC Gain:";
	
	private int StrToId(String [] Group, String str)
	{
		int ID = -1;
		
		for (int i = 0; i < Group.length; i ++)
		{
			if (Group[i].equals(str))
			{
				ID = i;
				return ID;
			}
		}
		
		return ID;
	}
	
	private int EnableWifi()
	{
		mWifiManager = (WifiManager) Advanced_WiFi_Tx.this.getSystemService(Context.WIFI_SERVICE);
	
		 if (mWifiManager != null) {
				mWifiInitStat = mWifiManager.getWifiState();
				Log.d(TAG, "InitStat " + mWifiInitStat);
				
	            if (mWifiManager.getWifiState() != WifiManager.WIFI_STATE_ENABLED) {
	                if (mWifiManager.setWifiEnabled(true)) {
	                    Log.d(TAG, "After enable wifi, state is : "
	                            + mWifiManager.getWifiState());
	                    while (mWifiManager.getWifiState() == WifiManager.WIFI_STATE_ENABLING
	                            || mWifiManager.getWifiState() == WifiManager.WIFI_STATE_DISABLED) {
	                        SystemClock.sleep(DEFAULT_WAIT_TIME);
	                    }
	                } else {
	                    Log.w(TAG, "enable wifi power failed");
	                    return -1;
	                }
	                
	                Log.d(TAG, "Now state is : "
                            + mWifiManager.getWifiState());
	            }
		 }
		 
		 return 0;
	}
	
	public void DisableWiFi() {
        if (mWifiManager != null) {
        	if (WifiManager.WIFI_STATE_DISABLED == mWifiInitStat)
        	{
	            if (mWifiManager.setWifiEnabled(false)) {
	                Log.d(TAG, "disable wifi power succeed");
	            } else {
	                Log.w(TAG, "disable wifi power failed");
	            }
	            while (mWifiManager.getWifiState() != WifiManager.WIFI_STATE_DISABLED) {
                    SystemClock.sleep(DEFAULT_WAIT_TIME);
                }
	            Log.d(TAG, "After disalbe Wifi, state is : "
                        + mWifiManager.getWifiState());
        	}
        }
    }
	
	@Override
	public void onDestroy() {
		// TODO Auto-generated method stub
		DisableWiFi();
		super.onDestroy();
	}
	
	@Override
	public void onPause() {
		// TODO Auto-generated method stub
		if (true == mWifiHWTest.bIsTxStart)
		{
			mWifiHWTest.WiFi_TxStop();	
			mWifiHWTest.bIsTxStart = false;
			setViewEnabled(true, iBGNRateFlag);
		}
		super.onPause();
	}
	
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		View contentView = LayoutInflater.from(this.getParent()).inflate(R.layout.advanced_wifi_tx, null);
		setContentView(contentView);
		
		EnableWifi();
		ChipID = mWifiHWTest.WiFi_Init();
			
		buttonStart = (Button)findViewById(R.id.WiFi_Tx_Start);
		buttonStop = (Button)findViewById(R.id.WiFi_Tx_Stop);
		editTextPktCnt = (EditText)findViewById(R.id.WiFi_Tx_Pktcnt_Edit);
		editTextPktLength = (EditText)findViewById(R.id.WiFi_Tx_PktLength_Edit);
		editTextTxPower  = (EditText)findViewById(R.id.WiFi_Tx_Power_Edit);
//		editTextFreqOffset = (EditText)findViewById(R.id.WiFi_Freq_Offset_Edit);
		
		mTextViewTxGain = (TextView)findViewById(R.id.WiFi_Tx_Power_Text);
		if (ChipID == CHIP_MT76xx)
		{
			mTextViewTxGain.setText(mMT76xxTxGainNmae);
		}
		
		channelSpinner = (Spinner) findViewById(R.id.WiFi_Tx_Channel_Spinner);
		bandwidthSpinner = (Spinner) findViewById(R.id.WiFi_TX_Bandwidth_Spinner);
		rateSpinner = (Spinner) findViewById(R.id.WiFi_Tx_Rate_Spinner);
		modeSpinner = (Spinner) findViewById(R.id.WiFi_Tx_Mode_Spinner);
		intervalSpinner = (Spinner) findViewById(R.id.WiFi_Tx_GuardInterval_Spinner);
		mPreambleSpinner = (Spinner) findViewById(R.id.WiFi_Tx_Preamble_Spinner);		
		
		m_adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, mChannelName);
		m_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		channelSpinner.setAdapter(m_adapter);
		
		m_adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, mBandwidth);
		m_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		bandwidthSpinner.setAdapter(m_adapter);
		
		m_adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, mRate);
		m_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		rateSpinner.setAdapter(m_adapter);
		
		m_adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, mMode);
		m_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		modeSpinner.setAdapter(m_adapter);
		
		m_adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, mInterval);
		m_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		intervalSpinner.setAdapter(m_adapter);
		
//		m_adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, mGIType);
//		m_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
//		mGITypenSpinner.setAdapter(m_adapter);
		
		
		PacketLength = fileparse.getValueByKey("[advance-wifi]", "TX_PACKET_LENGTH");
		PacketCount = fileparse.getValueByKey("[advance-wifi]", "TX_PACKET_COUNT");
		Power = fileparse.getValueByKey("[advance-wifi]", "TX_POWER");
		Channel = fileparse.getValueByKey("[advance-wifi]", "TX_CHANNEL");
		Bandwidth = fileparse.getValueByKey("[advance-wifi]", "TX_BANDWIDTH");
		Rate = fileparse.getValueByKey("[advance-wifi]", "TX_RATE");
		Mode = fileparse.getValueByKey("[advance-wifi]", "TX_MODE");
		Interval = fileparse.getValueByKey("[advance-wifi]", "TX_GUARD_INTERVAL");
		Preamble = fileparse.getValueByKey("[advance-wifi]", "TX_PREAMBLE");
//		FreqOffset = fileparse.getValueByKey("[advance-wifi]", "TX_FREQOFFSET");
		
		editTextPktCnt.setText(PacketCount);
		editTextPktLength.setText(PacketLength);
		editTextTxPower.setText(Power);
//		editTextFreqOffset.setText(FreqOffset);
		
		int ChannelPosition = StrToId(mChannelName, Channel);
		if (-1 == ChannelPosition)
		{
			ChannelPosition = 0;
		}
		int BandPosition = StrToId(mBandwidth, Bandwidth);
		if (-1 == BandPosition)
		{
			BandPosition = 0;
		}
		int RatePosition = StrToId(mRate, Rate);
		if (-1 == RatePosition)
		{
			RatePosition = 0;
		}
		int ModePosition = StrToId(mMode, Mode);
		if (-1 == ModePosition)
		{
			ModePosition = 0;
		}
		int IntervalPosition = StrToId(mInterval, Interval);
		if (-1 == IntervalPosition)
		{
			IntervalPosition = 0;
		}
		
		channelSpinner.setSelection(ChannelPosition);
		bandwidthSpinner.setSelection(BandPosition);
		rateSpinner.setSelection(RatePosition);
		modeSpinner.setSelection(ModePosition);
		intervalSpinner.setSelection(IntervalPosition);

		if (ChipID == CHIP_MT76xx)
		{
			mTxGain = mWifiHWTest.WiFi_MT76xxGetEfuse(MT76xx_TxGain, ChannelPosition);
			mFreqOffset = mWifiHWTest.WiFi_MT76xxGetEfuse(MT76xx_Freq_Offset, 0);
			editTextTxPower.setText(String.valueOf(mTxGain));
		}
		
		iBGNRateFlag = RatePosition;
		if (iBGNRateFlag < BGN_RATE_FLAG)
		{
			m_adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, mPreamble);
			m_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
			mPreambleSpinner.setAdapter(m_adapter);
			
			int PreamblePosition = StrToId(mPreamble, Preamble);
			if (-1 == PreamblePosition)
			{
				PreamblePosition = 0;
			}
			mPreambleSpinner.setSelection(PreamblePosition);
			mWifiHWTest.setPreamble(PreamblePosition);
		}
		else
		{
			m_adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, mPreamble11n);
			m_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
			mPreambleSpinner.setAdapter(m_adapter);
			
			int PreamblePosition = StrToId(mPreamble11n, Preamble);
			if (-1 == PreamblePosition)
			{
				PreamblePosition = 0;
			}
			mPreambleSpinner.setSelection(PreamblePosition);
			mWifiHWTest.setPreamble(PreamblePosition);
		}
		
		mWifiHWTest.setChannel(ChannelPosition);
		mWifiHWTest.setBandWidth(BandPosition);
		mWifiHWTest.setRate(RatePosition);
		mWifiHWTest.setMode(ModePosition);
		mWifiHWTest.setGIType(IntervalPosition);
				
		//初始化时,将stop disable.
		setViewEnabled(true, iBGNRateFlag);
		
		buttonStart.setOnClickListener(new Button.OnClickListener() {

			public void onClick(View arg0) {
				PacketCount = editTextPktCnt.getText().toString();
				PacketLength = editTextPktLength.getText().toString();
				Power = editTextTxPower.getText().toString();
//				FreqOffset = editTextFreqOffset.getText().toString();
				
				try {
					Integer.parseInt(PacketCount);
				} catch (Exception e) {
					Toast
							.makeText(
									getApplicationContext(),
									"Packet Count is invalid ! \n  Use default 3000",
									Toast.LENGTH_LONG).show();
					PacketCount = "3000";
					editTextPktCnt.setText(PacketCount);
					fileparse.updateKeyValue("[advance-wifi]", "TX_PACKET_COUNT", PacketCount);

				}
/*				
				try {
					Integer.parseInt(FreqOffset);
				} catch (Exception e) {
					Toast
							.makeText(
									getApplicationContext(),
									"Packet Count is invalid ! \n  Use default 30",
									Toast.LENGTH_LONG).show();
					FreqOffset = "30";
					editTextFreqOffset.setText(FreqOffset);
					fileparse.updateKeyValue("[advance-wifi]", "TX_FREQOFFSET", FreqOffset);

				}
*/				
				try {
					Integer.parseInt(PacketLength);
				} catch (Exception e) {
					Toast
							.makeText(
									getApplicationContext(),
									"Packet Length is invalid ! \n  Use default 1024",
									Toast.LENGTH_LONG).show();
					PacketLength = "1024";
					editTextPktLength.setText(PacketLength);
					fileparse.updateKeyValue("[advance-wifi]", "TX_PACKET_LENGTH", PacketLength);

				}
				
				try {
					Integer.parseInt(Power);
				} catch (Exception e) {
					Toast
							.makeText(
									getApplicationContext(),
									"Packet Length is invalid ! \n  Use default 1024",
									Toast.LENGTH_LONG).show();
					Power = "18";
					editTextTxPower.setText(Power);
					fileparse.updateKeyValue("[advance-wifi]", "TX_POWER", Power);

				}
				
				fileparse.updateKeyValue("[advance-wifi]", "TX_PACKET_COUNT", PacketCount);
				fileparse.updateKeyValue("[advance-wifi]", "TX_PACKET_LENGTH", PacketLength);
				fileparse.updateKeyValue("[advance-wifi]", "TX_POWER", Power);
//				fileparse.updateKeyValue("[advance-wifi]", "TX_FREQOFFSET", FreqOffset);
				
				mWifiHWTest.setCount(Integer.parseInt(PacketCount));
				mWifiHWTest.setPadloadLen(Integer.parseInt(PacketLength));
				mWifiHWTest.setTxGain(Integer.parseInt(Power));
//				mWifiHWTest.setFreqOffset(Integer.parseInt(FreqOffset));
				
				
				if (false == mWifiHWTest.WiFi_TxStart())
				{
					Log.e(TAG, "WiFi_TxStart() error!");
					onDestroy();
				}
				else
				{
					mWifiHWTest.bIsTxStart = true;
				}
				
				setViewEnabled(false, iBGNRateFlag);
				
				try {
					fileparse.updateConFile("ComboTool.ini");
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		});

		buttonStop.setOnClickListener(new Button.OnClickListener() {

			public void onClick(View arg0) {
				
				if (false == mWifiHWTest.WiFi_TxStop())
				{
					Log.e(TAG, "WiFi_TxStop() error!");
				}
				else
				{
					mWifiHWTest.bIsTxStart = false;
				}
				
				setViewEnabled(true, iBGNRateFlag);
			}
		});
		
		channelSpinner.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {

			public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2, long arg3)
			{
				fileparse.updateKeyValue("[advance-wifi]", "TX_CHANNEL", mChannelName[arg2]);
				//设置显示当前选择的项
				mWifiHWTest.setChannel(arg2);
				if (ChipID == CHIP_MT76xx)
				{
					mTxGain = mWifiHWTest.WiFi_MT76xxGetEfuse(MT76xx_TxGain, arg2);
					editTextTxPower.setText(String.valueOf(mTxGain));
				}
				arg0.setVisibility(View.VISIBLE);
			}

			public void onNothingSelected(AdapterView<?> arg0)
			{
				// TODO Auto-generated method stub
			}

		});
		
		bandwidthSpinner.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {

			public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2, long arg3)
			{
				fileparse.updateKeyValue("[advance-wifi]", "TX_BANDWIDTH", mBandwidth[arg2]);
				//设置显示当前选择的项
	/*			
				//work around for MT76xx MCS8-MCS15
				if (ChipID == CHIP_MT76xx)
				{
					if (1 == arg2)
					{
						m_adapter = new ArrayAdapter<String>(Advanced_WiFi_Tx.this, android.R.layout.simple_spinner_item, mBW40MRate);
					}
					else
					{
						m_adapter = new ArrayAdapter<String>(Advanced_WiFi_Tx.this, android.R.layout.simple_spinner_item, mRate);
					}
					m_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
					rateSpinner.setAdapter(m_adapter);
					
					rateSpinner.setSelection(0);
					mWifiHWTest.setRate(0);
				}
	 */				
				mWifiHWTest.setBandWidth(arg2);
				arg0.setVisibility(View.VISIBLE);
			}

			public void onNothingSelected(AdapterView<?> arg0)
			{
				// TODO Auto-generated method stub
			}

		});
		
		rateSpinner.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {

			public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2, long arg3)
			{
				fileparse.updateKeyValue("[advance-wifi]", "TX_RATE", mRate[arg2]);
				//设置显示当前选择的项
				mWifiHWTest.setRate(arg2);
				iBGNRateFlag = arg2;
				if (iBGNRateFlag < BGN_RATE_FLAG)
				{
					m_adapter = new ArrayAdapter<String>(Advanced_WiFi_Tx.this, android.R.layout.simple_spinner_item, mPreamble);
					m_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
					mPreambleSpinner.setAdapter(m_adapter);
					
					int PreamblePosition = StrToId(mPreamble, Preamble);
					if (-1 == PreamblePosition)
					{
						PreamblePosition = 0;
					}
					mPreambleSpinner.setSelection(PreamblePosition);
					mWifiHWTest.setPreamble(PreamblePosition);
				}
				else
				{
					m_adapter = new ArrayAdapter<String>(Advanced_WiFi_Tx.this, android.R.layout.simple_spinner_item, mPreamble11n);
					m_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
					mPreambleSpinner.setAdapter(m_adapter);
					
					int PreamblePosition = StrToId(mPreamble11n, Preamble);
					if (-1 == PreamblePosition)
					{
						PreamblePosition = 0;
					}
					mPreambleSpinner.setSelection(PreamblePosition);
					mWifiHWTest.setPreamble(PreamblePosition);
				}
				
				setViewEnabled(true, iBGNRateFlag);
				arg0.setVisibility(View.VISIBLE);
			}

			public void onNothingSelected(AdapterView<?> arg0)
			{
				// TODO Auto-generated method stub
			}

		});
		
		modeSpinner.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {

			public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2, long arg3)
			{
				fileparse.updateKeyValue("[advance-wifi]", "TX_MODE", mMode[arg2]);
				//设置显示当前选择的项
				mWifiHWTest.setMode(arg2);
				arg0.setVisibility(View.VISIBLE);
			}

			public void onNothingSelected(AdapterView<?> arg0)
			{
				// TODO Auto-generated method stub
			}

		});
		
		intervalSpinner.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {

			public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2, long arg3)
			{
				fileparse.updateKeyValue("[advance-wifi]", "TX_GUARD_INTERVAL", mInterval[arg2]);
				//设置显示当前选择的项
				mWifiHWTest.setGIType(arg2);
				arg0.setVisibility(View.VISIBLE);
			}

			public void onNothingSelected(AdapterView<?> arg0)
			{
				// TODO Auto-generated method stub
			}

		});
		
		mPreambleSpinner.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {

			public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2, long arg3)
			{
				if (iBGNRateFlag < BGN_RATE_FLAG)
				{
					fileparse.updateKeyValue("[advance-wifi]", "TX_PREAMBLE", mPreamble[arg2]);
					mWifiHWTest.setPreamble(arg2);
				}
				else
				{
					fileparse.updateKeyValue("[advance-wifi]", "TX_PREAMBLE", mPreamble11n[arg2]);
					mWifiHWTest.setPreamble(arg2);
				}
				arg0.setVisibility(View.VISIBLE);
			}

			public void onNothingSelected(AdapterView<?> arg0)
			{
				// TODO Auto-generated method stub
			}

		});
	}
	
	private void setViewEnabled(boolean state, int iBGN_Flag) 
	{
	
		buttonStart.setEnabled(state);
		buttonStop.setEnabled(!state);
		editTextPktCnt.setEnabled(state);
		editTextPktLength.setEnabled(state);		
		editTextTxPower.setEnabled(state);
		channelSpinner.setEnabled(state);
		bandwidthSpinner.setEnabled(state);
		rateSpinner.setEnabled(state);		
		modeSpinner.setEnabled(state);
		intervalSpinner.setEnabled(state);	
		
		if (ChipID == CHIP_MT76xx)
		{
			if (iBGN_Flag < BGN_RATE_FLAG)
			{
				mPreambleSpinner.setEnabled(false);
			}
			else
			{
				mPreambleSpinner.setEnabled(state);
			}
		}
		else
		{
			mPreambleSpinner.setEnabled(state);
		}
//		editTextFreqOffset.setEnabled(state);
	}
}
