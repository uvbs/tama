/********************************************************************
	@date:	2015/07/05 18:44
	@file: 	C:\xuzhu_work\Project\iPhone_may\Caribe\App\game_src\XWndObj.h
	@author:	xuzhu
	
	@brief:	월드맵상의 오브젝트
*********************************************************************/
#pragma once
#include "_Wnd2/XWndPopup.h"
#include "_Wnd2/XWndSprObj.h"
#include "XPropBgObjH.h"

class XSprObj;
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/07/05 18:44
*****************************************************************/
class XWndObjBird : public XWnd
{
public:
	XWndObjBird( const XE::VEC2& vPos, float dAng, float speed );
	virtual ~XWndObjBird() { Destroy(); }
	// get/setter
	// public member
// private member
private:
	XSprObj *m_pSprObj = nullptr;
	XE::VEC2 m_vDelta;		// 이동방향
// private method
private:
	void Init() {}
	void Destroy();
	int Process( float dt ) override;
	void Draw() override;
}; // class XWndObjBird

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/20 17:25
*****************************************************************/
class XWndPlayerInfo : public XWndPopup
{
public:
	XWndPlayerInfo();
	virtual ~XWndPlayerInfo() { Destroy(); }
	// get/setter
	// public member
private:
	// private member
	int m_Test = 0;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Update() override;
}; // class XWndPlayerInfo

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/04/25 17:03
*****************************************************************/
class XWndBgObj : public XWndSprObj
{
public:
//	XWndBgObj( const _tstring& strSpr, ID idAct, const XE::VEC2& vwPos );
	XWndBgObj( const xnBgObj::xProp& prop );
	virtual ~XWndBgObj() { Destroy(); }
	// get/setter
	GET_ACCESSOR_CONST( const xnBgObj::xProp&, Prop );
	// public member
	void SetTimer( float sec ) {
		if( m_timerOut.IsOff() )
			m_timerOut.Set( sec );
	}
	// 화면밖을 벗어난지 
	bool IsOverTime() const {
		return m_timerOut.IsOver() != FALSE;
	}
	void OffTimer() {
		m_timerOut.Off();
	}
	void UpdateProp( const xnBgObj::xProp* pProp ) {
		m_Prop = *pProp;
		SetbUpdate( true );
	}
private:
	// private member
	xnBgObj::xProp m_Prop;
	CTimer m_timerOut;			// 화면을 벗어남 타이머
	ID m_idStream = 0;			// 사운드 스트림(아이디는 0부터 나옴. -1이 실패한 아이디)
private:
	// private method
	void Init() {}
	void Destroy();
	void ProcessMsg( const std::string& strMsg ) override;
	int Process( float dt ) override;
	void Update() override;
}; // class XWndBgObj

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/04/25 18:01
*****************************************************************/
// class XWndBgObjSound : public XWnd
// {
// public:
// 	XWndBgObjSound() { Init(); }
// 	virtual ~XWndBgObjSound() { Destroy(); }
// 	// get/setter
// 	// public member
// private:
// 	// private member
// 	int m_Test = 0;
// private:
// 	// private method
// 	void Init() {}
// 	void Destroy() {}
// }; // class XWndBgObjSound
// ////////////////////////////////////////////////////////////////
// XWndBgObjSound::XWndBgObjSound( )
// {
// 	Init();
// }
// 
// void XWndBgObjSound::Destroy()
// {
// }

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/25 11:01
*****************************************************************/
class XWndBgObjLayer : public XWnd
{
public:
	XWndBgObjLayer();
	virtual ~XWndBgObjLayer() { Destroy(); }
	// get/setter
	// public member
#ifdef _xIN_TOOL
	ID GetidSelected() {
		return ( m_pPropSelected ) ? m_pPropSelected->m_idObj : 0;
	}
	void OnClickWorld() {
		m_pSelected = nullptr;
		m_pPropSelected = nullptr;
	}
	bool IsObjDragging() {
		return m_bDragging;
	}
	void UpdateSelected() {
		if( m_pSelected ) {
			m_pSelected->UpdateProp( m_pPropSelected );
			m_pSelected->SetbUpdate( true );
		}
	}
#endif // _xIN_TOOL
	void UpdateCurrFocus( const XE::VEC2& vFocus, const XE::VEC2& sizeView );
private:
	// private member
//	XVector<xnBgObj::xProp*> m_aryAllProps;
#ifdef _xIN_TOOL
	XWndBgObj* m_pSelected = nullptr;
	xnBgObj::xProp* m_pPropSelected = nullptr;
	bool m_bRButtDown = false;
	bool m_bCopyed = false;
	bool m_bMoved = false;
	bool m_bDragging = false;
	XE::VEC2 m_vTouchPrev;
	xnBgObj::xProp m_PropPrev;
#endif // _xIN_TOOL
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Update() override;
	bool UpdateSprObj( const xnBgObj::xProp* pProp, bool bIsIn );
	bool UpdateSoundObj( const xnBgObj::xProp* pProp, bool bIsIn );
#ifdef _xIN_TOOL
	void Draw() override;
	void OnRButtonDown( float lx, float ly ) override;
	void OnRButtonUp( float lx, float ly ) override;
	void OnMouseMove( float lx, float ly ) override;
	int OnClickBgObj( XWnd* pWnd, DWORD p1, DWORD p2 );
	BOOL OnKeyDown( int keyCode ) override;
	XWndBgObj* CreateWndBgObj( xnBgObj::xProp *pProp );
#endif // _xIN_TOOL
}; // class XWndBgObjLayer

/****************************************************************
* @brief 자원지에서 주기적으로 뜨는 생산량 메시지
* @author xuzhu
* @date	2015/03/18 13:49
*****************************************************************/
// class XWndProduceMsg2 : public XWndSpotMsg
// {
// 	void Init() {}
// 	void Destroy() {}
// public:
// 	XWndProduceMsg( ID idSpot, LPCTSTR szText, float size = 20.f, XCOLOR col = XCOLOR_WHITE );
// 	XWndProduceMsg( const XE::VEC2& vPos, LPCTSTR szText, float size = 20.f, XCOLOR col = XCOLOR_WHITE );
// 	virtual ~XWndProduceMsg() { Destroy(); }
// 	//
// 	int Process( float dt ) override;
// }; // class XWndProduceMsg
