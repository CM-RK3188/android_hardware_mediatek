package mediatek.android.application.Advanced_Tool;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Vector;

import mediatek.android.application.Basic_Tool.WiFi_Admin;
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

public class Advanced_WiFi_Service extends Service {


	boolean isRunning = false;

	private Handler objHandler = new Handler();

	private int intCounter = 0;

	//private WiFi_JNI mJNI = new WiFi_JNI(this);

	/*
	 * protected Vector execRootCmd(String paramString) { Vector localVector =
	 * new Vector(); try { Process localProcess =
	 * Runtime.getRuntime().exec("/system/bin/sh");//经过Root处理的android系统即有su命令
	 * OutputStream localOutputStream = localProcess.getOutputStream();
	 * DataOutputStream localDataOutputStream = new
	 * DataOutputStream(localOutputStream); InputStream localInputStream =
	 * localProcess.getInputStream(); DataInputStream localDataInputStream = new
	 * DataInputStream(localInputStream); String str1 =
	 * String.valueOf(paramString); String str2 = str1 + "\n";
	 * localDataOutputStream.writeBytes(str2); localDataOutputStream.flush();
	 * 
	 * //String str3 = localDataInputStream.readLine();
	 * 
	 * //localVector.add(str3); service_to_activity("test","null","null");
	 * localDataOutputStream.writeBytes("exit\n");
	 * localDataOutputStream.flush(); localProcess.waitFor();
	 * 
	 * 
	 * return localVector; } catch (Exception localException) {
	 * localException.printStackTrace(); } return null; }
	 */


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

		}

	};
	String SSID;
	String PASSWORD;
	String RSSI_Threshold;

	@Override
	public void onStart(Intent intent, int startId) {
		// TODO Auto-generated method stub

		super.onStart(intent, startId);
		//Bundle bunde = intent.getExtras();
		//mJNI.server_start();
		if (isRunning == false) {
			objHandler.postDelayed(mTasks, 1000);
		} else
			wifiservice_broadcast("Warning", "already started", "null");
		isRunning = true;

	}

	boolean isConnecting;
	boolean isWifiEnabling;

	@Override
	public void onCreate() {
		// TODO Auto-generated method stub
		mMessageReceiver receiver = new mMessageReceiver();
		IntentFilter filter = new IntentFilter();
		filter.addAction(Advanced_Combo_Tool.ADVANCED_WIFI_BROADCAST);
		filter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);
		registerReceiver(receiver, filter);

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
		//mJNI.server_stop();
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
					isRunning = false;
				}

			} catch (Exception e) {
				e.getStackTrace();
			      BluetoothAdapter mAdapter= BluetoothAdapter.getDefaultAdapter();
			}

		}

	}
	
	void wifiservice_broadcast(String msg1, String msg2, String msg3) {

		Intent i = new Intent(Advanced_Combo_Tool.ADVANCED_WIFI_BROADCAST);

		i.putExtra("STR_PARAM1", msg1);
		i.putExtra("STR_PARAM2", msg2);
		i.putExtra("STR_PARAM3", msg3);

		sendBroadcast(i);
	}

}
