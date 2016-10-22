package com.mtricks.xe;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.List;
import java.util.Date;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import org.json.JSONObject;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.pm.Signature;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.os.Message;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.provider.Settings.Secure;
import android.telephony.TelephonyManager;
import android.util.Base64;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.WindowManager;

import com.facebook.Request;
import com.facebook.Response;
import com.facebook.Session;
import com.facebook.SessionState;
import com.facebook.UiLifecycleHelper;
import com.facebook.model.GraphUser;
import com.facebook.widget.FacebookDialog;
import com.mtricks.trivialdrivesample.util.IabException;
import com.mtricks.trivialdrivesample.util.IabHelper;
import com.mtricks.trivialdrivesample.util.IabHelper.QueryInventoryFinishedListener;
import com.mtricks.trivialdrivesample.util.IabResult;
import com.mtricks.trivialdrivesample.util.Inventory;
import com.mtricks.trivialdrivesample.util.Purchase;
import com.mtricks.trivialdrivesample.util.SkuDetails;
import com.mtricks.xe.Cocos2dxHandler.xSoftnyxBuyItem;
import com.mtricks.xe.Cocos2dxHelper.Cocos2dxHelperListener;
//import com.ini3.Ini3UserAuthen;
//import com.ini3.Ini3UserAuthen.AuthenTaskListener;
//import com.pgman.inappbilling.*;
//import com.pgman.inappbilling.IabPurchaseFinishedListener;
//import com.pgman.inappbilling.IabResult;
//import com.pgman.inappbilling.IabSetupFinishedListener;
//import com.pgman.inappbilling.Purchase;
//import com.pgman.inappbilling.pgmanstore_IAB;
//import com.pgman.inappbilling.pgmanstore_IAB_Helper;

/*import com.pgman.auth.Auth;
import com.pgman.auth.AuthBuilder;
import com.pgman.auth.AuthConnectionFailListener;
import com.pgman.auth.AuthConnectionListener;
import com.pgman.auth.AuthResult;
import com.pgman.auth.AuthUserInfoListener;
*/
public class XeActivity extends Activity implements Cocos2dxHelperListener {
	static {
		System.loadLibrary("xlib");
	}
	public static XeActivity xeActivity;
	public static final String TAG = "xuzhu";
	public static final String TAG_GCM = "xuzhu gcm";
	public static final String TAG_FB = "xfacebook";
	public static final String TAG_NYX = "softnyx";
	private XGLSurfaceView mGLView;
	private Cocos2dxHandler mHandler;
	// ini3 authen
	// private Ini3UserAuthen ini3_authen;
	private boolean byFacebookLogin; // 
	private GraphUser user;
	private UiLifecycleHelper uiHelper;
	public static final String APP_PREFERENCES = "Ini3AppPrefs";
	boolean mLogin = false;
	boolean mIABProcessing = false;
	public static  int REQUEST_PGMAN_IAB = 569;
	private String m_payLoadForSoftnyx;
	private String m_idsProductForSoftnyx;
	// The helper object
	IabHelper mHelper;
	static final int RC_REQUEST = 10001;
	static String[] SKU_GOODS;
	private String strPrice; // 구매를 요청한 상품의 가격
	public static final boolean debugLog = true;
	public static final boolean debugLogFb = false;
	Purchase consumeProduct;

	// @SuppressWarnings("deprecation")
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Log.d( TAG, "XeActivity.onCreate" );
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		this.mHandler = new Cocos2dxHandler(this);
		mGLView = new XGLSurfaceView(this);
		Cocos2dxHelper.init(this, this);
		setContentView(mGLView);
		MobileAds
		//getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
//		com.pgman.util.Utility.setLocal( this, "ko" );
//		Start();	// Softnyx IAP  MyMainActivity에서 호출함.
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (debugLog)
			Log.d(TAG, "onActivityResult(" + requestCode + "," + resultCode
					+ "," + data);
		// if (requestCode == RC_BUY)
		// {
		// int responseCode = data.getIntExtra(RESPONSE_CODE);
		// String purchaseData =
		// data.getStringExtra(RESPONSE_INAPP_PURCHASE_DATA);
		// String signature = data.getStringExtra(RESPONSE_INAPP_SIGNATURE);

		// handle purchase here (for a permanent item like a premium upgrade,
		// this means dispensing the benefits of the upgrade; for a consumable
		// item like "X gold coins", typically the application would initiate
		// consumption of the purchase here)
		// }
		boolean defaultProcess = false;
		if (mHelper != null) {
			// Pass on the activity result to the helper for handling
			if (!mHelper.handleActivityResult(requestCode, resultCode, data)) {
				// not handled, so handle it ourselves (here's where you'd
				// perform any handling of activity results not related to
				// in-app
				// billing...
				// super.onActivityResult(requestCode, resultCode, data);
				defaultProcess = true;
			} else {
				Log.d(TAG, "IAP:onActivityResult handled by IABUtil.");
			}
		} else
			defaultProcess = true;
		//
		if (defaultProcess)
			super.onActivityResult(requestCode, resultCode, data);
		//
		if (uiHelper != null) {
			uiHelper.onActivityResult(requestCode, resultCode, data,
					dialogCallback);
			boolean processed = false;
			if (Session.getActiveSession() != null) {
//				Log.d(TAG_FB, "Facebook:Session.getActiveSession() != null");
				processed = Session.getActiveSession().onActivityResult(this,
														requestCode, resultCode, data);
			}
			if (processed == false) {
//				Log.d(TAG_FB, "Facebook:onActivityResult: processed == false");
			}
		}
	} // onActivityResult
	@Override
	protected void onStart() {
    	super.onStart();
	}
	@Override
	protected void onStop() {
    	super.onStop();
	}
	// 구글 인앱시스템 초기화
/*	public static void Callback_InitGoogleIAP(final String publicKey ) 
	{
		Log.d(TAG, "xeActivity.CreateGoogleIAP:" + publicKey );
		xeActivity.CreateGoogleIAP( publicKey );
	} */
	// IAP시스템을 초기화한다. 
	public void CreateGoogleIAP(String base64EncodedPublicKey) {
		// Some sanity checks to see if the developer (that's you!) really
		// followed the
		// instructions to run this sample (don't put these checks on your app!)
		if (base64EncodedPublicKey.contains("CONSTRUCT_YOUR")) {
			throw new RuntimeException(
					"Please put your app's public key in MainActivity.java. See README.");
		}
		// Create the helper, passing it our context and the public key to
		// verify signatures with
		Log.d(TAG, "GoogleIAP:Creating Google IAB helper.");
		mHelper = new IabHelper(this, base64EncodedPublicKey);
		// enable debug logging (for a production application, you should set
		// this to false).
		mHelper.enableDebugLogging(true);
		// Start setup. This is asynchronous and the specified listener
		// will be called once setup completes.
		Log.d(TAG, "GoogleIAP:IAP Starting setup.");
		/////////////////////////////////////////////////////////////////////////////
		// 초기화 시작
		mHelper.startSetup(new IabHelper.OnIabSetupFinishedListener() {
			public void onIabSetupFinished(IabResult result) {
				Log.d(TAG, "GoogleIAP:Setup finished.");
				if (!result.isSuccess()) {
					// Oh noes, there was a problem.
					complain("Problem setting up in-app billing: " + result);
					return;
				}
				// Have we been disposed of in the meantime? If so, quit.
				if (mHelper == null)
					return;
				// IAB is fully set up. Now, let's get an inventory of stuff we
				// own.
				Log.d(TAG, "GoogleIAP:Setup successful. Querying inventory.");
				// 구매내역(구매했던) 목록 요청
				mHelper.queryInventoryAsync(mGotInventoryListener);
			}
		});
	}
	// 아직 소진 못시킨 아이템이 있는지 검사한다.
	public void CheckUnConsumedItem() {
		Message msg = new Message();
		msg.what = Cocos2dxHandler.HANDLER_IAP_QUERY_INVENTORY;
		Log.d("xuzhu iap", "XeActivity.CheckUnConsumedItem" );
		this.mHandler.sendMessage(msg);
	}
	public void DoQueryInventory() {
		Log.d("xuzhu iap", "XeActivity.DoQueryInventory()" );
		mHelper.queryInventoryAsync(mGotInventoryListener3);
	}
	IabHelper.QueryInventoryFinishedListener mGotInventoryListener3
	= new IabHelper.QueryInventoryFinishedListener() {
		public void onQueryInventoryFinished(IabResult result, Inventory inventory) {
			Log.d(TAG, "3_GoogleIAP:Query inventory finished.");
			// Have we been disposed of in the meantime? If so, quit.
			if (mHelper == null)
				return;
			// Is it a failure?
			if (result.isFailure()) {
				complain("Failed to query inventory: " + result);
				return;
			}
			Log.d(TAG, "3_GoogleIAP:Query inventory was successful.");
			// 구매내역중 소진되는 상품류가 아직 남아있다면 서버에 보냄
			int size = SKU_GOODS.length;
			for (int i = 0; i < size; ++i) {
				Log.d(TAG, "GoogleIAP:check consuming item:" /* + SKU_GEM[i] */);
				Purchase gasPurchase = inventory.getPurchase(SKU_GOODS[i]);
				if (gasPurchase != null && verifyDeveloperPayload(gasPurchase)) {
					// 소진안된 아이템 발견. 
					Log.d(TAG, "GoogleIAP:We have gem. Consuming it.");
					consumeProduct = gasPurchase;
					mGLView.GetRenderer().nativeOnDetectedUnConsumedItem( gasPurchase.getOriginalJson()
																		, gasPurchase.getSignature() );
//					mHelper.consumeAsync(inventory.getPurchase(SKU_GOODS[i]), mConsumeFinishedListener);
				}
			}
			Log.d(TAG, "3_GoogleIAP:Initial inventory query finished;");
		}
	};
	// JNI로부터 컨슘을 명령받는다. Java에선 이것을 핸들러로 전달한다.
	@Override
	public void DoConsumeItem( final String idsProduct ) {
		Message msg = new Message();
		msg.what = Cocos2dxHandler.HANDLER_IAP_CONSUME;
		msg.obj = new Cocos2dxHandler.IAPConsumeMessag( idsProduct );
		Log.d("xuzhu iap", "XeActivity.DoConsumeItem: " + idsProduct );
		this.mHandler.sendMessage(msg);
	}
	// 핸들러(Cocos2dxHandler)로부터 호출되어 실제 기능을 실행한다.
	public void DoConsumeItemHandler( final String idsProduct ) {
		Log.d("xuzhu iap", "XeActivity.DoConsumeItemHandler: " + idsProduct );
		List<String> listProduct = new ArrayList<String>();
		Log.d("xuzhu iap", "list add: " + idsProduct );
		listProduct.add( idsProduct );
		Log.d("xuzhu iap", "mHelper.queryInventoryAsync2: " + idsProduct );
//		mHelper.queryInventoryAsync( true, listProduct, mGotInventoryListener2 );
//		if( consumeProduct != null )
//			Log.d("xuzhu iap", "consumeProduct: " + consumeProduct.toString() );
		if( mHelper == null )
			return;
		Log.d("xuzhu iap", "mHelper: " + mHelper.toString() );
		// idsProduct에 대한 거래 객체를 요청
		mHelper.queryInventoryAsync( true, listProduct, mGotInventoryListener2);
//		mHelper.consumeAsync(consumeProduct, mConsumeFinishedListener);
	}
	IabHelper.QueryInventoryFinishedListener mGotInventoryListener2 
	= new IabHelper.QueryInventoryFinishedListener() {
		public void onQueryInventoryFinished(IabResult result, Inventory inventory) {
			Log.d(TAG, "GoogleIAP:Query inventory finished. 2");
			// Have we been disposed of in the meantime? If so, quit.
			if (mHelper == null)
				return;
			// Is it a failure?
			if (result.isFailure()) {
				complain("Failed to query inventory: " + result);
				return;
			}
			Log.d(TAG, "GoogleIAP:Query inventory was successful. 2");
			/*
			 * Check for items we own. Notice that for each purchase, we check
			 * the developer payload to see if it's correct! See
			 * verifyDeveloperPayload().
			 */
			// Check for gas delivery -- if we own gas, we should fill up the
			// tank immediately
			// 요청한 아이템이 소진되지 않은 아이템이면 소진시킴.
			int size = SKU_GOODS.length;
			for (int i = 0; i < size; ++i) {
				Log.d(TAG, "GoogleIAP:check consuming item: 2" /* + SKU_GEM[i] */);
				Purchase gemPurchase = inventory.getPurchase(SKU_GOODS[i]);
				if (gemPurchase != null ) {
					// 소진할 아이템 발견 
					Log.d(TAG, "GoogleIAP:We have gem. Consuming it. 2");
					mHelper.consumeAsync(gemPurchase, mConsumeFinishedListener);
				}
			}
			Log.d(TAG, "GoogleIAP:Initial inventory query finished; enabling main UI. 2");
		}
	};	
	public void RegistIAPGoods(String[] arySku) {
		int size = arySku.length;
		// SKU_GOODS = new String[size];
		SKU_GOODS = arySku;
	}
	// Listener that's called when we finish querying the items and
	// subscriptions we own
	// 구매내역(구매했던) 목록 요청의 콜백
	IabHelper.QueryInventoryFinishedListener mGotInventoryListener 
	= new IabHelper.QueryInventoryFinishedListener() {
		public void onQueryInventoryFinished(IabResult result, Inventory inventory) {
			Log.d(TAG, "GoogleIAP:Query inventory finished.");
			// Have we been disposed of in the meantime? If so, quit.
			if (mHelper == null)
				return;
			// Is it a failure?
			if (result.isFailure()) {
				int size = SKU_GOODS.length;
				for (int i = 0; i < size; ++i) {
					Log.d(TAG, "GoogleIAP:check consuming item:" /* + SKU_GEM[i] */);
					Purchase gasPurchase = inventory.getPurchase(SKU_GOODS[i]);
					if (gasPurchase != null ) {
						// 소진안된 아이템 발견. 
						Log.d(TAG, "GoogleIAP:We have gem. Consuming it.");
						consumeProduct = gasPurchase;
						mHelper.consumeAsync(inventory.getPurchase(SKU_GOODS[i]), mConsumeFinishedListener);
					}
				}
				complain("Failed to query inventory: " + result);
				return;
			}
			Log.d(TAG, "GoogleIAP:Query inventory was successful.");
			/*
			 * Check for items we own. Notice that for each purchase, we check
			 * the developer payload to see if it's correct! See
			 * verifyDeveloperPayload().
			 */
			// Check for gas delivery -- if we own gas, we should fill up the
			// tank immediately
			// 구매내역중 소진되는 상품류가 아직 남아있다면 소진시킴
			int size = SKU_GOODS.length;
			for (int i = 0; i < size; ++i) {
				Log.d(TAG, "GoogleIAP:check consuming item:" /* + SKU_GEM[i] */);
				Purchase gasPurchase = inventory.getPurchase(SKU_GOODS[i]);
				if (gasPurchase != null && verifyDeveloperPayload(gasPurchase)) {
					// 소진안된 아이템 발견. 
					Log.d(TAG, "GoogleIAP:We have gem. Consuming it.");
					consumeProduct = gasPurchase;
					mGLView.GetRenderer().nativeOnDetectedUnConsumedItem( gasPurchase.getOriginalJson()
																		, gasPurchase.getSignature() );
//					mHelper.consumeAsync(inventory.getPurchase(SKU_GOODS[i]), mConsumeFinishedListener);
				}
			}
			Log.d(TAG, "GoogleIAP:Initial inventory query finished; enabling main UI.");
			// 초기화 완료
			mGLView.GetRenderer().nativeOnInitGoogleIAPAsync();
		}
	};
	// Called when consumption is complete
	// 소진 완료.
	IabHelper.OnConsumeFinishedListener mConsumeFinishedListener 
	= new IabHelper.OnConsumeFinishedListener() {
		public void onConsumeFinished(Purchase purchase, IabResult result) {
			if (debugLog) {
				Log.d(TAG, "GoogleIAP:Consumption finished2. Purchase: " + purchase
						+ ", result: " + result);
				Log.d(TAG, "GoogleIAP:Signature2: " + purchase.getSignature());
			} else
				Log.d(TAG, "GoogleIAP:Consumption finished.2");
			// if we were disposed of in the meantime, quit.
			if (mHelper == null)
				return;
			// We know this is the "gas" sku because it's the only one we
			// consume,
			// so we don't check which sku was consumed. If you have more than
			// one
			// sku, you probably should check...
			if (result.isSuccess()) {
				// successfully consumed, so we apply the effects of the item in
				// our
				// game world's logic, which in our case means filling the gas
				// tank a bit
				// 최종 완료. cpp에 콜백을 부른다
				// Log.d(TAG, date.toString() );
				final String strSku = purchase.getSku();
				// 소진 성공
				mGLView.GetRenderer().nativeOnFinishedConsumeItem( true, strSku );
				if (debugLog)
					Log.d(TAG, "GoogleIAP:Consumption successful. Provisioning."
							+ purchase.getSku());
				else
					Log.d(TAG, "GoogleIAP:Consumption successful. Provisioning.");
			} else {
				// 소진 실패
				final String strSku = purchase.getSku();
				mGLView.GetRenderer().nativeOnFinishedConsumeItem( false, strSku );
				complain("Error while consuming: " + result);
			}
			// updateUi();
			// setWaitScreen(false);
			Log.d(TAG, "GoogleIAP:End consumption flow.");
		}
	};
	@Override
	public void onBuyProduct(final String stridProduct, final String strPayload ) {
		if (mHelper != null) {
			ArrayList<String> moreSkus = new ArrayList<String>();
			moreSkus.add(stridProduct);
			Inventory inv;
			try {
				inv = mHelper.queryInventory(true, moreSkus);
				if( inv == null )
					Log.e(TAG, "GoogleIAP:inv = null");
				Log.d(TAG, "GoogleIAP:stridProduct=" + stridProduct);
				SkuDetails skuDetail = inv.getSkuDetails(stridProduct);
				if( skuDetail != null ) {
					strPrice = skuDetail.getPrice();
				} else {
					Log.e(TAG, "GoogleIAP:not found sku:" + stridProduct);
				}
//				strPrice = inv.getSkuDetails(stridProduct).getPrice();
				if( debugLog )
					Log.d(TAG, "GoogleIAP:onBuyProduct:price:" + strPrice );
			} catch (IabException e) {
			    Log.e(TAG, "GoogleIAP:Exception onBuyProduct.");
				// 에러처리
				e.printStackTrace();    
			}
			// 구매요청
			onBuyButtonClicked(stridProduct, strPayload );
			// mHelper.queryInventoryAsync(true, moreSkus, queryInvenListener);
		} // GoogleIAP
	} // onBuyProduct

	public void onBuyButtonClicked(final String stridProduct, final String strPayload ) {
		/*
		 * the comments on verifyDeveloperPayload() for more info. Since this is
		 * a SAMPLE, we just use an empty string, but on a production app you
		 * should carefully generate this.
		 */
//		String payload = "hello xuzhu mTricks";
		// 페이로드와 함께 보내 구매를 요청한다.
		if( mHelper != null )
			mHelper.launchPurchaseFlow(this, stridProduct, RC_REQUEST,
										mPurchaseFinishedListener, strPayload);
	}
	
	// Callback for when a purchase is finished
	// onBuyButtonClicked의 결과 콜백
	IabHelper.OnIabPurchaseFinishedListener mPurchaseFinishedListener = new IabHelper.OnIabPurchaseFinishedListener() {
		public void onIabPurchaseFinished(IabResult result, Purchase purchase) {
			if (purchase != null) {
				if (debugLog)
					Log.d(TAG, "GoogleIAP:Purchase finished: " + result + ", purchase: "
							+ purchase);
				else
					Log.d(TAG, "GoogleIAP:Purchase finished: success ");
			} else {
				if (debugLog)
					Log.d(TAG, "GoogleIAP:Purchase finished: " + result);
				else
					Log.d(TAG, "GoogleIAP:Purchase finished: fail ");
			}
			// if we were disposed of in the meantime, quit.
			if (mHelper == null)
				return;
			boolean bOk = true;
			if (result.isFailure()) {
				complain("Error purchasing: " + result);
				// 구매 실패
				bOk = false;
			}
			// 구매요청 완료.
			String strPurchase = "";
			String strSignature = "";
			String strIdsProduct = "";
			if( purchase != null ) {
//				strPurchase = purchase.toString();
				strPurchase = purchase.getOriginalJson();
				if( purchase.getSignature() != null )
					strSignature = purchase.getSignature();
				if( purchase.getSku() != null )
					strIdsProduct = purchase.getSku();
			}
			// 구매영수증을 서버로 보냄.
			Log.d(TAG, "GoogleIAP:Purchase successful.");
			mGLView.GetRenderer().nativeOncbOnClickedBuyItem( bOk
															, strPurchase
															, strSignature
															, strIdsProduct );
		}
	};
/*	IabHelper.OnIabPurchaseFinishedListener mPurchaseFinishedListener = new IabHelper.OnIabPurchaseFinishedListener() {
		public void onIabPurchaseFinished(IabResult result, Purchase purchase) {
			if (purchase != null) {
				if (debugLog)
					Log.d(TAG, "GoogleIAP:Purchase finished: " + result + ", purchase: "
							+ purchase);
				else
					Log.d(TAG, "GoogleIAP:Purchase finished: success ");
			} else {
				if (debugLog)
					Log.d(TAG, "GoogleIAP:Purchase finished: " + result);
				else
					Log.d(TAG, "GoogleIAP:Purchase finished: fail ");
			}
			// if we were disposed of in the meantime, quit.
			if (mHelper == null)
				return;
			if (result.isFailure()) {
				complain("Error purchasing: " + result);
				// 구매 실패
				return;
			}
			if (!verifyDeveloperPayload(purchase)) {
				complain("Error purchasing. Authenticity verification failed.");
				// setWaitScreen(false);
				return;
			}
			Log.d(TAG, "GoogleIAP:Purchase successful.");
			//
			// if (purchase != null && purchase.getSku().equals(SKU_GEM5)) {
			// �� ���ӿ��� ���?�Ҹ� ������ �̹Ƿ� ���� ó���� ����
			if (purchase != null) {
				// bought 1/4 tank of gas. So consume it.
				Log.d(TAG, "GoogleIAP:Purchase item. Starting item consumption.");
				mHelper.consumeAsync(purchase, mConsumeFinishedListener);
			}
		}
	}; */  // 구버전

	/** Verifies the developer payload of a purchase. */
	boolean verifyDeveloperPayload(Purchase p) {
		String payload = p.getDeveloperPayload();
		return true;
	}

	IabHelper.QueryInventoryFinishedListener queryInvenListener 
		= new QueryInventoryFinishedListener() {

		@Override
		public void onQueryInventoryFinished(IabResult result, Inventory inv) {
			if (mHelper == null)
				return;
			if (result.isFailure()) {
				Log.d(TAG, "GoogleIAP:onQueryInventoryFinished: fail");
				return;
			} else {
				Log.d(TAG, "GoogleIAP:onQueryInventoryFinished: success");
			}
			// String gemPrice1 = inv.getSkuDetails(SKU_GOODS[0]).getPrice();
			// String gemPrice2 = inv.getSkuDetails(SKU_GOODS[1]).getPrice();
			// Log.d(TAG, "gem1:" + gemPrice1 );
			// Log.d(TAG, "gem2:" + gemPrice2 );
		}
	};

	//////////////////////////////////////////////////////////////////////////////////////////////
	// facebook
	public void CreateFacebook(Bundle savedInstanceState) {
		// Facebook Connect Implement Code
		if( debugLogFb )
			Log.d(TAG_FB, "Facebook:new UiLifecycleHelper");
		uiHelper = new UiLifecycleHelper(this, callback);
		uiHelper.onCreate(savedInstanceState);
		// Facebook force logout
//		if (Session.getActiveSession() != null) {
//			Log.d(TAG_FB, "Facebook:Facebook force logout");
//			Session.getActiveSession().closeAndClearTokenInformation();
//		}
	}

	private Session.StatusCallback callback = new Session.StatusCallback() {
		@Override
		public void call(Session session, SessionState state,
				Exception exception) {
			if( debugLogFb )
				Log.d(TAG_FB, "Facebook:onSessionStateChange");
			onSessionStateChange(session, state, exception);
		}
	};

	private void onSessionStateChange(Session session, SessionState state,
			Exception exception) {
		if( debugLogFb )
			Log.d(TAG_FB, "Facebook:onSessionStateChange");
	}
	private FacebookDialog.Callback dialogCallback = new FacebookDialog.Callback() {
		@Override
		public void onError(FacebookDialog.PendingCall pendingCall,
				Exception error, Bundle data) {
			if( debugLogFb )
				Log.d(TAG_FB, "Facebook:FacebookDialog callback onError");
		}

		@Override
		public void onComplete(FacebookDialog.PendingCall pendingCall,
				Bundle data) {
			if( debugLogFb )
				Log.d(TAG, "Facebook:FacebookDialog callback onComplete");
		}
	};
	// ///////////////////////////////////////////////////////////////////////////////
	@Override
	public void showEditTextDialog(final String pTitle, final String pContent,
			final int pInputMode, final int pInputFlag, final int pReturnType,
			final int pMaxLength) {
		Message msg = new Message();
		msg.what = Cocos2dxHandler.HANDLER_SHOW_EDITBOX_DIALOG;
		msg.obj = new Cocos2dxHandler.EditBoxMessage(pTitle, pContent,
				pInputMode, pInputFlag, pReturnType, pMaxLength);
		this.mHandler.sendMessage(msg);
	}

	@Override
	public void DoLoginFacebook() {
		OnFacebookLoginButtonClicked();
	}
	@Override
	public void DoLogoutFacebook() {
		if( debugLogFb )
			Log.d(TAG_FB, "DoLogoutFacebook()");
		// Facebook force logout
		if (Session.getActiveSession() != null) {
			if( debugLogFb )
				Log.d(TAG_FB, "Facebook:force logout");
			Session.getActiveSession().closeAndClearTokenInformation();
		}
	}

	public void GetUserProfileImage(final String strID) {
		if( debugLogFb )
			Log.d(TAG_FB, "XeActivity:GetUserProfileImage:" + strID);
		new WebGetImage(strID, this).execute();
	}

	public void OnFacebookLoginButtonClicked() {
		// start Facebook Login
		Session.openActiveSession(this, true, new Session.StatusCallback() {
			// callback when session changes state
			@SuppressWarnings("deprecation")
			@Override
			public void call(Session session, SessionState state,
					Exception exception) {
				if( debugLogFb )
					Log.d(TAG_FB, "Facebook:facebook open session call");
				if (session.isOpened()) {
					Log.i(TAG_FB, "Facebook:session is opened ===================================================");
					// make request to the /me API
					Request.executeMeRequestAsync(session,
							new Request.GraphUserCallback() {
								// callback after Graph API response with user
								// object
								@Override
								public void onCompleted(GraphUser user,
										Response response) {
									Log.i(TAG, "Facebook:onCompleted ============================================================");
									if (user != null) {
										if( debugLogFb ) {
											Log.d(TAG_FB, "Facebook:user id=" + user.getId());
											Log.d(TAG_FB, "Facebook:user name=" + user.getUsername());
											Log.d(TAG_FB, "Facebook:name=" + user.getName());
	//										Log.d(TAG_FB, "Facebook:Location=" + user.getLocation();
										}
										JSONObject json = user.getInnerJSONObject();
										if( debugLogFb )
											Log.d(TAG_FB, "Facebook:json=" + json );
										try {
											byFacebookLogin = true;
											mGLView.GetRenderer()
													.nativeOnResultAuthenFacebook(
															user.getId(),
															user.getUsername());
										} catch (Exception e) {
											Log.e(TAG_FB,
													"Error Facebook Authen: ".concat(e
															.getMessage()));
										}
									}
								}
							});
				} else {
					Log.d(TAG_FB, "Facebook:session is not opened");
					try {
						PackageInfo info = null;
						String strPackage = getPackageName();
						if( debugLogFb )
							Log.e(TAG_FB, "My Package:" + strPackage);
						info = getPackageManager().getPackageInfo(strPackage,
								PackageManager.GET_SIGNATURES);
						if (info != null) {
							for (Signature signature : info.signatures) {
								MessageDigest md = MessageDigest
										.getInstance("SHA");
								md.update(signature.toByteArray());
								if (debugLog)
									Log.e(TAG_FB, Base64.encodeToString(
											md.digest(), Base64.DEFAULT));
							}
						}
					} catch (NameNotFoundException e) {
						e.printStackTrace();

					} catch (NoSuchAlgorithmException e) {
						e.printStackTrace();
					}

				}
			}
		});
	}

	@Override
	public void DoExitApp() {
		// Log.d(TAG, "DoExitApp");
		moveTaskToBack(true);
		finish();
		android.os.Process.killProcess(android.os.Process.myPid());

	}

	@Override
	public boolean CheckCDMA() {
		// Log.d(TAG, "CheckCDMA");
		ConnectivityManager cManager;
		NetworkInfo mobile;

		cManager = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
		mobile = cManager.getNetworkInfo(ConnectivityManager.TYPE_MOBILE);
		if (mobile == null)
			return true;
		return mobile.isConnected();
	}

	@Override
	public boolean CheckWiFi() {
		// Log.d(TAG, "CheckWiFi");
		ConnectivityManager cManager;
		NetworkInfo wifi;
		cManager = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
		wifi = cManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
		if (wifi == null)
			return false;
		return wifi.isConnected();
	}

	@Override
	public void InitGoogleIAP( final String publicKey ) {
		Log.d(TAG, "xeActivity.CreateGoogleIAP:" + publicKey );
	    CreateGoogleIAP( publicKey );
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	@Override
	public void showDialog(final String pTitle, final String pMessage) {
		Log.e(TAG, "showDialog: " + pTitle + ", " + pMessage );
		Message msg = new Message();
		msg.what = Cocos2dxHandler.HANDLER_SHOW_DIALOG;
		msg.obj = new Cocos2dxHandler.DialogMessage(pTitle, pMessage);
		this.mHandler.sendMessage(msg);
	}
	@Override
	public void runOnGLThread(final Runnable pRunnable) {
		this.mGLView.queueEvent(pRunnable);
	}
	@Override
	protected void onPause() {
		super.onPause();
		mGLView.onPause();
	}
	@Override
	protected void onResume() {
		// down if (null != mDownloaderClientStub) {
		// down mDownloaderClientStub.connect(this);
		// down }

		super.onResume();
		mGLView.onResume();
	}
	@Override
	public void onDestroy() {
		Log.d(TAG, "XeActivity.onDestroy()");
		mGLView.onDestroy();
		super.onDestroy();
	}
	@Override
	public boolean onKeyDown(int KeyCode, KeyEvent event) {
		if (event.getAction() == KeyEvent.ACTION_DOWN) {
			// 이 부분은 특정 키를 눌렀을때 실행 된다.
			if (KeyCode == KeyEvent.KEYCODE_BACK) {
				mGLView.OnKeyEvent(1000, 1000);
			} else
				mGLView.OnKeyEvent(KeyCode, event.getAction());
		}
		return false;
		// return super.onKeyDown( KeyCode, event );
	}

	@Override
	public boolean onKeyUp(int KeyCode, KeyEvent event) {
		if (event.getAction() == KeyEvent.ACTION_UP) {
			if (KeyCode == KeyEvent.KEYCODE_BACK) {
				mGLView.OnKeyEvent(1000, 1001);
			} else
				mGLView.OnKeyEvent(KeyCode, event.getAction());
		}
		return false;
		// return super.onKeyDown( KeyCode, event );
	}

	// /////////////////////////////////////////////////////////////////////////
	// util
	void complain(String message) {
		String strErr = "****Error: " + message;
		Log.e(TAG, strErr);
		mGLView.GetRenderer().nativeOnPurchaseError(strErr);
		alert("Error: " + message);
	}

	void alert(String message) {
		AlertDialog.Builder bld = new AlertDialog.Builder(this);
		bld.setMessage(message);
		bld.setNeutralButton("OK", null);
		// Log.d(TAG, "Showing alert dialog: " + message);
		bld.create().show();
	}

	public String getDeviceID() {
		String deviceId = "";

		// get IMEI
		TelephonyManager manager = (TelephonyManager) this
				.getSystemService(Context.TELEPHONY_SERVICE);
		deviceId = manager.getDeviceId();

		if (deviceId == null || deviceId.equals("")) {
			deviceId = Secure.getString(this.getContentResolver(),
					Secure.ANDROID_ID);

			if (deviceId == null || deviceId.equals("")) {
				// Pseudo-Unique ID
				deviceId = "35"
						+ // we make this look like a valid IMEI
						Build.BOARD.length() % 10 + Build.BRAND.length() % 10
						+ Build.CPU_ABI.length() % 10 + Build.DEVICE.length()
						% 10 + Build.DISPLAY.length() % 10
						+ Build.HOST.length() % 10 + Build.ID.length() % 10
						+ Build.MANUFACTURER.length() % 10
						+ Build.MODEL.length() % 10 + Build.PRODUCT.length()
						% 10 + Build.TAGS.length() % 10 + Build.TYPE.length()
						% 10 + Build.USER.length() % 10; // 13 digits (15 digits
															// total like IMEI)
			}
		}
		return deviceId;
	}

	public void goURL(String url) {
		Intent i = new Intent(Intent.ACTION_VIEW);
		// Uri u = Uri.parse("http://www.naver.com/");
		Uri u = Uri.parse(url);
		i.setData(u);
		startActivity(i);
	}

	public void onRecvProfileImage( final String strFbUserId, int[] pixels, int w, int h, int bpp) {
		mGLView.GetRenderer().nativeOnRecvProfileImage( strFbUserId, pixels, w, h, bpp);
    }
} // XeActivity

// ///////////////////////////////////////////////////////////////////
class XGLSurfaceView extends GLSurfaceView {
	XRenderer mRenderer;
	private PinchZoom Zoom;

	public XGLSurfaceView(Context context) {
		super(context);
		setEGLContextClientVersion(2);
		// this.setFocusableInTouchMode(true);
		mRenderer = new XRenderer();
		Zoom = new PinchZoom();
		setRenderer(mRenderer);
	}

	// ===========================================================
	// Methods for/from SuperClass/Interfaces
	// ===========================================================
	public XRenderer GetRenderer() {
		return mRenderer;
	}

	public void OnKeyEvent(final int keyCode, final int keyAction) {
		this.queueEvent(new Runnable() {
			public void run() {
				mRenderer.OnKeyEvent(keyCode, keyAction);
				// mRenderer.handleActionDown(pointerNumber, mx, my);
			}
		});
	}

	@Override
	public void onResume() {
		super.onResume();

		this.queueEvent(new Runnable() {
			@Override
			public void run() {
				mRenderer.handleOnResume();
			}
		});
	}
	public void onDestroy() {
//		mRenderer.handleOnDestroy();
	}
	@Override
	public void onPause() {
		this.queueEvent(new Runnable() {
			@Override
			public void run() {
				mRenderer.handleOnPause();
			}
		});

		super.onPause();
	}

	@Override
	public boolean onTouchEvent(final MotionEvent event) {
		final int pointerNumber = event.getPointerCount();
		final float mx = event.getX();
		final float my = event.getY();
		switch (event.getAction() & event.ACTION_MASK) {
		case MotionEvent.ACTION_POINTER_DOWN:
			break;
		case MotionEvent.ACTION_DOWN:
			this.queueEvent(new Runnable() {
				public void run() {
					mRenderer.handleActionDown(pointerNumber, mx, my);
					// mRenderer.setColor(event.getX()/getWidth(),
					// event.getY()/getHeight(), 1.0f);
				}
			});
			break;
		case MotionEvent.ACTION_MOVE:
			this.queueEvent(new Runnable() {
				public void run() {
					mRenderer.handleActionMove(pointerNumber, mx, my);
				}
			});
			break;
		case MotionEvent.ACTION_POINTER_UP:
			break;
		case MotionEvent.ACTION_UP:
			this.queueEvent(new Runnable() {
				public void run() {
					mRenderer.handleActionUp(pointerNumber, mx, my);
				}
			});
			break;
		case MotionEvent.ACTION_CANCEL:
			break;
		}
		// 터치다운 이벤트가 먼저 일어난다음 줌 이벤트가 일어난다.
		Zoom.TouchProcess(event);
		if (Zoom.IsZoom()) {
			mRenderer.handleActionZoom(pointerNumber, Zoom.GetCenterX(),
					Zoom.GetCenterY(), Zoom.GetScale());
			return true;
		}
		return true;
	}

	@Override
	protected void onSizeChanged(final int pNewSurfaceWidth,
			final int pNewSurfaceHeight, final int pOldSurfaceWidth,
			final int pOldSurfaceHeight) {
		if (!this.isInEditMode()) {
			this.mRenderer.widthScreen = pNewSurfaceWidth;
			this.mRenderer.heightScreen = pNewSurfaceHeight;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////
class XRenderer implements GLSurfaceView.Renderer {
	public int widthScreen, heightScreen;
	// public native void nativeCreateMain(int widthPhy, int heightPhy);
	public native void nativeInit(int widthPhy, int heightPhy, boolean debugFlag);
	public native void nativeFrameMove(float dt);
	public native void nativeDraw();
	public native void nativeTouchesDown(int id, float mx, float my);
	public native void nativeTouchesUp(int id, float mx, float my);
	public native void nativeTouchesMove(int id, float mx, float my);
	public native void nativeTouchesZoom(int id, float mx, float my, float scale);
	private native void nativeOnKeyEvent(int keyCode, int keyAction);
	private static native void nativeOnPause();
	private static native void nativeOnResume();
	private static native void nativeOnDestroy();
	public native void nativeOnPurchaseFinishedConsume(final String strJson,
			final String stridProduct, final String strToken,
			final String strPrice, final String strOrderId,
			final long usPurchaseTime);
	public native void nativeOnPurchaseError(final String strErr);
	public native void nativeOnInitGoogleIAPAsync();
	public native void nativeOnDetectedUnConsumedItem(final String jstrJson, final String jstrSignature );
	public native void nativeOncbOnClickedBuyItem( boolean bOk, final String jstrJsonReceipt, final String jstrSignature, final String jstrIdsProduct );
	public native void nativeOnFinishedConsumeItem( boolean bOk, final String jstrSku );
	
	// public native void nativeOnResultAuthenIni3(final String strUserId,
	// String strUserName, boolean byFaceboolLogin );
	public native void nativeOnResultAuthenFacebook(final String strUserId,
			String strUserName);
	public native void nativeOnRecvProfileImage( final String strFbUserId, int[] pixels, int w, int h, int bpp);
	//
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		nativeInit(widthScreen, heightScreen, XeActivity.debugLog);
	}
	public void onSurfaceChanged(GL10 gl, int w, int h) {
		// gl.glViewport(0, 0, w, h);
	}
	public void onDrawFrame(GL10 gl) {
		nativeFrameMove(1.0f);
		nativeDraw();
		// gl.glClearColor(mRed,mGreen,mBlue,1.0f);
		// gl.glClear( GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT );
		//
	}
	public void handleActionDown(final int pID, final float pX, final float pY) {
		nativeTouchesDown(pID, pX, pY);
	}

	public void handleActionUp(final int pID, final float pX, final float pY) {
		nativeTouchesUp(pID, pX, pY);
	}

	public void handleActionCancel(final int[] pIDs, final float[] pXs,
			final float[] pYs) {
	}

	public void handleActionMove(final int pID, final float pX, final float pY) {
		nativeTouchesMove(pID, pX, pY);
	}

	public void handleActionZoom(final int pID, final float pX, final float pY,
			final float scale) {
		nativeTouchesZoom(pID, pX, pY, scale);
	}
	public void handleOnPause() {
		nativeOnPause();
	}
	public void handleOnResume() {
		Log.i( "jni","nativeOnResume();" );
		nativeOnResume();
	}
	public void handleOnDestroy() {
		nativeOnDestroy();
	}
	public void OnKeyEvent(int keyCode, int keyAction) {
		nativeOnKeyEvent(keyCode, keyAction);
	}

}
