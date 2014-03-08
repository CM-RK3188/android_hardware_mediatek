package mediatek.android.application.Advanced_Tool;

//import mediatek.android.application.Basic_Tool.Basic_Combo_Tool;
import mediatek.android.application.R;
import mediatek.android.application.Basic_Tool.BT_Service;
import mediatek.android.application.Basic_Tool.Basic_Combo_Tool;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.app.TabActivity;
import android.content.Context;
import android.content.Intent;
import android.content.res.Configuration;
//import android.content.Intent;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.SystemClock;
//import android.widget.TabHost;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TabHost;
import android.widget.Toast;
import android.widget.TabHost.OnTabChangeListener;
import android.widget.TabHost.TabContentFactory;

public class Advanced_WIFI extends TabActivity implements
OnTabChangeListener{	
	private WifiManager mWifiManager = null;
	private static final String TAG = "Advanced_WiFi";
	private static final long DEFAULT_WAIT_TIME = 10;
	
	@Override
	public void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
	}
//	private Button mButton_bt_inquiry;
	private TabHost myTabhost;
	protected int myMenuSettingTag = 0;
	protected Menu myMenu;
//	private static final int myMenuResources[] = { R.menu.a_menu,
//			R.menu.b_menu, R.menu.c_menu };

	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
	}
	
	public void onPause() {
		// TODO Auto-generated method stub
		super.onPause();
	}
/*	
	@Override
    protected Dialog onCreateDialog(int id) {
        ProgressDialog dialog = null;
        dialog = new ProgressDialog(Advanced_WIFI.this.getParent().getParent());
        dialog.setTitle("WiFi Initializing");
        dialog.setMessage("Please wait for Wifi enabled ...");
        dialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
        dialog.setCancelable(false);
        dialog.setProgress(0);
        Log.d(TAG, "Dialog!");
        
        return dialog;
    }
*/	
	public void onStart() 
	{
		Log.d(TAG, "Enable Wifi");
//		showDialog(1);
//		removeDialog(1);
		
		super.onStart();
	}
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		myTabhost = this.getTabHost();
//		Intent iAdvacedWIFITx = new Intent(this,
//				Advanced_WIFI_TX.class);
//		iAdvacedWIFITx.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TOP);
		// get Tabhost
//		LayoutInflater.from(this).inflate(R.layout.advanced_wifi,
//				myTabhost.getTabContentView(), true);
//		LayoutInflater.from(this).inflate(R.layout.advanced_wifi,
//				myTabhost.getTabContentView());
		
		Intent iAdvnaced_WiFi_Rx = new Intent(this,
				Advanced_WiFi_Rx.class);		
		Intent iAdvaced_WiFi_Tx = new Intent(this,
				Advanced_WiFi_Tx.class);		
		Intent iAdvanced_WiFi_WFA = new Intent(this,
				Advanced_WiFi_WFA.class);
		Intent iAdvanced_WiFi_CTIA = new Intent(this,
				Advanced_WiFi_CTIA.class);
		
//		iAdvnaced_WiFi_Rx.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
//		iAdvaced_WiFi_Tx.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
//		iAdvanced_WiFi_WFA.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
//		iAdvanced_WiFi_CTIA.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		
		myTabhost.addTab(myTabhost.newTabSpec("RX").setIndicator("RX")
						.setContent(iAdvnaced_WiFi_Rx));
		myTabhost.addTab(myTabhost.newTabSpec("TX").setIndicator("TX")
					.setContent(iAdvaced_WiFi_Tx));
/*		myTabhost.addTab(myTabhost.newTabSpec("WFA").setIndicator("WFA")
				.setContent(iAdvanced_WiFi_WFA));
		myTabhost.addTab(myTabhost.newTabSpec("CTIA").setIndicator("CTIA")
				.setContent(iAdvanced_WiFi_CTIA));
*/		
		int height = myTabhost.getTabWidget().getChildAt(0).getLayoutParams().height;
		myTabhost.getTabWidget().getChildAt(0).getLayoutParams().height = height * 2/3;
		myTabhost.getTabWidget().getChildAt(1).getLayoutParams().height = height * 2/3;
//		myTabhost.getTabWidget().getChildAt(2).getLayoutParams().height = height * 2/3;
//		myTabhost.getTabWidget().getChildAt(3).getLayoutParams().height = height * 2/3;
		
//		setContentView(myTabhost);
			
//		myTabhost.setOnTabChangedListener(this);
		this.onTabChanged("RX");
	}
	
	public boolean onCreateOptionsMenu(Menu menu) {
		// TODO Auto-generated method stub
		// Hold on to this
		/*
		 * myMenu = menu; myMenu.clear();//清空MENU菜单 // Inflate the currently
		 * selected menu XML resource. MenuInflater inflater =
		 * getMenuInflater(); //从TabActivity这里获取一个MENU过滤器 switch
		 * (myMenuSettingTag) { case 1: inflater.inflate(myMenuResources[0],
		 * menu); //动态加入数组中对应的XML MENU菜单 break; case 2:
		 * inflater.inflate(myMenuResources[1], menu); break; case 3:
		 * inflater.inflate(myMenuResources[2], menu); break; default:
		 * inflater.inflate(myMenuResources[0], menu); break; }
		 */
		return super.onCreateOptionsMenu(menu);

	}

	public void onTabChanged(String tagString) {
		// TODO Auto-generated method stub
/*	
		if (tagString.equals("TX")) {
			Toast.makeText(
					getApplicationContext(),
					"TX",
					Toast.LENGTH_LONG).show();
			
		}
		if (tagString.equals("RX")) {
			Toast.makeText(
					getApplicationContext(),
					"RX",
					Toast.LENGTH_LONG).show();
			

			mButton_bt_inquiry = (Button) findViewById(R.id.WiFi_Go_Rx);
			mButton_bt_inquiry.setEnabled(true);
			
			mButton_bt_inquiry.setOnClickListener(new Button.OnClickListener() {

				 
				public void onClick(View arg0) {
					// TODO Auto-generated method stub
					
					mButton_bt_inquiry.setEnabled(false);

				}

			});
		}
		if (tagString.equals("WFA")) {
			Toast.makeText(
					getApplicationContext(),
					"WFA",
					Toast.LENGTH_LONG).show();
		}
		if (tagString.equals("CTIA")) {
			Toast.makeText(
					getApplicationContext(),
					"CTIA",
					Toast.LENGTH_LONG).show();
		}
		if (myMenu != null) {
			onCreateOptionsMenu(myMenu);
		}
		*/
	}
	
}
