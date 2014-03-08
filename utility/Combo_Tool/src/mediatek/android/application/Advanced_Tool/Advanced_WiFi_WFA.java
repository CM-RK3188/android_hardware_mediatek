package mediatek.android.application.Advanced_Tool;

import mediatek.android.application.R;
import android.app.Activity;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;

public class Advanced_WiFi_WFA extends Activity {
	@Override
	public void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
	}
	
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		View contentView = LayoutInflater.from(this.getParent()).inflate(R.layout.advanced_wifi_wfa, null);
		setContentView(contentView);
		
		
	}
}

