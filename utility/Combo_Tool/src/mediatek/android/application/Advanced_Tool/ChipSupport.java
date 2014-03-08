package mediatek.android.application.Advanced_Tool;

public class ChipSupport {

    /**
     * Chip support
     */
    public static final int MTK_UNKNOWN_SUPPORT = 0;
    public static final int MTK_6573_SUPPORT = 1;
    public static final int MTK_6516_SUPPORT = 2;
    public static final int MTK_6575_SUPPORT = 4;
    public static final int MTK_6577_SUPPORT = 8;

    /**
     * Feature support
     */
    public static final int MTK_FM_SUPPORT = 0;
    public static final int MTK_FM_TX_SUPPORT = 1;
    public static final int MTK_RADIO_SUPPORT = 2;
    public static final int MTK_AGPS_APP = 3;
    public static final int MTK_GPS_SUPPORT = 4;
    public static final int HAVE_MATV_FEATURE = 5;
    public static final int MTK_BT_SUPPORT = 6;
    public static final int MTK_WLAN_SUPPORT = 7;
    public static final int MTK_TTY_SUPPORT = 8;
    public static final int MTK_NFC_SUPPORT = 9;

    /**
     * Get Chip ID from native
     * 
     * @return Chip ID
     */
    public static native int getChip();

    /**
     * Check feature supported
     * 
     * @param featureId
     *            The feature ID
     * @return True if feature is supported
     */
    public static native boolean isFeatureSupported(int featureId);

    static {
        System.loadLibrary("em_support_jni");
    }
}
