//
//  XEViewController.h
//  xe
//
//  Created by JungWoo Sung on 2014. 1. 24..
//  Copyright (c) 2014년 JungWoo Sung. All rights reserved.
//

#import <UIKit/UIKit.h>
#ifdef _XGLKIT
#import <GLKit/GLKit.h>
#endif
#import <GameKit/GameKit.h>
#import <StoreKit/StoreKit.h>
//#import <Ini3iOSSDK/Ini3UserAuthen.h>
#import "VerificationController.h"
class XWndEdit;


#ifdef _XGLKIT
@interface XEViewController : GLKViewController
#else
@interface XEViewController : UIViewController
#endif
												<UITextFieldDelegate,
												UIAlertViewDelegate,
												SKProductsRequestDelegate,
												//Ini3UserAuthenDelegate,
												SKPaymentTransactionObserver>
{
    //Ini3UserAuthen *delegateIni3Authen;
	UITextField *tfName;
	XWndEdit *m_pWndEditHandler;
	float m_fRetinaScale;
}
// TextField
-(void)DoEditBox:(float)x y:(float)y w:(float)w h:(float)h handler:(XWndEdit*)pWnd;
-(void)DoModalEditBox;
// purchase
-(void)sendRequestPurchase:(NSString *)idProduct;
-(void)completeTransaction:(SKPaymentTransaction *)transaction;
-(void)failedTransaction:(SKPaymentTransaction *)transaction;
-(void)restoreTransaction:(SKPaymentTransaction *)transaction;
// facebook
-(void)DoLogOffFacebook;
-(void)DoLoginFacebook;
-(void)DoLoginIni3;

@end

