#pragma once
//#include "XWindow.h"
#include "_Wnd2/XWndPopup.h"
#include "_Wnd2/XWnd.h"
#include "_Wnd2/XWndImage.h"
#include "XSceneWorld.h"
#include "XWndResCtrl.h"

class XPostInfo;

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/27 20:37
*****************************************************************/
class XWndRank : public XWndPopup
{
public:
	XWndRank( const XVector<XSceneWorld::xRankInfo*>& aryRank );
	virtual ~XWndRank() { Destroy(); }
	// get/setter
	// public member
private:
	// private member
	int m_rankMe = 0;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Update() override;
}; // class XWndRank
//////////////////////////////////////////////////////////////////////////
class XWndRankingElem : public XWnd
{
	void Init(){};
	void Destroy(){};
public:
	XWndRankingElem( int nRank, LPCTSTR szName, int nLadder );
	virtual ~XWndRankingElem(){ Destroy(); }
};

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/27 21:22
*****************************************************************/
class XWndMail : public XWndPopup
{
public:
	XWndMail();
	virtual ~XWndMail() { Destroy(); }
	// get/setter
	// public member
private:
	// private member
	int m_Test = 0;
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XWndMail

/**
 @brief 
*/
class XWndMailElem : public XWndImage
{
	XPostInfo *m_pPostInfo;
	XWndImage *m_pAddIcon;
	XWndTextString *m_pTitle;
	void Init() {
		m_pPostInfo = nullptr;
		m_pAddIcon = nullptr;
		m_pTitle = nullptr;
	}
	void Destroy(){}
public:
	//아이템 정보를 받아와야함 : 아이콘, 이름, 가격 / 프랍을 통채로 받아오면 될듯
	XWndMailElem( XPostInfo* pPostInfo );
	~XWndMailElem(){ Destroy(); }

	GET_SET_ACCESSOR( XPostInfo*, pPostInfo );
	void SetTitle( LPCTSTR title );
};

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/05/27 16:13
*****************************************************************/
class XWndMailEach : public XWndPopup
{
public:
//	static XWndStoragyItemElem* sCreateWndAttachItem( XSPPostItem spItem );
public:
	XWndMailEach( XSPAccConst spAcc, ID snPost );
	virtual ~XWndMailEach() { Destroy(); }
	// get/setter
	// public member
private:
	// private member
	XSPAccConst m_spAcc;
	ID m_snPost = 0;
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XWndMailEach


/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/15 12:13
*****************************************************************/
class XWndProfileForSpot : public XWnd
{
public:
	XWndProfileForSpot( XSpot* m_pBaseSpot, const std::string& strFbUserId );
	virtual ~XWndProfileForSpot() { Destroy(); }
	// get/setter
	// public member
	bool IsEmptyPicture() {
//		return m_sizePicture.IsZero() != FALSE;
		return m_psfcProfile == nullptr;
	}
	void SetProfileImage( XSurface *psfcProfile );
	int Process( float dt ) override;
// 	void SetState( int state ) {
// 		if( state == 2 ) {
// 			SetAlphaLocal( 1.f );
// 			m_State = 2;
// 			m_timerDelay.Set( 10 );
// 		}
// 	}
	/// sec시간동안 사라진상태로 있다가 나타나기 시작한다.
	void SetDelayedAppear( float sec ) {
		m_State = 0;
		SetAlphaLocal( 0.f );
		if( sec == 0 )
			m_timerDelay.Set(0);
		else
			m_timerDelay.Set( (float)( 1 + xRandom( (int)sec ) ) );
	}
	bool IsShowProfile() {
		return m_State == 1 || m_State == 2 || m_State == 3;
	}
private:
	// private member
	std::string m_strcFbUserId;
	ID m_idSpot = 0;
	XE::VEC2 m_vwSpot;		// 스팟의 월드상 위치
	std::shared_ptr<XLayoutObj> m_spLayoutObj;  
	XSurface* m_psfcProfile = nullptr;
	CTimer m_timerFade;
	CTimer m_timerDelay;
	int m_State = 0;			// 0:없음 1:나타나는중 2:다나타남 3:사라지는중 
private:
	// private method
	void Init() {}
	void Destroy() {
		SAFE_RELEASE2( IMAGE_MNG, m_psfcProfile );
	}
}; // class XWndProfilePicture

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/20 10:16
*****************************************************************/
class XWndHello : public XWnd
{
public:
	XWndHello( ID idSpot, const XE::VEC2& vwSpot );
	virtual ~XWndHello() { Destroy(); }
	// get/setter
	// public member
	void SetText( const _tstring& strText );
	void UpdatePos();
private:
	// private member
	ID m_idSpot = 0;
	XE::VEC2 m_vSpot;
	CTimer m_timerFade;
	CTimer m_timerDelay;
	_tstring m_strHello;
	int m_State = 0;
private:
	// private method
	void Init() {}
	void Destroy() {}
	int Process( float dt ) override;
	void OnAutoUpdate() override;
	void UpdateSize();
	void Update() override;
}; // class XWndHello

/****************************************************************
* @brief 전투기록 창
* @author xuzhu
* @date	2015/02/24 18:22
*****************************************************************/
class XWndBattleLog : public XWndPopup
{
public:
  static bool s_bAlert;   // 로그버튼에 느낌표여부
private:
  bool m_bAttackLog = false;    // 공격기록/방어기록 탭
	void Init() {}
	void Destroy();
public:
	XWndBattleLog();
	virtual ~XWndBattleLog() { Destroy(); }
	//
  void Update() override;
  int OnClickLogTab( XWnd* pWnd, DWORD p1, DWORD p2 );
}; // class XWndBattleLog

/****************************************************************
* @brief
* @author xuzhu
* @date	2015/03/05 17:37
*****************************************************************/
class XWndSpotMsg : public XWnd
{
protected:
	_tstring m_strText;
	int m_State = 0;
	float m_Size = 20.f;
	XCOLOR m_Color = XCOLOR_WHITE;
	void Init() {}
	void Destroy();
	CTimer m_timerLife;
public:
	XWndSpotMsg( ID idSpot, LPCTSTR szText, float size = 20.f, XCOLOR col = XCOLOR_WHITE );
	XWndSpotMsg( const XE::VEC2& vPos, LPCTSTR szText, float size = 20.f, XCOLOR col = XCOLOR_WHITE );
	virtual ~XWndSpotMsg() {
		Destroy();
	}
	//
	int Process( float dt ) override;
}; // class XWndSpotMsg

/****************************************************************
* @brief 자원지에서 주기적으로 뜨는 생산량 메시지
* @author xuzhu
* @date	2015/03/18 13:49
*****************************************************************/
class XWndProduceMsg : public XWndResourceCtrl
{
	void Init() {}
	void Destroy() {}
public:
	XWndProduceMsg( ID idSpot, const std::vector<XGAME::xRES_NUM>& aryRes, XCOLOR colText = XCOLOR_WHITE );
	XWndProduceMsg( const XE::VEC2& vwPos, const std::vector<XGAME::xRES_NUM>& aryRes, XCOLOR colText = XCOLOR_WHITE );
	XWndProduceMsg( const XE::VEC2& vwPos, XGAME::xtResource typeRes, int num, XCOLOR colText = XCOLOR_WHITE );
	XWndProduceMsg( ID idSpot, XGAME::xtResource typeRes, int num, XCOLOR colText = XCOLOR_WHITE );
	// 	XWndProduceMsg( ID idSpot, LPCTSTR szText, float size = 20.f, XCOLOR col = XCOLOR_WHITE );
	// 	XWndProduceMsg( const XE::VEC2& vPos, LPCTSTR szText, float size = 20.f, XCOLOR col = XCOLOR_WHITE );
	~XWndProduceMsg() {
		Destroy();
	}
	//
	int Process( float dt ) override;
private:
//	ID m_idSpot = 0;
	int m_State = 0;
	CTimer m_timerLife;

}; // class XWndProduceMsg


/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/06/09 15:36
*****************************************************************/
class XWndOption : public XWndPopup
{
public:
	XWndOption( XSPAcc spAcc );
	virtual ~XWndOption() { Destroy(); }
	// get/setter
	// public member
private:
	// private member
	XSPAcc m_spAcc;
private:
	// private method
	void Init() {}
	void Destroy() {}
	int OnClickLow( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickSound( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickLanguage( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickSelectLanguage( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickRegist( XWnd* pWnd, DWORD p1, DWORD p2 );
	BOOL OnCreate();
	void Update();
}; // class XWndOption

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/06/09 16:11
*****************************************************************/
class XWndRegistAcc : public XWndPopup
{
public:
	XWndRegistAcc();
	virtual ~XWndRegistAcc() { Destroy(); }
	// get/setter
	// public member
private:
	// private member
	int m_Test = 0;
private:
	// private method
	void Init() {}
	void Destroy() {}
// 	int OnClickEmail( XWnd* pWnd, DWORD p1, DWORD p2 );
// 	int OnClickPassword( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickRegist( XWnd* pWnd, DWORD p1, DWORD p2 );
	BOOL OnCreate();
	void Update();
}; // class XWndRegistAcc
