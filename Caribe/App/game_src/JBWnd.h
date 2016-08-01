#pragma once
#include "XGameWnd.h"


#pragma warning( disable : 4250 )	//  warning C4250: 'XWndButtonHexNumber' : 우위에 따라 'XWndButton::XWndButton::Process'을(를) 상속합니다.

class XBaseItem;

//자원 아이콘
class XWndResIcon : public XWnd
{
	XWndImage* m_pBubble;
	int m_numRes;
	void Init()	{
		m_pBubble = nullptr;
		m_numRes = 0;
	}
	void Destroy(){}
	void Create( XGAME::xtSpot typeSpot, float x, float y );
	int OnClickResIcon( XWnd *pWnd, DWORD p1, DWORD p2 );
public:
	XWndResIcon( XGAME::xtSpot typeSpot, float x, float y );
	XWndResIcon( XGAME::xtSpot typeSpot, const XE::VEC2& vPos );
	virtual ~XWndResIcon(){ Destroy(); }
	//
	GET_SET_ACCESSOR( int, numRes );

	//int OnClickResIcon(XWnd* pWnd, DWORD p1, DWORD p2);
	void Draw( void );
};


//////////////////////////////////////////////////////////////////////////
class XWndResParticle : public XWndImage
{
	//XParticleMng *m_pParticleMng;
	CTimer m_timerLife;
	CTimer m_Timer;			//몇초뒤에 이동을 시작할지
	XE::VEC2 m_vTarget;

	XE::VEC2 m_vStart;
	XE::VEC2 m_vSpot;
//	XSpot *m_pSpot;
	XWndScrollView *m_pScrollWorld;
//	float m_slice = 0.f;
	int m_dir;
	float m_curveX, m_curveY;	//곡선 컨트롤
	XGAME::xtResource m_typeRes;
	int m_start;		//도착했을때 카운팅되는 숫자가 몇에서부터 시작하는지
	void Init() {
		m_start = 0;
//		m_pSpot = nullptr;
		//m_vStart = nullptr;
//		m_slice = 0.f;
		m_dir = 0;
		m_curveX = m_curveY = 0.f;
		m_pScrollWorld = nullptr;
	}
	void Destroy(){}
public:
// 	XWndResParticle( LPCTSTR szFilename, XSpot *pSpot, const XE::VEC2& vPos,
// 		const XE::VEC2& vTarget,
// 		XWndScrollView *pScrollWorld,
// 		XGAME::xtResource typeRes, int start, float time = 0.f );
	XWndResParticle( LPCTSTR szFilename,
									const XE::VEC2& vPosSpot,
//									const XE::VEC2& vPos,
									const XE::VEC2& vTarget,
									XWndScrollView *pScrollWorld,
									XGAME::xtResource typeRes, int start, float time = 0.f );
	virtual ~XWndResParticle(){ Destroy(); }

	int Process( float dt );
};


class XWndInvenHeroElem : public XWnd
{
public:
	static XWndInvenHeroElem* sUpdateCtrl( XWndList *pWndList, XHero *pHero );
private:
	XHero *m_pHero;
	XPropHero::xPROP *m_pProp = nullptr;
	XSurface *m_pBG;
	XSurface *m_pFace;
	XSurface *m_psfcBgGradation = nullptr;
// 	XSurface *m_pStar[ XGAME::xGD_MAX - 1 ];
// 	std::vector<XSurface*> m_aryStarEmpty;
	XSurface *m_psfcStar = nullptr;
	XSurface *m_psfcStarEmpty = nullptr;
	XSurface *m_pNameCard;
	XSurface *m_pSelect = nullptr;
	XSurface *m_pParty;
	XBaseFontObj *m_pName;
	XSurface *m_pUnitFace;
	XSurface *m_pUnitBg;
	XLegion *m_pLegion;
	bool m_bSelected = false;
	_tstring m_strName;
	bool m_bSoul = false;	//영웅은 없고 영혼석만 있는 상태
	//ID m_snSoulStone = 0;	//영혼석의 시리얼넘버
	XSurface *m_pSoulStone = nullptr;		//영혼석 아이콘

	void Init() {
		m_pHero = NULL;
		m_pBG = NULL;
		m_pFace = NULL;
		m_pNameCard = NULL;
		m_pName = nullptr;
		m_pUnitFace = nullptr;
		m_pUnitBg = nullptr;
		m_pParty = nullptr;
		m_pLegion = nullptr;
	}
	void Destory() {
		SAFE_RELEASE2( IMAGE_MNG, m_pBG );
		SAFE_RELEASE2( IMAGE_MNG, m_pFace );
		SAFE_RELEASE2( IMAGE_MNG, m_psfcBgGradation );
		SAFE_RELEASE2( IMAGE_MNG, m_pNameCard );
// 		for( int i = 0; i < 4; ++i ) {
// 			SAFE_RELEASE2( IMAGE_MNG, m_pStar[ i ] );
// 			SAFE_RELEASE2( IMAGE_MNG, m_aryStarEmpty[ i ] );
// 		}
		SAFE_RELEASE2( IMAGE_MNG, m_psfcStar );
		SAFE_RELEASE2( IMAGE_MNG, m_psfcStarEmpty );
		SAFE_RELEASE2( IMAGE_MNG, m_pSelect );
		SAFE_RELEASE2( IMAGE_MNG, m_pUnitFace );
		SAFE_RELEASE2( IMAGE_MNG, m_pUnitBg );
		SAFE_RELEASE2(IMAGE_MNG, m_pParty);
		SAFE_RELEASE2(IMAGE_MNG, m_pSoulStone);
		SAFE_DELETE(m_pName);
		//SAFE_DELETE(m_pSoulCount);
	}

public:
	XWndInvenHeroElem(XHero* pHero, XLegion *pLegion = nullptr);
	XWndInvenHeroElem(XPropHero::xPROP *pProp);	//이건 영혼석용 
	virtual ~XWndInvenHeroElem(){ Destory(); }
	XHero* GetpHero( void ) {
		return m_pHero;
	}
	XPropHero::xPROP* GetpProp();
	ID GetsnHero( void );
	int GetnumStars();
	GET_SET_BOOL_ACCESSOR( bSelected );
	bool IsSoulStone() {
		return m_bSoul;
	}
	virtual void Draw( void );
	virtual void DrawDrag( const XE::VEC2& vMouse );
	void SetUnitFace( void );
	XGAME::xtGrade GetGrade();
	//int GetNumSoulStone();
	//int GetNeedSoulStone();
};

class XWndInvenLegionElem : public XWnd
{
	XPropUnit::xPROP* m_pProp;
	XSurface* m_pBG;
	XSurface* m_pFace;
	XSurface* m_pSelect;
	BOOL m_bSelected = FALSE;
	bool m_bLock = false;

	void Init()
	{
		m_pProp = NULL;
		m_pBG = NULL;
		m_pFace = NULL;
		m_pSelect = NULL;
	}
	void Destroy()
	{
		SAFE_RELEASE2( IMAGE_MNG, m_pBG );
		SAFE_RELEASE2( IMAGE_MNG, m_pFace );
		SAFE_RELEASE2( IMAGE_MNG, m_pSelect );
	}
public:
	XWndInvenLegionElem( XPropUnit::xPROP* pPropUnit );
	virtual ~XWndInvenLegionElem(){ Destroy(); }
	virtual void Draw( void );

	GET_ACCESSOR( XPropUnit::xPROP*, pProp );
	GET_SET_ACCESSOR( BOOL, bSelected );
	void SetLock( bool bFlag );
	void SetOff() {
		m_bSelected = FALSE;
	}
	void SetOn() {
		m_bSelected = TRUE;
	}
};

class XWndLevelupElem : public XWndImage
{
	XWndTextString *m_pText;
	int m_nCount;
	void Init()
	{
		m_pText = nullptr;
		m_nCount = 0;
	}
	void Destroy(){}
public:
	XWndLevelupElem( XBaseItem* pItem );
	virtual ~XWndLevelupElem(){ Destroy(); }

	void Addnum()
	{
		++m_nCount;
		if( m_pText )
			m_pText->SetText( XE::Format( _T( "%d" ), m_nCount ) );
	}
};


//임시로 만들어두는 콜백기능을 포함한 SprObj
class XWndCallbackSpr : public XWndSprObj
{
	int ( XWnd::*m_pFunc )( XWnd *pWnd, DWORD p1, DWORD p2 );
	XWnd *m_pOwner;
	DWORD m_dwParam1;
	DWORD m_dwParam2;
	void Init() {
		m_pFunc = NULL;
		m_pOwner = NULL;
		m_dwParam1 = 0;;
		m_dwParam2 = 0;
	}
	void Destroy(){	}
public:
	template<typename T>
	XWndCallbackSpr( XWnd *pOwner, LPCTSTR szFilename, ID action, const XE::VEC2& vPos, int( T::*pFunc )( XWnd* pWnd, DWORD p1, DWORD p2 ), DWORD param1 = 0, DWORD param2 = 0 )
		: XWndSprObj( szFilename, action, vPos, xRPT_1PLAY_CONT )
	{
		Init();
		typedef int ( XWnd::*CALLBACK_FUNC )( XWnd *, DWORD dwParam1, DWORD dwParam2 );
		m_dwParam1 = param1;
		m_dwParam2 = param2;
		m_pOwner = pOwner;
		m_pFunc = static_cast<CALLBACK_FUNC> ( pFunc );
	}
	virtual ~XWndCallbackSpr(){ Destroy(); }

	int Process( float dt )
	{
		if( m_pOwner && GetpSprObj()->IsFinish() )
			( m_pOwner->*m_pFunc )( this, m_dwParam1, m_dwParam2 );

		return XWndSprObj::Process( dt );
	}
};


//레벨업시 스텟의 변동(증가,감소,변동없음)을 알려주는 화살표
class XWndStatArrow : public XWndImage
{
	void Init(){}
	void Destroy(){}
public:
	//직접 new하고 add해서 사용할때
	XWndStatArrow(float fAfter, float fBefore, float x, float y);
	virtual ~XWndStatArrow(){ Destroy(); }

	//레이아웃에 있는 이미지를 사용할때
	void SetDirection(float fAfter, float fBefore)
	{
		if (fAfter > fBefore)
			SetSurface(XE::MakePath(DIR_UI, _T("legion_stat_up.png")));
		else if (fAfter < fBefore)
			SetSurface(XE::MakePath(DIR_UI, _T("legion_stat_down.png")));
// 		else
// 			SetSurface(XE::MakePath(DIR_UI, _T("legion_stat_keep.png")));
	}
};

class XGuild;
class XWndGuildElem : public XWndImage
{
	XGuild* m_pGuild;
	BOOL m_bJoin;	//가입 가능?

	void Init()
	{
		m_pGuild = nullptr;
		m_bJoin = TRUE;
	};
	void Destroy();
public:
	//XWndGuildElem(LPCTSTR szName, int nNum);
	XWndGuildElem(XGuild* pGuild, BOOL bReqJoin = FALSE);
	virtual ~XWndGuildElem(){ Destroy(); }

	XGuild* GetpGuild()
	{
		return m_pGuild;
	}

	BOOL GetbJoin()
	{
		return m_bJoin;
	}
};



//class XWndAlertSmall : public XWndImage
//{
//	void Init(){};
//	void Destroy(){};
//public:
//	XWndAlertSmall(LPCTSTR szText);
//	virtual ~XWndAlertSmall(){ Destroy(); }
//};

class XWndGuildMember : public XWnd
{
	XGuild::SGuildMember *m_pMember;

	void Init(){
		m_pMember = nullptr;
	};
	void Destroy(){};
public:
	XWndGuildMember(XGuild::SGuildMember *pMember);
	virtual ~XWndGuildMember(){ Destroy(); };
	
	XGuild::SGuildMember* getpMemberInfo() {
		return m_pMember;
	}
};

class XWndGuildJoinReqMember : public XWndImage
{
	XGuild::SGuildMember *m_pUser = nullptr;

	void Init(){};
	void Destroy(){};
public:
	XWndGuildJoinReqMember(XGuild::SGuildMember *pUser);
	virtual ~XWndGuildJoinReqMember(){ Destroy(); };

	ID GetUserID(void)
	{
		return m_pUser->m_idAcc;
	}
};

class XWndPopupDuplicateAcc : public XWndPopup
{
	void Init(){};
	void Destroy(){};
public:
	XWndPopupDuplicateAcc();
	virtual ~XWndPopupDuplicateAcc(){ Destroy(); };

};

class XWndPopupTextScroll : public XWndPopup
{
	XWndScrollView *m_pScroll = nullptr;

	void Init(){};
	void Destroy(){};
public:
	XWndPopupTextScroll(LPCTSTR szText);
	virtual ~XWndPopupTextScroll(){ Destroy(); }

	int OnClickOk(XWnd *pWnd, DWORD p1, DWORD p2);
};

class XWndPopupGuildJoin : public XWndPopup
{
	XGuild *m_pGuild = nullptr;

	void Init(){};
	void Destroy();
public:
	XWndPopupGuildJoin(XWndGuildElem *pElem, XLayout *pLayout, const char *cKey, const char *cGroup = (const char*)nullptr);
	virtual ~XWndPopupGuildJoin(){ Destroy(); }
};

/****************************************************************
* @brief
* @author xuzhu
* @date	2015/05/19 18:03
*****************************************************************/
class _XWndPopupDaily : public XWndPopup
{
	XSpotDaily *m_pSpot = nullptr;
	int m_dowToday = 0;
	CTimer m_timerAlpha;
	void Init() {}
	void Destroy() {}
public:
	_XWndPopupDaily( XSpotDaily *pSpot );
	virtual ~_XWndPopupDaily() { Destroy(); }
	//
	void Update() override;
	void OnAutoUpdate() override;
	int OnClickDow( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickEnter( XWnd* pWnd, DWORD p1, DWORD p2 );
	int Process( float dt ) override;
	int OnClickReset( XWnd* pWnd, DWORD p1, DWORD p2 );
}; // class XWndPopupDaily
/**
 @brief 스팟 팝업
*/
class XWndPopupSpotMenu : public XWndPopup
{
public:
//	static void sDestroyWnd() { XE::GetGame()->DestroyWndByIdentifier("menu.circle"); }
	STATIC_DESTROY_WND("menu.circle");
private:
	enum xtMenu {
		xCM_ATTACK,
		xCM_RECON,
		xCM_COLLECT,		// 수거메뉴
		xCM_CHANGE_PLAYER_CASH,	// 대전상대 바꿈
		xCM_CHANGE_PLAYER_GOLD,	// 대전상대 바꿈
		xCM_REGEN,			// 새상대 리젠
		xCM_MANDRAKE_WITHDRAW,	// 만드레이크 회수
		xCM_LOG_SULFUR,				// 유황 약탈기록
//#ifdef _CHEAT
		xCM_KILL,			// 전투생략치트
//#endif
	};
//	XWndSpot *m_pWndSpot = nullptr;
//	std::shared_ptr<XSpot> m_spBaseSpot;		
	XSpot *m_spBaseSpot = nullptr;			// 월드스팟리스트가 shared_ptr로 바뀌면 그때 이것도 바꿀것.
	XArrayLinearN<XWndButton*, 8> m_aryMenus;		// 버튼 리스트.
	std::vector<XWndImage*> m_arrResourceImg;
	std::vector<XWndTextString*> m_arrResourceText;
	std::vector<XGAME::xDropItem> m_aryItems;	// 드랍가능 아이템목록.
	ID m_idClose = 0;			// 이 팝업이 닫힐때 같이 닫아줘야 할 윈도우가 있으면 지정
//	ID m_idItem = 0;			// 아이템을 보여줘야 한다면.
	ID m_idSpot = 0;
	int m_nParam[4];
	int m_Power = 0;			// 전투력
	int m_Level = 0;			// 지금 싸우려는 군단의 군단레벨
	_tstring m_strName;			// 상대 이름.
	bool m_bViewLog = false;		// 유황약탈기록 보는 모드.
	void Init() {
		XCLEAR_ARRAY( m_nParam );
	}
	void Destroy();
	XWndButton* AddMenu(xtMenu typeMenu);
public:
	XWndPopupSpotMenu( XSpot *spBaseSpot, int p1=0, int p2=0, int p3=0, int p4=0 );
	virtual ~XWndPopupSpotMenu() { Destroy(); }
	//
//	SET_ACCESSOR(XWndSpot*, pWndSpot);
//	GET_SET_ACCESSOR(ID, idItem);
	GET_SET_ACCESSOR( ID, idClose );
 	GET_SET_ACCESSOR( int, Power );
	GET_SET_ACCESSOR( int, Level );
	GET_SET_ACCESSOR( const _tstring&, strName );
	GET_ACCESSOR( std::vector<XGAME::xDropItem>&, aryItems );
	//
	XWndButton* AddMenuAttack() {
		return AddMenu(xCM_ATTACK);
	}
	XWndButton* AddMenuRecon() {
		return AddMenu(xCM_RECON);
	}
	XWndButton* AddMenuCollect() {
		return AddMenu(xCM_COLLECT);
	}
	XWndButton* AddMenuChangePlayerByCash() {
		return AddMenu(xCM_CHANGE_PLAYER_CASH);
	}
	XWndButton* AddMenuChangePlayerByGold() {
		return AddMenu(xCM_CHANGE_PLAYER_GOLD);
	}
	XWndButton* AddMenuRegen() {
		return AddMenu(xCM_REGEN);
	}
	XWndButton* AddMenuWithdraw() {
		return AddMenu(xCM_MANDRAKE_WITHDRAW);
	}
	XWndButton* AddMenuLogSulfur() {
		return AddMenu( xCM_LOG_SULFUR );
	}
//#ifdef _CHEAT
	XWndButton* AddMenuKill() {
		return AddMenu(xCM_KILL);
	}
//#endif // _CHEAT
//	void AddCollectResource(XGAME::xtResource resource);
	void Update() override;
	void AddItems( ID idItem, int num );
	void AddItems( LPCTSTR szIds, int num );
	void AddItems( XPropItem::xPROP* pProp, int num );
	void SetItems( const std::vector<XGAME::xDropItem>& aryItems ) {
		m_aryItems = aryItems;
	}
	bool IsHaveItems( ID idItem );
	int OnClickAttack( XWnd* pWnd, DWORD p1, DWORD p2 );
private:
	void OnNCModal() {
		SetbDestroy( true );
	}
	int OnLogSulfur( XWnd* pWnd, DWORD p1, DWORD p2 );
	void UpdateForCommon();
	void UpdateReward( bool bShow );
	void UpdateSpotImg();
	void UpdateForCastle();
	void UpdateForSulfur();
	void UpdateForMandrake();
	void UpdateForNpc();
	void UpdateDropReward();
	void UpdatePower( XWnd *pRoot );
	void UpdateLevel( bool bShow );
	void UpdateScore( XWnd *pRoot );
	void UpdateAP( XWnd *pRoot, bool bShow );
	void UpdateNumLose( XWnd *pRoot, bool bShow );
	void UpdateRemainRegenTime( XWnd *pRoot, bool bShow );
	void UpdateSpotName( bool bShow );
	void UpdateButtons();
	void OnAutoUpdate() override;
#ifdef _CHEAT
	void UpdateDebugInfo();
#endif // _CHEAT
	void UpdateForJewel();
	void UpdateJewelDefenseCtrl( XSpotJewel* pSpot, XWnd* pRoot, bool bMy );
};


#pragma warning ( default : 4250 )
