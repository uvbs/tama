#include "stdafx.h"
#include "XGoogleVerify.h"
#include "XFramework/XHttpMng.h"

#ifdef _INAPP_GOOGLE

//XGoogleVerifyMng* RANKING_DB = NULL;
XGoogleVerifyMng* XGoogleVerifyMng::s_pGoogleVerifyMng;

#ifdef _INAPP_GOOGLE
#define INAPP_GOOGLE_CLIENT_ID					""
#define INAPP_GOOGLE_CLIENT_SECRET				""
#define INAPP_GOOGLE_REDIRECT_URI				"https://accounts.google.com/o/oauth2/auth?scope=https://www.googleapis.com/auth/androidpublisher&response_type=code&accesstype=offline&redirect_uri=...&client_id=..."

#define KNOWN_TRANSACTIONS_KEY              "knownIAPTransactions"
#define ITC_CONTENT_PROVIDER_SHARED_SECRET  "206ba0852e3a4e3780b111e50cbec595"
#endif

XGoogleVerifyMng* XGoogleVerifyMng::sCreateInstance( void ) 
{
	s_pGoogleVerifyMng  = new XGoogleVerifyMng;
	return s_pGoogleVerifyMng;
}

void XGoogleVerifyMng::Init()
{	
	UpdateToken();
}


bool XGoogleVerifyMng::UpdateToken()
{	
	char cBody[8192] = {0,};
	void* pPurchaseInfo = nullptr;
	XHttpMng::sGet()->SendReqPOST(_T(INAPP_GOOGLE_REDIRECT_URI),
											cBody,
											this,		// delegate(this��ü�� ���۷��� ī��Ʈ ��ü�� �Ұ�)
											xHTTP_MAKE_DELEGATE(this, &XGoogleVerifyMng::OnReqFromGoogleToken),
											(void*)pPurchaseInfo);	// ������ ���ƿ����� �񱳿� �ʿ��� ����Ÿ�� ��� �ִ´�.

	if (m_timerGetToken.IsOver() || m_timerGetToken.IsOff())
	{
		m_timerGetToken.Set(120.f); //XGAME�� ���� �Ǿ��ִ� ��ū ���� �ð�����....		
	}
	return true;
}

void XGoogleVerifyMng::OnReqFromGoogleToken(const char *cResponse
																						, xHTTP::xREQ *pReq)
{
}
#endif _INAPP_GOOGLE