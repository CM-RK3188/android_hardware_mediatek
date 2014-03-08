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

public class Advanced_BT_Tx extends Activity{
	private BluetoothAdapter mAdapter;

    private Spinner mPattern = null;
    private Spinner mChannels = null;
    private Spinner mPktTypes = null;
    
	private Button buttonStart;
	private Button buttonStop;
	
	private EditText editTextTxFreq;
	private EditText editTextTxPktLength;
	
	private String PacketLength;
	private String Frequency;
	private String Channel;
	private String Pattern;
	private String PacketType;
	
	private int iPacketLen = 0;
	private int iFreq = 0;
	private int iPatternIdx = 0;
	private int iChannelIdx = 0;
	private int iPacketTypeIdx = 0;
	
	private EMBt mEMBt = new EMBt();
	
    private static final int BT_TEST_0 = 0;
    private static final int BT_TEST_3 = 3;
    private static final int RETURN_FAIL = -1;

    // dialog ID and MSG ID
    private static final int CHECK_BT_STATE = 1;
    private static final int CHECK_BT_DEVEICE = 2;
    private static final int TEST_TX = 3;

    private static final String TAG = "Advanced_BT_Tx";
	
	private ArrayAdapter<String>	m_adapter;
	FileParse fileparse = new FileParse("ComboTool.ini");
	
	final  String[] mTxPattern = {
            "0000", "1111",
            "1010", "pseudo random bit",
            "11110000",};
	
	final  String[] mTxChannel = {
            "Single Frequency", "79 Channel",};
	
	final  String[] mTxPacketType = {
            "NULL(0x00)", "POLL(0x01)",
            "FHS(0x02)", "DM1(0x03)",
            "DH1(0x04)", "HV1(0x05)",
            "HV2(0x06)", "HV3(0x07)",
            "DV(0x08)", "AUX(0x09)",
            "DM3(0x0A)", "DH3(0x0B)",
            "DM5(0x0E)", "DH5(0x0F)",
            "EV3(0x17)", "EV4(0x1C)",
            "EV5(0x1D)", "2-DH1(0x24)",
            "3-DH1(0x28)", "2-DH3(0x2A)",
            "3-DH3(0x2B)", "2-DH5(0x2E)",
            "3-DH5(0x2F)", "2-EV3(0x36)",
            "3-EV3(0x37)", "2-EV5(0x3C)",
            "3-EV5(0x3D)", "non-modulated",
            };

	private int mStateBt;
	
	    
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
	public void onPause() {
		// TODO Auto-generated method stub
		if (true == mEMBt.mBTIsInit)
		{
			mEMBt.TxOnlyTestEnd();
			setViewEnabled(true);
			mEMBt.mBTIsInit = false;
		}
		super.onPause();
	}
	
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		View contentView = LayoutInflater.from(this.getParent()).inflate(R.layout.advanced_bt_tx, null);
		setContentView(contentView);
		
		buttonStart = (Button)findViewById(R.id.BT_Tx_Start); 
		buttonStop = (Button)findViewById(R.id.BT_Tx_Stop); 
		
		mPattern = (Spinner) findViewById(R.id.BT_Tx_Pattern_Spinner);
		mChannels = (Spinner) findViewById(R.id.BT_Tx_Channel_Spinner);
		mPktTypes = (Spinner) findViewById(R.id.BT_Tx_Packet_Type_Spinner);
		editTextTxFreq = (EditText)findViewById(R.id.BT_Tx_Frequency_Edit);
		editTextTxPktLength = (EditText)findViewById(R.id.BT_Tx_Packet_Length_Edit);
		
		PacketLength = fileparse.getValueByKey("[advance-bt]", "TX_PACKET_LENGTH");
		Frequency = fileparse.getValueByKey("[advance-bt]", "TX_FREQUENCY");		
		editTextTxPktLength.setText(PacketLength);
		editTextTxFreq.setText(Frequency);

		
		m_adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, mTxPattern);
		m_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		mPattern.setAdapter(m_adapter);
		
		m_adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, mTxChannel);
		m_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		mChannels.setAdapter(m_adapter);
		
		m_adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, mTxPacketType);
		m_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		mPktTypes.setAdapter(m_adapter);
		
		Pattern = fileparse.getValueByKey("[advance-bt]", "TX_PATTERN");
		Channel = fileparse.getValueByKey("[advance-bt]", "TX_CHANNEL");
		PacketType = fileparse.getValueByKey("[advance-bt]", "TX_PACKET_TYPE");
		
		int PatternPosition = StrToId(mTxPattern, Pattern);
		if (-1 == PatternPosition)
		{
			PatternPosition = 0;
		}
		int ChannelPosition = StrToId(mTxChannel, Channel);
		if (-1 == ChannelPosition)
		{
			ChannelPosition = 0;
		}
		int PacketTypePosition = StrToId(mTxPacketType, PacketType);
		if (-1 == PacketTypePosition)
		{
			PacketTypePosition = 0;
		}
		mPattern.setSelection(PatternPosition);
		mChannels.setSelection(ChannelPosition);
		mPktTypes.setSelection(PacketTypePosition);
		iPatternIdx = PatternPosition;
		iChannelIdx = ChannelPosition;
		iPacketTypeIdx = PacketTypePosition;
		setViewEnabled(true);
		getBtState();
		
		buttonStart.setOnClickListener(new Button.OnClickListener() {

			public void onClick(View arg0) {
				
				boolean bResult = false;
				PacketLength = editTextTxPktLength.getText().toString();
				Frequency = editTextTxFreq.getText().toString();
				
				try {
					Integer.parseInt(PacketLength);
				} catch (Exception e) {
					Toast
							.makeText(
									getApplicationContext(),
									"Packet Length is invalid ! \n  Use default 100",
									Toast.LENGTH_LONG).show();
					PacketLength = "100";
					editTextTxPktLength.setText(PacketLength);
					fileparse.updateKeyValue("[advance-bt]", "TX_PACKET_LENGTH", PacketLength);

				}
				
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
								editTextTxFreq.setText(Frequency);
								fileparse.updateKeyValue("[advance-bt]", "TX_FREQUENCY", Frequency);
					}
				} catch (Exception e) {
					Toast
							.makeText(
									getApplicationContext(),
									"Frequency is invalid ! \n  Use default 60",
									Toast.LENGTH_LONG).show();
					Frequency = "60";
					editTextTxFreq.setText(Frequency);
					fileparse.updateKeyValue("[advance-bt]", "TX_FREQUENCY", Frequency);

				}
				
				fileparse.updateKeyValue("[advance-bt]", "TX_PACKET_LENGTH", PacketLength);
				fileparse.updateKeyValue("[advance-bt]", "TX_FREQUENCY", Frequency);
				
				try {
					fileparse.updateConFile("ComboTool.ini");
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				setViewEnabled(false);
			
//				enableBluetooth(true);
				getValues();
				
				bResult = mEMBt.TxOnlyTestStart(iPatternIdx, iChannelIdx, iFreq, iPacketTypeIdx, iPacketLen);
				if (bResult == false)
				{
					Log.i(TAG, "TxOnlyTest Failed.");
//					enableBluetooth(false);
				}
				else
				{
					mEMBt.mBTIsInit = true;
				}
	
			}
		});
		
		buttonStop.setOnClickListener(new Button.OnClickListener() {

			public void onClick(View arg0) {
				mEMBt.TxOnlyTestEnd();
				setViewEnabled(true);
				mEMBt.mBTIsInit = false;
			}
		});
		
		mPattern.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {

			public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2, long arg3)
			{
				fileparse.updateKeyValue("[advance-bt]", "TX_PATTERN", mTxPattern[arg2]);
				//设置显示当前选择的项
				iPatternIdx = mPattern.getSelectedItemPosition();
				arg0.setVisibility(View.VISIBLE);
			}

			public void onNothingSelected(AdapterView<?> arg0)
			{
				// TODO Auto-generated method stub
			}

		});
		
		mChannels.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {

			public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2, long arg3)
			{
				fileparse.updateKeyValue("[advance-bt]", "TX_CHANNEL", mTxChannel[arg2]);
				//设置显示当前选择的项
				iChannelIdx = mChannels.getSelectedItemPosition();
				arg0.setVisibility(View.VISIBLE);
			}

			public void onNothingSelected(AdapterView<?> arg0)
			{
				// TODO Auto-generated method stub
			}

		});
		
		mPktTypes.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {

			public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2, long arg3)
			{
				fileparse.updateKeyValue("[advance-bt]", "TX_PACKET_TYPE", mTxPacketType[arg2]);
				//设置显示当前选择的项
				iPacketTypeIdx = mPktTypes.getSelectedItemPosition();	
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
		editTextTxFreq.setEnabled(state);
		mPktTypes.setEnabled(state);
		mChannels.setEnabled(state);
		mPattern.setEnabled(state);		
		buttonStop.setEnabled(!state);
		buttonStart.setEnabled(state);
		editTextTxPktLength.setEnabled(state);
	}
	
    // implemented for DialogInterface.OnCancelListener
    public void onCancel(DialogInterface dialog) {
        // request that the service stop the query with this callback
        // mEMBtect.
        Log.v(TAG, "-->onCancel");
        finish();
    }

    @Override
    protected void onStart() {
        Log.v(TAG, "-->onStart");
        super.onStart();
        if (mAdapter == null) {
            mAdapter = BluetoothAdapter.getDefaultAdapter();
        }
        if (mAdapter != null) {
            if (mAdapter.getState() != BluetoothAdapter.STATE_OFF) {
//                showDialog(CHECK_BT_STATE);
            }
        } else {
//            showDialog(CHECK_BT_DEVEICE);
        }
    }

    // @Override
    // protected void onStop() {
    // Log.v(TAG, "before handlerThread quit.");
    // // if (mHandlerThread != null) {
    // // mHandlerThread.quit();
    // // mHandlerThread = null;
    // // }
    //
    // Log.v(TAG, "after  handlerThread quit.");
    // super.onStop();
    // // finish();
    // }

    @Override
    protected void onDestroy() {
        Log.v(TAG, "TXOnlyTest onDestroy.");

 //       removeDialog(TEST_TX);
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
        // mDialogSearchProgress = null;

    }

   

    private void getBtState() {
        Log.v(TAG, "Enter GetBtState().");
        BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
        if (null == btAdapter) {
            Log.i(TAG, "we can not find a bluetooth adapter.");
            // Toast.makeText(getApplicationContext(),
            // "We can not find a bluetooth adapter.", Toast.LENGTH_SHORT)
            // .show();
            finish();
            return;
        }
        mStateBt = btAdapter.getState();
        
        Log.i(TAG, "Leave GetBtState().");
    }

    private void enableBluetooth(boolean enable) {
        Log.v(TAG, "Enter EnableBluetooth().");
        BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
        if (null == btAdapter) {
            Log.i(TAG, "we can not find a bluetooth adapter.");
            return;
        }
        // need to enable
        if (enable) {
            Log.i(TAG, "Bluetooth is enabled");
            btAdapter.enable();
        } else {
            // need to disable
            Log.i(TAG, "Bluetooth is disabled");
            btAdapter.disable();
        }
        Log.i(TAG, "Leave EnableBluetooth().");
    }

    /**
     * Revert any changes the user has made, and finish the activity.
     */
    private boolean doRevertAction() {
        finish();
        return true;
    }

    // private BtTest mEMBt = null;

    public void getValues() {
        Log.i(TAG, "Enter GetValuesAndSend().");
       
    	iPacketLen = Integer.parseInt(editTextTxPktLength.getText().toString());
    	iFreq = Integer.parseInt(editTextTxFreq.getText().toString());    	
    	
        Log.i(TAG, "Leave getValuesAndSend().");
    }
}
