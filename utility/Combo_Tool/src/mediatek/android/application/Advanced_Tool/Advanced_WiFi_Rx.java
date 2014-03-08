package mediatek.android.application.Advanced_Tool;

import java.io.IOException;

import mediatek.android.application.R;
import mediatek.android.application.FileParse;

import android.app.Activity;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
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

public class Advanced_WiFi_Rx extends Activity {
	
	private static final String TAG = "Advanced_WiFi_Rx";
	private Button buttonStart = null;
	private Button buttonStop = null;
	private Spinner channelSpinner = null;
	private Spinner bandwidthSpinner = null;
	private TextView textViewFCSError = null;
	private TextView textViewRxOK = null;
	private TextView textViewPER = null;
	private int mWifiInitStat = WifiManager.WIFI_STATE_DISABLED;
	
	private static int CHIP_MT76xx = 0x02;
	private int ChipID = 0;
	
	private String Channel = null;
	private String Bandwidth = null;
	
    private static final int HANDLER_EVENT_RX = 2;
    protected static final long HANDLER_RX_DELAY_TIME = 1000;
    private static final int DIALOG_WAITING_FOR_WIFI_RX_START = 1;
    private static final float PERCENT = (float) 100.0;
    private static final String TEXT_ZERO = "0";
    private static final int WAIT_COUNT = 10;
    private WiFiHWTestNative mWiFiHWTest = new WiFiHWTestNative();
    
	private ArrayAdapter<String>	m_adapter = null;
	private ArrayAdapter<String>	mChannelAdapter = null;

	private int iRxChannelIdx = 1;
	private int iRxBandWidthIdx = 0;
	private WifiManager mWifiManager = null;

	private static final long DEFAULT_WAIT_TIME = 100;
	
	FileParse fileparse = new FileParse("ComboTool.ini");
	
	final  String[] mChannelName = {
            // 2.4GHz frequency serials
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
	
	private final Handler mHandler = new Handler() {

        public void handleMessage(Message msg) {
            if (!mWiFiHWTest.bIsRxStart) {
 //               showDialog(DIALOG_WIFI_ERROR);
                return;
            }
            if (HANDLER_EVENT_RX == msg.what) {
                int[] iRxResult = new int[3];
                int iRXError = -1;
                int iRXCntOk = -1;
                float fRxPer = -1;
                Log.i(TAG, "The Handle event is : HANDLER_EVENT_RX");
                try {
                	fRxPer = Float.parseFloat(textViewPER.getText().toString());
                } catch (NumberFormatException e) {
                    Log.d(TAG, "Long.parseLong NumberFormatException: "
                            + e.getMessage());
                }
                iRxResult = mWiFiHWTest.WiFi_GetRxResult();
                Log.d(TAG, "after rx test: rx ok = "
                                + String.valueOf(iRxResult[0]));
                Log.d(TAG, "after rx test: fcs error = "
                        + String.valueOf(iRxResult[1]));
                iRXCntOk = iRxResult[0]/* - i4Init[0] */;
                iRXError = iRxResult[1]/* - i4Init[1] */;
                if (iRXCntOk + iRXError != 0) {
                	fRxPer = iRXError * PERCENT / (iRXCntOk + iRXError);
                }
                textViewFCSError.setText(String.valueOf(iRXError));
                textViewRxOK.setText(String.valueOf(iRXCntOk));
                textViewPER.setText(String.format("%.2f", fRxPer) + "%");
            }
            mHandler.sendEmptyMessageDelayed(HANDLER_EVENT_RX, HANDLER_RX_DELAY_TIME);
        }
    };
	
/*	
	private String IdtoStr(String [] Group, int ID)
	{
		String Result = null;
		
		Result = Group[ID];
		
		return Result;
	}
*/
	
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
		mWifiManager = (WifiManager) Advanced_WiFi_Rx.this.getSystemService(Context.WIFI_SERVICE);
	
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
		if (true == mWiFiHWTest.bIsRxStart)
		{
			buttonStop.setEnabled(false);
			buttonStart.setEnabled(true);
			channelSpinner.setEnabled(true);
				
	        mHandler.removeMessages(HANDLER_EVENT_RX);
	        mWiFiHWTest.bIsRxStart = false;
//	        DisableWiFi();
		}
		super.onPause();
	}
	
	@Override
    protected Dialog onCreateDialog(int id) {
        ProgressDialog dialog = null;
        if (DIALOG_WAITING_FOR_WIFI_RX_START == id) {
            dialog = new ProgressDialog(Advanced_WiFi_Rx.this.getParent());
            dialog.setTitle(R.string.dialog_title);
            dialog.setMessage(getString(R.string.dialog_message_wifi));
            dialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
            dialog.setCancelable(false);
            dialog.setProgress(0);
            Log.d(TAG, "Dialog!");
        } else {
            dialog = new ProgressDialog(this.getParent());
            dialog.setTitle(R.string.dialog_title_error);
            dialog.setMessage(getString(R.string.dialog_message_error));
        }
        return dialog;
    }
	
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		View contentView = LayoutInflater.from(this.getParent()).inflate(R.layout.advanced_wifi_rx, null);
		setContentView(contentView);
		Log.d(TAG, "WiFi_RxStart !");
		
		EnableWifi();		
		ChipID = mWiFiHWTest.WiFi_Init();

		buttonStart = (Button)findViewById(R.id.WiFi_Start_Rx);
		buttonStop = (Button)findViewById(R.id.WiFi_Stop_Rx);
		textViewFCSError = (TextView)findViewById(R.id.WiFi_FCS_Content);
		textViewRxOK = (TextView)findViewById(R.id.WiFi_Rx_Content);
		textViewPER  = (TextView)findViewById(R.id.WiFi_PER_Content);
	
		//初始化时,将stop disable.
		buttonStop.setEnabled(false);
		
/*		channelSpinner = (Spinner) findViewById(R.id.WiFi_Rx_Channel_Spinner);
		mChannelAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, mChannel);
		mChannelAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		channelSpinner.setAdapter(mChannelAdapter);
	*/ 	
		bandwidthSpinner = (Spinner) findViewById(R.id.WiFi_Rx_Bandwidth_Spinner);
		m_adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, mBandwidth);
		m_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		bandwidthSpinner.setAdapter(m_adapter);
	
		channelSpinner = (Spinner) findViewById(R.id.WiFi_Rx_Channel_Spinner);
	    mChannelAdapter = new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item, mChannelName);
	    mChannelAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);	
	    channelSpinner.setAdapter(mChannelAdapter);
	    
		Channel = fileparse.getValueByKey("[advance-wifi]", "RX_CHANNEL");
		Bandwidth = fileparse.getValueByKey("[advance-wifi]", "RX_BANDWIDTH");
		
		int ChannelPosition = StrToId(mChannelName, Channel);
		if  (-1 == ChannelPosition)
		{
			ChannelPosition = 0;
		}
		int BandPosition = StrToId(mBandwidth, Bandwidth);
		if (-1 == BandPosition)
		{
			BandPosition = 0;
		}
		channelSpinner.setSelection(ChannelPosition);
		bandwidthSpinner.setSelection(BandPosition);
		iRxChannelIdx = ChannelPosition;
		iRxBandWidthIdx = BandPosition;

		
		buttonStart.setOnClickListener(new Button.OnClickListener() {

					public void onClick(View arg0) {
//						EnableWifi();
						
						buttonStop.setEnabled(true);
						channelSpinner.setEnabled(false);
						buttonStart.setEnabled(false);
						
						textViewFCSError.setText(TEXT_ZERO);
		                textViewRxOK.setText(TEXT_ZERO);
		                textViewPER.setText("0.00%");
		                
		                boolean bRet = false;
//		                showDialog(DIALOG_WAITING_FOR_WIFI_RX_START);
		                
		                bRet = mWiFiHWTest.WiFi_RxStart(iRxChannelIdx, iRxBandWidthIdx);
		                if (true == bRet)
		                {
		                	mWiFiHWTest.bIsRxStart = true;		                	
		                }
		                else
		                {
		                	Log.e(TAG, "WiFi_RxStart Error!");
		                }
//		                removeDialog(DIALOG_WAITING_FOR_WIFI_RX_START);
		                
						mHandler.sendEmptyMessage(HANDLER_EVENT_RX);
						
		                /*
		                try {
		        			Process p = Runtime.getRuntime().exec("wifitest -r -n 10 -c 1 > /data/1.txt");
		        			Log.d(TAG, "wifitest -r -n 10 -c 3 > /data/1.txt");
		        		} catch (IOException e1) {
		        			// TODO Auto-generated catch block
		        			e1.printStackTrace();
		        		}
		                */
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
				buttonStop.setEnabled(false);
				buttonStart.setEnabled(true);
				channelSpinner.setEnabled(true);
			
				boolean bRet = false; 
				
		        mHandler.removeMessages(HANDLER_EVENT_RX);
		        
		        for (int i = 0; i < WAIT_COUNT; i++) {
		        	bRet = mWiFiHWTest.WiFi_RxStop();
		            if (bRet == true) {
		            	mWiFiHWTest.bIsRxStart = false;
		                break;
		            } else {
		                SystemClock.sleep(WAIT_COUNT);
		                Log.w(TAG, "stop Rx test failed at the " + i + "times try");
		            }
		        }
		        
//		        DisableWiFi();
			}

		});
		
		channelSpinner.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {

			public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2, long arg3)
			{
				Log.d("Advanced_WiFi_Rx", "onItem Selected");
//				editTextFCSError.setText("你的血型是：" + m_Countries[arg2]);
//				Toast.makeText(Advanced_WiFi_Rx.this, mChannelName[arg2], 5).show();
				//设置显示当前选择的项
				fileparse.updateKeyValue("[advance-wifi]", "RX_CHANNEL", mChannelName[arg2]);
				
				iRxChannelIdx = arg2;
				
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
				Log.d("Advanced_WiFi_Rx", "onItem Selected");
//				editTextFCSError.setText("你的血型是：" + m_Countries[arg2]);
//				Toast.makeText(Advanced_WiFi_Rx.this, mBandwidth[arg2], 5).show();
				fileparse.updateKeyValue("[advance-wifi]", "RX_BANDWIDTH", mBandwidth[arg2]);
				iRxBandWidthIdx = arg2;
/*				
				mBandwidthIndex = arg2 < mBandwidth.length ? arg2 : mBandwidthIndex;
				if (BANDWIDTH_INDEX_40 == mBandwidthIndex)
				{
					 mChannelAdapter
		                     .remove(mChannel[CHANNEL_0]);
		             mChannelAdapter
		                     .remove(mChannel[CHANNEL_1]);
		             mChannelAdapter
		                     .remove(mChannel[CHANNEL_11]);
		             mChannelAdapter
		                     .remove(mChannel[CHANNEL_12]);
		       //      updateWifiChannel();
		        } 
				else 
		        {
		             boolean bUpdate = false;
		             if (-1 == mChannelAdapter
		                     .getPosition(mChannel[CHANNEL_0])) {
		                 mChannelAdapter.insert(
		                		 mChannel[CHANNEL_0], CHANNEL_0);
		                 bUpdate = true;
		             }
		             if (-1 == mChannelAdapter
		                     .getPosition(mChannel[CHANNEL_1])) {
		                 mChannelAdapter.insert(
		                		 mChannel[CHANNEL_1], CHANNEL_1);
		                 bUpdate = true;
		             }
		             if (mChannelInfo.isContains(CHANNEL_12)
		                     && -1 == mChannelAdapter
		                             .getPosition(mChannel[CHANNEL_11])) {
		                 mChannelAdapter.insert(
		                		 mChannel[CHANNEL_11], CHANNEL_11);
		                 bUpdate = true;
		             }
		             if (mChannelInfo.isContains(CHANNEL_13)
		                     && -1 == mChannelAdapter
		                             .getPosition(mChannel[CHANNEL_12])) {
		                 mChannelAdapter.insert(
		                		 mChannel[CHANNEL_12], CHANNEL_12);
		                 bUpdate = true;
		             }
		             if (bUpdate) {
		            //     updateWifiChannel();
		             }
		         }
		         */
				//设置显示当前选择的项
				arg0.setVisibility(View.VISIBLE);
			}

			public void onNothingSelected(AdapterView<?> arg0)
			{
				// TODO Auto-generated method stub
			}

		});
	}
}

