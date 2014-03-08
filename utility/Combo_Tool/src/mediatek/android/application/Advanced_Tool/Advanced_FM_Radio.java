package mediatek.android.application.Advanced_Tool;

import mediatek.android.application.R;
import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

public class Advanced_FM_Radio extends Activity{
	
	private Button mBtnTune;
	private EditText mEtFreq;
	private ImageButton mImbutMinusFreq;
	private ImageButton mImbutPlusFreq;
	private TextView mTvRssi;
	private RadioGroup mRadioGroup;
	private RadioButton mRadioBLongAntenna;
	private RadioButton mRadioBShortAntenna;
	
	private int mFMR_Freq = 0;
	private int iRssi = 0;
	private int FMR_DEFAULT = 900;
	private int FMR_MIN_FREQ = 875;
	private int FMR_MAX_FREQ = 1080;
	private int LONG_ANTENNA = 0;
	private int SHORT_ANTENNA = 1;
	private int iAntenna = LONG_ANTENNA;
	
	FMRadioNative mFmRadio = null;
	
	private static final String TAG = "Advanced_FM_Radio";
	private static final int HANDLER_EVENT_RX = 2;
	protected static final long HANDLER_RX_DELAY_TIME = 1000;
	
	@Override
	public void onDestroy() {
		// TODO Auto-generated method stub
		mHandler.removeMessages(HANDLER_EVENT_RX);
		super.onDestroy();
	}
	
	@Override
	public void onStop() {
		// TODO Auto-generated method stub
		mHandler.removeMessages(HANDLER_EVENT_RX);
		super.onStop();
	}
	
	@Override
	public void onPause() {
		// TODO Auto-generated method stub
		mHandler.removeMessages(HANDLER_EVENT_RX);
		super.onPause();
	}
	
	private final Handler mHandler = new Handler() {

        public void handleMessage(Message msg) {
           
            if (HANDLER_EVENT_RX == msg.what) {              
            	iRssi = mFmRadio.readRssi();
				mTvRssi.setText(String.valueOf(iRssi));                
            }
            mHandler.sendEmptyMessageDelayed(HANDLER_EVENT_RX, HANDLER_RX_DELAY_TIME);
        }
    };
	
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		View contentView = LayoutInflater.from(this.getParent()).inflate(R.layout.advanced_fm_radio, null);
		setContentView(contentView);
		
		mBtnTune = (Button)findViewById(R.id.FMR_Freq_tune);
		mEtFreq = (EditText)findViewById(R.id.FMR_Freq_edit);
		mImbutMinusFreq = (ImageButton)findViewById(R.id.minusFrequency);
		mImbutPlusFreq = (ImageButton)findViewById(R.id.plusFrequency);
		mTvRssi = (TextView)findViewById(R.id.FMR_Status_RSSI);
		mRadioGroup = (RadioGroup)findViewById(R.id.FMR_Antenna_type);;
		mRadioBLongAntenna = (RadioButton)findViewById(R.id.FMR_Antenna_long);;
		mRadioBShortAntenna = (RadioButton)findViewById(R.id.FMR_Antenna_short);;
		
		mFmRadio = new FMRadioNative();
		mFMR_Freq = (int)(Float.parseFloat(String.valueOf(mEtFreq.getText())) * 10);
		
		mRadioBLongAntenna.setChecked(true);
		
		mRadioGroup.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
			
			public void onCheckedChanged(RadioGroup group, int CheckedID) {
				// TODO Auto-generated method stub
				if (CheckedID == mRadioBLongAntenna.getId())
				{
					iAntenna = LONG_ANTENNA;
				}
				else if (CheckedID == mRadioBShortAntenna.getId())
				{
					iAntenna = SHORT_ANTENNA;
				}
				
				mFmRadio.switchAntenna(iAntenna);
			}
		});
		
		mBtnTune.setOnClickListener(new Button.OnClickListener() {

			public void onClick(View arg0) {
				try
				{
					mFMR_Freq = (int)(Float.parseFloat(String.valueOf(mEtFreq.getText())) * 10);
				}catch (Exception e)
				{
					Toast
					.makeText(
							getApplicationContext(),
							" Freq is invalid ! \n  Use default 90.0",
							Toast.LENGTH_LONG).show();
					mFMR_Freq = FMR_DEFAULT;
					mEtFreq.setText(String.valueOf((float)mFMR_Freq / 10));
				}
				if (mFMR_Freq > FMR_MAX_FREQ || mFMR_Freq < FMR_MIN_FREQ)
				{
					mFMR_Freq = FMR_DEFAULT;
					mEtFreq.setText(String.valueOf((float)mFMR_Freq / 10));
					return;
				}
				
				mFmRadio.opendev();
				mFmRadio.powerup(FMR_DEFAULT);
				mFmRadio.tune(mFMR_Freq);
				
				SystemClock.sleep(2000);
				
				mHandler.sendEmptyMessage(HANDLER_EVENT_RX);
			}
		});
		
		mImbutMinusFreq.setOnClickListener(new Button.OnClickListener() {

			public void onClick(View arg0) {
			
				mFMR_Freq = (int)(Float.parseFloat(String.valueOf(mEtFreq.getText())) * 10);
				if (mFMR_Freq > FMR_MAX_FREQ || mFMR_Freq < FMR_MIN_FREQ)
				{
					mFMR_Freq = FMR_DEFAULT;
					mEtFreq.setText(String.valueOf((float)mFMR_Freq / 10));
					return;
				}
				
				if (0 != mFMR_Freq && FMR_MIN_FREQ < mFMR_Freq)
				{
					mFMR_Freq = mFMR_Freq - 1;
				}
				
				mEtFreq.setText(String.valueOf((float)mFMR_Freq / 10));
			}
		});
		
		mImbutPlusFreq.setOnClickListener(new Button.OnClickListener() {

			public void onClick(View arg0) {
			
				mFMR_Freq = (int)(Float.parseFloat(String.valueOf(mEtFreq.getText())) * 10);
				if (mFMR_Freq > FMR_MAX_FREQ || mFMR_Freq < FMR_MIN_FREQ)
				{
					mFMR_Freq = FMR_DEFAULT;
					mEtFreq.setText(String.valueOf((float)mFMR_Freq / 10));
					return;
				}
				
				if (0 != mFMR_Freq && FMR_MAX_FREQ > mFMR_Freq)
				{
					mFMR_Freq = mFMR_Freq + 1;
				}
				
				mEtFreq.setText(String.valueOf((float)mFMR_Freq / 10));
			}
		});
	}
}
