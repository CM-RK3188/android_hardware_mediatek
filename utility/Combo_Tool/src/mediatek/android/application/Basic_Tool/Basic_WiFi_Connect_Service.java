package mediatek.android.application.Basic_Tool;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
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
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.bluetooth.BluetoothAdapter;

public class Basic_WiFi_Connect_Service extends Service {

	WiFi_Admin wifiAdmin;
	boolean isRunning = false;

	private Handler objHandler = new Handler();

	private int intCounter = 0;
	DataInputStream localDataInputStream;
	protected Vector execRootCmd(String paramString) {
		Vector localVector = new Vector();
		try {
			Process localProcess = Runtime.getRuntime().exec("/system/bin/sh");// 经过Root处理的android系统即有su命令
			OutputStream localOutputStream = localProcess.getOutputStream();
			DataOutputStream localDataOutputStream = new DataOutputStream(
					localOutputStream);
			InputStream localInputStream = localProcess.getInputStream();
			localDataInputStream = new DataInputStream(
					localInputStream);
			String str1 = String.valueOf(paramString);
			String str2 = str1 + "\n";
			localDataOutputStream.writeBytes(str2);
			localDataOutputStream.flush();
			for(int i=1;i<100000;i++);
			String str3 = localDataInputStream.readLine();
			wifiservice_broadcast("MSG", str3, "null");
			localVector.add(str3); 

			localDataOutputStream.writeBytes("exit\n");
			localDataOutputStream.flush();
			localProcess.waitFor();

			return localVector;
		} catch (Exception localException) {
			
		}
		return null;
	}

	private Runnable mTasks = new Runnable() {

		public void run() {
			// TODO Auto-generated method stub
			/*
			 * int e=mJNI.server_start(); if(e<0)
			 * service_to_activity("server_start()"+e);
			 * //execRootCmd("ifconfig wlan0");
			 * 
			 * intCounter++;
			 * 
			 * if(msJNI.s != "null") { Intent i = new
			 * Intent(HIPPO_SERVICE_IDENTIFIER);
			 * 
			 * i.putExtra ( "STR_PARAM1", msJNI.s ); sendBroadcast(i);
			 * 
			 * }
			 * Log.i("HIPPO","Service Running Counter"+Integer.toString(intCounter
			 * ));
			 */
	
			
			int timer = 0;
			do {
				if (isRunning == false) {
					break;
				}

				if (wifiAdmin.isWifiEnabled() == false) {
					wifiservice_broadcast("MSG_WIFI", "opening wifi", "null");
					wifiAdmin.OpenWifi();
					timer = 4000;
					break;
				}
				if (wifiAdmin.isConnected() == false) {
					wifiservice_broadcast("MSG_WIFI", "connecting to " + SSID,
							"null");
					wifiAdmin.AddNetwork(wifiAdmin.CreateWifiInfo(SSID,
							PASSWORD, 3));
					timer = 6000;
					break;
				}
				if (wifiAdmin.isConnected() == true) {
					String NewSSID = wifiAdmin.GetSSID();
					if (NewSSID.startsWith("\"") && NewSSID.endsWith("\""))
					{
						NewSSID = NewSSID.substring(1, NewSSID.length() - 1);
					}
					if (SSID.equals(NewSSID)) {
						int rssi = wifiAdmin.GetRssi();
						String result = (rssi > Integer
								.parseInt(RSSI_Threshold)) ? "PASS!" : "FAIL!";

						String str_rssi = Integer.toString(rssi);
						String str_isConnected = wifiAdmin.isConnected() ? "wifi_connected"
								: "wifi_disconnected";

						wifiservice_broadcast(str_isConnected, str_rssi, result);
						timer = 5000;
					} else {
						wifiservice_broadcast("MSG_WIFI",SSID + " disconnecting to "
								+ NewSSID, "null");
						int netId = wifiAdmin.GetNetworkId();
						wifiAdmin.DisconnectWifi(netId);
						timer = 4000;

					}
				}

			} while (false);

			if (isRunning == true)
				objHandler.postDelayed(mTasks, timer);
		}

	};
	String SSID;
	String PASSWORD;
	String RSSI_Threshold;

	@Override
	public void onStart(Intent intent, int startId) {
		// TODO Auto-generated method stub

		super.onStart(intent, startId);
		Bundle bunde = intent.getExtras();
		//execRootCmd("ls /system/lib/modules/");
		if (isRunning == false) {
			SSID = bunde.getString("STR_PARAM1");
			PASSWORD = bunde.getString("STR_PARAM2");
			RSSI_Threshold = bunde.getString("STR_PARAM3");
			objHandler.postDelayed(mTasks, 1000);
		} else
			wifiservice_broadcast("MSG_WIFI", "Waring: already started", "null");
		isRunning = true;

	}

	boolean isConnecting;
	boolean isWifiEnabling;

	@Override
	public void onCreate() {
		// TODO Auto-generated method stub
		mMessageReceiver receiver = new mMessageReceiver();
		IntentFilter filter = new IntentFilter();
		filter.addAction(Basic_Combo_Tool.WIFI_CONNECT_BROADCAST);
		filter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);
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
		objHandler.removeCallbacks(mTasks);
		super.onDestroy();
	}

	public class mMessageReceiver extends BroadcastReceiver {

		@Override
		public void onReceive(Context context, Intent intent) {

			// TODO Auto-generated method stub
			try {
				Bundle bunde = intent.getExtras();
				String strParam1 = bunde.getString("STR_PARAM1");
				String strParam2 = bunde.getString("STR_PARAM2");
				if (strParam1.equals("closewifi")) {
					wifiAdmin.CloseWifi();
					isRunning = false;
				}

			} catch (Exception e) {
				e.getStackTrace();
				BluetoothAdapter mAdapter = BluetoothAdapter
						.getDefaultAdapter();
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

}
