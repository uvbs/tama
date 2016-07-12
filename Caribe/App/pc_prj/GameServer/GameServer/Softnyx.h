/********************************************************************
	@date:	2016/03/31 17:03
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\pc_prj\GameServer\GameServer\Softnyx.h
	@author:	xuzhu
	
	@brief:	소프트닉스 관련 모듈들
*********************************************************************/
#pragma once

#ifndef _SERVER
#error "서버에서만 사용해야함"
#endif // not server

namespace XGAME {
	struct xInApp;
};

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/03/31 17:03
*****************************************************************/
class XSoftnyx
{
	XSoftnyx() { Init(); }		// static으로만 씀.
public:
	virtual ~XSoftnyx() { Destroy(); }
	// get/setter
	// public member
	static XGAME::xtErrorIAP sDoVerifyAfterPurchase( 
																						const std::string& strcJson
																					, const std::string& strPayload
																					, const _tstring& idsProduct
																					, XGAME::xInApp* pOut );
private:
	// private member
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XSoftnyx
