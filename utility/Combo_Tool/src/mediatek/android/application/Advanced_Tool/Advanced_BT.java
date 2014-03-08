package mediatek.android.application.Advanced_Tool;

import mediatek.android.application.R;
import android.app.TabActivity;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.view.Menu;
import android.widget.TabHost;
import android.widget.Toast;
import android.widget.TabHost.OnTabChangeListener;

public class Advanced_BT extends TabActivity implements
OnTabChangeListener{
	@Override
	public void onDestroy() {
		// TODO Auto-generated method stub
		BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
		if (btAdapter.getState() != BluetoothAdapter.STATE_OFF)
	    {
			btAdapter.disable();
	    }
		
		super.onDestroy();
	}

	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
	}
	
	private TabHost myTabhost;
	protected int myMenuSettingTag = 0;
	protected Menu myMenu;

	/*
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		myTabhost = this.getTabHost();
		// get Tabhost
		LayoutInflater.from(this).inflate(R.layout.advanced_bt,
				myTabhost.getTabContentView(), true);
		
		myTabhost.addTab(myTabhost.newTabSpec("RX").setIndicator("RX")
						.setContent(R.id.advance_bt_rx));
		myTabhost.addTab(myTabhost.newTabSpec("TX").setIndicator("TX")
						.setContent(R.id.advance_bt_tx));
		
		int height = myTabhost.getTabWidget().getChildAt(0).getLayoutParams().height;
		myTabhost.getTabWidget().getChildAt(0).getLayoutParams().height = height / 2;
		myTabhost.getTabWidget().getChildAt(1).getLayoutParams().height = height / 2;
		
		setContentView(myTabhost);
			
//		myTabhost.setOnTabChangedListener(this);
	}
	*/
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		myTabhost = this.getTabHost();
		// get Tabhost
		Intent iAdvacedBT_Rx = new Intent(this,
				Advanced_BT_Rx.class);
		Intent iAdvacedBT_Tx = new Intent(this,
				Advanced_BT_Tx.class);
		Intent iAdvacedBT_Testmode = new Intent(this,
				Advanced_BT_Testmode.class);
		
		iAdvacedBT_Tx.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		iAdvacedBT_Rx.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		iAdvacedBT_Testmode.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		
		myTabhost.addTab(myTabhost.newTabSpec("RX").setIndicator("RX")
						.setContent(iAdvacedBT_Rx));
		myTabhost.addTab(myTabhost.newTabSpec("TX").setIndicator("TX")
						.setContent(iAdvacedBT_Tx));
		myTabhost.addTab(myTabhost.newTabSpec("TestMode").setIndicator("TestMode")
				.setContent(iAdvacedBT_Testmode));
		
		int height = myTabhost.getTabWidget().getChildAt(0).getLayoutParams().height;
		myTabhost.getTabWidget().getChildAt(0).getLayoutParams().height = height * 2/3;
		myTabhost.getTabWidget().getChildAt(1).getLayoutParams().height = height * 2/3;
		myTabhost.getTabWidget().getChildAt(2).getLayoutParams().height = height * 2/3;
		
		BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
		if (btAdapter.getState() != BluetoothAdapter.STATE_OFF)
	    {
	       Toast.makeText(Advanced_BT.this, "Please turn off Bluetooth first", Toast.LENGTH_LONG).show();
	       finish();
	    }
//		setContentView(myTabhost);
			
//		myTabhost.setOnTabChangedListener(this);
	}
	public void onTabChanged(String arg0) {
		// TODO Auto-generated method stub
		
	}
}
