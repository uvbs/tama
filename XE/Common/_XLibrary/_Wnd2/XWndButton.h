#pragma once
#include "XWnd.h"
#include "XFontSpr.h"
#include "XFontMng.h"
//#include "sprite/SprObj.h"
#include "XWndText.h"
//#include "XWndCtrls.h"

#pragma warning( disable : 4250 )	//  warning C4250: 'XWndButtonHexNumber' : 우위에 따라 'XWndButton::XWndButton::Process'을(를) 상속합니다.

// #define xfALIGN_LEFT			0x01
// #define xfALIGN_RIGHT		0x02
// #define xfALIGN_HCENTER		0x04
// #define xfALIGN_VCENTER		0x08
// #define xfALIGN_TOP			0x10
// #define xfALIGN_BOTTOM		0x20
// #define xfALIGN_ALL			0x3F

namespace XE {
	enum xtButtAni {
		xBA_NORMAL,
		xBA_BOUNCE,		///< 눌림 애니메이션이 따로 없고 누르면 살짝 커지면서 튕기는 애니메이션으로 대체한다.
		xBA_MOVE,		///< 대각선아래로 살짝 움직였다 돌아옴
	};
};

//class XWndMng;
class XSprObj;
////////////////////////////////////////////////////////////////////////
class XWndButton : virtual public XWnd
{
public:
//	static XWndButton* Find( XWndMng *pWndMng, ID idWnd );
	// fuck
	static XWND_MESSAGE_MAP s_mmSoundDown;		// 디폴트 메시지. 사운드.
	static XWND_MESSAGE_MAP s_mmCreate;				// 디폴트 메시지. 생성
	static XWND_MESSAGE_MAP s_mmQuestClicked;		// 디폴트 메시지. 버튼이 눌리면 퀘스트 객체로 이벤트 전달.
	template<typename T>
	static void SetDefaultEvent( DWORD msg, XWnd *pOwner, T func, DWORD param1=0, DWORD param2=0 ) {
		typedef int (XWnd::*CALLBACK_FUNC)( XWnd *, DWORD dwParam1, DWORD dwParam2 );
		XWND_MESSAGE_MAP *pMsgMap = NULL;
		switch( msg ) {
		case XWM_SOUND_DOWN:	pMsgMap = &s_mmSoundDown;	break;
		case XWM_CREATE:				pMsgMap = &s_mmCreate;		break;
		case XWM_QUEST_CLICKED:	pMsgMap = &s_mmQuestClicked;	break;
		}
		if( pMsgMap )	{
			pMsgMap->msg = msg;
			pMsgMap->param = param1;
			pMsgMap->pOwner = pOwner;
			pMsgMap->pHandler = static_cast<CALLBACK_FUNC>( func );
		}
	}
	static XE::xtButtAni s_modeAnimationDefault;	///< 애니메이션 모드 디폴트값
private:
	XSprObj *m_pSprObj;
	XSurface *m_pSurface[4];		// 버튼이미지를 surface(png)로 쓰는 버전용
	XE::xtButtAni m_modeAnimaion;			///< 0: 애니메이션 없음. 1:눌릴때 바운스
	CTimer m_timerAni;				///< 애니메이션을 위한 타이머
	int m_stateAni;					///< 애니메이션일때 상태
	CTimer m_timerGlow;
	bool m_bShowGlow = true;	// glow를 표시할건지 말건지. glow를 로딩한것과는관계가 없다. 로딩하고도 false면 출력하지 않는다.
	bool m_bLock = false;			// 잠김상태. 누를수는 있으나 lock플래그가 보내짐.
	XE::VEC2 m_vAdjGlow;			// glow이미지의 보정좌표
	void Init() {
		m_idActDisable = m_idActDown = m_idActUp = 0;
		m_idActEnter = m_idActLeave = 0;
		m_bPush = m_bEdge = m_bFirstPush = FALSE;
//		m_bSprNum = FALSE;
		m_pSprObj = NULL;	
		XCLEAR_ARRAY( m_pSurface );
		m_strDebug = _T("butt");
		m_modeAnimaion = XE::xBA_NORMAL;
		m_stateAni = 0;
		// fuck
		if( s_mmSoundDown.pOwner )
			SetEvent( XWM_SOUND_DOWN, s_mmSoundDown.pOwner, s_mmSoundDown.pHandler );
		if( s_mmCreate.pOwner )
			SetEvent( XWM_CREATE, s_mmCreate.pOwner, s_mmCreate.pHandler );
		if( s_mmQuestClicked.pOwner )
			SetEvent( XWM_QUEST_CLICKED, s_mmQuestClicked.pOwner, s_mmQuestClicked.pHandler );
		m_modeAnimaion = s_modeAnimationDefault;
	}
	void Destroy();
protected:
	BOOL m_bPush, m_bEdge, m_bFirstPush;
	DWORD m_idActUp;		// 안눌려졌을때 액션번호
	DWORD m_idActDown;		// 눌려졌을때 액션번호
	DWORD m_idActDisable;	// 비활성화 됐을때 액션번호
	ID m_idActEnter;		// 등장 애니메이션
	ID m_idActLeave;		// 퇴장 애니메이션
//	DWORD m_xfFlag = 0;			// 다용도 플래그
//	XE::VEC2 m_vAdjPush;		// 이걸사용하면 버튼 껍데기만 움직이고 내용물은 안움직여서 어색하다. 좌표보정으로 눌림애니메이션을 할때 보정좌표.(XWnd::m_vAdjDraw는 버튼 전체의 보정으로 사용되어야 하므로 별도로 만듬)
public:
	XWndButton() { Init(); }
	// 상속받는 객체는 virtual public에 의하여 XWnd()생성자를 구현해줄것
	// 템플릿으로 하려했더니 헤더에 넣어야 하고 그럼  SprObj.h도 인클루드 해야해서 포기
//	XWndButton( float x, float y, LPCTSTR szSpr, DWORD idActUp, DWORD idActDown = 0, DWORD idActDisable = 0, BOOL bSprNum = FALSE, XToolTip *pToolTip=NULL );
	XWndButton( float x, float y ) 
		: XWnd( x, y ) {
			Init();
	}
	XWndButton( const XE::VEC2& vPos ) 
		: XWnd( vPos.x, vPos.y ) {
		Init();
	}
// 	XWndButton( float x, float y, LPCTSTR szSpr, DWORD idActUp, DWORD idActDown = 0, DWORD idActDisable = 0, XEToolTip *pToolTip=NULL, BOOL bSrcKeep = FALSE );
	XWndButton( float x, float y, XSprObj *pSprObj, DWORD idActUp, DWORD idActDown = 0, DWORD idActDisable = 0 );			// KeepSrc된 SprObj가 필요할때 외부에서 생성시킨걸 쓴다
	XWndButton( float x, float y, float w, float h, XEToolTip *pToolTip=NULL );
	XWndButton( float x, float y, LPCTSTR szImgUp, LPCTSTR szImgDown = nullptr, LPCTSTR szImgDisable=NULL, XEToolTip *pToolTip=NULL, DWORD xfFlag=0, BOOL bSrcKeep = FALSE );
	void Create( float x, float y, XSprObj *pSprObj, DWORD idActUp, DWORD idActDown, DWORD idActDisable ) ;
	void Create( XSprObj *pSprObj, DWORD idActUp, DWORD idActDown, DWORD idActDisable );
	virtual ~XWndButton() { Destroy(); }
//	void Update() override;
	inline void SetPush( BOOL bFlag ) { m_bPush = bFlag; }
	inline void SetPush( bool bFlag ) {
		m_bPush = (bFlag)? TRUE : FALSE;
	}
	inline BOOL GetPush() const { return m_bPush; }
	GET_ACCESSOR_CONST( DWORD, idActUp );
 	GET_ACCESSOR_CONST( DWORD, idActDown );
	GET_ACCESSOR_CONST( DWORD, idActDisable );
	GET_ACCESSOR_CONST( XSprObj*, pSprObj );
	GET_SET_BOOL_ACCESSOR( bLock );
//	GET_ACCESSOR( DWORD, xfFlag );
	GET_SET_ACCESSOR_CONST( bool, bShowGlow );
	XSurface* GetpSurface( int idx ){
		XBREAK( idx < 0 || idx > 2 );
		return m_pSurface[idx];
	}
	void SetFlag( DWORD xfFlag, XWnd *pParent=NULL );
	/// 버튼눌림 애니메이션 방식을 바꾼다.
	void SetModeAnimation( XE::xtButtAni modeAni ) {
		m_modeAnimaion = modeAni;
	}

	void CancelPush() { m_bPush = m_bFirstPush = FALSE; } // OnLButtonDown()에서 한 눌림처리를 취소시킨다. 버튼 안눌린걸로 해야하는데 XWndButton::OnLButtonDown()은 불러야 할때 사용
	void DrawNoSpr();
	void LoadSprObj( LPCTSTR szSpr, ID idAct );
	inline void LoadSprObj( const _tstring& strSpr, ID idAct ) {
		LoadSprObj( strSpr.c_str(), idAct );
	}
	void SetSprObj( LPCTSTR szSpr, ID idActUp = 0, ID idActDown = 0 );
	void SetSprButton( int idxButt, LPCTSTR szSpr, ID idAct );
	void SetpSurface( int idx, LPCTSTR szRes );
	void SetGlow( LPCTSTR szPostfix = _T("_glow"), const XE::VEC2& vAdj = XE::VEC2(0), bool bOn = true );
	void DrawFromSurface( const XE::VEC2& vPos );
	// Enter/Leave animation
	void SetEnterAnimationByID( ID idActEnter );
	void SetEnterAnimationByTime( float sec );
	void SetLeaveAnimationByID( ID idActEnter );
	void SetLeaveAnimationByTime( float sec );
	virtual BOOL OnInitEnterLeaveAnimation( BOOL bEnter );
	virtual int OnProcessEnterLeaveAnimation( BOOL bEnter, float dt );
	void OnFinishEnterLeave( BOOL bEnter );
	inline void SetText( const _tstring& strText ) {
		auto pText = dynamic_cast<XWndTextString*>( Find("__text.label") );
		if( pText )
			pText->SetText( strText );
	}
	inline void SetText( LPCTSTR szString ) {
		auto pText = dynamic_cast<XWndTextString*>( Find( "__text.label" ) );
		if( pText )
			pText->SetText( szString );
	}
	/**
	 버튼이 클릭이 되었을때 발생하는 델리게이트
	*/
//	virtual void OnDelegateLButtonUp( float lx, float ly ) {}
//	virtual void OnInitLeaveAnimation();
//	virtual void OnUnitLeaveAnimation();
	// virtual
	virtual int Process( float dt ) override;
	virtual void Draw() override;
	virtual void Draw( const XE::VEC2& vPos ) override;
	virtual BOOL IsWndAreaIn( float lx, float ly ) override;
	// handler
	virtual void OnLButtonDown( float lx, float ly ) override;
	virtual void OnMouseMove( float lx, float ly ) override;
	virtual void OnLButtonUp( float lx, float ly ) override;
	virtual void OnNCMouseMove( float lx, float ly ) override;
	virtual void OnNCLButtonUp( float lx, float ly ) override;
	virtual void OnPlaySound( ID id ) override;
	virtual BOOL OnCreate() override;
	void GetMatrix( MATRIX* pOut );
protected:
	virtual void OnPushDown() {}
	int DispatchEvent();
	virtual bool IsAbleAlign() const override;
private:
	bool DrawButt( const XE::VEC2& vPos, int idxButt, XE::xtBlendFunc blendFunc );
	XE::VEC2 GetvChildLocal( const XE::VEC2& vLocal, XWnd* pParent, XWnd* pGrandParent ) const override;
}; // class XWndButton
////////////////////////////////////////////////////////////////////////
// check button
class XWndButtonCheck : virtual public XWndButton
{
	void Init() {}
	void Destroy() { 	}
public:
	// 상속받는 객체는 virtual public에 의하여 XWnd()생성자를 구현해줄것
	template<typename T>
	XWndButtonCheck( T x, T y, LPCTSTR szSpr, DWORD idActUp, DWORD idActDown = 0, BOOL bPush=FALSE ) 
		: XWnd( x, y ), XWndButton( x, y, szSpr, idActUp, idActDown )  {
		Init();
		m_bPush = bPush;
	}
	virtual ~XWndButtonCheck() { Destroy(); }
	//
	BOOL GetCheck() { return GetPush(); }
	void SetCheck( BOOL bFlag ) { SetPush( bFlag ); }
	//
	virtual void OnMouseMove( float lx, float ly );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnNCMouseMove( float lx, float ly ) {}	// 상위 함수를 처리하지 못하게함
	virtual void OnNCLButtonUp( float lx, float ly ) ;
};

////////////////////////////////////////////////////////////////////////
// 스트링을 버튼위에 그리는 버튼
class XWndButtonString : public XWndButton
{
private:
	XCOLOR m_Color;			// 텍스트 컬러
	XWndTextString *m_pText = nullptr;
	void Init() {	
		m_Color = XCOLOR_WHITE;
	}
	void Destroy() { 	}
protected:
public:
// 	XWndButtonString( float x, float y, LPCTSTR szString, XCOLOR col, 
// 							XBaseFontDat *pFontDat, 
// 							LPCTSTR szSpr, ID idActUp, ID idActDown=0, ID idActDisable=0 );
// 	XWndButtonString( float x, float y, LPCTSTR szString, XCOLOR col, 
// 							LPCTSTR szFont, float sizeFont,
// 							LPCTSTR szSpr, ID idActUp, ID idActDown=0, ID idActDisable=0 );
	XWndButtonString( float x, float y, LPCTSTR szString, XCOLOR col, XBaseFontDat *pFontDat, 
							LPCTSTR szImgUp, LPCTSTR szImgDown=NULL, LPCTSTR szImgDisable=NULL );
	XWndButtonString( float x, float y, LPCTSTR szString, XCOLOR col, 
							LPCTSTR szFont, float sizeFont,
							LPCTSTR szImgUp, LPCTSTR szImgDown=NULL, LPCTSTR szImgDisable=NULL );
	XWndButtonString( float x, float y, LPCTSTR szString, 
										float sizeFont,
										LPCTSTR szImgUp, 
										LPCTSTR szImgDown=NULL, 
										LPCTSTR szImgDisable=NULL, 
										XCOLOR col = XCOLOR_WHITE  ) 
		: XWndButtonString( x, y, szString, col, FONT_NANUM_BOLD, sizeFont, szImgUp, szImgDown, szImgDisable ) 
  {	}
	XWndButtonString( const XE::VEC2& vPos, LPCTSTR szString, XCOLOR col, 
							LPCTSTR szFont, float sizeFont,
							LPCTSTR szImgUp, LPCTSTR szImgDown=NULL, LPCTSTR szImgDisable=NULL ) 
							: XWndButtonString( vPos.x, vPos.y, szString, col, szFont, sizeFont, szImgUp, szImgDown, szImgDisable ) {
	}
	XWndButtonString( const XE::VEC2& vPos, LPCTSTR szString, 
										float sizeFont,
										LPCTSTR szImgUp, 
										LPCTSTR szImgDown=NULL, 
										LPCTSTR szImgDisable=NULL, 
										XCOLOR col = XCOLOR_WHITE  ) 
			: XWndButtonString( vPos.x, vPos.y, szString, sizeFont, szImgUp, szImgDown, szImgDisable ) {

	}
	virtual ~XWndButtonString() { Destroy(); }
	//
	virtual void Draw();
	inline void SetStyle( xFONT::xtStyle style ) {
		if( m_pText )
			m_pText->SetStyle( style );
	}
	inline void SetStyleNormal() {
		if( m_pText )
			m_pText->SetStyle( xFONT::xSTYLE_NORMAL );
	}
	inline void SetStyleShadow() {
		if( m_pText )
			m_pText->SetStyle( xFONT::xSTYLE_SHADOW );
	}
	inline void SetStyleStroke() {
		if( m_pText )
			m_pText->SetStyle( xFONT::xSTYLE_STROKE );
	}
// 	inline void SetAlign( XE::xAlign align ) {
// 		if( m_pText )
// 			m_pText->SetAlign( align );
// 	}
	inline void SetText( const _tstring& strText ) {
		if( m_pText )
			m_pText->SetText( strText );
	}
	inline void SetText( LPCTSTR szString ) {
		if( m_pText )
			m_pText->SetText( szString );
	}
};
////////////////////////////////////////////////////////////////////////
// 리소스가 필요없는 디버깅용 심플한 버튼
class XWndButtonDebug;
class XWndButtonDebug : public XWndButton
{
public:
	static XWndButtonDebug* sUpdateCtrl( XWnd *pRoot, const char* cIds, const XE::VEC2& v, const XE::VEC2& vSize, LPCTSTR szText, bool bShow );
private:
	XBaseFontObj *m_pFontObj;
	TCHAR m_szText[ 64 ];
	bool m_bCheck = false;
	void Init() {
		m_pFontObj = NULL;
		m_szText[0] = 0;
		SetModeAnimation( XE::xBA_NORMAL );		// 디버깅버튼은 항상 노말모드.
	}
	void Destroy() {
		SAFE_DELETE( m_pFontObj );
	}
public:
	XWndButtonDebug( float x, float y, float w, float h, LPCTSTR szText, XBaseFontDat *pFontDat = nullptr )
		: XWndButton( x, y, w, h ), XWnd( x, y, w, h )	{
		Init();
		Create( XE::VEC2(w,h), szText, pFontDat );
	}
	XWndButtonDebug( const XE::VEC2& vPos, const XE::VEC2& vSize, LPCTSTR szText, XBaseFontDat *pFontDat = nullptr )
		: XWndButton( vPos.x, vPos.y, vSize.w, vSize.h ), 
		  XWnd( vPos.x, vPos.y, vSize.w, vSize.h )	{
			Init();
			Create( vSize, szText, pFontDat );
	}
	virtual ~XWndButtonDebug() { Destroy(); }
	//
	GET_SET_ACCESSOR_CONST( bool, bCheck );
	void Create( const XE::VEC2& vSize, LPCTSTR szText, XBaseFontDat *pFontDat = nullptr, XCOLOR col=XCOLOR_WHITE );
	void SetText( LPCTSTR szText ) {
		if( szText )
			_tcscpy_s( m_szText, szText );
		else
			m_szText[0] = 0;
	}
// 	BOOL IsWndAreaIn( float lx, float ly ) override;
// 	int Process( float dt ) override;
	void Draw() override;
};
////////////////////////////////////////////////////////////////////////
// 체크버튼위에 스트링 표시하는 버전
// class XWndButtonCheckString : public XWndButtonCheck, public XWndTextString
// {
// private:
// 	void Init() {	}
// 	void Destroy() { 	}
// protected:
// public:
// 	XWndButtonCheckString( float x, float y, LPCTSTR szString, XBaseFontDat *pFontDat, LPCTSTR szSpr, DWORD idActUp, DWORD idActDown = 0, BOOL bPush=FALSE ) :
// // 		: XWnd( x, y ), 
// 		XWndButton( x, y, szSpr, idActUp, idActDown, 0 ),
// 		XWndButtonCheck( x, y, szSpr, idActUp, idActDown, bPush ), 
// 		XWndTextString( 0, 0, szString, pFontDat, XCOLOR_BLACK ) {
// 		Init();
// 	}
// 	virtual ~XWndButtonCheckString() { Destroy(); }
// 	
// 	virtual void Draw() {
// 		XWndButtonCheck::Draw();
// 		XWndTextString::Draw();
// 	}
// 	virtual void Draw( const XE::VEC2& vPos ) {
// 		XWndTextString::Draw( vPos );
// 	}
// 	virtual BOOL IsWndAreaIn( float lx, float ly ) {
// 		return XWndButtonCheck::IsWndAreaIn( lx, ly );
// 	}
// }; // class XWndButtonCheckString 

//////////////////////////////////////////////////////////////////////////
XWndButton* xGET_BUTT_CTRL( XWnd *pRoot, const char *cKey );
XWndButton* xGET_BUTT_CTRLF( XWnd *pRoot, const char *cKeyFormat, ... );


template<typename T>
XWndButton* xSetButtHander(XWnd *pKeyRoot, XWnd *pOwner, const char *cKey, T funcCallback, DWORD param1 = 0)
{
	typedef int (XWnd::*CALLBACK_FUNC)(XWnd *, DWORD, DWORD);
	if (XBREAK(pKeyRoot == NULL))
		return nullptr;
	auto pButt = pKeyRoot->Find(cKey);
	if (pButt)
		pButt->SetEvent(XWM_CLICKED, pOwner, funcCallback, param1);
/*
#ifdef _VER_IOS
	return (XWndButton*)pButt;
#else*/
	return dynamic_cast<XWndButton*>(pButt);
/*#endif*/
}

template<typename T>
inline XWndButton* xSET_BUTT_HANDLER_NEW( XWnd *pRoot, const char *cKey, XWnd *pCallback, T func, DWORD param1=0 ) {
	typedef int ( XWnd::*CALLBACK_FUNC )( XWnd *, DWORD, DWORD );
	if( XBREAK( pRoot == NULL ) )
		return nullptr;
	auto pButt = pRoot->Find( cKey );
	if( pButt )
		pButt->SetEvent( XWM_CLICKED, pCallback, func, param1 );	
	return dynamic_cast<XWndButton*>( pButt );
};

#define xSET_BUTT_HANDLER( ROOT, KEY, CALL ) \
	xSET_BUTT_HANDLER_NEW( ROOT, KEY, this, CALL )

#define xSET_BUTT_HANDLER_OWNER( ROOT, KEY, OWNER, CALL ) { \
	XWnd *_pButt; \
	XBREAK( ROOT == NULL ); \
	_pButt = ROOT->Find( KEY ); \
	if( _pButt ) \
		_pButt->SetEvent( XWM_CLICKED, OWNER, CALL ); \
	else \
		XLOGXNA( "xSET_BUTT_HANDLER: %s not found", KEY ); \
}

// 이제 이 매크로는 쓰지 말것. xSET_BUTT_HANDLER_PARAM2를 사용할것.
#define __xSET_BUTT_HANDLER_PARAM( ROOT, KEY, CALL, P1, P2 ) { \
	XWnd *_pButt; \
	XBREAK( ROOT == NULL ); \
	_pButt = ROOT->Find( KEY ); \
	if( _pButt ) \
		_pButt->SetEvent( XWM_CLICKED, this, CALL, P1, P2 ); \
	else \
		XLOGXNA( "xSET_BUTT_HANDLER: %s not found", KEY ); \
}

#define xSET_BUTT_HANDLER_PARAM2( ROOT, KEY, OWNER, CALL, P1, P2 ) { \
	XWnd *_pButt; \
	XBREAK( ROOT == NULL ); \
	_pButt = ROOT->Find( KEY ); \
	if( _pButt ) \
		_pButt->SetEvent( XWM_CLICKED, OWNER, CALL, P1, P2 ); \
	else \
		XLOGXNA( "xSET_BUTT_HANDLER: %s not found", KEY ); \
}
#define xSET_BUTT_HANDLER_PARAM( ROOT, KEY, OWNER, CALL, P1 ) { \
	XWnd *_pButt; \
	XBREAK( ROOT == NULL ); \
	_pButt = ROOT->Find( KEY ); \
	if( _pButt ) \
		_pButt->SetEvent( XWM_CLICKED, OWNER, CALL, P1 ); \
		else \
		XLOGXNA( "xSET_BUTT_HANDLER: %s not found", KEY ); \
}


#pragma warning ( default : 4250 )

