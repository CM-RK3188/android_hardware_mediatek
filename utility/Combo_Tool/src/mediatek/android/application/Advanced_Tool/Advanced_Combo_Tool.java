package mediatek.android.application.Advanced_Tool;

import mediatek.android.application.FileParse;
import mediatek.android.application.R;
import mediatek.android.application.Advanced_Tool.YGPS.YgpsActivity;
import mediatek.android.application.Basic_Tool.Basic_Combo_Tool;
import mediatek.android.application.Basic_Tool.Basic_WiFi_Connect_Service;
import mediatek.android.application.Basic_Tool.Basic_WiFi_Scan_Service;
import mediatek.android.application.Basic_Tool.Basic_Combo_Tool.mMessageReceiver;
import android.app.Activity;
import android.app.TabActivity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TabHost;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.TabHost.OnTabChangeListener;

public class Advanced_Combo_Tool extends TabActivity implements
OnTabChangeListener{   
	static final String ADVANCED_ACTIVITY_BROADCAST = "ADVANCED_ACTIVITY_BROADCAST";
	static final String ADVANCED_BT_BROADCAST = "ADVANCED_BT_BROADCAST";
	static final String ADVANCED_WIFI_BROADCAST = "ADVANCED_WIFI_BROADCAST";
	static final String ADVANCED_GPS_BROADCAST = "ADVANCED_GPS_BROADCAST";
	static final String ADVANCED_FM_BROADCAST = "ADVANCED_FM_BROADCAST";

	private TabHost myTabhost;
	//Button mButton_test1;
	Button mButton_test2;
	
	public void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
	}
    @Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		// setContentView(R.layout.main);
		// final TextView tv = new TextView(this );
		myTabhost = this.getTabHost();
		// get Tabhost
//		LayoutInflater.from(this).inflate(R.layout.advanced_combo_tool,
//				myTabhost.getTabContentView(), true);
//		myTabhost.setBackgroundColor(Color.argb(150, 22, 70, 150));
/*
		myTabhost.addTab(myTabhost.newTabSpec("One")// make a new Tab
				.setIndicator("WIFI")
				// set the Title and Icon
				.setContent(R.id.wifi));
*/	
		Intent iAdvacedWIFI = new Intent(this,
				Advanced_WIFI.class);		
		Intent iAdvacedBT = new Intent(this,
				Advanced_BT.class);		
		Intent iAdvacedGPS = new Intent(this,
				YgpsActivity.class);
		Intent iAdvacedFM = new Intent(this,
				Advanced_FM.class);
		
		iAdvacedFM.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		iAdvacedGPS.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		iAdvacedBT.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		iAdvacedWIFI.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		
		myTabhost.addTab(myTabhost.newTabSpec("WIFI").setIndicator("WIFI",
				null).setContent(iAdvacedWIFI));
		myTabhost.addTab(myTabhost.newTabSpec("BT").setIndicator("BT",
				null).setContent(iAdvacedBT));
		myTabhost.addTab(myTabhost.newTabSpec("GPS").setIndicator("GPS",
				null).setContent(iAdvacedGPS));
		myTabhost.addTab(myTabhost.newTabSpec("FM").setIndicator("FM",
				null).setContent(iAdvacedFM));
		
/*		int height = myTabhost.getTabWidget().getChildAt(0).getLayoutParams().height;

		myTabhost.getTabWidget().getChildAt(0).getLayoutParams().height = height  * 2 / 3;
		myTabhost.getTabWidget().getChildAt(1).getLayoutParams().height = height  * 2 / 3;
		myTabhost.getTabWidget().getChildAt(2).getLayoutParams().height = height  * 2 / 3;
		myTabhost.getTabWidget().getChildAt(3).getLayoutParams().height = height  * 2 / 3;
*/		
		myTabhost.setOnTabChangedListener(this);

//		mMessageReceiver receiver = new mMessageReceiver();
//		IntentFilter filter = new IntentFilter();
//		filter.addAction(ADVANCED_ACTIVITY_BROADCAST);
//		registerReceiver(receiver, filter);
		//mButton_test1 = (Button) findViewById(R.id.Button_advanced_wifi_test);
	/*	
		mButton_test1.setOnClickListener(new Button.OnClickListener() {

			public void onClick(View arg0) {

				Intent i = new Intent(Advanced_Combo_Tool.this, Advanced_WiFi_Service.class);
				i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				//i.putExtra("STR_PARAM1", SSID);
				//i.putExtra("STR_PARAM2", PASSWORD);
				//i.putExtra("STR_PARAM3", RSSI_Threshold);
				startService(i);
			}

		});
		*/
		
}

	public void onTabChanged(String arg0) {
		// TODO Auto-generated method stub
		
	} 
	
	public class mMessageReceiver extends BroadcastReceiver {

		@Override
		public void onReceive(Context context, Intent intent) {

			// TODO Auto-generated method stub
			

		}

	}
	
	
}