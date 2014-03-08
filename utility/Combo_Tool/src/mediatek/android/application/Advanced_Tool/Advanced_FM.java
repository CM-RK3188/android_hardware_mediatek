package mediatek.android.application.Advanced_Tool;

import android.app.TabActivity;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.view.Menu;
import android.widget.TabHost;
import android.widget.Toast;
import android.widget.TabHost.OnTabChangeListener;

public class Advanced_FM extends TabActivity implements
OnTabChangeListener{
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
		Intent iAdvacedFM_RX = new Intent(this,
				Advanced_FM_Radio.class);
//		Intent iAdvacedFM_TX = new Intent(this,
//				Advanced_FM_Transmitter.class);
		
//		iAdvacedFM_TX.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		iAdvacedFM_RX.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		
		myTabhost.addTab(myTabhost.newTabSpec("Radio").setIndicator("Radio")
						.setContent(iAdvacedFM_RX));
//		myTabhost.addTab(myTabhost.newTabSpec("Transmitter").setIndicator("Transmitter")
//						.setContent(iAdvacedFM_TX));
		
		int height = myTabhost.getTabWidget().getChildAt(0).getLayoutParams().height;
		myTabhost.getTabWidget().getChildAt(0).getLayoutParams().height = height * 2 / 3;
//		myTabhost.getTabWidget().getChildAt(1).getLayoutParams().height = height / 2;
		
//		setContentView(myTabhost);
			
//		myTabhost.setOnTabChangedListener(this);
	}
	public void onTabChanged(String arg0) {
		// TODO Auto-generated method stub
		
	}
}
