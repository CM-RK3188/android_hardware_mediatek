package mediatek.android.application.Basic_Tool;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import mediatek.android.application.AssetIniReader;
import mediatek.android.application.FileParse;
import mediatek.android.application.R;
import mediatek.android.application.Advanced_Tool.Advanced_WiFi_Rx;
import mediatek.android.application.R.drawable;
import mediatek.android.application.R.id;
import mediatek.android.application.R.layout;
import mediatek.android.application.R.menu;

import android.app.Activity;
import android.app.TabActivity;
import android.bluetooth.BluetoothAdapter;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Configuration;
import android.graphics.Color;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.text.Editable;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.TabHost;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.TabHost.OnTabChangeListener;

public class Basic_Combo_Tool extends TabActivity implements
		OnTabChangeListener {

	public TextView mTextView01;
	private Button mButton_wifi_Connect;
	private Button mButton_wifi_Disconnect;
	private Button mButton_wifi_Scan;
	private Button mButton_wifi_StopScan;
	private Button mButton_bt_inquiry;
	private Button mButton_bt_stop_inquiry;
	private EditText mEditText_SSID;
	private EditText mEditText_PASSWORD;
	private EditText mEditText_RSSI_Threshold;

	public TextView mTextView_Connect;
	public TextView mTextView_RSSI;
	public TextView mTextView_Result;
	public ListView mListView_BTinquiry;
	public ListView mListView_WIFIscan;

	String SSID, PASSWORD, RSSI_Threshold;
	String INIT_SSID, INIT_PASSWORD, INIT_RSSI_THRESHOLD;

	// static final String ACTIVITY_BROADCAST = "ACTIVITY_BROADCAST";
	static final String BT_BROADCAST = "BT_BROADCAST";
	static final String WIFI_CONNECT_BROADCAST = "WIFI_CONNECT_BROADCAST";
	static final String WIFI_SCAN_BROADCAST = "WIFI_SCAN_BROADCAST";
	static final String GPS_BROADCAST = "GPS_BROADCAST";
	static final String FM_BROADCAST = "FM_BROADCAST";
	static final String ACTIVITY_BROADCAST = "ACTIVITY_BROADCAST";

	private TabHost myTabhost;
	protected int myMenuSettingTag = 0;
	protected Menu myMenu;
	private static final int myMenuResources[] = { R.menu.a_menu,
			R.menu.b_menu, R.menu.c_menu };
	
	FileParse fileparse = new FileParse("ComboTool.ini");

	@Override
	public void onDestroy() {
		// TODO Auto-generated method stub

		super.onDestroy();
	}

	@Override
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
		if (tagString.equals("One")) {
			myMenuSettingTag = 1;
		}
		if (tagString.equals("Two")) {
			myMenuSettingTag = 2;
		}
		if (tagString.equals("Three")) {
			myMenuSettingTag = 3;
		}
		if (myMenu != null) {
			onCreateOptionsMenu(myMenu);
		}
	}

	 
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
	}

	 
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		/*
					AssetIniReader iniReader = new AssetIniReader("ComboTool.ini",
							Basic_Combo_Tool.this);
					INIT_SSID = iniReader.getValue("WifiSet", "SSID");
					INIT_PASSWORD = iniReader.getValue("WifiSet", "PASSWORD");
					INIT_RSSI_THRESHOLD = iniReader.getValue("WifiSet",
							"RSSI_THRESHOLD");
		*/
			
					
		INIT_SSID = fileparse.getValueByKey("[simple-wifi]","SSID");
		INIT_PASSWORD = fileparse.getValueByKey("[simple-wifi]","PASSWORD");
		INIT_RSSI_THRESHOLD = fileparse.getValueByKey("[simple-wifi]","RSSI_THRESHOLD");
		
		//避免初始化文件中的RSSI_THRESHOLD中存在除去数字和'-'的非法字符.
		try {
			Integer.parseInt(INIT_RSSI_THRESHOLD);
		} catch (Exception e) {
			INIT_RSSI_THRESHOLD = "-70";
			fileparse.updateKeyValue("[simple-wifi]", "RSSI_THRESHOLD", INIT_RSSI_THRESHOLD);
		}

		myTabhost = this.getTabHost();
		// get Tabhost
		LayoutInflater.from(this).inflate(R.layout.basic_combo_tool,
				myTabhost.getTabContentView(), true);
		myTabhost.setBackgroundColor(Color.argb(65, 190, 240, 150));

		myTabhost.addTab(myTabhost.newTabSpec("One")// make a new Tab
				.setIndicator("WIFI")
				// set the Title and Icon
				.setContent(R.id.wifi));
		// set the layout

		myTabhost.addTab(myTabhost.newTabSpec("Two")// make a new Tab
				.setIndicator("BT")
				// set the Title and Icon
				.setContent(R.id.bt));
		// set the layout

		int height = myTabhost.getTabWidget().getChildAt(0).getLayoutParams().height;
		myTabhost.getTabWidget().getChildAt(0).getLayoutParams().height = height * 2 / 3;
		myTabhost.getTabWidget().getChildAt(1).getLayoutParams().height = height * 2 / 3;
		
		myTabhost.setOnTabChangedListener(this);

		mMessageReceiver receiver = new mMessageReceiver();
		IntentFilter filter = new IntentFilter();
		filter.addAction(ACTIVITY_BROADCAST);

		registerReceiver(receiver, filter);

		mTextView01 = (TextView) findViewById(R.id.TextView01);

		mEditText_SSID = (EditText) findViewById(R.id.SSID);
		mEditText_PASSWORD = (EditText) findViewById(R.id.PASSWORD);
		mEditText_RSSI_Threshold = (EditText) findViewById(R.id.RSSI_Threshold);

		mEditText_SSID.setText(INIT_SSID);
		mEditText_PASSWORD.setText(INIT_PASSWORD);
		mEditText_RSSI_Threshold.setText(INIT_RSSI_THRESHOLD);

		mTextView_Connect = (TextView) findViewById(R.id.Connect_Content);
		mTextView_RSSI = (TextView) findViewById(R.id.RSSI_Content);
		mTextView_Result = (TextView) findViewById(R.id.Result_Content);

		mButton_wifi_Connect = (Button) findViewById(R.id.Button_wifi_Connect);
		mButton_wifi_Connect.setEnabled(true);

		mButton_wifi_Disconnect = (Button) findViewById(R.id.Button_wifi_Disconnect);
		mButton_wifi_Disconnect.setEnabled(false);

		mButton_wifi_Scan = (Button) findViewById(R.id.Button_wifi_Scan);
		mButton_wifi_Scan.setEnabled(true);

		mButton_wifi_StopScan = (Button) findViewById(R.id.Button_wifi_StopScan);
		mButton_wifi_StopScan.setEnabled(false);

		mButton_bt_inquiry = (Button) findViewById(R.id.Button_bt_inquiry);
		mButton_bt_inquiry.setEnabled(true);
		// mTextView01.setText("test");
		mButton_bt_stop_inquiry = (Button) findViewById(R.id.Button_bt_stop_inquiry);
		mButton_bt_stop_inquiry.setEnabled(false);

		mButton_wifi_Connect.setOnClickListener(new Button.OnClickListener() {
			//  
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
//				Toast.makeText(Basic_Combo_Tool.this, SSID, 5).show();
				mListView_WIFIscan.setVisibility(View.GONE);
				SSID = mEditText_SSID.getText().toString();
				PASSWORD = mEditText_PASSWORD.getText().toString();
				RSSI_Threshold = mEditText_RSSI_Threshold.getText().toString();
		
				fileparse.updateKeyValue("[simple-wifi]", "SSID", SSID);
				fileparse.updateKeyValue("[simple-wifi]", "PASSWORD", PASSWORD);
				fileparse.updateKeyValue("[simple-wifi]", "RSSI_THRESHOLD", RSSI_Threshold);
				
//				Toast.makeText(Basic_Combo_Tool.this, SSID, 5).show();
				
				try {
					Integer.parseInt(RSSI_Threshold);
				} catch (Exception e) {
					Toast
							.makeText(
									getApplicationContext(),
									"Error: RSSI threshold is invalid ! \n Use default -70",
									Toast.LENGTH_LONG).show();
					RSSI_Threshold = "-70";
					mEditText_RSSI_Threshold.setText(RSSI_Threshold);
					fileparse.updateKeyValue("[simple-wifi]", "RSSI_THRESHOLD", RSSI_Threshold);

				}
				
				try {
						fileparse.updateConFile("ComboTool.ini");
					} catch (IOException e1) {
						// TODO Auto-generated catch block
						e1.printStackTrace();
				}
				
				Intent i = new Intent(Basic_Combo_Tool.this,
						Basic_WiFi_Connect_Service.class);
				i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				i.putExtra("STR_PARAM1", SSID);
				i.putExtra("STR_PARAM2", PASSWORD);
				i.putExtra("STR_PARAM3", RSSI_Threshold);
				startService(i);
				mButton_wifi_Connect.setEnabled(false);
				mButton_wifi_Disconnect.setEnabled(true);

			}

		});

		mButton_wifi_Disconnect
				.setOnClickListener(new Button.OnClickListener() {

					 
					public void onClick(View arg0) {
						activity_broadcast(WIFI_CONNECT_BROADCAST, "closewifi",
								"null", "null");
						mTextView_Connect.setText("Disconnected");

						mTextView_RSSI.setText("RSSI = " + "?" + "dBm");

						mTextView_Result.setText("");
						mButton_wifi_Disconnect.setEnabled(false);
						mButton_wifi_Connect.setEnabled(true);

						Intent i = new Intent(Basic_Combo_Tool.this,
								Basic_WiFi_Connect_Service.class);
						stopService(i);

						// TODO Auto-generated method stub
						/*
						 * Intent i = new
						 * Intent(Combo_Tool.this,mService1.class);
						 * if(stopService(i) == true) { String s =
						 * getResources().getText
						 * (R.string.str_service_offline).toString();
						 * Toast.makeText(getApplicationContext(), s,
						 * Toast.LENGTH_LONG).show();
						 * 
						 * } else { String s =
						 * getResources().getText(R.string.str_error_stopservice
						 * ).toString(); Toast.makeText(getApplicationContext(),
						 * s, Toast.LENGTH_LONG).show(); }
						 */

					}

				});

		mButton_wifi_Scan.setOnClickListener(new Button.OnClickListener() {

			 
			public void onClick(View arg0) {
				mListView_WIFIscan.setVisibility(View.VISIBLE);
				Intent i = new Intent(Basic_Combo_Tool.this,
						Basic_WiFi_Scan_Service.class);
				i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				i.putExtra("STR_PARAM1", SSID);
				i.putExtra("STR_PARAM2", PASSWORD);
				i.putExtra("STR_PARAM3", RSSI_Threshold);
				startService(i);
				mButton_wifi_Scan.setEnabled(false);
				mButton_wifi_StopScan.setEnabled(true);
			}

		});

		mButton_wifi_StopScan.setOnClickListener(new Button.OnClickListener() {

			 
			public void onClick(View arg0) {
				activity_broadcast(WIFI_SCAN_BROADCAST, "Wifi_StopScan",
						"null", "null");
				Intent i = new Intent(Basic_Combo_Tool.this,
						Basic_WiFi_Scan_Service.class);
				stopService(i);
				mButton_wifi_StopScan.setEnabled(false);
				mButton_wifi_Scan.setEnabled(true);

			}

		});

		mButton_bt_inquiry.setOnClickListener(new Button.OnClickListener() {

			 
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				mList_bt_inquiry.ClearData();
				Intent i = new Intent(Basic_Combo_Tool.this, BT_Service.class);
				i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				i.putExtra("STR_PARAM1", SSID);
				i.putExtra("STR_PARAM2", PASSWORD);
				i.putExtra("STR_PARAM3", RSSI_Threshold);
				startService(i);
				mButton_bt_inquiry.setEnabled(false);
				mButton_bt_stop_inquiry.setEnabled(true);

			}

		});

		mButton_bt_stop_inquiry
				.setOnClickListener(new Button.OnClickListener() {

					 
					public void onClick(View arg0) { // TODO Auto-generated
						activity_broadcast(BT_BROADCAST, "stopScan", "null",
								"null");
						mButton_bt_inquiry.setEnabled(true);
						mButton_bt_stop_inquiry.setEnabled(false);

						Intent i = new Intent(Basic_Combo_Tool.this,
								BT_Service.class);
						stopService(i);
					}
				});

		ListView_bt_adapter = new SimpleAdapter(this, mList_bt_inquiry
				.getData(), R.layout.vlist, new String[] { "title", "info",
				"img" }, new int[] { R.id.title, R.id.info, R.id.img });
		mListView_BTinquiry = (ListView) findViewById(R.id.ListView_bt_inquiry);
		mListView_BTinquiry.setAdapter(ListView_bt_adapter);

		ListView_wifi_adapter = new SimpleAdapter(this, mList_wifi_scan
				.getData(), R.layout.vlist, new String[] { "title", "info",
				"img" }, new int[] { R.id.title, R.id.info, R.id.img });
		mListView_WIFIscan = (ListView) findViewById(R.id.ListView_wifi_scan);
		mListView_WIFIscan.setAdapter(ListView_wifi_adapter);

		setContentView(myTabhost);
	}

	SimpleAdapter ListView_bt_adapter;
	SimpleAdapter ListView_wifi_adapter;

	private String local_start() {
		try {
			ServerSocket serverSocket = new ServerSocket(4999);
			// Socket
			Socket socket = serverSocket.accept();
			// 输入流
			InputStream in = socket.getInputStream();
			// 接收缓冲区
			byte[] msg = new byte[1024];

			for (int i = 0; i < 1024 * 5000; i++) {
				// 每次接收1204字节

				in.read(msg);

			}
			String a = new String();
			System.out.println("接受结束.");
			return in.toString();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		return "error";

	}

	public class mMessageReceiver extends BroadcastReceiver {

		 
		public void onReceive(Context context, Intent intent) {

			// TODO Auto-generated method stub
			try {
				Bundle bunde = intent.getExtras();
				String strParam1 = bunde.getString("STR_PARAM1");
				String strParam2 = bunde.getString("STR_PARAM2");
				String strParam3 = bunde.getString("STR_PARAM3");

				if (strParam1.equals("wifi_connected")) {
					mTextView_Connect.setText("Connected");

					mTextView_RSSI.setText("RSSI = " + strParam2 + "dBm");

					mTextView_Result.setText(strParam3);
					if (strParam3.equals("PASS!")) {
						mTextView_Result.setTextSize(20);
						mTextView_Result.setTextColor(Color.GREEN);
					} else {
						mTextView_Result.setTextSize(20);
						mTextView_Result.setTextColor(Color.RED);
					}

				}
				if (strParam1.equals("wifi_disconnected")) {

					mTextView_Connect.setText("Disconnected");

				} else if (strParam1.equals("MSG_WIFI")) {

					mTextView_Connect.setText(strParam2);

				}

				else if (strParam1.equals("MSG")) {

					Toast.makeText(getApplicationContext(),
							"Message: " + strParam2, Toast.LENGTH_SHORT).show();

				}

				else if (strParam1.equals("Warning")) {

					Toast.makeText(getApplicationContext(),
							"Warning: " + strParam2, Toast.LENGTH_LONG).show();
				} else if (strParam1.equals("wifi_scan_result")) {

					int wifi_icon;

					String strParam4 = bunde.getString("STR_PARAM4");
					String strParam5 = bunde.getString("STR_PARAM5");
					String strParam6 = bunde.getString("STR_PARAM6");

					if (strParam4.contains("[IBSS]"))
						wifi_icon = R.drawable.ad_hoc;
					else
						wifi_icon = R.drawable.infrastructure;

					mList_wifi_scan.addData(strParam2 + "  " + strParam3,
							strParam4 + "\n" + strParam5 + "  " + strParam6,
							wifi_icon);

				} else if (strParam1.equals("clear_wifi_scan_result")) {

					mList_wifi_scan.ClearData();

				} else if (strParam1.equals("BT_inquiry")) {

					int bt_icon;
					String strParam4 = bunde.getString("STR_PARAM4");
					String strParam5 = bunde.getString("STR_PARAM5");

					if (strParam5.equals("computer"))
						bt_icon = R.drawable.computer;
					else if (strParam5.equals("phone"))
						bt_icon = R.drawable.phone;
					else
						bt_icon = R.drawable.unknown;
					mList_bt_inquiry.addData(strParam2, strParam3, bt_icon);

				} else if (strParam1.equals("find over")) {

					mButton_bt_inquiry.setEnabled(true);
					mButton_bt_stop_inquiry.setEnabled(false);
				}

			} catch (Exception e) {
				mTextView01.setText(e.toString());
				e.getStackTrace();
			}

		}

	}

	void activity_broadcast(String type, String msg1, String msg2, String msg3) {

		Intent i = new Intent(type);
		i.putExtra("STR_PARAM1", msg1);
		i.putExtra("STR_PARAM2", msg2);
		i.putExtra("STR_PARAM3", msg3);
		sendBroadcast(i);
	}

	List_bt_inquiry mList_bt_inquiry = new List_bt_inquiry();

	public class List_bt_inquiry {

		List<Map<String, Object>> list_bt_inquiry = new ArrayList<Map<String, Object>>();

		private void addData(String title, String info, int icon) {
			Map<String, Object> map = new HashMap<String, Object>();
			map.put("title", title);
			map.put("info", info);
			map.put("img", icon);
			list_bt_inquiry.add(map);
			ListView_bt_adapter.notifyDataSetChanged();

		}

		private void ClearData() {

			list_bt_inquiry.clear();
			ListView_bt_adapter.notifyDataSetChanged();

		}

		private List<Map<String, Object>> getData() {

			return list_bt_inquiry;
		}

	}

	List_wifi_scan mList_wifi_scan = new List_wifi_scan();

	public class List_wifi_scan {

		List<Map<String, Object>> list_wifi_scan = new ArrayList<Map<String, Object>>();

		private void addData(String title, String info, int icon) {
			Map<String, Object> map = new HashMap<String, Object>();
			map.put("title", title);
			map.put("info", info);
			map.put("img", icon);
			list_wifi_scan.add(map);
			ListView_wifi_adapter.notifyDataSetChanged();

		}

		private void ClearData() {

			list_wifi_scan.clear();
			ListView_wifi_adapter.notifyDataSetChanged();

		}

		private List<Map<String, Object>> getData() {

			return list_wifi_scan;
		}

	}

}
