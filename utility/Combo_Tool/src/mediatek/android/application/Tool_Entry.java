package mediatek.android.application;

import mediatek.android.application.Advanced_Tool.Advanced_Combo_Tool;
import mediatek.android.application.Basic_Tool.Basic_Combo_Tool;
import mediatek.android.application.Basic_Tool.Basic_WiFi_Connect_Service;
import mediatek.android.application.Basic_Tool.Basic_WiFi_Scan_Service;
import android.app.Activity;
import android.app.TabActivity;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.TabHost;
import android.widget.Toast;

public class Tool_Entry extends Activity {
	@Override
	public void onDestroy() {
		// TODO Auto-generated method stub
		System.exit(0);
		super.onDestroy();
	}

	private Button basciButton; 
	private Button AdvancedButton;
	
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		
		
		
		setContentView(R.layout.main);
		
		basciButton = (Button)findViewById(R.id.Basic_Tool);
		AdvancedButton = (Button)findViewById(R.id.Advanced_Tool);
		
		basciButton.setOnClickListener(new Button.OnClickListener() {
			
			public void onClick(View arg0) {
				Intent iBasicTool = new Intent(Tool_Entry.this,
						Basic_Combo_Tool.class);
				iBasicTool.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				
				startActivity(iBasicTool);

			}

		});
		
		AdvancedButton.setOnClickListener(new Button.OnClickListener() {
			
			public void onClick(View arg0) {
				Intent iAdvacedTool = new Intent(Tool_Entry.this,
						Advanced_Combo_Tool.class);
				
						
				iAdvacedTool.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				startActivity(iAdvacedTool);
				Toast.makeText(
						getApplicationContext(),
						"Please waiting for Wifi Initialzing ...",
						Toast.LENGTH_LONG).show();
			}

		});
	}
}