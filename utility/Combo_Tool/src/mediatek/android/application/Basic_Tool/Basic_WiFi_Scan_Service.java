package mediatek.android.application.Basic_Tool;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import mediatek.android.application.Basic_Tool.Basic_Combo_Tool.mMessageReceiver;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.NetworkInfo.State;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.bluetooth.BluetoothAdapter;

public class Basic_WiFi_Scan_Service extends Service {

	WiFi_Admin wifiAdmin;

	private Handler objHandler = new Handler();

	private int intCounter = 0;

	boolean isRunning = false;

	private Runnable ScanTasks = new Runnable() {

		public void run() {


			int timer = 0;
			do {
				if (isRunning == false) {
					break;
				}

				if (wifiAdmin.isWifiEnabled() == false) {
					// wifiservice_broadcast("MSG", "opening wifi", "null");
					wifiAdmin.OpenWifi();
					timer = 4000;
					break;
				}
				if (wifiAdmin.isWifiEnabled() == true) {
					// wifiservice_broadcast("MSG", "wifi start scan", "null");
					wifiAdmin.StartScan();
					// wifiservice_broadcast("MSG", "wifi_scan_result","null");
					getScanResult();
					timer = 6000;
					break;
				}

			} while (false);

			if (isRunning == true)
				objHandler.postDelayed(ScanTasks, timer);
		}

	};

	@Override
	public void onStart(Intent intent, int startId) {
		// TODO Auto-generated method stub

		super.onStart(intent, startId);
		Bundle bunde = intent.getExtras();

		if (isRunning == false) {

			objHandler.postDelayed(ScanTasks, 1000);
		} else
			wifiservice_broadcast("Warning", "scan already started", "null");
		isRunning = true;

	}

	ArrayList<String> wifiNames;
	ArrayList<WifiConfiguration> wifiDevices;

	@Override
	public void onCreate() {
		// TODO Auto-generated method stub
		mMessageReceiver receiver = new mMessageReceiver();
		IntentFilter filter = new IntentFilter();
		wifiNames = new ArrayList<String>();
		wifiDevices = new ArrayList<WifiConfiguration>();
		filter.addAction(Basic_Combo_Tool.WIFI_SCAN_BROADCAST);
		filter.addAction(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION);
		registerReceiver(receiver, filter);
		wifiAdmin = new WiFi_Admin(getApplicationContext());

		isRunning = false;
		super.onCreate();
	}

	@Override
	public IBinder onBind(Intent arg0) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void onDestroy() {
		// TODO Auto-generated method stub
		objHandler.removeCallbacks(ScanTasks);
		super.onDestroy();
	}

	public void getScanResult() {
		// TODO Auto-generated method stub
		wifiservice_broadcast("clear_wifi_scan_result", "null", "null");
		List<ScanResult> scanlist = wifiAdmin.GetWifiList();
		for (int i = 0, len = scanlist.size(); i < len; i++) {
			ScanResult ap = scanlist.get(i);
			// wifiservice_broadcast("MSG", "find AP: \n"+
			// ap.SSID+"\n"+ap.BSSID+"\n"+ap.capabilities+"\n"+Integer.toString(ap.level),"null"
			// );
			if (ap.SSID.contains("Err"))
				continue;
			String channel = "channel "
					+ Integer.toString((ap.frequency - 2412) / 5 + 1);
			String capabilities;
			if (ap.capabilities.equals(""))
				capabilities = "[open]";
			else
				capabilities = ap.capabilities;
			wifiservice_broadcast("wifi_scan_result", ap.SSID, ap.BSSID,
					capabilities, Integer.toString(ap.level) + "dbm", channel
							+ "  " + Integer.toString(ap.frequency) + "MHz");
		}
	}

	public class mMessageReceiver extends BroadcastReceiver {

		@Override
		public void onReceive(Context context, Intent intent) {

			// TODO Auto-generated method stub
			try {
				Bundle bunde = intent.getExtras();
				String strParam1 = bunde.getString("STR_PARAM1");
				String strParam2 = bunde.getString("STR_PARAM2");

				if (strParam1.equals("Wifi_StopScan")) {
					//wifiservice_broadcast("MSG", "Wifi_StopScan", "msg");
					isRunning = false;
				} else {

					//wifiservice_broadcast("MSG", "wifi_scan_result #2", "null");
					getScanResult();
				}

			} catch (Exception e) {
				e.getStackTrace();

			}

		}

	}

	void wifiservice_broadcast(String msg1, String msg2, String msg3) {

		Intent i = new Intent(Basic_Combo_Tool.ACTIVITY_BROADCAST);

		i.putExtra("STR_PARAM1", msg1);
		i.putExtra("STR_PARAM2", msg2);
		i.putExtra("STR_PARAM3", msg3);

		sendBroadcast(i);
	}

	void wifiservice_broadcast(String msg1, String msg2, String msg3,
			String msg4, String msg5, String msg6) {

		Intent i = new Intent(Basic_Combo_Tool.ACTIVITY_BROADCAST);

		i.putExtra("STR_PARAM1", msg1);
		i.putExtra("STR_PARAM2", msg2);
		i.putExtra("STR_PARAM3", msg3);
		i.putExtra("STR_PARAM4", msg4);
		i.putExtra("STR_PARAM5", msg5);
		i.putExtra("STR_PARAM6", msg6);

		sendBroadcast(i);
	}

}
