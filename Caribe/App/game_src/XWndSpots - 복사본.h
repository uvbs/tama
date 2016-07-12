#pragma once
#include "_Wnd2/XWndSprObj.h"
#include "_Wnd2/XWndView.h"
#include "XStruct.h"

class XSpot;
class XSpotCastle;
class XSpotJewel;
class XSpotSulfur;
class XSpotMandrake;
class XSpotNpc;
class XSpotVisit;
class XSpotCampaign;
class XSpotCash;
class XSpotDaily;
class XSpotSpecial;
class XSpotCommon;
class XWndTextString;
class XWndImage;
class XSprObj;
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/01/07 18:24
*****************************************************************/
class XWndSpot : public XWndSprObj
{
public:
	static ID s_idLastTouch;
private:
	XSpot *m_pBaseSpot = nullptr;
	CTimer m_timerBlink;
// 	XSprObj *m_psoBlilliant = nullptr;
	XWndTextString *m_pTextName = nullptr;
	XWndTextString *m_pLevelText = nullptr;	// 레벨 텍스트
	XWndTextString *m_pTextDebug = nullptr;	// 디버그용 텍스트
	XWndImage *m_pImg = nullptr;				// 심볼 이미지
	bool m_bHideDeactive = false;			// 스팟이 비활성화 상태일때 이름을 감출것인지.
	XSprObj *m_psoEff = nullptr;
	XSprObj *m_psoIndicate = nullptr;
	void Init() {}
	void Destroy();
protected:
	ID m_idSpot = 0;
public:
	XWndSpot( XSpot *pBaseSpot, LPCTSTR szSpr, ID idAct, const XE::VEC2& vPos );
	virtual ~XWndSpot() { Destroy(); }
	//
	GET_ACCESSOR( XSpot*, pBaseSpot );
	GET_SET_ACCESSOR( bool, bHideDeactive );
	GET_ACCESSOR( XWndTextString*, pTextName );
	GET_ACCESSOR( XWndTextString*, pLevelText );
	GET_ACCESSOR( XWndImage*, pImg );
	virtual void UpdateInfoText( _tstring& strOut ) {}
//	virtual LPCTSTR GetszName() = 0;	// 스팟의 이름을 리턴
	//virtual LPCTSTR GetszLevel() = 0;	// 레벨 텍스트.
	virtual bool IsEnemySpot() = 0;		// 스팟이 적스팟인가	
	virtual _tstring GetstrDebugText() { return _tstring(); }
	virtual XCOLOR GetColorTextName() { return XCOLOR_WHITE; }
	virtual _tstring GetstrName();
	void DoBlink( float sec ) {
		m_timerBlink.Set( sec );
	}
	void Draw() override;
	void Update() override;
	void Update2();
	void OnAutoUpdate() override;
	int Process( float dt ) override;
	void OnLButtonDown( float lx, float ly ) override;
	void OnLButtonUp( float lx, float ly ) override;
protected:
	virtual XWndTextString* UpdateName();
}; // class XWndSpot

////////////////////////////////////////////////////////////////
// 유저 스팟
class XWndCastleSpot : public XWndSpot
{
	XSpotCastle *m_pSpot = nullptr;			// 스팟 정보
//	XWndTextString *m_pText;		// 이름 텍스트
// 	XWndTextString *m_pLevelText;	// 레벨 텍스트
// 	XWndImage *m_pImg;				// 심볼 이미지
//	XWndTextString *m_pTextDebug;	// 디버그용 텍스트
	//XWndResIcon *m_pResIcon;
//	CTimer m_TimerUpdate;
	void Init() {}
	void Destroy() {}
public:
	XWndCastleSpot( XSpotCastle* pSpot );
	virtual ~XWndCastleSpot() { Destroy(); }
	//
	void Update( void ) override;
// 	int Process( float dt ) override;
	void UpdateInfoText( _tstring& strOut ) override;
// 	LPCTSTR GetszName() override { 
// 		if( m_pSpot->IsQuestion() )
// 			return _T("?"); 
// 		else
// 			return m_pSpot->GetszName();
// 	}
//	LPCTSTR GetszLevel() override;
	bool IsEnemySpot() override;
	_tstring GetstrDebugText() override;
	_tstring GetstrName() override;

//	GET_SET_ACCESSOR(XWndTextString*, pText);
//	void Draw( void );
};

////////////////////////////////////////////////////////////////
// 보석광산 스팟
class XWndJewelSpot : public XWndSpot
{
	XSpotJewel *m_pSpot = nullptr;
// 	XWndTextString *m_pText;
// 	XWndTextString *m_pLevelText;	// 레벨 텍스트
// 	XWndImage *m_pImg;				// 심볼 이미지
	XWndTextString *m_pTextDebug;	// 디버그용 텍스트
//	CTimer m_timerProduceUI;		// x초마다 한번씩 생산량을 보옂주는 타이머	
	void Init() {
//		m_pText = nullptr;
	}
	void Destroy();
public:
	XWndJewelSpot( XSpotJewel* pSpot );
	virtual ~XWndJewelSpot() { Destroy(); }
	//
	void Update( void ) override;
	void UpdateInfoText( _tstring& strOut ) override;
// 	LPCTSTR GetszName() override {
// 		if( m_pSpot->IsQuestion() )
// 			return _T( "?" );
// 		else
// 			return m_pSpot->GetszName();
// 	}
	bool IsEnemySpot() override;
	_tstring GetstrDebugText() override;
};


/**
 유황 스팟
*/
class XWndSulfurSpot : public XWndSpot
{
	XSpotSulfur *m_pSpot = nullptr;
//	XWndTextString *m_pText;
	void Init() {}
	void Destroy();
public:
	XWndSulfurSpot( XSpotSulfur* pSpot );
	virtual ~XWndSulfurSpot() { Destroy(); }
	//
	void Update( void ) override;
	void UpdateInfoText( _tstring& strOut ) override;
	bool IsEnemySpot() override { return true; }
	_tstring GetstrDebugText() override;
#ifdef WIN32
	void Draw( void ) override;
#endif // WIN32
};

////////////////////////////////////////////////////////////////
// 만드레이크 스팟
class XWndMandrakeSpot : public XWndSpot
{
	XSpotMandrake *m_pSpot = nullptr;
	XWndTextString *m_pTextWin = nullptr;

	void Init() {}
	void Destroy() {}
public:
	XWndMandrakeSpot( XSpotMandrake* pSpot );
	virtual ~XWndMandrakeSpot() { Destroy(); }
	//
	void Update( void ) override;
	void UpdateInfoText( _tstring& strOut ) override;
	bool IsEnemySpot() override;
	_tstring GetstrDebugText() override;
	_tstring GetstrName() override;
};

////////////////////////////////////////////////////////////////
// Npc 스팟
class XWndNpcSpot : public XWndSpot
{
	XSpotNpc *m_pSpot = nullptr;		// shared_ptr로 바꿀것.
// 	XWndTextString *m_pText;
// 	XWndTextString *m_pTextDebug;	// 디버그용 텍스트
	void Init() {}
	void Destroy() {}
public:
	XWndNpcSpot( XSpotNpc* pSpot );
	virtual ~XWndNpcSpot() { Destroy(); }
	//
	void Update( void ) override;
	void UpdateInfoText( _tstring& strOut ) override;
	bool IsEnemySpot() override { return true; }
	_tstring GetstrDebugText() override;
	_tstring GetstrName() override;
	void SetSprSpot();
//	XCOLOR GetColorTextName() override;
};

////////////////////////////////////////////////////////////////
// Daily 스팟
class XWndDailySpot : public XWndSpot
{
	XSpotDaily *m_pSpot = nullptr;
	void Init() {}
	void Destroy() {}
public:
	XWndDailySpot( XSpotDaily* pSpot );
	virtual ~XWndDailySpot() { Destroy(); }
	//
	virtual void Update( void );
	void UpdateInfoText( _tstring& strOut ) override;
	bool IsEnemySpot() override { return true; };
	_tstring GetstrDebugText() override;
	XCOLOR GetColorTextName() override { return XCOLOR_YELLOW; }
	_tstring GetstrName() override {
		return XTEXT(80190);
	}
};

////////////////////////////////////////////////////////////////
// Special 스팟
class XWndSpecialSpot : public XWndSpot
{
	XSpotSpecial *m_pSpot = nullptr;
// 	XWndTextString *m_pText;
	void Init() {}
	void Destroy() {}
public:
	XWndSpecialSpot( XSpotSpecial* pSpot );
	virtual ~XWndSpecialSpot() { Destroy(); }
	//
	virtual void Update( void );
	void UpdateInfoText( _tstring& strOut ) override;
	bool IsEnemySpot() override { return true; };
	_tstring GetstrDebugText() override;
	XCOLOR GetColorTextName() override { return XCOLOR_YELLOW; }
};

//////////////////////////////////////////////////////////////////////////
class XWndCampaignSpot : public XWndSpot
{
	XSpotCampaign *m_pSpot = nullptr;
// 	XWndTextString *m_pText;
	void Init() {}
	void Destroy() {}
public:
	XWndCampaignSpot( XSpotCampaign* pSpot );
	virtual ~XWndCampaignSpot() { Destroy(); }
	//
	virtual void Update( void );
	bool IsEnemySpot() override { return true; }
	_tstring GetstrDebugText() override;
	XCOLOR GetColorTextName() override { return XCOLOR_YELLOW; }
};

//////////////////////////////////////////////////////////////////////////
class XWndVisitSpot : public XWndSpot
{
	XSpotVisit *m_pSpot = nullptr;
	void Init() {}
	void Destroy() {}
public:
	XWndVisitSpot( XSpotVisit* pSpot );
	virtual ~XWndVisitSpot() { Destroy(); }
	//
	virtual void Update( void );
	bool IsEnemySpot() override { return true; }
	_tstring GetstrDebugText() override;
	XCOLOR GetColorTextName() override { return XCOLOR_YELLOW; }
};

//////////////////////////////////////////////////////////////////////////
class XWndCashSpot : public XWndSpot
{
	XSpotCash *m_pSpot = nullptr;
	void Init() {}
	void Destroy() {}
public:
	XWndCashSpot( XSpotCash* pSpot );
	virtual ~XWndCashSpot() { Destroy(); }
	//
	virtual void Update( void );
	bool IsEnemySpot() override { return true; };
	_tstring GetstrDebugText() override;
};

//////////////////////////////////////////////////////////////////////////
class XWndCommonSpot : public XWndSpot
{
	XSpotCommon *m_pSpot = nullptr;
	void Init() {}
	void Destroy() {}
public:
	XWndCommonSpot( XSpotCommon* pSpot );
	virtual ~XWndCommonSpot() { Destroy(); }
	//
	virtual void Update( void );
	bool IsEnemySpot() override;
	_tstring GetstrDebugText() override;
	XCOLOR GetColorTextName() override { return XCOLOR_YELLOW; }
};
