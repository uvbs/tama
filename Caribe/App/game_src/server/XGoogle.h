/********************************************************************
	@date:	2016/03/31 17:20
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\server\XGoogle.h
	@author:	xuzhu
	
	@brief:	구글 관련 코드(서버용)
*********************************************************************/
#pragma once

#ifndef _SERVER
#error "서버에서만 사용해야함"
#endif // not server

namespace XGAME {
	struct xInApp;
};

XE_NAMESPACE_START( xGoogle )
//
struct xReceipt {
	_tstring m_idsProduct;
	std::string m_strOrderId;
	std::string m_strTime;
	int statePurchase = 0;
	std::string m_strToken;
	std::string m_strPayload;
};
//
XE_NAMESPACE_END; // xGoogle


/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/03/31 17:21
*****************************************************************/
class XGoogle
{
	XGoogle() { Init(); }
public:
	virtual ~XGoogle() { Destroy(); }
	// get/setter
	// public member
	static XGAME::xtErrorIAP sDoVerifyAfterPurchase( const std::string& strcJson
																								, const std::string& strcSignature
																								, XGAME::xInApp* pOut );
private:
	// private member
private:
	// private method
	void Init() {}
	void Destroy() {}
	static bool sParseJsonRecipt( const std::string& json
																						, xGoogle::xReceipt* pOut );
	static XGAME::xtErrorIAP sCheckError( const xGoogle::xReceipt& receipt );
}; // class XGoogle
