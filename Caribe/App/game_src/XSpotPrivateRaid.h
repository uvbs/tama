/********************************************************************
	@date:	2016/09/30 14:24
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XSpotPrivateRaid.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XSpot.h"
#include "constGame.h"


/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/09/30 14:24
*****************************************************************/
class XSpotPrivateRaid : public XSpot
{
public:
	XSpotPrivateRaid( XWorld* pWorld );
	XSpotPrivateRaid( XWorld* pWorld, XPropWorld::xBASESPOT* pProp );
	virtual ~XSpotPrivateRaid() {
		Destroy();
	}
	// get/setter
	// public member
	bool IsNpc() const override {
		return true;
	}
	bool IsPC() const override {
		return !IsNpc();
	}
private:
	// private member
	XList4<XHero*> m_listEnter;			// 출전영웅 리스트
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Serialize( XArchive& ar ) override;
	BOOL DeSerialize( XArchive& ar, DWORD ver ) override;
}; // class XSpotPrivateRaid
