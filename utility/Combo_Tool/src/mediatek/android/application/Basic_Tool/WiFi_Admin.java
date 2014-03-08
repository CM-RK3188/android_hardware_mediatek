package mediatek.android.application.Basic_Tool;

import java.util.List;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.NetworkInfo.State;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiManager.WifiLock;
import android.util.Log;

public class WiFi_Admin {
	// ����WifiManager����
	private WifiManager mWifiManager;
	// ����WifiInfo����
	private WifiInfo mWifiInfo;
	// ɨ��������������б�
	private List<ScanResult> mWifiList;
	// ���������б�
	private List mWifiConfiguration;
	// ����һ��WifiLock
	WifiLock mWifiLock;
	// ������
	Context WifiAdmin_context;

	public WiFi_Admin(Context context) {
		WifiAdmin_context = context;
		// ȡ��WifiManager����
		mWifiManager = (WifiManager) context
				.getSystemService(Context.WIFI_SERVICE);
		// ȡ��WifiInfo����
		mWifiInfo = mWifiManager.getConnectionInfo();

	}

	// ��WIFI
	public void OpenWifi() {
		if (!mWifiManager.isWifiEnabled()) {
			mWifiManager.setWifiEnabled(true);

		}
	}

	public boolean isWifiEnabled() {

		return mWifiManager.isWifiEnabled();

	}

	// �ر�WIFI
	public void CloseWifi() {
		if (mWifiManager.isWifiEnabled()) {
			mWifiManager.setWifiEnabled(false);
		}
	}

	// ����WifiLock
	public void AcquireWifiLock() {
		mWifiLock.acquire();
	}

	// ����WifiLock
	public void ReleaseWifiLock() {
		// �ж�ʱ������
		if (mWifiLock.isHeld()) {
			mWifiLock.acquire();
		}
	}

	// ����һ��WifiLock
	public void CreatWifiLock() {
		mWifiLock = mWifiManager.createWifiLock("Test");
	}

	// �õ����úõ�����
	public List GetConfiguration() {
		return mWifiConfiguration;
	}

	// ָ�����úõ������������
	public void ConnectConfiguration(int wcgID) {
		// �����������úõ�������������
		// if(wcgID > mWifiConfiguration.size())
		// {
		// return;
		// }
		// �������úõ�ָ��ID������
		mWifiManager.enableNetwork(wcgID, true);

	}

	public void StartScan() {
		mWifiManager.startScan();
		// �õ�ɨ����

		// �õ����úõ���������
		mWifiConfiguration = mWifiManager.getConfiguredNetworks();
	}

	// �õ������б�
	public List<ScanResult> GetWifiList() {
		mWifiList = mWifiManager.getScanResults();
		return mWifiList;
	}

	// �鿴ɨ����
	public StringBuilder LookUpScan() {
		StringBuilder stringBuilder = new StringBuilder();
		for (int i = 0; i < mWifiList.size(); i++) {
			stringBuilder
					.append("Index_" + new Integer(i + 1).toString() + ":");
			// ��ScanResult��Ϣת����һ���ַ�����
			// ���аѰ�����BSSID��SSID��capabilities��frequency��level
			String TAG = "WifiAdmin";
			Log.d(TAG, (mWifiList.get(i)).toString());
			stringBuilder.append((mWifiList.get(i)).toString());
			stringBuilder.append("\n");
		}
		return stringBuilder;
	}

	// �õ�MAC��ַ
	public String GetMacAddress() {
		mWifiInfo = mWifiManager.getConnectionInfo();
		return (mWifiInfo == null) ? "NULL" : mWifiInfo.getMacAddress();
	}

	// �õ�������BSSID
	public String GetBSSID() {
		mWifiInfo = mWifiManager.getConnectionInfo();
		return (mWifiInfo == null) ? "NULL" : mWifiInfo.getBSSID();
	}

	public String GetSSID() {
		mWifiInfo = mWifiManager.getConnectionInfo();
		return (mWifiInfo == null) ? "NULL" : mWifiInfo.getSSID();
	}

	// �õ�IP��ַ
	public int GetIPAddress() {
		mWifiInfo = mWifiManager.getConnectionInfo();
		return (mWifiInfo == null) ? 0 : mWifiInfo.getIpAddress();
	}

	// �õ����ӵ�ID
	public int GetNetworkId() {
		mWifiInfo = mWifiManager.getConnectionInfo();
		return (mWifiInfo == null) ? 0 : mWifiInfo.getNetworkId();
	}

	// �õ����ӵ�ID
	public int GetRssi() {
		mWifiInfo = mWifiManager.getConnectionInfo();
		return (mWifiInfo == null) ? 0 : mWifiInfo.getRssi();
	}

	// �õ�WifiInfo��������Ϣ��
	public String GetWifiInfo() {
		mWifiInfo = mWifiManager.getConnectionInfo();
		return (mWifiInfo == null) ? "NULL" : mWifiInfo.toString();
	}

	// ���һ�����粢����
	public void AddNetwork(WifiConfiguration wcg) {
		int wcgID = mWifiManager.addNetwork(wcg);
		mWifiManager.enableNetwork(wcgID, true);

	}

	// �Ͽ�ָ��ID������
	public void DisconnectWifi(int netId) {
		mWifiManager.disableNetwork(netId);
		mWifiManager.disconnect();
	}

	public WifiConfiguration CreateWifiInfo(String SSID, String Password,
			int Type) {
		WifiConfiguration config = new WifiConfiguration();
		config.allowedAuthAlgorithms.clear();
		config.allowedGroupCiphers.clear();
		config.allowedKeyManagement.clear();
		config.allowedPairwiseCiphers.clear();
		config.allowedProtocols.clear();
		config.SSID = "\"" + SSID + "\"";

		WifiConfiguration tempConfig = this.IsExsits(SSID);
		if (tempConfig != null) {
			mWifiManager.removeNetwork(tempConfig.networkId);
		}

		if (Type == 1) // WIFICIPHER_NOPASS
		{
			config.wepKeys[0] = "";
			config.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.NONE);
			config.wepTxKeyIndex = 0;
		}
		if (Type == 2) // WIFICIPHER_WEP
		{
			config.hiddenSSID = true;
			config.wepKeys[0] = Password;
			config.allowedAuthAlgorithms
					.set(WifiConfiguration.AuthAlgorithm.SHARED);
			config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.CCMP);
			config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.TKIP);
			config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.WEP40);
			config.allowedGroupCiphers
					.set(WifiConfiguration.GroupCipher.WEP104);
			config.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.NONE);
			config.wepTxKeyIndex = 0;
		}
		if (Type == 3) // WIFICIPHER_WPA
		{
			config.preSharedKey = "\"" + Password + "\"";
			config.hiddenSSID = true;
			config.allowedAuthAlgorithms
					.set(WifiConfiguration.AuthAlgorithm.OPEN);
			config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.TKIP);
			config.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.WPA_PSK);
			config.allowedPairwiseCiphers
					.set(WifiConfiguration.PairwiseCipher.TKIP);
			// config.allowedProtocols.set(WifiConfiguration.Protocol.WPA);
			config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.CCMP);
			config.allowedPairwiseCiphers
					.set(WifiConfiguration.PairwiseCipher.CCMP);
			config.status = WifiConfiguration.Status.ENABLED;
		}
		return config;
	}

	private WifiConfiguration IsExsits(String SSID) {
		List<WifiConfiguration> existingConfigs = mWifiManager
				.getConfiguredNetworks();
		for (WifiConfiguration existingConfig : existingConfigs) {
			if (existingConfig.SSID.equals("\"" + SSID + "\"")) {
				return existingConfig;
			}
		}
		return null;
	}

	public boolean isConnected() {
		ConnectivityManager cm = (ConnectivityManager) WifiAdmin_context
				.getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo ni = cm.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
		System.out.println(ni.getState());
		System.out.println(ni.getDetailedState() + "----");
		if (ni.getState() == State.CONNECTED)
			return true;
		return false;
	}

}
