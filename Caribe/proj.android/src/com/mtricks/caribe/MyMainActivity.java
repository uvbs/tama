package com.mtricks.caribe;

import java.io.IOException;

import java.util.concurrent.atomic.AtomicInteger;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;

import org.json.JSONArray;
import org.json.JSONObject;

/*
//인앱
import com.example.android.trivialdrivesample.util.IabHelper;
import com.example.android.trivialdrivesample.util.IabResult;
import com.example.android.trivialdrivesample.util.Inventory;
import com.example.android.trivialdrivesample.util.Purchase;
*/

// gcm
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GooglePlayServicesUtil;
import com.google.android.gms.gcm.GoogleCloudMessaging;
import com.mtricks.caribe.R;
import com.mtricks.xe.Cocos2dxHelper;
import com.mtricks.xe.Cocos2dxHelper.Cocos2dxHelperListener;
import com.mtricks.xe.XeActivity;

// gcm 관련
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.URL;
import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.BasicResponseHandler;
import org.apache.http.impl.client.DefaultHttpClient;


public class MyMainActivity extends XeActivity implements Cocos2dxHelperListener {
	public static Version ver = new Version();
	static String[] SKU_GEM;
	
	public static MyMainActivity myActivity;
	
	static {
		System.loadLibrary("xlib_test");
	}
	
	static native String JavaToCpp(String id, String param1, String param2);
	static public String CppToJava(String id, String param1, String param2)		// C++ - > Java
	{
		Log.v(TAG, "id : \"" + id + "\" param1 : \"" + param1 + "\" parma2 : \"" + param2 +"\"" );
		
//		if(id.equals("gcm_regid"))	{
		if( id.equals("gcm_regid") ) {
			return myActivity.regid;
    	} else 
    	if(id.equals("buy")) {
//			myActivity.onBuyProduct(param1, null );
	
			return "";
    	} else {
    		Log.e(TAG, "CppToJava:unknown id:" + id); 
    	}
		return "";
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		if(Build.VERSION.SDK_INT >= 9)	// 2.3버전 이상
			setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);
		
		myActivity = this;
		GcmBroadcastReceiver.isAppOn = true;
		
		gcmInit();
		
//		SendPush("hello xuzhu!");
		Log.d( TAG, "model:" + Cocos2dxHelper.getDeviceModel() );
		
		if(Version._XKOREA) {
		    Log.d(TAG, "country: Korea");
		} else
		if(Version._XTHAILAND) {
		    Log.d(TAG, "country: Thailand");
		} else
		if(Version._XINDONESIA) {
		    Log.d(TAG, "country: Indonesia");
		} else 
		if(Version._SOFTNYX) {
		    Log.d(TAG, "country: softnyx");
		} else {
		    Log.e(TAG, "country: unknown");
		}
		// 
		if(Version._XUNIVERSAL) {
		    Log.d(TAG, "function: universal");
		} 
		if( Version._XFACEBOOK ) {
		    Log.d(TAG, "function: facebook");
		}
		if( Version._XGOOGLE_IAP ) {
		    Log.d(TAG, "function: Google InApp");
		}
		if( Version._XINI3 ) {
		    Log.d(TAG, "country: Thailand ini3");
		}
		if(Version._XKOREA) { 
			SKU_GEM = new String[5];
			SKU_GEM[0] = "inapp_cash00001";
			SKU_GEM[1] = "inapp_cash00002";
			SKU_GEM[2] = "inapp_cash00003";
			SKU_GEM[3] = "inapp_cash00004";
			SKU_GEM[4] = "inapp_cash0000500";
			/*
			SKU_GEM[0] = "mtricks_mw_kr_carrot006";
			SKU_GEM[1] = "mtricks_mw_kr_carrot012";
			SKU_GEM[2] = "mtricks_mw_kr_carrot030";
			SKU_GEM[3] = "mtricks_mw_kr_carrot060";
			SKU_GEM[4] = "mtricks_mw_kr_carrot085";
			*/
		} else
		if(Version._XTHAILAND) {
			SKU_GEM = new String[6];
			SKU_GEM[0] = "mtricks_epic_thl_gem002";
			SKU_GEM[1] = "mtricks_epic_thl_gem011";
			SKU_GEM[2] = "mtricks_epic_thl_gem023";
			SKU_GEM[3] = "mtricks_epic_thl_gem035";
			SKU_GEM[4] = "mtricks_epic_thl_gem060";
			SKU_GEM[5] = "mtricks_epic_thl_gem130";
		} else
		if(Version._XINDONESIA) { 
			SKU_GEM = new String[5];
			SKU_GEM[0] = "xtest_gem006";
			SKU_GEM[1] = "xtest_gem012";
			SKU_GEM[2] = "xtest_gem30";
			SKU_GEM[3] = "xtest_gem60";
			SKU_GEM[4] = "xtest_gem85";
		}

		
		if(Version._XGOOGLE_IAP) {
			String base64EncodedPublicKey;
			if(Version._XKOREA) {
				// 가짜 키(해킹 혼란용)
				base64EncodedPublicKey = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAm8bv6VAFIJdxVqUWDNqJvVVQ64jfMT2Iwd9NZNg8AX8XWrjhU7rljC7RGb+p+7XhGwty3LMS4fj8ooxwVz9Ce6cCXm82nqqf+JBMqcng8p3QbcmfKXBEoEZc6qBptPJe4i0uExPo5ncfX+14OhvYqBg0wHE4BTp9wPRKU4IygdPWzdp/n7bM3MhhNctWtDOl+q40cykPLpRhACNNk6jUijTB3A5I+lbHDObXTyUBkagNDF4Rq53RjGavTc1773mJKRKe1zzheV5FtyNIH9OWf4VQ1y3iUXXxkmkcebh1jaZhVzIbq1lozZ9wQPMSA4cRpqG7PpN8bgCX/MK6JAZwNQIDAQAB";
				Log.d("caribe", "key:" + base64EncodedPublicKey );
//				base64EncodedPublicKey = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAm5PqbcvMo7gjcKafqQ24mfI67rL/kF8vQFoVXqZnv/7WaUkFUqD9MdUWsDPhg1rb+3P4UHjnpE7a+JnUefnlJurI1edgifrkgtQtcmT9acrIctqTXbSHglEosceVF+R/1W3lD2aDp7TczJyhKUNH38ivRKC460479sUvqy4I9yNB8eN7t6KcBPvQ6t0eF5e65yd9pXq08srrNYSaAjNWFJdYMDG3X8P/MBFo+R0vUXtfz34uceYGDyMi/J2J4grng9KF1fcunOsgPnQiYdFLqebCEgC/wNxKkVViIAEYd0MjFeTkqkG/6H01WVSIz7tbQsumSjxBP7hcX4DiFlrnvQIDAQAB";
			} else
			if(Version._XTHAILAND)
				base64EncodedPublicKey = "";
			else
			if(Version._XINDONESIA)
				base64EncodedPublicKey = "";
			//
			RegistIAPGoods( SKU_GEM );
//			CreateGoogleIAP( base64EncodedPublicKey );
		} // XGOOGLE_IAP
		if( Version._XSOFTNYX ) {
			com.pgman.util.Utility.setLocal( this, "ko" );
			PGMAN_Start();
		}
		// Facebook
		if( Version._XFACEBOOK || Version._XINI3 ) {
			CreateFacebook(savedInstanceState);
		}
	} // onCreate
	
	@Override
	public void onResume() {
		super.onResume();
		
		GcmBroadcastReceiver.isAppOn = true;
	}

	@Override
	public void onPause() {
		super.onPause();
		
		GcmBroadcastReceiver.isAppOn = false;
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_my_main, menu);
		return true;
	}
	
	public static void LoadMarketURL(final String url)
	{
		Intent i = new Intent(Intent.ACTION_VIEW);
		Uri u = Uri.parse(url);
		i.setData(u);
		myActivity.startActivity(i);
	}
	// ============================================================================
	// ============================================================================
	// ============================================================================

	// GCM

	public static final String EXTRA_MESSAGE = "message";
	public static final String PROPERTY_REG_ID = "registration_id";
	private static final String PROPERTY_APP_VERSION = "appVersion";
	private static final int PLAY_SERVICES_RESOLUTION_REQUEST = 9000;

//	String SENDER_ID = "449217004536"; // API 콘솔에 있는 프로젝트 번호
	String SENDER_ID = "424401185423"; // API 콘솔에 있는 프로젝트 번호

	GoogleCloudMessaging gcm; // gcm 객체
	AtomicInteger msgId = new AtomicInteger();
	Context context;

	public String regid;

	void gcmInit() {
		if (checkPlayServices()) {
			gcm = GoogleCloudMessaging.getInstance(this); // gcm 객체
			context = getApplicationContext();
			// sharedPref에 저장된 regid를 꺼내온다. 앱버전이 바꼈으면 새로 등록한다.
			regid = getRegistrationId(context);
			if (regid.isEmpty() ) {
				Log.i(TAG_GCM, "async regenerate gcm regid.......");
				registerInBackground();
			} else {
				Log.i(TAG_GCM, "regid=" + regid);
			}
		} else {
			Log.i(TAG_GCM, "No valid Google Play Services APK found.");
		}
	}

	/**
	 * 그것은 구글이 서비스 APK 재생이 있는지 확인하기 위해 장치를 확인하십시오. 그것은 사용자가 구글이 장치의 시스템 설정에
	 * 저장을하거나 활성화에서 APK를 다운로드 할 수있는 대화 상자를 표시하지 않습니다.
	 * 
	 * Check the device to make sure it has the Google Play Services APK. If it
	 * doesn't, display a dialog that allows users to download the APK from the
	 * Google Play Store or enable it in the device's system settings.
	 */

	private boolean checkPlayServices() {
		int resultCode = GooglePlayServicesUtil
				.isGooglePlayServicesAvailable(this);
		if (resultCode != ConnectionResult.SUCCESS) // 실패했으면
		{
			if (GooglePlayServicesUtil.isUserRecoverableError(resultCode)) {
				GooglePlayServicesUtil.getErrorDialog(resultCode, this,
						PLAY_SERVICES_RESOLUTION_REQUEST).show();
			} else {
				Log.i(TAG_GCM, "This device is not supported.");
				finish();
			}
			return false;
		}
		return true; // 성공
	}
	/**
	 * 등록 ID 및 응용 프로그램의 SharedPreferences에서 응용 프로그램 versionCode를 저장합니다.
	 * 
	 * Stores the registration ID and the app versionCode in the application's
	 * {@code SharedPreferences}.
	 * 
	 * @param context
	 *            application's context.
	 * @param regId
	 *            registration ID
	 */
	
	private void storeRegistrationId(Context context, String regId) {
		final SharedPreferences prefs = getGcmPreferences(context);
		int appVersion = getAppVersion(context);
		Log.i(TAG_GCM, "Saving regId on app version " + appVersion);
		SharedPreferences.Editor editor = prefs.edit();
		editor.putString(PROPERTY_REG_ID, regId);
		editor.putInt(PROPERTY_APP_VERSION, appVersion);
		editor.commit();
	}

	/**
	 * 하나가있는 경우, GCM 서비스의 응용 프로그램에 대한 현재 등록 ID를 가져옵니다.
	 * 
	 * Gets the current registration ID for application on GCM service, if there
	 * is one.
	 * <p>
	 * 
	 * 검색 결과가 비어있는 경우, 앱 등록해야
	 * 
	 * If result is empty, the app needs to register.
	 * 
	 * @return registration ID, or empty string if there is no existing
	 *         registration ID.
	 */
	
	private String getRegistrationId(Context context) {
		final SharedPreferences prefs = getGcmPreferences(context);
		String registrationId = prefs.getString(PROPERTY_REG_ID, "");
		if (registrationId.isEmpty()) {
			Log.i(TAG_GCM, "Registration not found.");
			return "";
		}
		// 응용 프로그램이 업데이트 된 경우 확인; 이렇게하면 기존 REGID가 새로운 애플리케이션 버전으로
		// 작동하도록 보장하지 않으므로, 그것의 등록 ID를 취소한다.
		// Check if app was updated; if so, it must clear the registration ID
		// since the existing regID is not guaranteed to work with the new
		// app version.
		int registeredVersion = prefs.getInt(PROPERTY_APP_VERSION,
				Integer.MIN_VALUE);
		int currentVersion = getAppVersion(context);
		if (registeredVersion != currentVersion) {
			Log.i(TAG_GCM, "App version changed.");
			return "";
		}
		return registrationId;
	}

	/**
	 * 비동기 GCM 서버와 응용 프로그램을 등록합니다.
	 * 
	 * Registers the application with GCM servers asynchronously.
	 * <p>
	 * 
	 * 상점은 응용 프로그램의 등록 ID 및 응용 프로그램 versionCode를가 환경 설정을 공유합니다.
	 * 
	 * Stores the registration ID and the app versionCode in the application's
	 * shared preferences.
	 */
	
	private void registerInBackground() {
		new AsyncTask<Void, Void, String>() {
			@Override
			protected String doInBackground(Void... params) {
				String msg = "";
				try {
					if (gcm == null) {
						gcm = GoogleCloudMessaging.getInstance(context);
					}
					regid = gcm.register(SENDER_ID);
					msg = "Device registered, registration ID=" + regid;

					// 당신은 HTTP를 통해 서버에 등록 ID를 보내야한다,
					// 그래서 당신의 응용 프로그램에 메시지를 보낼 수 GCM / HTTP 또는 CCS를 사용할 수 있습니다.
					// You should send the registration ID to your server over
					// HTTP, so it
					// can use GCM/HTTP or CCS to send messages to your app.
					sendRegistrationIdToBackend();

					// 이 데모의 경우 : 장치가 메시지에서 주소를 사용하여 메시지를 다시 에코 서버에 업스트림 메시지를
					// 보낼 것이기 때문에 우리는 그것을 보낼 필요는 없습니다.
					// For this demo: we don't need to send it because the
					// device will send
					// upstream messages to a server that echo back the message
					// using the
					// 'from' address in the message.

					// REGID를 유지하지 - 다시 등록 할 필요 없게
					// Persist the regID - no need to register again.
					storeRegistrationId(context, regid);
				} catch (IOException ex) {
					msg = "Error :" + ex.getMessage();

					// 오차가있는 경우에, 단지 등록을 계속 시도하지 않는다.
					// 다시 버튼을 클릭, 또는 지수 백 오프 (back-off)을 수행하는 사용자가 필요합니다.
					// If there is an error, don't just keep trying to register.
					// Require the user to click a button again, or perform
					// exponential back-off.
				}
				return msg;
			}

			@Override
			protected void onPostExecute(String msg) {
				// mDisplay.append(msg + "\n");
			}
		}.execute(null, null, null);
	}

	/**
	 * 패키지 매니저에서 응용 프로그램의 버전 번호
	 * 
	 * @return Application's version code from the {@code PackageManager}.
	 */
	private static int getAppVersion(Context context) {
		try {
			PackageInfo packageInfo = context.getPackageManager()
					.getPackageInfo(context.getPackageName(), 0);
			return packageInfo.versionCode;
		} catch (NameNotFoundException e) {
			// should never happen
			throw new RuntimeException("Could not get package name: " + e);
		}
	}

	/**
	 * @return Application's {@code SharedPreferences}.
	 */
	
	private SharedPreferences getGcmPreferences(Context context) {
		// 이 샘플 응용 프로그램은 공유 환경 설정에서 등록 ID를 유지하지만, 당신은 당신의 응용 프로그램에서 REGID를 저장하는
		// 방법을 당신에게 달려 있습니다.
		// This sample app persists the registration ID in shared preferences,
		// but
		// how you store the regID in your app is up to you.

		return getSharedPreferences(
				MyMainActivity.class.getSimpleName(),
				Context.MODE_PRIVATE);
	}

	/**
	 * HTTP를 통해 서버에 등록 ID를 전송, 그래서 당신의 응용 프로그램에 메시지를 보낼 수 GCM / HTTP 또는 CCS를 사용할
	 * 수 있습니다. 장치가 메시지의 주소에서를 사용하여 메시지를 확대 에코 서버에 업스트림 메시지를 송신 한이 데모 필요 없음.
	 * 
	 * Sends the registration ID to your server over HTTP, so it can use
	 * GCM/HTTP or CCS to send messages to your app. Not needed for this demo
	 * since the device sends upstream messages to a server that echoes back the
	 * message using the 'from' address in the message.
	 */
	
	private void sendRegistrationIdToBackend() {
		// 여기에 구현.
		// Your implementation here.
		Log.i(TAG_GCM, "등록 ID : " + regid);
		// 여기서 등록 ID를 서버로 보내자
//		JavaToCpp( "gcm_regid", regid, null );
		JavaToCpp( "gcm_regid", regid, "" );
		Log.d(TAG_GCM, "sendRegistrationIdToBackend");
	}


	// 기기에서 푸시를 보내자!
	private void SendPush(final String push)
	{
		new Thread(new Runnable() {
			public void run(){
				
				StringBuffer postDataBuilder = new StringBuffer();
			//	postDataBuilder.append("Content-Type:application/json" );
			//	postDataBuilder.append("Authorization:key=AIzaSyDGo3mEIx6ykmWSr0AFZzwkvYvZBywW8QA" );
				
			    postDataBuilder.append("{\"registration_ids\" : [\"" + myActivity.regid + "\"]," ); //등록ID
			    postDataBuilder.append("\"data\" : { \"msg\":\"data\"}, \"time_to_live\": 1000000,}");

			    byte[] postData;
				try {
					postData = postDataBuilder.toString().getBytes("UTF8");
			        URL url = new URL("https://android.googleapis.com/gcm/send");

			        HttpURLConnection conn = (HttpURLConnection) url.openConnection();
			        conn.setDoOutput(true);
			        conn.setUseCaches(false);
			        conn.setRequestMethod("POST");
			        conn.setRequestProperty("Content-Type", "application/json");
//			        conn.setRequestProperty("Authorization", "key=AIzaSyDGo3mEIx6ykmWSr0AFZzwkvYvZBywW8QA");
//			        conn.setRequestProperty("Authorization", "key=AIzaSyC8J2qO3b-CUkVlx6wduBzdobs_DREK8f0");
			        conn.setRequestProperty("Authorization", "key=AIzaSyCAnAgzJXDtYJSjBTX1uTR2vQ6NScQmTFA");
			        OutputStream out = conn.getOutputStream();

			        out.write(postData);
			        out.flush();
			        out.close();

			        int responseCode = conn.getResponseCode();
			        Log.d(TAG_GCM,"msg : " + responseCode+ "   " + conn.getResponseMessage());
			        conn.disconnect();
					} catch (UnsupportedEncodingException e) {
						// TODO 자동 생성된 catch 블록
						e.printStackTrace();
					} catch (IOException e) {
						// TODO 자동 생성된 catch 블록
						e.printStackTrace();
					}
			}
		}).start();
	}
	
}