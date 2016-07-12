#pragma once
#include <string>
#include "etc/InputMng.h"

class XClientMain;
class XEContent;

namespace XE {
	// 인증결과를 받는 범용 구조체
	struct xRESULT_AUTHEN	{
		std::string string[4];
		DWORD dwValue[4];
		xRESULT_AUTHEN() {
			XCLEAR_ARRAY(dwValue);
		}
	};
	XClientMain* GetMain();
	XEContent* GetGame();
	void SetApp( XClientMain *pApp );
	void RestoreDevice();
	void OnPauseHandler();
	void OnResumeHandler();
	void OnPurchaseFinishedConsume( BOOL bGoogle, const char *cJson, const char *cidProduct, const char *cToken, const char *cPrice, const char *cOrderId, const char *cTransactDate );
	void OnPurchaseError( const char *cErr );
	void OnPurchaseStart( const char *cSku );
	//	xRESULT_AUTHEN* GetpResultAuthen();
	void OnResultLoginAuthen( xRESULT_AUTHEN *pResultAuthen );
		//	void DestroyResultAuthen( void );
// 	BOOL GetbResumeBlock( void );
	void SetbResumeBlock( BOOL bBlock );
//  	void cbOnRecvProfileImageByFacebook( const std::string& strFbUserId, DWORD* pImage, int w, int h, int bpp );
// 	xResult_FacebookProfileImage PopResultFacebookProfileImage();
//	void cbOnRecvProfileImageByFacebook( const xResult_FacebookProfileImage& resultProfile );
};

////////////////////////////////////////////////////////////////
class XApp
{
public:
	static XClientMain* sGetMain() {
		return s_pAppMain;
	}
	static bool s_bInitedIAP;
private:
	static XClientMain *s_pAppMain;

	void Init() {
		s_pAppMain = NULL;
	}
	void Destroy();
public:
	XApp();
	virtual ~XApp() { Destroy(); }
	//
//	GET_ACCESSOR( XClientMain*, pAppMain );
	//
	virtual void Create( XE::xtDevice device, float widthPhy, float heightPhy );
	virtual XClientMain* OnCreateAppMain( XE::xtDevice device, float widthPhy, float heightPhy ) = 0;
	// 인앱시스템 초기화 완료
	void cbRecvInitGoogleIAP() {
		s_bInitedIAP = true;
	}

};
////////////////////////////////////////////////////////////////
