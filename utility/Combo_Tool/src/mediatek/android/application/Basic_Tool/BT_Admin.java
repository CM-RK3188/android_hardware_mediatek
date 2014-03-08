package mediatek.android.application.Basic_Tool;

import java.util.List;

import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.NetworkInfo.State;
import android.util.Log;

public class BT_Admin {
	// ����BTManager����
	BluetoothAdapter mBTAdapter;
	// ����һ��BTLock
	// ������
	Context Context;

	public BT_Admin(Context context) {

		Context = context;
		mBTAdapter = BluetoothAdapter.getDefaultAdapter();
	}

	// ��BT
	public void OpenBT() {

		if (mBTAdapter == null)
			return;

		else if (!mBTAdapter.isEnabled()) {
			mBTAdapter.enable();
		}
	}

	public boolean isBTEnabled() {

		if (mBTAdapter == null)
			return false;
		else
			return mBTAdapter.isEnabled();

	}

	// �ر�BT
	public void CloseBT() {
		if (mBTAdapter == null)
			return;
		else if (mBTAdapter.isEnabled()) {
			mBTAdapter.disable();
		}
	}

	public void StartScan() {
		if (mBTAdapter == null)
			return;
		else
			mBTAdapter.startDiscovery();

	}

	public void StopScan() {
		if (mBTAdapter == null)
			return;
		else
			mBTAdapter.cancelDiscovery();

	}

	public boolean isBTScanning() {

		mBTAdapter = BluetoothAdapter.getDefaultAdapter();
		if (mBTAdapter == null)
			return false;
		else
			return mBTAdapter.isDiscovering();

	}

	public void GetCount() {

	}
	/*
	 * // ����BTLock public void AcquireBTLock() { mBTLock.acquire(); }
	 * 
	 * // ����BTLock public void ReleaseBTLock() { // �ж�ʱ������ if (mBTLock.isHeld())
	 * { mBTLock.acquire(); } }
	 * 
	 * // ����һ��BTLock public void CreatBTLock() { mBTLock =
	 * mBTManager.createBTLock("Test"); }
	 * 
	 * // �õ����úõ����� public List GetConfiguration() { return mBTConfiguration; }
	 * 
	 * // ָ�����úõ������������ public void ConnectConfiguration(int wcgID) { //
	 * �����������úõ������������� // if(wcgID > mBTConfiguration.size()) // { // return; //
	 * } // �������úõ�ָ��ID������ mBTManager.enableNetwork(wcgID, true);
	 * 
	 * }
	 * 
	 * 
	 * 
	 * // �õ������б� public List GetBTList() { return mBTList; }
	 * 
	 * // �鿴ɨ���� public StringBuilder LookUpScan() { StringBuilder stringBuilder
	 * = new StringBuilder(); for (int i = 0; i < mBTList.size(); i++) {
	 * stringBuilder .append("Index_" + new Integer(i + 1).toString() + ":"); //
	 * ��ScanResult��Ϣת����һ���ַ����� // ���аѰ�����BSSID��SSID��capabilities��frequency��level
	 * String TAG = "BTAdmin"; Log.d(TAG, (mBTList.get(i)).toString());
	 * stringBuilder.append((mBTList.get(i)).toString());
	 * stringBuilder.append("\n"); } return stringBuilder; }
	 * 
	 * // �õ�MAC��ַ public String GetMacAddress() { mBTInfo =
	 * mBTManager.getConnectionInfo(); return (mBTInfo == null) ? "NULL" :
	 * mBTInfo.getMacAddress(); }
	 * 
	 * // �õ�������BSSID public String GetBSSID() { mBTInfo =
	 * mBTManager.getConnectionInfo(); return (mBTInfo == null) ? "NULL" :
	 * mBTInfo.getBSSID(); }
	 * 
	 * public String GetSSID() { mBTInfo = mBTManager.getConnectionInfo();
	 * return (mBTInfo == null) ? "NULL" : mBTInfo.getSSID(); }
	 * 
	 * // �õ�IP��ַ public int GetIPAddress() { mBTInfo =
	 * mBTManager.getConnectionInfo(); return (mBTInfo == null) ? 0 :
	 * mBTInfo.getIpAddress(); }
	 * 
	 * // �õ����ӵ�ID public int GetNetworkId() { mBTInfo =
	 * mBTManager.getConnectionInfo(); return (mBTInfo == null) ? 0 :
	 * mBTInfo.getNetworkId(); }
	 * 
	 * // �õ����ӵ�ID public int GetRssi() { mBTInfo =
	 * mBTManager.getConnectionInfo(); return (mBTInfo == null) ? 0 :
	 * mBTInfo.getRssi(); }
	 * 
	 * // �õ�BTInfo��������Ϣ�� public String GetBTInfo() { mBTInfo =
	 * mBTManager.getConnectionInfo(); return (mBTInfo == null) ? "NULL" :
	 * mBTInfo.toString(); }
	 * 
	 * // ���һ�����粢���� public void AddNetwork(BTConfiguration wcg) { int wcgID =
	 * mBTManager.addNetwork(wcg); mBTManager.enableNetwork(wcgID, true);
	 * 
	 * }
	 * 
	 * // �Ͽ�ָ��ID������ public void DisconnectBT(int netId) {
	 * mBTManager.disableNetwork(netId); mBTManager.disconnect(); }
	 * 
	 * public BTConfiguration CreateBTInfo(String SSID, String Password, int
	 * Type) { BTConfiguration config = new BTConfiguration();
	 * config.allowedAuthAlgorithms.clear(); config.allowedGroupCiphers.clear();
	 * config.allowedKeyManagement.clear();
	 * config.allowedPairwiseCiphers.clear(); config.allowedProtocols.clear();
	 * config.SSID = "\"" + SSID + "\"";
	 * 
	 * BTConfiguration tempConfig = this.IsExsits(SSID); if (tempConfig != null)
	 * { mBTManager.removeNetwork(tempConfig.networkId); }
	 * 
	 * if (Type == 1) // BTCIPHER_NOPASS { config.wepKeys[0] = "";
	 * config.allowedKeyManagement.set(BTConfiguration.KeyMgmt.NONE);
	 * config.wepTxKeyIndex = 0; } if (Type == 2) // BTCIPHER_WEP {
	 * config.hiddenSSID = true; config.wepKeys[0] = Password;
	 * config.allowedAuthAlgorithms .set(BTConfiguration.AuthAlgorithm.SHARED);
	 * config.allowedGroupCiphers.set(BTConfiguration.GroupCipher.CCMP);
	 * config.allowedGroupCiphers.set(BTConfiguration.GroupCipher.TKIP);
	 * config.allowedGroupCiphers.set(BTConfiguration.GroupCipher.WEP40);
	 * config.allowedGroupCiphers .set(BTConfiguration.GroupCipher.WEP104);
	 * config.allowedKeyManagement.set(BTConfiguration.KeyMgmt.NONE);
	 * config.wepTxKeyIndex = 0; } if (Type == 3) // BTCIPHER_WPA {
	 * config.preSharedKey = "\"" + Password + "\""; config.hiddenSSID = true;
	 * config.allowedAuthAlgorithms .set(BTConfiguration.AuthAlgorithm.OPEN);
	 * config.allowedGroupCiphers.set(BTConfiguration.GroupCipher.TKIP);
	 * config.allowedKeyManagement.set(BTConfiguration.KeyMgmt.WPA_PSK);
	 * config.allowedPairwiseCiphers .set(BTConfiguration.PairwiseCipher.TKIP);
	 * // config.allowedProtocols.set(BTConfiguration.Protocol.WPA);
	 * config.allowedGroupCiphers.set(BTConfiguration.GroupCipher.CCMP);
	 * config.allowedPairwiseCiphers .set(BTConfiguration.PairwiseCipher.CCMP);
	 * config.status = BTConfiguration.Status.ENABLED; } return config; }
	 * 
	 * private BTConfiguration IsExsits(String SSID) { List<BTConfiguration>
	 * existingConfigs = mBTManager .getConfiguredNetworks(); for
	 * (BTConfiguration existingConfig : existingConfigs) { if
	 * (existingConfig.SSID.equals("\"" + SSID + "\"")) { return existingConfig;
	 * } } return null; }
	 * 
	 * public boolean isConnected() { ConnectivityManager cm =
	 * (ConnectivityManager) Context
	 * .getSystemService(Context.CONNECTIVITY_SERVICE); NetworkInfo ni =
	 * cm.getNetworkInfo(ConnectivityManager.TYPE_BT);
	 * System.out.println(ni.getState());
	 * System.out.println(ni.getDetailedState() + "----"); if (ni.getState() ==
	 * State.CONNECTED) return true; return false; }
	 */
}
