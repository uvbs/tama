/********************************************************************
	@date:	2015/06/22 15:42
	@file: 	C:\xuzhu_work\Project\iPhone_may\Caribe\App\game_src\XOrder.h
	@author:	xuzhu
	
	@brief:	컷씬 및 튜토리얼등에 사용되는 명령객체들
*********************************************************************/
#pragma once
#include "XPropHelpH.h"
#include <stack>

XE_NAMESPACE_START( xHelp )
class XPropOrder;
class XPropDialog;
class XPropIndicate;
class XPropCamera;
class XPropTouch;
class XPropReadyTouch;
class XPropDelay;
class XPropMode;
class XSeq;
class XOrder;
/****************************************************************
* @brief 명령객체
* @author xuzhu
* @date	2015/06/22 15:43
*****************************************************************/
class XOrder : public std::enable_shared_from_this < XOrder >
{
public:
	XOrder( XSeq *pSeq, int idxOrder );
	virtual ~XOrder() { Destroy(); }
	//
	GET_ACCESSOR( int, idxOrder );
	virtual void Process( float dt ) {}
	virtual void OnCreate() {}
	virtual void OnDestroy() {}
	virtual void OnClickWnd( const std::string& idsWnd );
	virtual void DispatchEvent( xHelp::xtEvent event );
	virtual void SetForceClose() {
		m_bEnd = true;
	}
	PropOrderPtr GetspProp();
	OrderPtr GetThis() {
		return shared_from_this();
	}
	ID getid() {
		return (ID)m_idxOrder;
	}
	GET_ACCESSOR( XSeq*, pSeq );
	GET_SET_ACCESSOR_CONST( bool, bDestroy );
	inline bool IsDestroy() const {
		return m_bDestroy;
	}
	GET_ACCESSOR( ID, snOrder );
	GET_SET_ACCESSOR( bool, bEnd );
	GET_ACCESSOR( bool, bSceneActive );
	bool IsEndTypeTouch();
private:
	ID m_snOrder = 0;
	XSeq *m_pSeq = nullptr;
	int m_idxOrder = 0;
	bool m_bDestroy = false;
	bool m_bEnd = false;
	bool m_bSceneActive = false;	// 이객체가 생성되던당시 씬액티브상태
	void Init() {
		m_snOrder = XE::GenerateID();
	}
	void Destroy() {}
}; // class XOrder

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/06/22 19:17
*****************************************************************/
class XOrderDialog : public XOrder
{
public:
	static XE::VEC2 s_vSavePos;
public:
	XOrderDialog( XSeq *pSeq, int idxOrder );
	virtual ~XOrderDialog() { Destroy(); }
	// get/setter
	// public member
	void OnCreate() override;
	void OnClickWnd( const std::string& idsWnd ) override;
	std::shared_ptr<XOrderDialog> 
	GetThis() {
		return std::static_pointer_cast<XOrderDialog>( XOrder::GetThis() );
	}
	void OnDestroy() override;
	void SetForceClose() override;
// private member
private:
	std::shared_ptr<XPropDialog> m_spProp;
	std::string m_idsWnd;
	CTimer m_timerSec;
// private method
private:
	void Init() {}
	void Destroy();
}; // class XOrderDialog

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/06/23 13:41
*****************************************************************/
class XOrderIndicate : public XOrder
{
public:
	XOrderIndicate( XSeq *pSeq, int idxOrder );
	virtual ~XOrderIndicate() { Destroy(); }
	// get/setter
	// public member
	void OnCreate() override;
	void OnDestroy() override;
	void OnClickWnd( const std::string& idsClickedWnd ) override;
// private member
private:
	std::shared_ptr<XPropIndicate> m_spProp;
	bool m_bTargeted = false;
	CTimer m_timerSec;
	CTimer m_timerFinish;
	std::vector<std::string> m_aryIdsWnd;
	int m_numFailed = 0;
	PropSeqPtr m_spPropSeq;
// private method
private:
	void Init() {}
	void Destroy() {}
	bool CreateIndicator();
	bool CreateIndicator( const std::string& idsTarget );
	void Process( float dt ) override;
	bool CreateSpr( XWnd *pRoot, const std::string& ids );
	bool CreateSpr( const XE::VEC2& vPos, XWnd *pRoot, const XE::VEC2& sizeTarget );
}; // class XOrderIndicate

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/06/23 15:03
*****************************************************************/
class XOrderCamera : public XOrder
{
public:
	static std::stack<XE::VEC2> s_stackPos;		// push_pos명령으로 저장된 좌표스택
public:
	XOrderCamera( XSeq *pSeq, int idxOrder );
	virtual ~XOrderCamera() { Destroy(); }
	// get/setter
	// public member
	void OnEndPanning();
	void DispatchEvent( xHelp::xtEvent event ) override;
// private member
private:
	std::shared_ptr<XPropCamera> m_spProp;
// private method
private:
	void Init() {}
	void Destroy() {}
}; // class XOrderCamera

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/06/23 19:19
*****************************************************************/
class XOrderTouchLock : public XOrder
{
public:
	XOrderTouchLock( XSeq *pSeq, int idxOrder );
	virtual ~XOrderTouchLock() { Destroy(); }
	// get/setter
	// public member
	void OnCreate() override;
	void OnEndPanning();
	void Process( float dt ) override;
	// private member
private:
	std::shared_ptr<XPropTouch> m_spProp;
	bool m_bControled = false;	// 입력을 잠그거나 푸는작업을 수행했다.
	CTimer m_timerSec;
	// private method
private:
	void Init() {}
	void Destroy() {}
	bool DoControlWnd();
}; // class XOrderTouch

/****************************************************************
* @brief
* @author xuzhu
* @date	2015/06/23 19:19
*****************************************************************/
class XOrderDelay : public XOrder
{
public:
	XOrderDelay( XSeq *pSeq, int idxOrder );
	virtual ~XOrderDelay() { Destroy(); }
	// get/setter
	// public member
	// private member
private:
	std::shared_ptr<XPropDelay> m_spProp;
	CTimer m_Timer;
	// private method
private:
	void Init() {}
	void Destroy();
	void Process( float dt ) override;
}; // class XOrderDelay

/****************************************************************
* @brief
* @author xuzhu
* @date	2015/06/23 19:19
*****************************************************************/
// class XOrderReadyTouch : public XOrder
// {
// public:
// 	XOrderReadyTouch( XSeq *pSeq, int idxOrder );
// 	virtual ~XOrderReadyTouch() { Destroy(); }
// 	// get/setter
// 	// public member
// 	// private member
// private:
// 	std::shared_ptr<XPropReadyTouch> m_spProp;
// 	// private method
// private:
// 	void Init() {}
// 	void Destroy() {}
// 	void OnClickWnd( const std::string& idsWnd ) override;
// }; // class XOrderReadyTouch
/****************************************************************
* @brief
* @author xuzhu
* @date	2015/06/23 19:19
*****************************************************************/
class XOrderMode : public XOrder
{
public:
	XOrderMode( XSeq *pSeq, int idxOrder );
	virtual ~XOrderMode() { Destroy(); }
	void OnCreate() override;
	// get/setter
	// public member
	// private member
private:
	std::shared_ptr<XPropMode> m_spProp;
	// private method
private:
	void Init() {}
	void Destroy() {}
}; // class XOrderMode

OrderPtr sCreateOrder( XSeq *pSeq, int idxOrder );

XE_NAMESPACE_END;
