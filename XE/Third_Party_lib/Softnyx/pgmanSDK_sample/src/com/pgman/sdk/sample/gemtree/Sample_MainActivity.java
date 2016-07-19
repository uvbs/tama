package com.pgman.sdk.sample.gemtree;


import com.pgman.inappbilling.IabPurchaseFinishedListener;
import com.pgman.inappbilling.IabResult;
import com.pgman.inappbilling.IabSetupFinishedListener;
import com.pgman.inappbilling.Purchase;
import com.pgman.inappbilling.pgmanstore_IAB;
import com.pgman.inappbilling.pgmanstore_IAB_Helper;
import com.pgman.sdk.sample.gemtree.R;

import com.pgman.auth.Auth;
import com.pgman.auth.AuthBuilder;
import com.pgman.auth.AuthConnectionFailListener;
import com.pgman.auth.AuthConnectionListener;
import com.pgman.auth.AuthResult;
import com.pgman.auth.AuthUserInfoListener;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;



/**
 * @author master
 *	PGMAN SDK 기능 샘플 클레스 
 *  스토어 인증 / 유저 정보 가져오기 / 캐쉬 사용  
 */
public class Sample_MainActivity extends Activity {

	TextView mView;	
	Auth mPGManAuth;	
	boolean mLogin = false;
	boolean mIABProcessing = false;
	
	// in app billing 액티비티에서 돌아올때 확인하는 리턴코드
	public static  int REQUEST_PGMAN_IAB = 569;
	
	public static  Sample_MainActivity instance = null;
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
      
    	// SDK Lanauge를 바꾼다 ko, es, pt, en 지원
    	// 에러코드 메세지는 ko
    	com.pgman.util.Utility.setLocal( this, "ko" );
    	
    	super.onCreate(savedInstanceState);
        setContentView(R.layout.pgman_sample_main);
        
        String title = "PGMan SDK Sample ( Ver." + com.pgman.sdk.StateCode.PGMAN_GAMES_SDK_VERSION + " )"; 
        this.setTitle(title);
        
        View.OnClickListener lintener = new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				int id = v.getId();
				if (id == R.id.Button_login) {
					if ( !mLogin ) {
						setTextView( "Auth wait.... ");
						Login();						
					}
					else
						Logout();
				} else if (id == R.id.Button_userinfo) {
					StartUserInfo();
				} else if (id == R.id.Button_IAB) {
					if ( com.pgman.sdk.StateCode.PGMAN_GAMES_SDK_VERSION < 3 ) {
						startIAB();
					}					
					else {
						IABHelper_launchPurchaseFlow();
					}
				}
			}
		};
        
        Button bt = (Button)findViewById(R.id.Button_login);
        bt.setOnClickListener( lintener );
                
        bt = (Button)findViewById(R.id.Button_userinfo);
        bt.setOnClickListener( lintener );
        
        bt = (Button)findViewById(R.id.Button_IAB);
        bt.setOnClickListener( lintener );
        
        mView = (TextView)findViewById(R.id.test_textView);
        
        instance = this;
        
        Start();
    }
    
    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
    
        // 로그인 아이디와 암호입력 액티비티창의 결과값을 리턴한다 
        if ( requestCode == Auth.REQUEST_PGMAN_LOGIN_CODE ){
        	if ( resultCode == Activity.RESULT_OK ) {	
        		mLogin = true;        		
        	}
        	if ( resultCode == Activity.RESULT_CANCELED ) {	
        		mLogin = false;      
        		setTextView( "Auth cancel ");
        	}
        }
        
        // IAB 액티비티창의 결과값을 리턴한다 
        if ( requestCode == REQUEST_PGMAN_IAB ) {
        	
        }
    }
        
    @Override
   	protected void onStart(){
    	if ( mPGManAuth != null )
    		mPGManAuth.onStart();
    	
   		super.onStart();
   	}
      
    
    @Override
	protected void onStop(){
    	if ( mPGManAuth != null )
    		mPGManAuth.onStop();
    	
		super.onStop();
	}
   
    void getUserInfo(){    	
    	if ( mPGManAuth == null ) {
    		setTextView( "Auth Null");
    		return;
    	}
    	
		String userId = mPGManAuth.getApiId();
    	String Name = mPGManAuth.getApiName();
		String userProfileImagepath = mPGManAuth.getApiProfileImage();
		int cash = mPGManAuth.getApiCash();
		
    	String s = 	"< id : " + userId + " >\n" + 
    					"< name : " + Name + " >\n" +
    					"< image Path : " + userProfileImagepath + " >\n" +
    					"< cash : " + cash + " >";
    							
    	if ( com.pgman.sdk.StateCode.PGMAN_GAMES_SDK_VERSION > 3 )
    	{
    		boolean TesterAccount =  mPGManAuth.IsApiDevloper();
    		s += "\n< Tester : " + TesterAccount + " >";
    	}				
    	    	
    	setTextView( s );
    }
  
    public void setTextView( String s ){    	   
    	final String text = s;
    	this.runOnUiThread( new Runnable(){
           	public void run(){
           		mView.setText(text);
           	}
       	});
    }
    
    /**
     * com.pgmangames.auth.Auth.Builder 를 사용하여 
     * com.pgmangames.auth.Auth 를 생성해야한다
     */
    public void Start(){
    	/**
    	 *정보를 가져오기 위해 재정의된  Listener
    	 */    	
    	// 계정 인증 성공에 대한 Listener 로그아웃시 Result - false 를 반환한다
    	AuthConnectionListener connectionListener = new AuthConnectionListener() {
			@Override
			public void OnConnection(AuthResult arg0) {
				mLogin = arg0.isSuccess();
				if ( mLogin ) {
					setTextView( "Hello World - Auth success " + arg0.getMessage() );
					
					runOnUiThread( new Runnable(){
						@Override public void run() {
							Button bt = (Button)findViewById(R.id.Button_login);
							bt.setText("logout");
						}
					});
				}
				else
					setTextView( "Auth out " +  arg0.getMessage() );
			}
		};
		// 계정 인증 실패에 대한 Listener
    	AuthConnectionFailListener connectionFailListener = new AuthConnectionFailListener() {
			@Override
			public void OnConnectionFail(AuthResult arg0) {
				mLogin = false;
				setTextView( "Auth fail " + arg0.getMessage() );
				
				if ( arg0.getResultCode() == com.pgman.sdk.StateCode.ERROR_CODE_LOGIN_SESSION_ERROR ){
					// 같은 appID로 서로 다른 앱에서 인증 요청을 할 경우 이전 사용한 앱의 인증토큰과   
					// 인증을 요청하는 앱의 인증 토큰이 충돌하여 세션 에러가 생김
					// 이경우 로그아웃 하여 세션을 클리어하면 다시 인증이 가능하지만
					// 같은 appID로 인해 치명적 버그가 되므로 주의가 필요함 					
					mPGManAuth.Logout();
				}
			}
		};
		// 유저정보 가져오기에 대한 콜백 
		// 유저 네임/아이디/캐쉬금액/유저 프로파일 이미지 정보를 가져올수 있다
    	AuthUserInfoListener useInfoListener = new AuthUserInfoListener() {
			@Override
			public void OnUserInfo(AuthResult arg0) {
				getUserInfo();	
			}
		};
		
		
		// 인증 클레스를 만들기 위한 빌더
    	AuthBuilder builder = new AuthBuilder(this)
		    						.setAppId()
		    						.setConnectionListener( connectionListener )    	
							    	.setConnectionFailedListener( connectionFailListener )
							    	.setApiForUser( useInfoListener );
    	
    	// 인증 클레스 생성
    	mPGManAuth = builder.build();
    	if ( mPGManAuth == null ) {
    		Auth.setError( "Auth build Fail.............." );
    		return;
    	}
    	
    	mLogin = mPGManAuth.IsLogin();
    	
    	if ( mLogin ) {
    		Button bt = (Button)findViewById(R.id.Button_login);
			bt.setText("logout");
    	}
    	
    	if ( com.pgman.sdk.StateCode.PGMAN_GAMES_SDK_VERSION > 2  ) {
    		InitIAB();
    	}    	
    }
    
    /**
     * 성공시 Auth.Builder 클래스의 AuthListener()에서 
     *  정의한 AuthBuilder.AuthListener.UserInfo() 함수를 호출한다 
     */
    public void Login(){
    	if ( mPGManAuth == null )
    		return;
    	
    	// 이전의 인증정보가 없으면 인증창을 띄운다    	
    	mPGManAuth.Login();
    	
    	// 이전의 인증정보가 없으면 로그아웃 상태로 인식후 다음진행 
    	//mPGManAuth.LoginToBackground();
    }
    
    public void Logout(){
    	if ( mPGManAuth == null )
    		return;
    	
    	mPGManAuth.Logout();    	
    	
    	mLogin = mPGManAuth.IsLogin();
    	if ( mLogin == false ) { 
	    	runOnUiThread( new Runnable() {			
				@Override
				public void run() {
					Button bt = (Button)findViewById(R.id.Button_login);
					bt.setText("login");
					
					setTextView("bye World");		
				}
			});
    	}
    }
    
    /**
     *  성공시 Auth.Builder 클래스의 AuthListener()에서 
     *  정의한 AuthBuilder.AuthListener.UserInfo() 함수를 호출한다 
     */
	public void StartUserInfo(){
		if ( mPGManAuth == null )
    		return;
    	
		mPGManAuth.UserInfo();	    	
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// in app billing
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
		
	/**
	 * 구매창을 열기전 스토어서버에 연결되어있는지 확인한다
	 * ver2 이하에서만 사용 
	 * 	 */
	public void startIAB() {		
		if ( mIABProcessing == true )
			return;
		
		IabSetupFinishedListener listener = new IabSetupFinishedListener() {
			@Override
			public void onIabSetupFinished(IabResult arg0) {				
				if (arg0.isSuccess() ) {
					if ( Auth.Instance().IsLogin() ) {
						setTextView( "Auth success " +  arg0.getErrorMessage()  );
						runOnUiThread( new Runnable(){
							@Override public void run() {
								Button bt = (Button)findViewById(R.id.Button_login);
								bt.setText("logout");
							}
						});
					}
			
					// 구매 요청 시작
					StartPurchases();				
				}
				else {
					Log.i("PGMAN IAB Error", " PGMAN IAB Error :: <code : " + arg0.getErrorCode() + "> msg : " + arg0.getErrorMessage() );
					mIABProcessing = false;
				}
			}
		};
		
		mIABProcessing = true;
		
		// listener 를 통해 연결상태에 대한 정보를 받는다
		pgmanstore_IAB.startSetup( this, listener );		
	}
	
	/**
	 * 스토어와의 인증을 끝낸후 실재 구매할 정보를 보낸다
	 */
	void StartPurchases() {		
		
		// 구매에대한 결과를 Purchase 로 받는다
		IabPurchaseFinishedListener finishlistener = new IabPurchaseFinishedListener() {
			@Override
			public void onIabPurchaseFinished(IabResult arg0, Purchase arg1) {
				if ( !arg0.isSuccess() ) {
					String error = "PGMAN IAB Error :: " + "<code : " + arg0.getErrorCode() + ">  msg : " + arg0.getErrorMessage() ;
					Log.i( "PGMAN IAB Error" , error );
					setTextView( error  );
				}
				else {					
					String result = "< orderID : " + arg1.getOrderId() + " >\n< token : " +  arg1.getToken() + " >";
					setTextView( result );
					Log.i( "PGMAN IAB Purchase Finish" , result );
				}
				
				mIABProcessing = false;
			}
		};
				
		// 구매할 상품명
		EditText et_title = (EditText)findViewById(R.id.editText_pruchase_title);		
		String PurchaseId = et_title.getText().toString();
		if ( PurchaseId.length() == 0 )
			PurchaseId ="Input Purchase Name";

		// 구매할 금액
		EditText et_price = (EditText)findViewById(R.id.editText_purchase_price);
		String Price = et_price.getText().toString();
		if ( Price.length() == 0 )
			Price = "100";
		
		int Payment = Integer.parseInt( Price );
				
		// 구매 요청 시작
		pgmanstore_IAB.launchPurchaseFlow( PurchaseId, REQUEST_PGMAN_IAB, finishlistener, Payment );
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// com.pgman.sdk.StateCode.PGMAN_GAMES_SDK_VERSION  4 이상에서
	// com.pgman.inappbilling.pgmanstore_IAB_Helper 사용권장
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/**
	 * Version 3
	 * 인앱결재 초기화 Helper 생성
	 * listener가 바뀌지 않는한 처음 한번만 호출한다 
	 */
	void InitIAB() {
		
		// 구매전 서버와 동기화 결과를 받는다
		IabSetupFinishedListener setuplistener = new IabSetupFinishedListener() {
			@Override
			public void onIabSetupFinished(IabResult arg0) {				
				if (arg0.isSuccess() ) {
					if ( Auth.Instance().IsLogin() ) {
						setTextView( "Auth success " +  arg0.getErrorMessage()  );
						runOnUiThread( new Runnable(){
							@Override public void run() {
								Button bt = (Button)findViewById(R.id.Button_login);
								bt.setText("logout");
							}
						});
					}				
				}
				else {
					Log.i("PGMAN IAB Error", " PGMAN IAB Error :: <code : " + arg0.getErrorCode() + "> msg : " + arg0.getErrorMessage() );
				}
			}
		};
		
		// 구매에대한 결과를 Purchase 로 받는다
		IabPurchaseFinishedListener finishlistener = new IabPurchaseFinishedListener() {
			@Override
			public void onIabPurchaseFinished(IabResult arg0, Purchase arg1) {
				if ( !arg0.isSuccess() ) {
					String error = "PGMAN IAB Error :: " + "<code : " + arg0.getErrorCode() + ">  msg : " + arg0.getErrorMessage() ;
					Log.i( "PGMAN IAB Error" , error );
					setTextView( error  );
				}
				else {					
					String result = "< orderID : " + arg1.getOrderId() + " >\n< token : " +  arg1.getToken() + " >";
					setTextView( result );
					Log.i( "PGMAN IAB Purchase Finish" , result );
				}
				
				mIABProcessing = false;
			}
		};
		
		// IAB helper 생성
		boolean bSuccess  = com.pgman.inappbilling.pgmanstore_IAB_Helper.SetupHelper(this, setuplistener, finishlistener);
	}
	
	/**
	 * Version 3
	 * Helper class 사용
	 * 상품목록과 사용금액을 인자로 인앱호출을 시작한다
	 */	
	void IABHelper_launchPurchaseFlow() {
		
		// 구매할 상품명
		EditText et_title = (EditText)findViewById(R.id.editText_pruchase_title);		
		String PurchaseId = et_title.getText().toString();
		if ( PurchaseId.length() == 0 )
			PurchaseId ="Input Purchase Name";

		// 구매할 금액
		EditText et_price = (EditText)findViewById(R.id.editText_purchase_price);
		String Price = et_price.getText().toString();
		if ( Price.length() == 0 )
			Price = "100";
		
		int Payment = Integer.parseInt( Price );
		boolean Success = false;
		
		// 구매 요청 시작
		Success = pgmanstore_IAB_Helper.launchPurchaseFlow( PurchaseId, Payment, REQUEST_PGMAN_IAB );
		
		if ( Success == true ) {
			Log.i("PGMAN IAB", " launchPurchaseFlow Success" );
		}
		else {
			Log.i("PGMAN IAB", " launchPurchaseFlow Fail" );
		}
	}
}

