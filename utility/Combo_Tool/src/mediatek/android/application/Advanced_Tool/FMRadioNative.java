package mediatek.android.application.Advanced_Tool;

public class FMRadioNative {
	static {
        System.loadLibrary("hwtest_fm_jni");
    }

    /**
     * open fm device, call before power up
     * 
     * @return (true,success; false, failed)
     */
    public native boolean opendev();

    /**
     * close fm device, call after power down
     * 
     * @return (true, success; false, failed)
     */
    public native boolean closedev();

    /**
     * power up FM with frequency use long antenna
     * 
     * @param frequency
     *            frequency(100KHZ, 875)
     * @return (true, success; false, failed)
     */
    public native boolean powerup(int frequency);

    /**
     * power down FM
     * 
     * @param type
     *            (0, FMRadio; 1, FMTransimitter)
     * @return (true, success; false, failed)
     */
    public native boolean powerdown(int type);

    /**
     * tune to frequency
     * 
     * @param frequency
     *            frequency(100KHZ, 875)
     * @return (true, success; false, failed)
     */
    public native boolean tune(int frequency);
    
    /**
     * get rssi from hardware(use for engineer mode)
     * 
     * @return rssi value
     */
    public native int readRssi();
    
    /**
     * switch antenna
     * 
     * @param antenna
     *            antenna (0, long antenna, 1 short antenna)
     * @return (0, success; 1 failed; 2 not support)
     */
    static native int switchAntenna(int antenna);
}
