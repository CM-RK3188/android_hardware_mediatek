package mediatek.android.application.Advanced_Tool;
import android.util.Log;

public class ChannelInfo {
	private static final String TAG = "EM/Wifi_ChannelInfo";
    public static final int CHANNEL_NUMBER_14 = 14;
    private static final int DEFAULT_CHANNEL_COUNT = 11;
    private static final int MAX_CHANNEL_COUNT = 65;
    protected String mChannelSelect = null;
    protected static long[] sChannels = null;
    protected static boolean sHas14Ch = false;
    protected static boolean sHasUpper14Ch = false;
    final  String[] mChannelName = {
            // 2.4GHz frequency serials
            "Channel 1 [2412MHz]", "Channel 2 [2417MHz]",
            "Channel 3 [2422MHz]", "Channel 4 [2427MHz]",
            "Channel 5 [2432MHz]", "Channel 6 [2437MHz]",
            "Channel 7 [2442MHz]", "Channel 8 [2447MHz]",
            "Channel 9 [2452MHz]", "Channel 10 [2457MHz]",
            "Channel 11 [2462MHz]", "Channel 12 [2467MHz]",
            "Channel 13 [2472MHz]", "Channel 14 [2484MHz]", };

    final String[] mFullChannelName = {
            // 2.4GHz frequency serials
            "Channel 1 [2412MHz]", "Channel 2 [2417MHz]",
            "Channel 3 [2422MHz]", "Channel 4 [2427MHz]",
            "Channel 5 [2432MHz]", "Channel 6 [2437MHz]",
            "Channel 7 [2442MHz]", "Channel 8 [2447MHz]",
            "Channel 9 [2452MHz]", "Channel 10 [2457MHz]",
            "Channel 11 [2462MHz]", "Channel 12 [2467MHz]",
            "Channel 13 [2472MHz]", "Channel 14 [2484MHz]",
            // 5GHz frequency serials
            "Channel 36 [5180MHz]", "Channel 40 [5200MHz]",
            "Channel 44 [5220MHz]", "Channel 48 [5240MHz]",
            "Channel 52 [5260MHz]", "Channel 56 [5280MHz]",
            "Channel 60 [5300MHz]", "Channel 64 [5320MHz]",
            "Channel 100 [5500MHz]", "Channel 104 [5520MHz]",
            "Channel 108 [5540MHz]", "Channel 112 [5560MHz]",
            "Channel 116 [5580MHz]", "Channel 120 [5600MHz]",
            "Channel 124 [5620MHz]", "Channel 128 [5640MHz]",
            "Channel 132 [5660MHz]", "Channel 136 [5680MHz]",
            "Channel 140 [5700MHz]", "Channel 149 [5745MHz]",
            "Channel 153 [5765MHz]", "Channel 157 [5785MHz]",
            "Channel 161 [5805MHz]", "Channel 165 [5825MHz]",
            "Channel 169 [5845MHz]", "Channel 173 [5865MHz]",
            "Channel 177 [5885MHz]", "Channel 180 [5905MHz]",
            "Channel 183 [4915MHz]", "Channel 184 [4920MHz]",
            "Channel 185 [4925MHz]", "Channel 187 [4935MHz]",
            "Channel 188 [4940MHz]", "Channel 189 [4945MHz]",
            "Channel 192 [4960MHz]", "Channel 196 [4980MHz]" };
    final int[] mChannelFreq = {
            // 2.4GHz frequency serials' channels
            2412000, 2417000, 2422000, 2427000, 2432000, 2437000, 2442000,
            2447000, 2452000, 2457000, 2462000, 2467000, 2472000, 2484000,
            // 5GHz frequency serials' channels
            5180000, 5200000, 5220000, 5240000, 5260000, 5280000, 5300000,
            5320000, 5500000, 5520000, 5540000, 5560000, 5580000, 5600000,
            5620000, 5640000, 5660000, 5680000, 5700000, 5745000, 5765000,
            5785000, 5805000, 5825000, 5845000, 5865000, 5885000, 5905000,
            4915000, 4920000, 4925000, 4935000, 4940000, 4945000, 4960000,
            4980000 };

    public int getChannleIndexByName(String Channel)
    {
    	for (int i = 0; i < mFullChannelName.length; i++) {
            if (mFullChannelName[i].equals(Channel)) {
                return i;
            }
        }
        return 0;
    }
    
    public String[] getChannelName()
    {
    	return mChannelName;
    }
    
    public String[] getFullChannelName()
    {
    	return mFullChannelName;
    }
    /**
     * Get channel frequency
     * 
     * @return Channel frequency
     */
    public int getChannelFreq() {
        return mChannelFreq[getChannelIndex()];
    }

    /**
     * Get WiFi chip support channels
     */

    
    /**
     * Get index in the channel array by Channel name
     * 
     * @return The channel index
     */
    public int getChannelIndex() {
        for (int i = 0; i < mFullChannelName.length; i++) {
            if (mFullChannelName[i].equals(mChannelSelect)) {
                return i;
            }
        }
        return 0;
    }

    /**
     * Check the channel is support or not in the phone
     * 
     * @param channel
     *            Need to check the channel's number
     * @return True if phone support the channel, or false
     */
    public boolean isContains(int channel) {
        for (int i = 1; i <= sChannels[0]; i++) {
            if (channel == sChannels[i]) {
                return true;
            }
        }
        return false;
    }
    
    public ChannelInfo() {
        mChannelSelect = mFullChannelName[0];
    }
}
