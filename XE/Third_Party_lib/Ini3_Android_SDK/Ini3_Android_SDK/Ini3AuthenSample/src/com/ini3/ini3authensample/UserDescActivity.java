package com.ini3.ini3authensample;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.widget.TextView;

public class UserDescActivity extends Activity {
	
	public static final String APP_PREFERENCES = "Ini3AppPrefs";
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_userdesc);
		
		SharedPreferences shared_preferences = getSharedPreferences(APP_PREFERENCES, 0);
		TextView tv_username = (TextView) findViewById(R.id.txt_username);
		tv_username.setText(shared_preferences.getString("Username", ""));
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	public void onLogoutBtnClick(View view) {
		
		SharedPreferences shared_preferences = getSharedPreferences(APP_PREFERENCES, 0);
		SharedPreferences.Editor shared_preferences_editor = shared_preferences.edit();
		shared_preferences_editor.clear().commit();
		
		Intent MainIntent = new Intent(UserDescActivity.this, MainActivity.class);
		startActivity(MainIntent);
		finish();
	}
}