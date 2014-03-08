package mediatek.android.application.Advanced_Tool;

import mediatek.android.application.R;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class Advanced_BT_Testmode extends Activity{

	private EditText mEdPowerLevel = null;
    private Button mButEnabled = null;
    private Button mButDisabled = null;
	
    private String TAG = "Advanced_BT_Testmode";
    private int iLevel = 0;
    EMBt mEMBt  = new EMBt();
    
	private int mStateBt;
    
	@Override
	public void onPause() {
		// TODO Auto-generated method stub
    	if (true == mEMBt.mBTIsInit)
    	{
    		mEMBt.TestModeDisable();
    		setViewEnabled(true);
    		mEMBt.mBTIsInit = false;
    	}
		super.onPause();
	}
	
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		View contentView = LayoutInflater.from(this.getParent()).inflate(R.layout.advanced_bt_testmode, null);
		setContentView(contentView);
		
		mEdPowerLevel = (EditText)findViewById(R.id.BT_TestMode_PowerLevel_Edit);
		mButEnabled = (Button)findViewById(R.id.BT_TestMode_Enable);
		mButDisabled = (Button)findViewById(R.id.BT_TestMode_Disable);
		setViewEnabled(true);

		getBtState();
	
		
		mButEnabled.setOnClickListener(new Button.OnClickListener() {

			public void onClick(View arg0) {
				
				setViewEnabled(false);
				boolean bRst = true;
				
				iLevel = Integer.parseInt(mEdPowerLevel.getText().toString());
				if (iLevel < 0 || iLevel > 7)
				{
					Log.w(TAG, "Power Level input error! Use default Level 5 \n");
					iLevel = 7;
					mEdPowerLevel.setText('5');
				}
				
				bRst = mEMBt.TestModeEnable(iLevel);
				if (false == bRst)
				{
					Log.e(TAG, "Enable Test Mode Error!\n");
				}
				else
				{
					mEMBt.mBTIsInit = true;
				}
			}
		});
		
		mButDisabled.setOnClickListener(new Button.OnClickListener() {

			public void onClick(View arg0) {				
				setViewEnabled(true);
				boolean bRst = true;
				
				bRst = mEMBt.TestModeDisable();
				if (false == bRst)
				{
					Log.e(TAG, "Disable Test Mode Error!\n");
				}
				else
				{
					mEMBt.mBTIsInit = false;
				}
			}
		});
	}
	
	private void setViewEnabled(boolean state) 
	{
		mEdPowerLevel.setEnabled(state);
		mButEnabled.setEnabled(state);
		mButDisabled.setEnabled(!state);
	}
	
	private void getBtState(){
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
	
	protected void onDestroy() {
        Log.v(TAG, "Test mode onDestroy.");

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
}
