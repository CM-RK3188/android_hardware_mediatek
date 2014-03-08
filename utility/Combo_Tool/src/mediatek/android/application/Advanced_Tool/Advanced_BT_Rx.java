package mediatek.android.application.Advanced_Tool;

import java.io.IOException;

import mediatek.android.application.R;
import mediatek.android.application.FileParse;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
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


public class Advanced_BT_Rx extends Activity {
	
private static final String TAG = "Advanced_BT_Rx";
    
    public static final int OP_IN_PROCESS = 8;
    public static final int OP_FINISH = 9;
    public static final int OP_RX_FAIL = 10;
    public static final int OP_ADDR_DEFAULT = 11;
    public static final int OP_TEST_OK_STEP1 = 12;
    public static final int OP_TEST_OK_STEP2 = 13;
    
    private static final int CHECK_BT_STATE = 20;
    private static final int DIALOG_RX_FAIL = 21;
    private static final int DIALOG_RX_TEST = 22;
    private static final int CHECK_BT_DEVEICE = 23;
    
    private static final int TEST_STATUS_BEGIN = 100;
    private static final int TEST_STATUS_RESULT = 101;
    
    private BluetoothAdapter mBtAdapter;
    
    private Spinner mPattern;
    private Spinner mPocketType;
    private EditText mEdFreq;
    private EditText mEdAddr;

    private Button mBtnStartTest;
    private Button mBtnStopTest;

    private TextView mPackCnt;
    private TextView mPackErrRate;
    private TextView mRxByteCnt;
    private TextView mBitErrRate;
	
	private ArrayAdapter<String>	m_adapter;
	
	FileParse fileparse = new FileParse("ComboTool.ini");
	
	private String Pattern;
	private String PacketType;
	private String Frequency;
	private String Tester_Address;
	
	private int[] mResult = null; //
    private int mTestStatus = TEST_STATUS_BEGIN;
    private int mStateBt;

    // used to record weather the button clicked action
    private boolean mDoneFinished = true;

    private static Handler sWorkHandler = null; // used to handle the
    
//    private static Handler mUiHandler = null;
    private EMBt mEMBt = new EMBt();
    
	final  String[] mRxPattern = {
            "0000", "1111",
            "1010", "pseudo random bit",
            "11110000",};
	
	final  String[] mRxPacketType = {
            "DM1(0x03)", "DH1(0x04)",
            "DM3(0x0A)", "DH3(0x0B)",
            "DM5(0x0E)", "DH5(0x0F)",
            "2-DH1(0x24)", "3-DH1(0x28)",
            "2-DH3(0x2A)", "3-DH3(0x2B)",
            "2-DH5(0x2E)", "3-DH5(0x2F)",};
    
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

    @Override
    protected Dialog onCreateDialog(int id) {
        if (id == CHECK_BT_STATE) {
            AlertDialog dialog = new AlertDialog.Builder(this)
                .setCancelable(false)
                .setTitle(R.string.Error)
                .setMessage(R.string.BT_turn_bt_off) // put in strings.xml
                .setPositiveButton(R.string.OK,
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog,
                                int which) {
                            finish();
                        }
                    }).create();
            return dialog;
        } else if (id == CHECK_BT_DEVEICE) {
            // "Error").setMessage("Can't find any bluetooth device") // put in
            // strings.xml
            AlertDialog dialog = new AlertDialog.Builder(this)
                .setCancelable(false)
                .setTitle(R.string.Error)
                .setMessage(R.string.BT_no_dev) // put in strings.xml
                .setPositiveButton(R.string.OK,
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog,
                                int which) {
                            finish();
                        }
                    }).create();
            return dialog;
        } else if (id == DIALOG_RX_FAIL) {
            // "Error").setMessage("Can't find any bluetooth device") // put in
            // strings.xml
            AlertDialog dialog = new AlertDialog.Builder(this).setCancelable(
                    false).setTitle(R.string.Error)
                    .setMessage(R.string.BT_no_sig_rx_fail) // put in strings.xml
                    .setPositiveButton(R.string.OK,
                            new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog,
                                        int which) {
                                    // finish();
                                }
                            }).create();
            return dialog;
        } else if (id == DIALOG_RX_TEST) {
            ProgressDialog dialog = new ProgressDialog(Advanced_BT_Rx.this);
            if (null != dialog) {
                dialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
//                String text = getString(R.string.BT_tx_test);
                dialog.setMessage(getString(R.string.BT_tx_test));
                dialog.setTitle(R.string.BTRxTitle);
                dialog.setCancelable(false);
                // dialog.show();
            } else {
                Log.w(TAG, "new mDialogSearchProgress failed");
            }
            return dialog;
        }
        return null;
    }
	
    @Override
	public void onPause() {
		// TODO Auto-generated method stub
    	if (true == mEMBt.mBTIsInit)
    	{
    		getResult();
    		setViewEnabled(true);
    		mEMBt.mBTIsInit = false;
    	}
		super.onPause();
	}

	private Handler mUiHandler = new Handler() {
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case OP_IN_PROCESS: 
                Log.w(TAG, "OP_IN_PROCESS");
//                showDialog(DIALOG_RX_TEST);            
                break;
            case OP_FINISH:
                Log.w(TAG, "OP_FINISH");
//                dismissDialog(DIALOG_RX_TEST);
//                finish();
                break;
            case OP_RX_FAIL:
                Log.w(TAG, "OP_RX_FAIL");
//                showDialog(DIALOG_RX_FAIL);
//                finish();
                break;
            case OP_ADDR_DEFAULT:
                mEdAddr.setText("A5F0C3");
                break;
            case OP_TEST_OK_STEP1:
                mTestStatus = TEST_STATUS_RESULT;
                setViewEnabled(false);
                break;
            case OP_TEST_OK_STEP2:
                mPackCnt.setText(String.valueOf(mResult[0]));
                mPackErrRate.setText(String.format("%.3f",
                        mResult[1] / 100000.0)
                        + "%");
                mRxByteCnt.setText(String.valueOf(mResult[2]));
                mBitErrRate.setText(String.format("%.3f",
                        mResult[3] / 100000.0)
                        + "%");
                mTestStatus = TEST_STATUS_BEGIN;
                mBtnStartTest.setText("Start");
                break;
            default:
                break;
            }
        }
    };
	
    private class WorkRunnable implements Runnable {
        // public Handler mHandler;

        public void run() {
            mUiHandler.sendEmptyMessage(OP_IN_PROCESS);
            mDoneFinished = false;
            doSendCommandAction();
            mDoneFinished = true;
            mUiHandler.sendEmptyMessage(OP_FINISH);
        }
    }
    
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		View contentView = LayoutInflater.from(this.getParent()).inflate(R.layout.advanced_bt_rx, null);
		setContentView(contentView);
		
		mPackCnt = (TextView) findViewById(R.id.BT_Rx_Packet_Count_Content);
		mPackErrRate = (TextView) findViewById(R.id.BT_Rx_Pacekt_Error_Rate_Content);
		mRxByteCnt = (TextView) findViewById(R.id.BT_Rx_Byte_Count_Content);
		mBitErrRate = (TextView) findViewById(R.id.BT_Rx_Bit_Error_Rate_Content);
		mEdFreq = (EditText)findViewById(R.id.BT_Rx_Frequency_Edit);
		mEdAddr = (EditText)findViewById(R.id.BT_Rx_Tester_Address_Edit);
		mBtnStartTest = (Button)findViewById(R.id.BT_Rx_Start);
		mBtnStopTest = (Button)findViewById(R.id.BT_Rx_Stop);
		
		mPattern = (Spinner) findViewById(R.id.BT_Rx_Pattern_Spinner);
		mPocketType = (Spinner) findViewById(R.id.BT_Rx_Packet_Type_Spinner);
		
		m_adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, mRxPattern);
		m_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		mPattern.setAdapter(m_adapter);
		
		m_adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, mRxPacketType);
		m_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		mPocketType.setAdapter(m_adapter);
		
		Frequency = fileparse.getValueByKey("[advance-bt]", "RX_FREQUENCY");
		Tester_Address = fileparse.getValueByKey("[advance-bt]", "RX_TESTER_ADDRESS");
		Pattern = fileparse.getValueByKey("[advance-bt]", "RX_PATTERN");
		PacketType = fileparse.getValueByKey("[advance-bt]", "RX_PACKET_TYPE");
		
		mEdFreq.setText(Frequency);
		mEdAddr.setText(Tester_Address);
		setViewEnabled(true);
		
		int PatternPosition = StrToId(mRxPattern, Pattern);
		if (-1 == PatternPosition)
		{
			PatternPosition = 0;
		}
		int PacketTypePosition = StrToId(mRxPacketType, PacketType);
		if (-1 == PacketTypePosition)
		{
			PacketTypePosition = 0;
		}
		mPattern.setSelection(PatternPosition);
		mPocketType.setSelection(PacketTypePosition);
		
		HandlerThread mHandlerThread = new HandlerThread("doneHandler");
        mHandlerThread.start();

        sWorkHandler = new Handler(mHandlerThread.getLooper());
        
        getBtState();
        
		mBtnStartTest.setOnClickListener(new Button.OnClickListener() {

			public void onClick(View arg0) {
				Frequency = mEdFreq.getText().toString();
				Tester_Address = mEdAddr.getText().toString();
				
				try {
					int iFreq = Integer.parseInt(Frequency);
					
					if (iFreq < 0 || iFreq > 78)
					{
						Toast
						.makeText(
								getApplicationContext(),
								"Frequency is out of range[0-78] \n  Use default 60",
								Toast.LENGTH_LONG).show();
								Frequency = "60";
								mEdFreq.setText(Frequency);
								fileparse.updateKeyValue("[advance-bt]", "RX_FREQUENCY", Frequency);
					}
				} catch (Exception e) {
					Toast
							.makeText(
									getApplicationContext(),
									"Frequency is invalid ! \n  Use default 60",
									Toast.LENGTH_LONG).show();
					Frequency = "60";
					mEdFreq.setText(Frequency);
					fileparse.updateKeyValue("[advance-bt]", "RX_FREQUENCY", Frequency);

				}
				
				fileparse.updateKeyValue("[advance-bt]", "RX_FREQUENCY", Frequency);
				fileparse.updateKeyValue("[advance-bt]", "RX_TESTER_ADDRESS", Tester_Address);

				try {
					fileparse.updateConFile("ComboTool.ini");
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				
				if (mDoneFinished) {
		            // if the last click action has been handled, send another event
		            // request
//		            mPackCnt.setText(".");
//		            mPackErrRate.setText(".");
//		             mRxByteCnt.setText(".");
//		             mBitErrRate.setText(".");
		            sWorkHandler.post(new WorkRunnable());
		        } else {
		            Log.w(TAG, "last click is not finished yet."); // log
		        }
				
			}
		});
		
		mBtnStopTest.setOnClickListener(new Button.OnClickListener() {

			public void onClick(View arg0) {
				getResult();
				setViewEnabled(true);
				mEMBt.mBTIsInit = false;
			}
		});
		
		mPattern.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {

			public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2, long arg3)
			{
				fileparse.updateKeyValue("[advance-bt]", "RX_PATTERN", mRxPattern[arg2]);
				//设置显示当前选择的项
				arg0.setVisibility(View.VISIBLE);
			}

			public void onNothingSelected(AdapterView<?> arg0)
			{
				// TODO Auto-generated method stub
			}

		});
		
		mPocketType.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {

			public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2, long arg3)
			{
				fileparse.updateKeyValue("[advance-bt]", "RX_PACKET_TYPE", mRxPacketType[arg2]);
				//设置显示当前选择的项
				arg0.setVisibility(View.VISIBLE);
			}

			public void onNothingSelected(AdapterView<?> arg0)
			{
				// TODO Auto-generated method stub
			}

		});
	}
	
	private void setViewEnabled(boolean state) 
	{
		mEdFreq.setEnabled(state);
		mEdAddr.setEnabled(state);
		mBtnStartTest.setEnabled(state);
		mBtnStopTest.setEnabled(!state);		
		mPattern.setEnabled(state);
		mPocketType.setEnabled(state);
	}
	public void onClick(View arg0) {
        if (mDoneFinished) {
            // if the last click action has been handled, send another event
            // request
//            mPackCnt.setText(".");
//            mPackErrRate.setText(".");
//             mRxByteCnt.setText(".");
//             mBitErrRate.setText(".");
            sWorkHandler.post(new WorkRunnable());
        } else {
            Log.w(TAG, "last click is not finished yet."); // log
        }
    }

    /**
     * Send command the user has made, and finish the activity.
     */
    private boolean doSendCommandAction() {
        if (mTestStatus == TEST_STATUS_BEGIN) {
            enableBluetooth(false);
            getValuesAndSend();
        } else if (mTestStatus == TEST_STATUS_RESULT) {
            getResult();
        }

        return true;
    }

    // implemented for DialogInterface.OnCancelListener
    public void onCancel(DialogInterface dialog) {
        // request that the service stop the query with this callback object.
        Log.v(TAG, "-->onCancel");
        finish();
    }

    @Override
    protected void onStart() {
        Log.v(TAG, "-->onStart");
        super.onStart();
        if (mBtAdapter == null) {
            mBtAdapter = BluetoothAdapter.getDefaultAdapter();
        }
        if (mBtAdapter != null) {
            if (mBtAdapter.getState() != BluetoothAdapter.STATE_OFF) {
            	
            }
        } else {
 //           showDialog(CHECK_BT_DEVEICE);
        }
    }

    @Override
    protected void onStop() {
        Log.v(TAG, "before handlerThread quit.");
//        mHandlerThread.quit();
//        mHandlerThread = null;
        Log.v(TAG, "after  handlerThread quit.");
        super.onStop();
        finish();
    }

    @Override
    protected void onDestroy() {
    	
    	Log.v(TAG, "NoSignalRx onDestroy.");
    	if (mEMBt != null) {
            // if (-1 == mEMBt.doBtTest(3)) {
            if (-1 == mEMBt.Bt_deinit()) 
            {
                Log.i(TAG, "stop failed.");

            }
        } else {
            Log.i(TAG, "BtTest does not start yet.");
        }

    	BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
        btAdapter.disable();
        super.onDestroy();

    }

    private void getBtState() {
        Log.v(TAG, "Enter GetBtState().");
        BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
        if (null == btAdapter) {
            Log.v(TAG, "we can not find a bluetooth adapter.");
            // Toast.makeText(getApplicationContext(),
            // "We can not find a bluetooth adapter.", Toast.LENGTH_SHORT)
            // .show();
            mUiHandler.sendEmptyMessage(OP_RX_FAIL);
            return;
        }
        mStateBt = btAdapter.getState();
       
        Log.v(TAG, "Leave GetBtState().");
    }

    private void enableBluetooth(boolean enable) {
        Log.v(TAG, "Enter EnableBluetooth().");
        BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
        if (null == btAdapter) {
            Log.v(TAG, "we can not find a bluetooth adapter.");
            return;
        }
        // need to enable
        if (enable) {
            Log.v(TAG, "Bluetooth is enabled");
            btAdapter.enable();
        } else {
            // need to disable
            Log.v(TAG, "Bluetooth is disabled");
            btAdapter.disable();
        }
        Log.v(TAG, "Leave EnableBluetooth().");
    }

    public void getValuesAndSend() {
        Log.v(TAG, "Enter GetValuesAndSend().");
       
        if (mEMBt == null) {
            Log.v(TAG, "We cannot find BtTest object.");
            return;
        }
        int nPatternIdx = mPattern.getSelectedItemPosition();
        int nPocketTypeIdx = mPocketType.getSelectedItemPosition();
        int nFreq = 0;
        int nAddress = 0;
        try {
            nFreq = Integer.valueOf(mEdFreq.getText().toString());
            nAddress = Integer.valueOf(mEdAddr.getText().toString(), 16);
            if (nFreq < 0 || nFreq > 78) {
                // Toast.makeText(getApplicationContext(),
                // "Error: Frequency error, must be 0-78.",
                // Toast.LENGTH_SHORT).show();
                mUiHandler.sendEmptyMessage(OP_RX_FAIL);
                return;
            }
            if (nAddress == 0) {
                nAddress = 0xA5F0C3;
                mUiHandler.sendEmptyMessage(OP_ADDR_DEFAULT);
            }
        } catch (NumberFormatException e) {
            Log.i(TAG, "input number error!");
            return;
        }

        // send command to....
        boolean rc = mEMBt.noSigRxTestStart(nPatternIdx, nPocketTypeIdx,
                nFreq, nAddress);
        if (!rc) {
            Log.i(TAG, "no signal rx test failed.");
            if ((BluetoothAdapter.STATE_TURNING_ON == mStateBt)
                    || (BluetoothAdapter.STATE_ON == mStateBt)) {
                enableBluetooth(true);
            }
            // Toast.makeText(getApplicationContext(),
            // "no signal rx test failed.",
            // Toast.LENGTH_SHORT).show();
            // showDialog(DIALOG_RX_FAIL);
            mUiHandler.sendEmptyMessage(OP_RX_FAIL);
        } else {
            mUiHandler.sendEmptyMessage(OP_TEST_OK_STEP1);
            mEMBt.mBTIsInit = true;
        }

        Log.i(TAG, "Leave GetValuesAndSend().");
    }

    private void getResult() {
        if (mEMBt == null) {
            return;
        }

        mResult = mEMBt.noSigRxTestResult();
        if (mResult == null) {
            Log.i(TAG, "no signal rx test failed.");
            if ((BluetoothAdapter.STATE_TURNING_ON == mStateBt)
                    || (BluetoothAdapter.STATE_ON == mStateBt)) {
                enableBluetooth(true);
            }
            // Toast.makeText(getApplicationContext(),
            // "no signal rx test failed.",
            // Toast.LENGTH_SHORT).show();
            // showDialog(DIALOG_RX_FAIL);
            mUiHandler.sendEmptyMessage(OP_RX_FAIL);

        } else {
            mUiHandler.sendEmptyMessage(OP_TEST_OK_STEP2);

        }

        Log.i(TAG, "Leave getresult().");
    }

}
