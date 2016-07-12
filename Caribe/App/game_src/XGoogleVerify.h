#pragma once

#ifdef _INAPP_GOOGLE
#include "XRefObj.h"
#include "XFramework/XHttpMng.h"

class XGoogleVerifyMng : XRefObj
{
	static XGoogleVerifyMng *s_pGoogleVerifyMng;

public:
	static XGoogleVerifyMng* sCreateInstance(void);
	static XGoogleVerifyMng* sGet() {
		return s_pGoogleVerifyMng;
	}	
public:
	XGoogleVerifyMng(){ Init(); }
	~XGoogleVerifyMng(){ }

	void Init();
	
	bool UpdateToken();
	GET_ACCESSOR(CTimer, timerGetToken);
private:	
	DWORD m_dwlastGetTime = 0;
	CTimer m_timerGetToken;					//토큰 갱신 확인용 타이머.

	_tstring m_RefreshToken;
	_tstring m_AccessToken;
	void OnReqFromGoogleToken(const char *cResponse, xHTTP::xREQ *pReq);
};

inline XGoogleVerifyMng* GetGoogleVerify() {
	return XGoogleVerifyMng::sGet();
}
#endif