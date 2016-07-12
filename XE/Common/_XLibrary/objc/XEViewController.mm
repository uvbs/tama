//
//  XEViewController.m
//  xe
//
//  Created by JungWoo Sung on 2014. 1. 24..
//  Copyright (c) 2014년 JungWoo Sung. All rights reserved.
//

#import "XEViewController.h"
#include <list>
#include "stdafx.h"
#include "_Wnd2/XWndEdit.h"
#include "client/XClientMain.h"
//#include "client/XGame.h"
#include "client/XApp.h"
#import "objc/xe_ios.h"
#import "objc/VerificationController.h"
#import "objc/XEAppDelegate.h"

namespace IOS
{
	void SetController( id idController );
	id GetController( void );
};

@interface XEViewController ()

#ifdef _XGLKIT
@property (strong, nonatomic) EAGLContext *context;
#endif
//@property Ini3UserAuthen *ini3_auth;
@end

@implementation XEViewController
#ifndef _XGLKIT
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
		IOS::SetController(self);
    }
    return self;
}
#endif // not GLKit
- (void)viewDidUnload
{
	[super viewDidUnload];
    [[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
#ifdef _XGLKIT
	// GL Setup
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
	
    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormatNone;
	view.drawableColorFormat = GLKViewDrawableColorFormatRGB565;
	view.drawableMultisample = GLKViewDrawableMultisampleNone;
	
    [EAGLContext setCurrentContext:self.context];
#endif
    //self.ini3_auth = [[Ini3UserAuthen alloc] init];
    //[self.ini3_auth setDelegate:self];
	
	// Do any additional setup after loading the view.
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    if( [SKPaymentQueue canMakePayments] )
        NSLog(@"canMakePayments good");
    else
        NSLog(@"canMakePayments failed!!!!!!!");
}


#ifdef _XGLKIT
- (void)dealloc
{
	[self tearDownGL];
	
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];

    // delete GL buffer
}
#endif
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

#ifdef _XGLKIT
    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        [self tearDownGL];
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }
#endif
	
    // Dispose of any resources that can be recreated.
}

/////////////////////////////////////////////////////////////////////

/*
-(void)SendRequestFacebookUserInfo
{
	[FBRequestConnection startForMeWithCompletionHandler:^(FBRequestConnection *connection, NSDictionary<FBGraphUser> *me, NSError *error) {
		if(error) {
			IOS::nativeOnResultAuthenFacebook("", "");
			return;
		}
		NSLog(@"%@", me.name);
		NSLog(@"%@", me.id);
		NSLog(@"%@", me.username);
		const char *cUserId = me.id.UTF8String;
		const char *cUserName = me.username.UTF8String;
		IOS::nativeOnResultAuthenFacebook(cUserId, cUserName);
	}];
}

-(void)DoLoginFacebook
{
	if( FBSession.activeSession.isOpen ) {
		[self SendRequestFacebookUserInfo];
	} else {
        [FBSession openActiveSessionWithReadPermissions:nil
                                           allowLoginUI:YES
                                      completionHandler:^
		 (FBSession *session, FBSessionState status, NSError *error)
		 {
                                          // if login fails for any reason, we alert
			  if (error) {
				  UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Error"
																  message:error.localizedDescription
																 delegate:nil
														cancelButtonTitle:@"OK"
														otherButtonTitles:nil];
				  [alert show];
				  IOS::nativeOnResultAuthenFacebook("", "");
				  // if otherwise we check to see if the session is open, an alternative to
				  // to the FB_ISSESSIONOPENWITHSTATE helper-macro would be to check the isOpen
				  // property of the session object; the macros are useful, however, for more
				  // detailed state checking for FBSession objects
			  } else if (FB_ISSESSIONOPENWITHSTATE(status)) {
				  // send our requests if we successfully logged in
				  [self SendRequestFacebookUserInfo];
			  }
		  }
		 ];
	}
}

// for ini3
-(void)didFinishIni3UserAuthentication:(NSInteger *)retVal
{
	NSInteger idUser = [self.ini3_auth getUserId];
	NSString *userId = [NSString stringWithFormat:@"%d", idUser];
	NSString *userName = [self.ini3_auth getUserName];
	const char *cUserId = userId.UTF8String;
	const char *cUserName = userName.UTF8String;
	IOS::nativeOnResultAuthenFacebook(cUserId, cUserName);
}

-(void)SendRequestIni3UserInfo
{
	[FBRequestConnection startForMeWithCompletionHandler:
			^(FBRequestConnection *connection, NSDictionary<FBGraphUser> *me,NSError *error) {
		if(error) {
			IOS::nativeOnResultAuthenFacebook("", "");
			return;
		}
		NSLog(@"%@", me.name);
		NSLog(@"%@", me.id);
		NSLog(@"%@", me.username);
		[self.ini3_auth authenUserFacebook:me.id];
	}];
}

-(void)DoLoginIni3
{
	if( FBSession.activeSession.isOpen ) {
		[self SendRequestIni3UserInfo];
	} else {
        [FBSession openActiveSessionWithReadPermissions:nil
                                           allowLoginUI:YES
                                      completionHandler:^
		 (FBSession *session, FBSessionState status, NSError *error)
		 {
			 // if login fails for any reason, we alert
			 if (error) {
				 UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Error"
																 message:error.localizedDescription
																delegate:nil
													   cancelButtonTitle:@"OK"
													   otherButtonTitles:nil];
				 [alert show];
				 IOS::nativeOnResultAuthenFacebook("", "");
				 // if otherwise we check to see if the session is open, an alternative to
				 // to the FB_ISSESSIONOPENWITHSTATE helper-macro would be to check the isOpen
				 // property of the session object; the macros are useful, however, for more
				 // detailed state checking for FBSession objects
			 } else if (FB_ISSESSIONOPENWITHSTATE(status)) {
				 // send our requests if we successfully logged in
				 [self SendRequestIni3UserInfo];
			 }
		 }
		 ];
	}
}

-(void)DoLogOffFacebook
{
}

-(void)DoModalEditBox
{
	//	g_bEditing = TRUE;
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle: @"입력"
													message: @" \r\n "
												   delegate:self
										  cancelButtonTitle:@"취소"
										  otherButtonTitles:@"확인", nil];
	
	alert.transform = CGAffineTransformTranslate( alert.transform, 0.0, 0.0 );
	// 이름 필드
	UITextField *tf = [[UITextField alloc] initWithFrame:CGRectMake(12,45,260,25)];
	tf.placeholder = @"이름 입력";
	[tf setBackgroundColor:[UIColor whiteColor]];
	tf.clearButtonMode = UITextFieldViewModeWhileEditing;
	tf.keyboardType = UIKeyboardTypeDefault;
	tf.keyboardAppearance = UIKeyboardAppearanceAlert;
	tf.autocapitalizationType = UITextAutocapitalizationTypeNone;
	[tf setReturnKeyType:UIReturnKeyDone];
	//	tf.autocorrectionType = UITextAutocorrectionTypeNo;
	[alert addSubview:tf];
	tfName = tf;
	
	if( [tf isFirstResponder] )
		[tf resignFirstResponder];
	[alert show];
}*/

// DoModalEditBox에서 버튼을 누르면 이쪽으로 온다.
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if( buttonIndex == 1 )  // ok누를때만
    {
        if( GetAlertHandler() )
            GetAlertHandler()->OnAlert();
        
        //    m_pMain->SetFrameMoveStop(FALSE);  // 멈췄던거 풀어줌
        if( tfName != nil )	// 메모리워닝 창 같은거 확인 누를때도 여기 들어오기땜에 검사해줘야 함
        {
            NSLog(@"alert pushed index=%d text=%@", buttonIndex, tfName.text);
            NSString *str = tfName.text;
            if( str )
            {
                const char *utf8 = [str UTF8String];
                if( buttonIndex == 1 )
                    XE::GetMain()->GetpGame()->OnEndEditBox( IOS::x_idEditField, utf8 );
            }
        }
    }
    tfName = nil;
}

-(void)DoEditBox:(float)x y:(float)y w:(float)w h:(float)h handler:(XWndEdit*)pWnd
{
	float rWidth = GRAPHICS->GetRatioWidth();
	float rHeight = GRAPHICS->GetRatioHeight();
	float x2 = x * rWidth;
	float y2 = y * rHeight;
	float w2 = w * rWidth;
	float h2 = h * rHeight;
	float left_space = (GRAPHICS->GetLogicalScreenSize().w - XE::GetGameWidth()) / 2.f;
	left_space *= rWidth;
	x2 += left_space;
	x2 /= m_fRetinaScale;
	y2 /= m_fRetinaScale;
	w2 /= m_fRetinaScale;
	h2 /= m_fRetinaScale;
	// 이름 필드
	UITextField *tf = [[UITextField alloc] initWithFrame:CGRectMake(x2,y2,w2,h2)];
	tf.placeholder = @"메시지 입력";
    NSLog( @"DoEditBox:%@", tf.placeholder );
	[tf setBackgroundColor:[UIColor whiteColor]];
	tf.clearButtonMode = UITextFieldViewModeWhileEditing;
	tf.keyboardType = UIKeyboardTypeDefault;
	tf.keyboardAppearance = UIKeyboardAppearanceAlert;
	tf.autocapitalizationType = UITextAutocapitalizationTypeNone;
	[tf setReturnKeyType:UIReturnKeyDone];
    tf.delegate = self;
	if( [tf isFirstResponder] )
		[tf resignFirstResponder];
    [[self view] addSubview:tf];
    m_pWndEditHandler = pWnd;
    
}
-(BOOL)textFieldShouldReturn:(UITextField *)textField
{
    NSString *str = textField.text;
    const char *utf8 = [str UTF8String];
    NSLog(@"%@", str );
    if( m_pWndEditHandler )
    {
        m_pWndEditHandler->SetszString( utf8 );
        m_pWndEditHandler->CallEventHandler( XWM_ENTER, (DWORD)utf8 );
		if( m_pWndEditHandler )
			if( m_pWndEditHandler->GetpDelegate() )
				m_pWndEditHandler->GetpDelegate()->OnDelegateEnterEditBox(
													m_pWndEditHandler, utf8);
			
    }
    [textField resignFirstResponder];
    [textField removeFromSuperview];
    return TRUE;
}

-(void)textFieldDidEndEditing:(UITextField *)textField
{
    [textField resignFirstResponder];
    [textField removeFromSuperview];
}

#pragma mark IAP
static NSString *s_strPurchased = nil;
// 상품정보 유무확인시 델리게이트
-(void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
	NSLog(@"productRequest num=%d", [response.products count]);
	//    NSArray *product = response.products;
    for( SKProduct *product in response.products )
    {
        NSLog(@"%@", product.localizedTitle);
        NSLog(@"%@", [product localizedDescription]);
        NSLog(@"%@", [product price]);
        NSLog(@"%@", [product.priceLocale description]);
        NSLog(@"%@", [product productIdentifier]);
        // 상품정보가 확실히 존재하므로 구매요청을 날림
        SKPayment *payment = [SKPayment paymentWithProduct:product];
        [[SKPaymentQueue defaultQueue] addPayment:payment];
        s_strPurchased = product.productIdentifier;     // 구매요청한 상품의 id를 받아둠
		const char *cSku = [s_strPurchased UTF8String];
		XE::OnPurchaseStart( cSku );
    }
	
//a    [request autorelease];
}

// 결제 결과에 따른 델리게이트
-(void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    for( SKPaymentTransaction *transaction in transactions )
    {
        switch( transaction.transactionState )
        {
            case SKPaymentTransactionStatePurchased:    // 구매완료
                NSLog(@"SKPaymentTransactionStatePurchased. %@ %@", s_strPurchased, transaction.transactionIdentifier );
                [self completeTransaction:transaction];
                break;
            case SKPaymentTransactionStateFailed:       // 구매실패
                NSLog(@"SKPaymentTransactionStateFailed");
                [self failedTransaction:transaction];
                break;
            case SKPaymentTransactionStateRestored:     // 재구매
                NSLog(@"SKPaymentTransactionStateRestored");
                [self restoreTransaction:transaction];
                break;
            case SKPaymentTransactionStatePurchasing:
                NSLog(@"SKPaymentTransactionStatePurchasing");
                break;
        }
        s_strPurchased = nil;
    }
}

-(void)completeTransaction:(SKPaymentTransaction *)transaction
{
	/*
    // 구매완료 처리
    //
    // 이전 프로세스에서 구매중 다운되거나 해서 트랜잭션이 제대로 끝나지 않았다면 재시작시 여기로 들어올수 있다.
	// base64인코딩된 json형태의 영수증 문자열
	NSString *strTransactionReceipt = [[VerificationController sharedInstance] getTransactionReceipt:transaction];
	if( strTransactionReceipt == nil )
	{
		NSLog(@"failed verification receipt");
		[self failedTransaction:transaction];
		return;
	}
	NSString *jsonPurchaseInfo = [[VerificationController sharedInstance] getPurchaseInfo:transaction];
/*	NSString *jsonTransactionReceipt = @"hello";
	if([[VerificationController sharedInstance] verifyPurchase:transaction] == FALSE )
	{
		NSLog(@"failed verification receipt");
		[self failedTransaction:transaction];
		return;
	} */
	
	/*
	NSLog(@"completeTransaction:id=%@", [transaction transactionIdentifier] );
	NSLog(@"transDate=%@", [transaction transactionDate] );
	SKPayment *payment = [transaction payment];
	NSLog(@"applicationUsername=%@",[payment applicationUsername]);
	NSLog(@"product id=%@",[payment productIdentifier]);
	NSLog(@"quantity=%d",[payment quantity]);
	NSLog(@"requestData=%@",[payment requestData]);
	const char *cIdProduct = [[payment productIdentifier] UTF8String];
	const char *cIdTransaction = [[transaction transactionIdentifier] UTF8String];
	const char *cTranDate = [[[transaction transactionDate] description] UTF8String];
	const char *cReceipt = [strTransactionReceipt UTF8String];
	const char *cjsonPurchaseInfo = [jsonPurchaseInfo UTF8String];
	XE::OnPurchaseFinishedConsume( FALSE,
								  cjsonPurchaseInfo,
								  cIdProduct,
								  cReceipt,
								  "",
								  cIdTransaction,
								  cTranDate);
    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
	 
	 */
}
-(void)failedTransaction:(SKPaymentTransaction *)transaction
{
    // 구매실패 처리
    //
    if( transaction.error.code != SKErrorPaymentCancelled )
    {
        int errorCode = transaction.error.code;
        NSLog(@"error code = %d", transaction.error.code);
        XAlert( 0, "purchase fail: error code=%d", errorCode );
    }
	XE::OnPurchaseError("failedTransaction");
    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}
-(void)restoreTransaction:(SKPaymentTransaction *)transaction
{
    // 재구매 처리
    //
//    XAlert( 0, "재구매 입니다" );
	XBREAKF(1, "restoreTransaction");
    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}

// 구매요청을 날린다
-(void)sendRequestPurchase:(NSString *)idProduct
{
	// 우선 구매 물건이 제대로 등록되어있는건지 확인한다. productsRequest로 돌아온다.
	NSLog(@"XEViewController::sendRequestPurchase");
    SKProductsRequest *request = [[SKProductsRequest alloc] initWithProductIdentifiers:[NSSet setWithObject:idProduct]];
    request.delegate = self;
    //
    [request start];
}


@end
