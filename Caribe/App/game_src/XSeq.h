/********************************************************************
	@date:	2015/06/22 19:25
	@file: 	C:\xuzhu_work\Project\iPhone_may\Caribe\App\game_src\XSeq.h
	@author:	xuzhu
	
	@brief:	스퀀스 인스턴스
*********************************************************************/
#pragma once
#include "XOrder.h"

XE_NAMESPACE_START( xHelp )
class XPropSeq;
/****************************************************************
* @brief 시퀀스 프로퍼티를 바탕으로 순차적으로 명령객체를 만들고 실행시킨다.
* @author xuzhu
* @date	2015/06/22 18:39
*****************************************************************/
#include "_Wnd2/XWnd.h"
class XSeq : public XWnd
{
public:
	XSeq( PropSeqPtr& spProp );
	virtual ~XSeq() { Destroy(); }
	// get/setter
	GET_ACCESSOR_CONST( PropSeqPtr, spProp );
	// public member
	void OnClickWnd( const std::string& idsButt );
	std::string& GetidsSeq();
	void DispatchEvent( xHelp::xtEvent event );
	OrderPtr FindOrderObj( const std::string& idsOrder );
	void SetDestroyForce();
// private member
private:
	PropSeqPtr m_spProp;
	int m_idxOrder = -1;
	XList4<OrderPtr> m_listOrder;	// 현재 실행중인 오더객체
	bool m_bSceneActive = true;		// 시퀀스가 실행될때 씬의 블로킹상태
	bool m_bCreateOrder = false;
	bool m_bDestroySeq = false;
// private method
private:
	void Init() {}
	void Destroy();
	int Process( float dt ) override;
	OrderPtr FindOrderObj( int idxOrder );
	int GetNumActiveOrder();
	void CreateOrder();
	void OnEndOrder( OrderPtr& spOrder );
}; // class XSeqObj



XE_NAMESPACE_END;
