/********************************************************************
	@date:	2016/03/02 12:53
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\pc_prj\Bot\Bot\XBotMng.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XFSMConnector.h"

class XBotObj;
/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/03/02 12:53
*****************************************************************/
class XBotMng
{
public:
	static std::shared_ptr<XBotMng>& sGet();
	static void sDestroyInstance();
public:
	XBotMng();
	virtual ~XBotMng() { Destroy(); }
	//
	XBotObj* AddBotObj( XBotObj* pObj );
	void DestroyBotObj( XBotObj* pObj );
	void Process( float dt );
	void DoStart();
	int GetMaxObj() const {
		return m_listBots.size();
	}
	int GetNumConnectingL() {
		return m_aryNumBots[xConnector::xFID_CONNECTING_LOGINSVR ];
	}
	int GetNumLoginingL() {
		return m_aryNumBots[ xConnector::xFID_LOGINING_LOGINSVR ];
	}
	int XBotMng::GetNumConnectingG() {
		return m_aryNumBots[ xConnector::xFID_CONNECTING_GAMESVR ];
	}
	int GetNumLoginingG() {
		return m_aryNumBots[ xConnector::xFID_LOGINING_GAMESVR ];
	}
	int GetNumOnline() {
		return m_aryNumBots[xConnector::xFID_ONLINE ];
	}
	int GetNumBotByState( xConnector::xtFSMId state ) {
		return m_aryNumBots[ state ];
	}
private:
	static std::shared_ptr<XBotMng> s_spInstance;
	XList4<XBotObj*> m_listBots;
	XVector<int> m_aryNumBots;		// 각 상태의 봇의 개수
	void Init() {}
	void Destroy();
}; // class XBotMng

