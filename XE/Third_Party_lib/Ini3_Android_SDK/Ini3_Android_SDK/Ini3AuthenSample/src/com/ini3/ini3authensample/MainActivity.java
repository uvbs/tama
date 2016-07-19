package com.ini3.ini3authensample;

import android.os.Bundle;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Intent;
import android.content.SharedPreferences;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import com.facebook.*;
import com.facebook.model.GraphUser;
import com.facebook.widget.*;

import com.ini3.Ini3UserAuthen;
import com.ini3.Ini3UserAuthen.AuthenTaskListener;

public class MainActivity extends Activity {
//	158972340971062
	private Ini3UserAuthen ini3_authen;
	
	private LoginButton loginButton;
	private GraphUser user;
	private UiLifecycleHelper uiHelper;
	
	private ProgressDialog loading_pd;
	public static final String APP_PREFERENCES = "Ini3AppPrefs";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		// Facebook Connect Implement Code
		uiHelper = new UiLifecycleHelper(this, callback);
		uiHelper.onCreate(savedInstanceState);
		        
		// Facebook force logout
		if (Session.getActiveSession() != null) {
			Session.getActiveSession().closeAndClearTokenInformation();
		}
		
		loginButton = (LoginButton) findViewById(R.id.login_button);
		loginButton.setUserInfoChangedCallback(new LoginButton.UserInfoChangedCallback() {
			@Override
			public void onUserInfoFetched(GraphUser user) {
				MainActivity.this.user = user;
				updateUI();
			}
		});
		
		loading_pd = new ProgressDialog(this);
		loading_pd.setProgressStyle(ProgressDialog.STYLE_SPINNER);
		loading_pd.setMessage("Please wait...");
		loading_pd.setIndeterminate(true);
		loading_pd.setCancelable(true);
		loading_pd.setCanceledOnTouchOutside(false);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	// Username Authentication
	@SuppressLint("NewApi")
	public void onLoginBtnClick(View view) {
		EditText txt_username = (EditText)findViewById(R.id.editText1);
		EditText txt_password = (EditText)findViewById(R.id.editText2);
		
		if(txt_username.getText().toString().isEmpty() || txt_password.getText().toString().isEmpty()) {
			Toast.makeText(MainActivity.this, "WARNING: Username and Password not found!", Toast.LENGTH_LONG).show();
		} else {
			try {
				if(!loading_pd.isShowing()) {
					loading_pd.show();
				}
				ini3_authen = new Ini3UserAuthen();
				ini3_authen.authenUser(txt_username.getText().toString(), txt_password.getText().toString(), _myTaskListener);
			} catch (Exception e) {
				Log.e("DEBUG", "Error Authen: ".concat(e.getMessage()));
			}
		}
	}

	private AuthenTaskListener _myTaskListener = new AuthenTaskListener() {
		@Override
		public void onComplete(int result) {
			// TODO Auto-generated method stub
			if(loading_pd.isShowing()) {
				loading_pd.dismiss();
			}
			if(result<0) {
				EditText txt_visaid = (EditText)findViewById(R.id.editText1);
				EditText txt_password = (EditText)findViewById(R.id.editText2);
				txt_visaid.setText("");
				txt_password.setText("");
				txt_visaid.requestFocus();
				Toast.makeText(MainActivity.this, "FAIL: Authentication fail", Toast.LENGTH_LONG).show();
			} else {
				onSuccessUserAuthen();
			}
		}

		@Override
		public void onExecute(Boolean result) {
			// TODO Auto-generated method stub
			Log.i("DEBUG", "Ini3UserAuthen is onExecute process");
		}
	};
		
	private void onSuccessUserAuthen() {
		SharedPreferences shared_preferences = getSharedPreferences(APP_PREFERENCES, 0);
		SharedPreferences.Editor shared_preferences_editor = shared_preferences.edit();
		
		shared_preferences_editor.putString("UserId", Integer.toString(ini3_authen.getUserId()));
		shared_preferences_editor.putString("Username", ini3_authen.getUserName());
		shared_preferences_editor.commit();
		Log.d("DEBUG", "UserId:" + Integer.toString(ini3_authen.getUserId()) );
		Log.d("DEBUG", "Username:" + ini3_authen.getUserName() );
		
		Intent UserDescIntent = new Intent(MainActivity.this, UserDescActivity.class);
		startActivity(UserDescIntent);
		finish();
	}
		
	// Facebook Connect Implement Code
	private void updateUI() {
	       Session session = Session.getActiveSession();
	       boolean enableButtons = (session != null && session.isOpened());
	        if (enableButtons && user != null) {
        	//Toast.makeText(MainActivity.this, "FACEBOOK: Has login Id: ".concat(user.getId()), Toast.LENGTH_LONG).show();
        	try {
        		EditText txt_visaid = (EditText)findViewById(R.id.editText1);
				EditText txt_password = (EditText)findViewById(R.id.editText2);
				txt_visaid.setText("");
				txt_password.setText("");
        		ini3_authen = new Ini3UserAuthen();
				ini3_authen.authenUserFacebook(user.getId(), _myTaskListener);
			} catch (Exception e) {
				Log.e("DEBUG", "Error Authen: ".concat(e.getMessage()));
			}
        } else {
        	//TODO : If facebook is not loged-in
        }
    }
	
	private Session.StatusCallback callback = new Session.StatusCallback() {
        @Override
        public void call(Session session, SessionState state, Exception exception) {
            onSessionStateChange(session, state, exception);
        }
    };
	
	private void onSessionStateChange(Session session, SessionState state, Exception exception) {
        updateUI();
    }
		
	@Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        uiHelper.onActivityResult(requestCode, resultCode, data, dialogCallback);
    }
	
	private FacebookDialog.Callback dialogCallback = new FacebookDialog.Callback() {
        @Override
        public void onError(FacebookDialog.PendingCall pendingCall, Exception error, Bundle data) {
            //TODO : ...
        }
	        @Override
        public void onComplete(FacebookDialog.PendingCall pendingCall, Bundle data) {
        	//TODO : ...
        }
    };

}
