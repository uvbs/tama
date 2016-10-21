package com.gemtree2.caribe;

import java.io.IOException;

import java.util.Hashtable;
import java.util.concurrent.atomic.AtomicInteger;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
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
//import com.gemtree2.caribe.R;
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
import com.tapjoy.TJActionRequest;
import com.tapjoy.TJError;
import com.tapjoy.TJPlacement;
import com.tapjoy.TJPlacementListener;
import com.tapjoy.Tapjoy;
import com.tapjoy.TJAwardCurrencyListener;
import com.tapjoy.TapjoyConnectFlag;
import com.tapjoy.TJConnectListener;
import com.tapjoy.TJEarnedCurrencyListener;
import com.tapjoy.TJGetCurrencyBalanceListener;
import com.tapjoy.TapjoyLog;
import com.tapjoy.TJSpendCurrencyListener;
import com.tapjoy.TJPlacementVideoListener;
import com.tapjoy.TJVideoListener;

public class MyMainActivity extends XeActivity implements Cocos2dxHelperListener, TJGetCurrencyBalanceListener, TJPlacementListener, TJPlacementVideoListener {
	public static Version ver = new Version();
	static String[] SKU_GEM;
	// Tapjoy
	private boolean earnedCurrency = false;
	// Tapjoy Placements
	private TJPlacement directPlayPlacement;
	private TJPlacement examplePlacement;
	private TJPlacement offerwallPlacement;

	public static MyMainActivity myActivity;
	
	static {
		System.loadLibrary("xlib_test");
	}

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

		// Facebook
		if( Version._XFACEBOOK || Version._XINI3 ) {
			CreateFacebook(savedInstanceState);
		}
		// tapjoy init
		connectToTapjoy();
		callShowOffers();
	} // onCreate

	@Override
	protected void onStart() {
		super.onStart();
		Tapjoy.onActivityStart(this);
	}

	/**
	 * Notify Tapjoy the end of this activity for session tracking
	 */
	@Override
	protected void onStop() {
		super.onStop();
		Tapjoy.onActivityStop(this);
	}
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

/*	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_my_main, menu);
		return true;
	}*/
	
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
//	String SENDER_ID = "424401185423"; // API 콘솔에 있는 프로젝트 번호
	String SENDER_ID = "988195830797"; // API 콘솔에 있는 프로젝트 번호

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
//		JavaToCpp( "gcm_regid", regid, "" );
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
						e.printStackTrace();
					} catch (IOException e) {
						e.printStackTrace();
					}
			}
		}).start();
	}
	// TODO: Tapjoy start
	/**
	 * Attempts to connect to Tapjoy
	 */
	private void connectToTapjoy() {
		// OPTIONAL: For custom startup flags.
		Hashtable<String, Object> connectFlags = new Hashtable<String, Object>();
		connectFlags.put(TapjoyConnectFlag.ENABLE_LOGGING, "true");

		// If you are not using Tapjoy Managed currency, you would set your own user ID here.
		//	connectFlags.put(TapjoyConnectFlag.USER_ID, "A_UNIQUE_USER_ID");

		// Connect with the Tapjoy server.  Call this when the application first starts.
		// REPLACE THE SDK KEY WITH YOUR TAPJOY SDK Key.
		String tapjoySDKKey = "u6SfEbh_TA-WMiGqgQ3W8QECyiQIURFEeKm0zbOggubusy-o5ZfXp33sTXaD";
//		String tapjoySDKKey = "sejWMAfxSVaHo0I_tGAFlwECYf0xIUSPfqsagthqtDq7Ys5i1grR-rjus1-e";


		Tapjoy.setGcmSender("34027022155");

		// NOTE: This is the only step required if you're an advertiser.
		Tapjoy.connect(this, tapjoySDKKey, connectFlags, new TJConnectListener() {
			@Override
			public void onConnectSuccess() {
				MyMainActivity.this.Tapjoy_onConnectSuccess();
			}

			@Override
			public void onConnectFailure() {
				MyMainActivity.this.Tapjoy_onConnectFail();
			}
		});
		Tapjoy.setDebugEnabled(true);
	}

	/**
	 * Handles a successful connect to Tapjoy. Pre-loads direct play placement
	 * and sets up Tapjoy listeners
	 */
	public void Tapjoy_onConnectSuccess() {
		Log.i( TAG_TAP, "Tapjoy SDK connected" );
		//Log.i( TAG_TAP, "Tapjoy SDK connected");

		// Start preloading direct play event upon successful connect
		directPlayPlacement = new TJPlacement(this, "video_unit", this);

		// Set Video Listener to anonymous callback
		directPlayPlacement.setVideoListener(new TJPlacementVideoListener() {
			@Override
			public void onVideoStart(TJPlacement placement) {
				Log.i(TAG, "Video has started has started for: " + placement.getName());
			}

			@Override
			public void onVideoError(TJPlacement placement, String message) {
				Log.i(TAG, "Video error: " + message + " for " + placement.getName());
			}

			@Override
			public void onVideoComplete(TJPlacement placement) {
				Log.i(TAG, "Video has completed for: " + placement.getName());

				// Best Practice: We recommend calling getCurrencyBalance as often as possible so the user�s balance is always up-to-date.
				Tapjoy.getCurrencyBalance( MyMainActivity.this);
			}

		});

		directPlayPlacement.requestContent();

		// NOTE:  The get/spend/award currency methods will only work if your virtual currency
		// is managed by Tapjoy.
		//
		// For NON-MANAGED virtual currency, Tapjoy.setUserID(...)
		// must be called after requestTapjoyConnect.

		// Setup listener for Tapjoy currency callbacks
		Tapjoy.setEarnedCurrencyListener(new TJEarnedCurrencyListener() {
			@Override
			public void onEarnedCurrency(String currencyName, int amount) {
				Log.i( TAG_TAP, "onEarnedCurrency" );
				earnedCurrency = true;
				Log.i( TAG_TAP, "You've just earned " + amount + " " + currencyName);
//				showPopupMessage("You've just earned " + amount + " " + currencyName);
			}
		});
	}

	/**
	 * Handles a failed connect to Tapjoy
	 */
	public void Tapjoy_onConnectFail() {
		Log.e(TAG_TAP, "Tapjoy connect call failed");
		//Log.i( TAG_TAP, "Tapjoy connect failed!");
	}
	/**
	 * Constructs TJPlacement {@link TapjoyEasyApp#examplePlacement} and
	 * requests placement content from Tapjoy. The name of the placement is
	 * pulled from {@link TapjoyEasyApp#placementNameInput}.
	 */
	private void requestPlacement() {
		// Grab placement name from input field
		//String placementName = placementNameInput.getText().toString();
		String placementName = "video_unit";

		// Construct TJPlacement
		examplePlacement = new TJPlacement(this, placementName, new TJPlacementListener() {
			@Override
			public void onRequestSuccess(TJPlacement placement) {
				Log.i( TAG_TAP, "onRequestSuccess for placement " + placement.getName());

				if (!placement.isContentAvailable()) {
					Log.i( TAG_TAP, "No content available for placement " + placement.getName());
				}

				//setButtonEnabledInUI(buttonRequestPlacement, true);
			}

			@Override
			public void onRequestFailure(TJPlacement placement, TJError error) {
				//setButtonEnabledInUI(buttonRequestPlacement, true);
				Log.i( TAG_TAP, "onRequestFailure for placement " + placement.getName() + " -- error: " + error.message);
			}

			@Override
			public void onContentReady(TJPlacement placement) {
				Log.i( TAG_TAP, "onContentReady for placement " + placement.getName());
				//setButtonEnabledInUI(buttonShowPlacement, true);
			}

			@Override
			public void onContentShow(TJPlacement placement) {
				TapjoyLog.i(TAG, "onContentShow for placement " + placement.getName());
			}

			@Override
			public void onContentDismiss(TJPlacement placement) {
				//setButtonEnabledInUI(buttonRequestPlacement, true);
				//setButtonEnabledInUI(buttonShowPlacement, false);

				TapjoyLog.i(TAG, "onContentDismiss for placement " + placement.getName());

				// Best Practice: We recommend calling getCurrencyBalance as often as possible so the user's balance is always up-to-date.
				Tapjoy.getCurrencyBalance(MyMainActivity.this);
			}

			@Override
			public void onPurchaseRequest(TJPlacement placement, TJActionRequest request, String productId) {
				// Dismiss the placement content
				Intent intent = new Intent(getApplicationContext(), MyMainActivity.class);
				intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
				startActivity(intent);

				String message = "onPurchaseRequest -- product id: " + productId + ", token: " + request.getToken() + ", request id: " + request.getRequestId();
				AlertDialog dialog = new AlertDialog.Builder(MyMainActivity.this).setTitle("Got on purchase request").setMessage(message)
						.setPositiveButton("Okay", new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int whichButton) {
								dialog.dismiss();
							}
						}).create();
				dialog.show();

				// Your app must call either callback.completed() or callback.cancelled() to complete the lifecycle of the request
				request.completed();
			}

			@Override
			public void onRewardRequest(TJPlacement placement, TJActionRequest request, String itemId, int quantity) {
				// Dismiss the placement content
				Intent intent = new Intent(getApplicationContext(), MyMainActivity.class);
				intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
				startActivity(intent);

				String message = "onRewardRequest -- item id: " + itemId + ", quantity: " + quantity + ", token: " + request.getToken() + ", request id: " + request.getRequestId();
				AlertDialog dialog = new AlertDialog.Builder(MyMainActivity.this).setTitle("Got on reward request").setMessage(message)
						.setPositiveButton("Okay", new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int whichButton) {
								dialog.dismiss();
							}
						}).create();
				dialog.show();

				// Your app must call either callback.completed() or callback.cancelled() to complete the lifecycle of the request
				request.completed();
			}
		}); // new TJPlacement(

		// Add this class as a video listener
		examplePlacement.setVideoListener(this);

		Log.i( TAG_TAP, "Requesting placement content");
		examplePlacement.requestContent();
	} // 	private void requestPlacement() {
	//================================================================================
	// TapjoyListener Methods
	//================================================================================
	@Override
	public void onGetCurrencyBalanceResponse(String currencyName, int balance) {
		Log.i(TAG, "currencyName: " + currencyName);
		Log.i(TAG, "balance: " + balance);

		if (earnedCurrency) {
			Log.i( TAG_TAP, /*displayText + "\n" +*/ currencyName + ": " + balance);
			earnedCurrency = false;
		} else {
			Log.i( TAG_TAP, currencyName + ": " + balance);
		}
		//setButtonEnabledInUI(getCurrencyBalanceButton, true);
	}

	@Override
	public void onGetCurrencyBalanceResponseFailure(String error) {
		Log.i( TAG_TAP, "getCurrencyBalance error: " + error);
		//setButtonEnabledInUI(getCurrencyBalanceButton, true);
	}
	/*
	 * TJPlacement callbacks
	 */
	@Override
	public void onRequestSuccess(TJPlacement placement) {
		// If content is not available you can note it here and act accordingly as best suited for your app
		Log.i(TAG, "Tapjoy on request success, contentAvailable: " + placement.isContentAvailable());
	}

	@Override
	public void onRequestFailure(TJPlacement placement, TJError error) {
		Log.i(TAG, "Tapjoy send event " + placement.getName() + " failed with error: " + error.message);
	}

	@Override
	public void onContentReady(TJPlacement placement) {
	}

	@Override
	public void onContentShow(TJPlacement placement) {
	}

	@Override
	public void onContentDismiss(TJPlacement placement) {
		Log.i(TAG, "Tapjoy direct play content did disappear");

		//setButtonEnabledInUI(getDirectPlayVideoAd, true);

		// Best Practice: We recommend calling getCurrencyBalance as often as possible so the user's balance is always up-to-date.
		Tapjoy.getCurrencyBalance(MyMainActivity.this);

		// Begin preloading the next placement after the previous one is dismissed
		directPlayPlacement = new TJPlacement(this, "video_unit", this);

		// Set Video Listener to anonymous callback
		directPlayPlacement.setVideoListener(new TJPlacementVideoListener() {
			@Override
			public void onVideoStart(TJPlacement placement) {
				Log.i(TAG, "Video has started has started for: " + placement.getName());
			}

			@Override
			public void onVideoError(TJPlacement placement, String errorMessage) {
				Log.i(TAG, "Video error: " + errorMessage +  " for " + placement.getName());
			}

			@Override
			public void onVideoComplete(TJPlacement placement) {
				Log.i(TAG, "Video has completed for: " + placement.getName());

				// Best Practice: We recommend calling getCurrencyBalance as often as possible so the user�s balance is always up-to-date.
				Tapjoy.getCurrencyBalance(MyMainActivity.this);
			}
		});

		directPlayPlacement.requestContent();
	}

	@Override
	public void onPurchaseRequest(TJPlacement placement, TJActionRequest request, String productId) {
	}

	@Override
	public void onRewardRequest(TJPlacement placement, TJActionRequest request, String itemId, int quantity) {
	}

	/**
	 * Video listener callbacks
	 */
	@Override
	public void onVideoStart(TJPlacement placement) {
		Log.i(TAG, "Video has started has started for: " + placement.getName());
	}

	@Override
	public void onVideoError(TJPlacement placement, String errorMessage) {
		Log.i(TAG, "Video error: " + errorMessage +  " for " + placement.getName());
	}

	@Override
	public void onVideoComplete(TJPlacement placement) {
		Log.i(TAG, "Video has completed for: " + placement.getName());

		// Best Practice: We recommend calling getCurrencyBalance as often as possible so the user�s balance is always up-to-date.
		Tapjoy.getCurrencyBalance(MyMainActivity.this);
	}
	/**
	 * Shows pre-loaded direct play placement. Content is initially requested
	 * and pre-loaded in {@link TapjoyEasyApp#onConnectSuccess} and re-sent
	 * again after the content has been dismissed in
	 * {@link TapjoyEasyApp#onContentDismiss}
	 */
	private void showDirectPlayContent() {
		// Check if content is available and if it is ready to show
		if (directPlayPlacement.isContentAvailable()) {
			if (directPlayPlacement.isContentReady()) {
				directPlayPlacement.showContent();
			} else {
				//setButtonEnabledInUI(currentButton, true);
				Log.i( TAG_TAP, "Direct play video not ready to show");
			}

		} else {
			//setButtonEnabledInUI(currentButton, true);
			Log.i( TAG_TAP, "No direct play video to show");
		}
	}

	private void callShowOffers() {
		// Construct TJPlacement to show Offers web view from where users can download the latest offers for virtual currency.
		offerwallPlacement = new TJPlacement(this, "offerwall_unit", new TJPlacementListener() {
			@Override
			public void onRequestSuccess(TJPlacement placement) {
				Log.i( TAG_TAP, "onRequestSuccess for placement " + placement.getName());

				if (!placement.isContentAvailable()) {
					Log.i( TAG_TAP, "No Offerwall content available");
				}

				//setButtonEnabledInUI(currentButton, true);
			}

			@Override
			public void onRequestFailure(TJPlacement placement, TJError error) {
				//setButtonEnabledInUI(currentButton, true);
				Log.i( TAG_TAP, "Offerwall error: " + error.message);
			}

			@Override
			public void onContentReady(TJPlacement placement) {
				TapjoyLog.i(TAG, "onContentReady for placement " + placement.getName());

				Log.i( TAG_TAP, "Offerwall request success");
				placement.showContent();
			}

			@Override
			public void onContentShow(TJPlacement placement) {
				TapjoyLog.i(TAG, "onContentShow for placement " + placement.getName());
			}

			@Override
			public void onContentDismiss(TJPlacement placement) {
				TapjoyLog.i(TAG, "onContentDismiss for placement " + placement.getName());
			}

			@Override
			public void onPurchaseRequest(TJPlacement placement, TJActionRequest request, String productId) {
			}

			@Override
			public void onRewardRequest(TJPlacement placement, TJActionRequest request, String itemId, int quantity) {
			}
		});

		// Add this class as a video listener
		offerwallPlacement.setVideoListener(this);
		offerwallPlacement.requestContent();
	}
	// tapjoy end
} // MyMainActivity