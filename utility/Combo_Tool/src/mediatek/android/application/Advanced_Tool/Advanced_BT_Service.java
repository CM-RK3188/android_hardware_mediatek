package mediatek.android.application.Advanced_Tool;

import java.io.DataInputStream;
import android.app.Activity;
import java.io.DataOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Vector;

import mediatek.android.application.Basic_Tool.BT_Admin;



import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothClass;
import android.bluetooth.BluetoothDevice;
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
import android.widget.Button;

public class Advanced_BT_Service extends Service {

	BT_Admin BTAdmin;
	boolean isRunning = false;


	private Handler objHandler = new Handler();

	private int intCounter = 0;

	// private BT_JNI mJNI = new BT_JNI(this);

	private Runnable mTasks = new Runnable() {

		public void run() {
			// TODO Auto-generated method stub

			
			int timer = 0;
			do {

				if (BTAdmin.isBTEnabled() == false) {
					btservice_broadcast("MSG", "opening BT", "null");
					BTAdmin.OpenBT();
					timer = 4000;
					break;
				}
				if (BTAdmin.isBTScanning() == false)
				{
				BTAdmin.StartScan();
				btservice_broadcast("MSG", "BT StartScan", "null");
				}
				else
				{
					btservice_broadcast("MSG", "BT is scanning", "null");
				}
				isRunning = false;
				break;
				/*
				 * if (BTAdmin.isConnected() == false) {
				 * btservice_broadcast("MSG", "connecting to " + SSID, "null");
				 * BTAdmin.AddNetwork(BTAdmin.CreateBTInfo(SSID, PASSWORD, 2));
				 * timer = 6000; break; } if (BTAdmin.isConnected() == true) {
				 * if (SSID.equals(BTAdmin.GetSSID())) { int rssi =
				 * BTAdmin.GetRssi(); String result = (rssi > Integer
				 * .parseInt(RSSI_Threshold)) ? "PASS!" : "FAIL!";
				 * 
				 * String str_rssi = Integer.toString(rssi); String
				 * str_isConnected = BTAdmin.isConnected() ? "connected" :
				 * "disconnected";
				 * 
				 * btservice_broadcast(str_isConnected, str_rssi, result); timer
				 * = 5000; } else { BTservice_broadcast("MSG",
				 * "disconnecting to " + BTAdmin.GetSSID(), "null"); int netId =
				 * BTAdmin.GetNetworkId(); BTAdmin.DisconnectBT(netId); timer =
				 * 2000;
				 * 
				 * } }
				 */

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
		// Bundle bunde = intent.getExtras();

		if (isRunning == false) {
			objHandler.postDelayed(mTasks, 1000);
		} else
			btservice_broadcast("Warning", "already started", "null");
		isRunning = true;

	}

	boolean isConnecting;
	boolean isBTEnabling;

	@Override
	public void onCreate() {
		// TODO Auto-generated method stub
		mMessageReceiver receiver = new mMessageReceiver();
		IntentFilter filter = new IntentFilter();
		filter.addAction(Advanced_Combo_Tool.ADVANCED_BT_BROADCAST);
		registerReceiver(receiver, filter);
		BTAdmin = new BT_Admin(getApplicationContext());

		isRunning = false;
		super.onCreate();
	}

	@Override
	public void onDestroy() {
		// TODO Auto-generated method stub
		// mJNI.server_stop();
		objHandler.removeCallbacks(mTasks);
		super.onDestroy();
	}

	public class mMessageReceiver extends BroadcastReceiver {

		@Override
		public void onReceive(Context context, Intent intent) {

			// TODO Auto-generated method stub
			try {
	
				
				
				String action = intent.getAction();
				
			

				// 找到设备

				if (Advanced_Combo_Tool.ADVANCED_ACTIVITY_BROADCAST.equals(action))
				{
					Bundle bunde = intent.getExtras();
					String strParam1 = bunde.getString("STR_PARAM1");
					String strParam2 = bunde.getString("STR_PARAM2");
					if (strParam1.equals("closeBT")) {
						BTAdmin.CloseBT();
						btservice_broadcast("MSG", "BT CloseBT", "null");
					}
					else if(strParam1.equals("stopScan"))
					{		
						BTAdmin.StopScan();
						btservice_broadcast("MSG", "BT StopScan", "null");
					}	
				}
				else if (BluetoothDevice.ACTION_FOUND.equals(action)) {


					
					BluetoothDevice device = intent
							.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

					if (device.getBondState() != BluetoothDevice.BOND_BONDED) {

						// Log.v("BT", "find device:" + device.getName()+
						// device.getAddress());

						short rssi = intent.getExtras().getShort(BluetoothDevice.EXTRA_RSSI);
						int device_class = device.getBluetoothClass().getMajorDeviceClass();
						String string_device_class = "uncategorized device";
						switch (device_class)
						{
						case BluetoothClass.Device.Major.COMPUTER:
							string_device_class = "computer";
							break;
						case BluetoothClass.Device.Major.PHONE:
							string_device_class = "phone";
							break;
						case BluetoothClass.Device.Major.AUDIO_VIDEO:
							string_device_class = "audio_video";
							break;
						case BluetoothClass.Device.Major.IMAGING:
							string_device_class = "imaging";
							break;
						case BluetoothClass.Device.Major.PERIPHERAL:
							string_device_class = "peripheral";
							break;
						case BluetoothClass.Device.Major.TOY:
							string_device_class = "toy";
							break;
						case BluetoothClass.Device.Major.WEARABLE:
							string_device_class = "wearable";
							break;
						case BluetoothClass.Device.Major.NETWORKING:
							string_device_class = "networking";
							break;
						default:
							string_device_class = "uncategorized device";
						}
							
					
						btservice_broadcast("MSG", "find device: \n"
								+ device.getName() + device.getAddress()+ "\n"+Short.toString(rssi)+"\n"+string_device_class,
								"null");
						btservice_broadcast("BT_inquiry", device.getName(), device.getAddress()+ "("+string_device_class+Integer.toString(rssi)+")",Integer.toString(rssi),string_device_class);
						

					}

				}

				// 搜索完成

				else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED
						.equals(action)) {

					// setTitle("搜索完成");

					// if (mNewDevicesAdapter.getCount() == 0) {

					// Log.v(TAG,"find over");

					// }
					btservice_broadcast("find over","null","null");

				}

				// 执行更新列表的代码



			} catch (Exception e) {
				e.getStackTrace();
			}

		}

	}

	void btservice_broadcast(String msg1, String msg2, String msg3) {

		Intent i = new Intent(Advanced_Combo_Tool.ADVANCED_BT_BROADCAST);

		i.putExtra("STR_PARAM1", msg1);
		i.putExtra("STR_PARAM2", msg2);
		i.putExtra("STR_PARAM3", msg3); 
		sendBroadcast(i);
	}
	
	void btservice_broadcast(String msg1, String msg2, String msg3,String msg4,String msg5) {

		Intent i = new Intent(Advanced_Combo_Tool.ADVANCED_BT_BROADCAST);

		i.putExtra("STR_PARAM1", msg1);
		i.putExtra("STR_PARAM2", msg2);
		i.putExtra("STR_PARAM3", msg3);
		i.putExtra("STR_PARAM4", msg4);
		i.putExtra("STR_PARAM5", msg5);

		sendBroadcast(i);
	}

	@Override
	public IBinder onBind(Intent arg0) {
		// TODO Auto-generated method stub
		return null;
	}
}
