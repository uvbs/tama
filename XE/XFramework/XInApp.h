/********************************************************************
	@date:	2016/02/25 21:35
	@file: 	C:\xuzhu_work\Project\iPhone_zero\XE\XFramework\XInApp.h
	@author:	xuzhu
	
	@brief:	IAP 관리자
*********************************************************************/
#pragma once

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/02/25 21:35
*****************************************************************/
class XInApp
{
public:
	static std::shared_ptr<XInApp>& sGet();
	static void sDestroyInstance();
public:
	XInApp() { Init(); }
	virtual ~XInApp() { Destroy(); }
	//
	GET_ACCESSOR_CONST( const std::string&, strcJsonByUnconsumed );
	GET_ACCESSOR_CONST( const std::string&, strcSignature );
	void InitGoogleIAPAsync( const std::string& strPublicKey );
	void cbInitGoogleIAPAsync();
	void cbDetectedUnConsumedItem( const std::string& strcJson, const std::string& strcSignature );
	void cbOnRecvBuyRequest( bool bOk, const std::string& strcJsonReceipt, const std::string& strcSignature, const std::string& strcIdsProduct );
	void DoConsumeItemAsync( const _tstring& _idsProduct );
	void cbFinishedConsumItem( bool bOk, const _tstring& cidsProduct );
	void DoCheckUnconsumedItemAsync();
private:
	static std::shared_ptr<XInApp> s_spInstance;
	bool m_bInited;		// IAP시스템이 초기화가 되었는가
	std::string m_strcJsonByUnconsumed;
	std::string m_strcSignature;			// 암호화된 영수증
	void Init() {}
	void Destroy() {}

}; // class XInApp

