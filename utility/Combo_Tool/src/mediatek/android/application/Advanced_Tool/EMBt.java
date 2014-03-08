package mediatek.android.application.Advanced_Tool;

public class EMBt {
	private int mPatter;
    private int mChannels;
    private int mPocketType;
    private int mPocketTypeLen;
    private int mFreq;
    private int mPower;
    public boolean mBTIsInit;
    
    static {
        System.loadLibrary("hwtest_bt_jni");
    }

    // this function is only called in BLE feature test
//    public native int getChipId();

//    public native int init();

    /*
     * -1: error 0: BLE is not supported 1: BLE is supported
     */
//    public native int isBLESupport();

    public native int Bt_init();
    public native int Bt_deinit();

    public native boolean noSigRxTestStart(int nPatternIdx, int nPocketTypeIdx,
            int nFreq, int nAddress);

    public native int[] noSigRxTestResult();
    
    public native boolean TxOnlyTestStart(int nPatternIdx, int nChannel, int nFreq, 
    			int nPacketTypeIdx, int nPacketLen);
    public native boolean TxOnlyTestEnd();

    public native boolean TestModeEnable(int iLevel);
    public native boolean TestModeDisable();
    
//    public native int unInit(); // for uninitialize

//    public native char[] hciCommandRun(char[] hciCmd, int cmdLength);

//    public native char[] hciReadEvent();

 //   public native int relayerStart(int portNumber, int serialSpeed);

 //   public native int relayerExit();

 //   public native int getChipIdInt();

 //   public native int getChipEcoNum();

 //   public native char[] getPatchId();

 //   public native long getPatchLen();

    public EMBt() {
        mPatter = -1;
        mChannels = -1;
        mPocketType = -1;
        mPocketTypeLen = 0;
        mFreq = 0;
        mPower = 7;
        mBTIsInit = false;
    }

    public int getPatter() {
        return mPatter;
    }

    public int getChannels() {
        return mChannels;
    }

    public int getPocketType() {
        return mPocketType;
    }

    public int getPocketTypeLen() {
        return mPocketTypeLen;
    }

    public int getFreq() {
        return mFreq;
    }

    public int getPower() {
        return mPower;
    }

    public void setPatter(int val) {
        mPatter = val;
    }

    public void setChannels(int val) {
        mChannels = val;
    }

    public void setPocketType(int val) {
        mPocketType = val;
    }

    public void setPocketTypeLen(int val) {
        mPocketTypeLen = val;
    }

    public void setFreq(int val) {
        mFreq = val;
    }

    public void setPower(int val) {
        mPower = val;
    }
}
