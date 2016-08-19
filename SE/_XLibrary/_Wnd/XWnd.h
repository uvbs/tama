#pragma once
#include "XGraphics.h"
#include "InputMng.h"
#include "Timer.h"
#include "XSoundMng.h"
#include "XTextTable.h"
#include "XFont.h"
#include "XFontMng.h"
#include "SprObj.h"
#include "XLua.h"
#pragma warning( disable : 4250 )	//  warning C4250: 'XWndButtonHexNumber' : 우위에 따라 'XWndButton::XWndButton::Process'을(를) 상속합니다.

#define XWM_STOP_ROLLING			1
#define XWM_DESTROY				2
#define XWM_OK						3
#define XWM_SLIDING					4		// 슬라이더 컨트롤을 드래그할때 발생
#define XWM_CLICKED					5		// 버튼 클릭
#define XWM_INIT_VIEW				6


struct XWND_RES_FRAME
{
	int			nBlockSize;
	TCHAR	szFrame[64];
	XSurface*		psfcFrame[9];
	XWND_RES_FRAME() {
		nBlockSize = 0;
		XCLEAR_ARRAY( szFrame );
		XCLEAR_ARRAY( psfcFrame );
	}
	~XWND_RES_FRAME() {
		for( int i = 0; i < 9; i ++ )
			SAFE_DELETE( psfcFrame[i] );
	}
};

//{
class XWndMng;
class XWndButton;
struct XWND_RES_FRAME;
// px,py가 x,y-w,h 영역안에 있는지 검사.
inline BOOL		IsAreaIn( int px, int py, int x, int y, int w, int h )
{
	return XE::IsArea2( x, y, w, h, px, py );
//	if( px > x && px < x+w )
//		if( py > y && py < y+h )
//			return TRUE;
//	return FALSE;
}
class XWnd;
class XBaseScene;
class XToolTip;
struct XWND_MESSAGE_MAP
{
	DWORD msg;
	XBaseScene *pOwner;
	int (XBaseScene::*pHandler)( XWnd* );
};	
////////////////////////////////////////////////////////////////////////////////////
class XWnd
{
public:
	static XWnd *s_pFocusWindow;
	void SetFocus( XWnd *pWindow ) { s_pFocusWindow = pWindow; }
	XWnd* GetFocus( void ) { return s_pFocusWindow; }
	static XBaseFontDat *s_pFontDat;
private:
	void Init( void ) {
		m_id = 0;
		m_vScale.x = m_vScale.y = 1.0f;
		m_dRotateZ = 0;
		m_fAlpha = 1.0f;
//		SetScaleLocal( 1.0f );
		m_pParent = NULL;
		m_pWndMng = NULL;
		m_pFirstPush = NULL;
		m_bShow = TRUE;
		m_bEnable = TRUE;
		m_bActive = TRUE;
		m_pModalDlg = NULL;
		m_bDestroy = FALSE;
		m_prefToolTip = NULL;
		m_bToolTip = FALSE;
	}
	void Destroy( void );
	// protected로 만들지 말것 반드시 GetScaleLocal/Final을 구분해서 쓸것
	XE::VEC2 m_vPos;		// local
	XE::VEC2 m_vScale, _m_vstackScale;	// local
	float m_dRotateZ;
	BOOL m_bDestroy;		// self destroy
	float m_fAlpha;		
	XToolTip *m_prefToolTip;
	BOOL m_bToolTip;
	CTimer m_timerPush;		// 누른시간

	GET_SET_ACCESSOR( XWnd*, pFirstPush );
protected:
	list<XWND_MESSAGE_MAP> m_listMessageMap;
	XE::VEC2 m_vSize;		// local fuck 잠시 프로텍트로 옮겨옴
	ID	m_id;
	XWnd *m_pParent;
	list<XWnd*> m_listItems;
	XWndMng *m_pWndMng;
	XWnd *m_pFirstPush;		// 최초클릭됐을때 컨트롤의 포인터
	BOOL m_bShow;			// show/hide
	BOOL m_bEnable;
	BOOL m_bActive;			// 마우스클릭등의 작동을 멈추게 한다
	XWnd* m_pModalDlg;		// 모달다이얼로그가 떠있는지
public:
	XWnd() { Init(); }
	XWnd( XWndMng *pWndMng ) { Init(); m_pWndMng = pWndMng; SetScaleLocal( 1.0f ); }
	template<typename T>
	XWnd( XWndMng *pWndMng, T x, T y, T w, T h ) {
		Init();
		m_pWndMng = pWndMng;
		m_vPos.Set( x, y ); 
		m_vSize.Set( w, h );
		SetScaleLocal( 1.0f );
	}
	template<typename T>
	XWnd( XWndMng *pWndMng, T x, T y ) {
		Init();
		m_pWndMng = pWndMng;
		m_vPos.Set( x, y );
		SetScaleLocal( 1.0f );
	}
/*	template<typename T>
	XWnd( T x, T y, T w, T h ) {
		Init();
		m_vPos.Set( x, y );
		m_vSize.Set( w, h );
		SetScaleLocal( 1.0f );
	}
	template<typename T>
	XWnd( T x, T y ) {
		Init();
		m_vPos.Set( x, y );
		SetScaleLocal( 1.0f );
	} */
	virtual ~XWnd() { Destroy(); }


	GET_SET_ACCESSOR( XWndMng*, pWndMng );
	GET_ACCESSOR( XWnd*, pParent );
	ID		GetID( void ) { return m_id; }
	void	SetID( ID id ) { m_id = id; }
	GET_SET_ACCESSOR( BOOL, bShow );
	GET_SET_ACCESSOR( BOOL, bEnable );
	GET_SET_ACCESSOR( BOOL, bDestroy );
	GET_SET_ACCESSOR( BOOL, bActive );
	XE::VEC2 GetAdjust( void );
	GET_ACCESSOR( XWnd*, pModalDlg );
	GET_SET_ACCESSOR( XToolTip*, prefToolTip );
	GET_SET_ACCESSOR( BOOL, bToolTip );
	GET_ACCESSOR( CTimer&, timerPush );
	//
	void SetModalDlg( XWnd* pModalDlg ) {
		SAFE_DELETE( m_pModalDlg );	// 먼저있던건 삭제
		m_pModalDlg = pModalDlg;
	}
	void DestroyModalDlg( void ) { SetModalDlg( NULL ); }
	// 접미사 규칙: Final(m_pWndMng로부터 상속누적된 최종값. Scale이나 Rotate도 모두 반영된 트랜스폼된 값이다), Local(상속되지 않은 this의 지역값)
	// transform x, y
	const XE::VEC2& GetPosLocal( void ) { return m_vPos; }
	void SetPosLocal( const XE::VEC2& vPos ) { m_vPos = vPos; }
	template<typename T>
	void SetPosLocal( T x, T y ) { m_vPos.Set( x, y ); }
	XE::VEC2 GetPosFinal( void ) { 
		if( m_pParent ) {
			return m_pParent->GetPosFinal() + GetPosLocal() * m_pParent->GetScaleLocal(); // child일때만 좌표까지 스케일해주고. 부모의 스케일과 내 스케일을 곱해준걸 써야한다 
		} else
			return m_vPos;	
	}	// 값을 곱하는게 있어서 레퍼런스로 못넘기는데 만약 이게 속도문제가 된다면 WndMng.CalcTransform()정도를 만들어서 Final을 한번에 계산해놓고 쓰기만 해야할듯
	// transform width, height
	float GetWidthLocal( void ) { return m_vSize.x * GetScaleLocal().x; }
	float GetHeightLocal( void ) { return m_vSize.y * GetScaleLocal().y; }
	XE::VEC2 GetSizeLocal( void ) { return m_vSize * GetScaleLocal(); }
	XE::VEC2 GetSizeFinal( void ) { return m_vSize * GetScaleFinal(); }
	void PushScale( void ) { _m_vstackScale = m_vScale; }
	void PopScale( void ) { m_vScale = _m_vstackScale; }
//	const XE::VEC2& GetSizeOrig( void ) { return m_vSize; }		// 트랜스폼하지 않은 원래 크기...로 쓰려고 했으나 부모로부터 스케일링을 상속받고 하는등 복잡하기때문에 이 값을 직접 쓸일은 없을듯하다
	template<typename T>
	void SetSizeLocal( T w, T h ) { m_vSize.Set( w, h ); }
	void SetSizeLocal( const XE::VEC2& vSize ) { m_vSize.x = vSize.x; m_vSize.y = vSize.y; }
	// transform scale
	const XE::VEC2& GetScaleLocal( void ) { return m_vScale; }
	XE::VEC2 GetScaleFinal( void ) { return (m_pParent)? m_pParent->GetScaleFinal() * m_vScale : m_vScale; 	}	// 상속된 최종 스케일값 parent.scale * localScale
	void SetScaleLocal( const XE::VEC2& vScale ) { SetScaleLocal( vScale.x, vScale.y );	}
	void SetScaleLocal( float scalexy ) { SetScaleLocal( scalexy, scalexy ); }
	void SetScaleLocal( float sx, float sy );
	// transform alpha
	float GetAlphaLocal( void ) { return m_fAlpha; }	// 로컬 알파값(원본알파값)
	void SetAlphaLocal( float alpha ) { m_fAlpha = alpha; }	// 로컬 알파값(원본알파값)
	float GetAlphaFinal( void ) {	// 최종 화면에 그려질 알파값
		if( m_pParent ) {
			return m_pParent->GetAlphaFinal() * GetAlphaLocal();	// 부모의 알파외 local알파를 곱한다
		} else
			return m_fAlpha;	
	}
	void SetRotateLocal( float rotZ );
	float GetRotateFinal( void ) { return (m_pParent)? m_pParent->GetRotateFinal() + m_dRotateZ : m_dRotateZ; }
	// virtual
//	virtual void OnInitialUpdate( void ) = 0;
	virtual void Disappear( void ) {}
	virtual BOOL IsDisappear( void ) { return TRUE; }
	virtual BOOL IsDisappearing( void ) { return FALSE; }
	virtual void SendKeyMsg( TCHAR keyCode ) {}
	XWnd* Add( XWnd *pChild ) { return Add( XE::GenerateID(), pChild ); }
	XWnd* Add( int id, XWnd *pChild );
	void DestroyID( int id )
	{
		if( id == 0 )
			XLOG( "id == 0" );
		LIST_LOOP( m_listItems, XWnd*, itor, pWnd )
		{
			pWnd->DestroyID( id );	// 자식의 자식도 검사해봐야함
			if( pWnd->GetID() == id )
			{
				XBREAKF( id == 0, "id가 0인 윈도우가 있다" );
				delete pWnd;
				m_listItems.erase( itor );
				return;
			}
		} END_LOOP;
//		if( id )
//			XBREAKF( 1, "id==%d(0x%08x) 윈도우를 찾지 못했습니다", id );
	}
	int CallEventHandler( ID msg );
	virtual int Process( float dt );
	virtual void Draw( void ) {
		if( GetbShow() == FALSE )	return;	// hide상태에선 본인과 자식들모두 감춤
		LIST_LOOP( m_listItems, XWnd*, itor, pWnd )
		{
			if( pWnd->GetbShow() )
				pWnd->Draw();
		} END_LOOP;
		if( m_pModalDlg ) {		// 모달다이얼로그는 최상위에 찍힌다
			m_pModalDlg->Draw();
		}
	}
	void DrawFrame( const XWND_RES_FRAME& frame );
	XWnd* Find( ID id ) {
		XBREAK( id == 0 );
		LIST_LOOP( m_listItems, XWnd*, itor, pWnd )
		{
			if( pWnd->GetID() == id )
				return pWnd;
			XWnd *pChild;
			if( pChild = pWnd->Find( id ) )	// child에서도 검사
				return pChild;
		} END_LOOP;
		return NULL;
	}
	// lx, ly좌표가 this Wnd영역의내부에 있는가 // lx, ly: this를기준으로한 로컬 마우스좌표
	virtual BOOL IsWndAreaIn( float lx, float ly ) 
	{
		int w = (int)GetSizeFinal().x;
		int h = (int)GetSizeFinal().y;
//		if( IsAreaIn( (int)lx, (int)ly, 0, 0, (int)w, (int)h ) )		// 로컬좌표로 계산함
		if( XE::IsArea2( 0, 0, w, h, lx, ly ) )
			return TRUE;
		return FALSE;
	}
	BOOL IsWndAreaIn( const XE::VEC2& vLocal ) { return IsWndAreaIn( vLocal.x, vLocal.y ); }
	// event handler
//	void SetEvent( DWORD msg, XBaseScene *pOwner, void (XBaseScene::*pFunc)( XWnd* ) ) {
	template<typename T>
	void SetEvent( DWORD msg, XBaseScene *pOwner, T func ) {
		typedef int (XBaseScene::*CALLBACK_FUNC)( XWnd * );
		XWND_MESSAGE_MAP msgMap = { msg, pOwner, static_cast<CALLBACK_FUNC>( func ) };
		m_listMessageMap.push_back( msgMap );
	}
	XWND_MESSAGE_MAP FindMsgMap( DWORD msg ) {
		LIST_LOOP( m_listMessageMap, XWND_MESSAGE_MAP, itor, msgMap )
		{
			if( msgMap.msg == msg )
				return msgMap;
		} END_LOOP;
		XWND_MESSAGE_MAP msgMap = { 0, NULL, NULL };
		return msgMap;
	}
	// handler
	virtual XWnd* OnLButtonDown( float lx, float ly );
	virtual XWnd* OnMouseMove( float lx, float ly );
	virtual ID OnLButtonUp( float lx, float ly );
	virtual void OnNCMouseMove( float lx, float ly ) {}		// 윈도우 영역밖에서 움직일때 호출되는 핸들러
	virtual void OnNCLButtonUp( float lx, float ly ) {}
	virtual void OnPlaySound( ID idSnd ) {}
	virtual XToolTip* CreateToolTip( void ) { return NULL; }		// 툴팁을 실시간으로 생성시키고 싶다면 이것을 재정의 할것.
};

#include "XToolTip.h"
#include "XFontSpr.h"

//-------------------------------------------------------------------------------------
class XWndView : public XWnd
{
	XLua *m_prefLua;
	char m_cDrawHandler[ 32 ];	// lua draw handler
	float m_dt;
	BOOL m_bDrawFrame;
	CTimer m_timerAppear;	// 나타날때 타이머
	void Init() {
		m_prefLua = NULL;
		XCLEAR_ARRAY( m_cDrawHandler );
		strcpy_s( m_cDrawHandler, "OnDrawView" );		// default handler name
		m_dt = 1.0f;
		m_bDrawFrame = TRUE;
//		m_timerAppear.SetRealTimer( TRUE );
		m_nStateAppear = 0;
		m_nBlockSize = 0;
	}
	void Destroy() {}
protected:
	int m_nBlockSize;
	XWND_RES_FRAME m_frameImage;
	int m_nStateAppear;		// 나타날때 처리중 다용도 상태
public:
	XWndView() { Init(); }
	XWndView( XWndMng *pWndMng, float x, float y, float w, float h, const char *cDrawHandler, XLua *pLua )
		: XWnd( pWndMng, x, y, w, h ) {
		Init(); 
		if( cDrawHandler )
			strcpy_s( m_cDrawHandler, cDrawHandler );
		m_prefLua = pLua;
	}
	XWndView( XWndMng *pWndMng, float x, float y, float w, float h )
		: XWnd( pWndMng, x, y, w, h ) {
		Init(); 
	}
	XWndView( XWndMng *pWndMng )
		: XWnd( pWndMng ) {
		Init(); 
	}
	virtual ~XWndView() { Destroy(); }
	GET_SET_ACCESSOR( BOOL, bDrawFrame );
	//
    BOOL LoadRes( LPCTSTR szRes );

	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnFinishAppear( void ) {}
};

//}
class XWndDialog : public XWndView
{
public:
	// 예약된 다이얼로그 리턴값
	enum xtReturn { xNONE_RETURN=0, 
						xYES=0x7fffff01, // 루아에는 unsigned형이 없는듯? 그래서 -가 되지 않도록 함
						xNO=0x7fffff02, 
						xOK=0x7fffff03, 
						xCANCEL=0xffffff04 };
private:
//	int m_nBlockSize;
//	CTimer m_timerAppear;	// 나타날때 타이머
//	int m_nStateAppear;		// 나타날때 처리중 다용도 상태
//	XWND_RES_FRAME m_frameImage;
	void Init() {
//		m_nBlockSize = 0;
//		m_nStateAppear = 0;
		m_pFontDat = NULL;
		m_pFont = NULL;
//		m_timerAppear.SetRealTimer( TRUE );
	}
	void Destroy() {
		SAFE_RELEASE2( FONTMNG, m_pFontDat );
		SAFE_DELETE( m_pFont );
	}
protected:
	XBaseFontDat *m_pFontDat;
	XBaseFontObj *m_pFont;
public:
	XWndDialog() {}
	template<typename T>
	XWndDialog( XWndMng *pWndMng, T x, T y, T width, T height, LPCTSTR szRes, LPCTSTR szFont ) 
		: XWndView( pWndMng, x, y, width, height ) { 
			Init(); 
			LoadRes( szRes );
			m_pFontDat = FONTMNG->Load( szFont, 20.f );
			m_pFont = m_pFontDat->CreateFontObj();
			SetDialogSize( width, height );
	}
	template<typename T>
	XWndDialog( XWndMng *pWndMng, T width, T height, LPCTSTR szRes, LPCTSTR szFont ) 
		: XWndView( pWndMng ) { 
			Init(); 
			LoadRes( szRes );
			m_pFontDat = FONTMNG->Load( szFont, 20.f );
			m_pFont = m_pFontDat->CreateFontObj();
			SetDialogSize( width, height );

	}
	virtual ~XWndDialog() { Destroy(); }
	// get/set
	template<typename T>
	void SetDialogSize( T w, T h ) {
		float nw=(float)w, nh=(float)h;
		if( m_nBlockSize > 0 )
		{
			nw = (float)(m_nBlockSize * ((int)w / m_nBlockSize));
			nh = (float)(m_nBlockSize * ((int)h / m_nBlockSize));	// 블럭사이즈에 떨어지도록 크기를 조정한다
		}
		SetSizeLocal( nw, nh );
		SetPosLocal( XSCREEN_WIDTH/2-(int)nw/2, XSCREEN_HEIGHT/2-(int)nh/2 );
	}
	//
//    BOOL LoadRes( LPCTSTR szRes );
	XWndButton* AddButton( ID id, float x, float y, LPCTSTR szText, LPCTSTR szSpr, ID idActUp, ID idActDown=0 );
	XWndButton* AddButton( ID id, const XE::VEC2& vPos, const XE::VEC2& size, LPCTSTR szText );
	XWndButton* AddButton( ID id, float x, float y, LPCTSTR szSpr, ID idActUp, ID idActDown  );
	XWnd* AddStaticText( ID id, float x, float y, float w, float h, LPCTSTR szText, XBaseFontDat *pFontDat, xAlign align, XCOLOR col=XCOLOR_WHITE, xFONT::xtStyle style=xFONT::xSTYLE_NORMAL );
	XWnd* AddStaticText( float x, float y, LPCTSTR szText, XBaseFontDat *pFontDat, xAlign align, XCOLOR col = XCOLOR_WHITE, xFONT::xtStyle style=xFONT::xSTYLE_NORMAL ) {
		return AddStaticText( XE::GenerateID(), x, y, 0, 0, szText, pFontDat, align, col, style );
	}
	//
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual ID OnLButtonUp( float lx, float ly );
	virtual void OnOk( void ) {}
	// lua
	// 삭제
//	XWndButton* LuaAddButton( ID id, float x, float y, ID idText, ID idActUp, ID idActDown ) { 
//		if( XWnd::s_pFont == NULL )
//			XWnd::s_pFont = FONTMNG->Load( _T("NanumGothicBold.ttf"), 20.f );
//		return AddButton( id, x, y, XTEXT( idText )/*XWnd::s_pFont*/, idActUp, idActDown ); 
//	}
	int LuaDoModal( lua_State *L );

};	// XWndDialog
//-------------------------------------------------------------------------------------
class XWndImage : public XWnd
{
	XSurface *m_pSurface;
	XE::VEC2 m_vPos;
	void Init() {
		m_pSurface = NULL;
	}
	void Destroy() {
		SAFE_DELETE( m_pSurface );
	}
public:
	template<typename T>
	XWndImage( XWndMng *pWndMng, BOOL bHighReso, LPCTSTR szImg, T x, T y ) 
		: XWnd( pWndMng, x, y )
	{
		Init();
		m_vPos.Set( x, y );
		Create( bHighReso, szImg );
	}
	virtual ~XWndImage() { Destroy(); }
	void Create( BOOL bHighReso, LPCTSTR szImg );
	virtual void Draw( void );
};
//-------------------------------------------------------------------------------------
class XWndSprObj : public XWnd
{
	XSprObj *m_pSprObj;
//	XE::VEC2 m_vPos;
	void Init() {
		m_pSprObj = NULL;
	}
	void Destroy() {
		SAFE_DELETE( m_pSprObj );
	}
public:
	template<typename T>
	XWndSprObj( XWndMng *pWndMng, LPCTSTR szSpr, ID idAct, T x, T y )		// 이거쓰지말고 밑에거 쓸것.
		: XWnd( pWndMng, x, y )
	{
		Init();
//		m_vPos.Set( x, y );
		SetPosLocal( x, y );
		m_pSprObj = new XSprObj( szSpr );
		m_pSprObj->SetAction( idAct );

	}
	template<typename T>
	XWndSprObj( LPCTSTR szSpr, ID idAct, T x, T y ) 
		: XWnd( NULL, x, y )
	{
		Init();
		SetPosLocal( x, y );
		m_pSprObj = new XSprObj( szSpr );
		m_pSprObj->SetAction( idAct );

	}
	virtual ~XWndSprObj() { Destroy(); }
	virtual void Draw( void ) {
		m_pSprObj->Draw( GetPosFinal() );
	}
};


#pragma warning ( default : 4250 )

//////////////////////////////////////////////////////////////////
// 등장 이펙트

// 점점 커지거나 작아지는 이펙트
class XWndEffectGradually : virtual public XWnd
{
	BOOL m_bToBig;		// 1:커지기모드 0:작아지기모드
//	XE::VEC2 m_vPos;		// 이펙트중 좌표
	XE::VEC2 m_vPosStart, m_vPosEnd;	// 출발좌표 목표좌표
	CTimer m_Timer;			// 이펙트 보간 타이머
	float m_fSec;
	int m_State;				// 0:아직시작전 1:플레이중 2:끝나서멈춰있음
	void Init() {
		m_fSec = 0;
		m_State = 0;
		m_bToBig = 1;
	}
public:
	XWndEffectGradually() { Init(); }
	virtual ~XWndEffectGradually() {}
	//
	GET_ACCESSOR( float, fSec );
	//
	BOOL IsPlaying( void ) { return (m_State==1)? TRUE : FALSE; }	// 이펙트 중인가
	BOOL IsFinish( void ) { return (m_State==2)? TRUE : FALSE; }	// 이펙트가 끝났나
	BOOL IsReady( void ) { return (m_State==0)? TRUE : FALSE; }	// 아직 이펙트 시작전임
	void SetReady( void ) { m_State = 0; }		// 시작전 대기상태로 만든다
	void SetRestart( void ) {
		SetStart( m_bToBig, m_vPosStart, m_vPosEnd, m_fSec );
	}
	void SetStart( BOOL bToBig, float sec ) {
		SetStart( bToBig, GetPosLocal(), GetPosLocal(), sec );
	}
	virtual void SetStart( BOOL bToBig, const XE::VEC2& vStart, const XE::VEC2& vEnd, float sec ) {	// bOn:0작아기지 1커지기
		m_Timer.Set( sec );
		m_fSec = sec;
		m_vPosStart = vStart;
		m_vPosEnd = vEnd;
		m_bToBig = bToBig;
		m_State = 1;		// 커지기 시작함
		SetPosLocal( vStart );
		if( bToBig ) {
			SetAlphaLocal( 0 );
			SetScaleLocal( 0 );			// 첨엔 작아져있다
		}
		else {
			SetAlphaLocal( 1.0f );
			SetScaleLocal( 1.0f );
		}
	}
	virtual int Process( float dt );
};

//////////////////////////////////////////////////////////////////
// 슬라이더 컨트롤
class XWndSlider : public XWnd
{
	int m_Push;
	void Init() {
		m_Min = m_Max = 0;
		m_Curr = 0;
		m_Push = FALSE;
	}
	void Destroy() {}
protected:
	float m_Min, m_Max;		// 슬라이더 최소 최대값
	float m_Curr;
public:
	template<typename T>
	XWndSlider( T x, T y, T w, T h, float min, float max, float curr=0 ) 
		: XWnd( NULL, x, y, w, h ) { 
			Init(); 
			m_Min = min;
			m_Max = max;
			if( curr == 0 )
				m_Curr = min;
			else
				m_Curr = curr;
	}
	virtual ~XWndSlider() { Destroy(); }
	GET_ACCESSOR( float, Curr );

	virtual XWnd* OnLButtonDown( float lx, float ly );
	virtual void OnNCMouseMove( float lx, float ly );
	virtual void OnNCLButtonUp( float lx, float ly );
	virtual void Draw( void ) {}
};
/////////////////////////////////////////////////////////////////////////////
class XWndPopup : public XWndDialog
{
public:
	enum xType { xTYPE_OK, xTYPE_OKCANCEL, xTYPE_YESNO };
private:
	float m_yAuto;
	void Init() {
		m_yAuto = 5;
	}
	void Destroy() {	}
public:
	XWndPopup() : XWndDialog( NULL, 32, 32, NULL, NULL ) { Init(); }		// for lua
//	XWndPopup( XWndMng *pWndMng ) 
//		: XWndDialog( pWndMng, 32, 32, NULL ) {
//	}
	// only ok
	XWndPopup( LPCTSTR szMessage, xType type, LPCTSTR szFont, LPCTSTR szButtSpr, ID idOkUp, ID idOkDown ) 
		: XWndDialog( NULL, 0, 0, _T("ui_profile_box.png"), szFont ) {
		SetText( szMessage );
		XE::VEC2 vSize = m_pFont->GetLayoutSize( szMessage );		// 텍스트가 차지하는 최소의 영역크기
		vSize.w *= 1.5f;		// 텍스트 너비의 1.5배로 다이얼로그 크기로 하고
		vSize.h += 50.f;		// 세로는 50포인트 더 크게 한다
		XWndDialog::SetDialogSize( vSize.w, vSize.h );		// 다이얼로그 크기 정함
		XE::VEC2 vPos;	// 버튼이 위치할좌표. 버튼은 중앙정렬 시켜놓아야 한다
		vPos.x = vSize.w / 2.f;
		vPos.y = vSize.h * 0.75f;		// 세로크기의 3/4 위치에 배치 
		AddButton( XWndDialog::xOK, vPos.x, vPos.y, szButtSpr, idOkUp, idOkDown );
	}
	// ok / cancel
	XWndPopup( LPCTSTR szMessage, xType type, LPCTSTR szFont, LPCTSTR szButtSpr, ID idOkUp, ID idOkDown, ID idCancelUp, ID idCancelDown ) 
		: XWndDialog( NULL, 0, 0, _T("ui_profile_box.png"), szFont ) {
		SetText( szMessage );
		XE::VEC2 vSize = m_pFont->GetLayoutSize( szMessage );		// 텍스트가 차지하는 최소의 영역크기
		vSize.w *= 1.5f;		// 텍스트 너비의 1.5배로 다이얼로그 크기로 하고
		vSize.h += 50.f;		// 세로는 50포인트 더 크게 한다
		XWndDialog::SetDialogSize( vSize.w, vSize.h );		// 다이얼로그 크기 정함
		XE::VEC2 vPos;	// 버튼이 위치할좌표. 버튼은 중앙정렬 시켜놓아야 한다
		vPos.x = vSize.w * 0.33f;		// 1/3위치에 ok버튼 배치
		vPos.y = 20.f;		// 세로크기의 3/4 위치에 배치 
		AddButton( XWndDialog::xOK, vPos.x, vPos.y, szButtSpr, idOkUp, idOkDown );
		vPos.x = vSize.w * 0.66f;		// 2/3위치에 ok버튼 배치
		vPos.y = 20.f;		// 세로크기의 3/4 위치에 배치 
		AddButton( XWndDialog::xCANCEL, vPos.x, vPos.y, szButtSpr, idCancelUp, idCancelDown );
	}
	virtual ~XWndPopup() { Destroy(); }

//	XWndButton* AddMenu( ID id, LPCTSTR szText, int idActUp, int idActDown=0 );
//	XWndButton* AddMenu( ID id, int idActUp, int idActDown=0 );
	XWnd* SetText( LPCTSTR szText ) {		// 팝업메뉴에 "xxx하시겠습니까?" 뭐 이런 텍스트
//		_tcscpy_s( m_szText, szText );
		// 팝업창의 0,0, width, height*0.5크기만큼 영역에 텍스트를 배치
		return AddStaticText( XE::GenerateID(), 0, 0, GetSizeFinal().x, GetSizeFinal().y * 0.5f, szText, m_pFontDat, xALIGN_CENTER );
	}
	// 팝업을 Yes No 메시지창 형태로 만든다
//	void SetYesNo( LPCTSTR szText );
	// 팝업을 Yes No 메시지창 형태로 만든다
//	void SetOkCancel( LPCTSTR szText );
	//
//	void SetOk( LPCTSTR szText );
	// lua
	XWnd* LuaSetText( ID idText ) { return SetText( XTEXT( idText ) ); } 
	void LuaSetYesNo( ID idText ) { /*SetYesNo( XTEXT( idText ) );*/ }
//	XWndButton* LuaAddMenuTextID( ID id, ID idText, int idActUp, int idActDown=0 ) { return AddMenu( id, XTEXT( idText ), idActUp, idActDown ); }
//	XWndButton* LuaAddMenu( ID id, int idActUp, int idActDown=0 ) { return AddMenu( id, NULL, idActUp, idActDown ); }
	// virtual
}; // class XWndPopup

// iOS방식의 노티피케이션 알림창
class XWndTextString;
class XWndNotification : public XWndView
{
//	TCHAR m_szTitle[ 256 ];
//	TCHAR m_szMsg[ 1024 ];
	CTimer m_Timer;
//	XWndTextString *m_pTitle;
//	XWndTextString *m_pMsg;
	void Init() {
//		m_szTitle[0] = 0;
//		m_szMsg[0] = 0;
//		m_pTitle = m_pMsg = NULL;
	}
	void Destroy();
	template<typename T>
	void SetSize( T w, T h ) {
		float nw=(float)w, nh=(float)h;
		if( m_nBlockSize > 0 )
		{
			nw = (float)(m_nBlockSize * ((int)w / m_nBlockSize));
			nh = (float)(m_nBlockSize * ((int)h / m_nBlockSize));	// 블럭사이즈에 떨어지도록 크기를 조정한다
		}
		SetSizeLocal( nw, nh );
		SetPosLocal( XSCREEN_WIDTH/2-(int)nw/2, 5 );
	}
public:
	XWndNotification() { Init(); }
	XWndNotification( float w, float h, LPCTSTR szTitle, LPCTSTR szMsg, XBaseFontDat *pFontDat );
	virtual ~XWndNotification() { Destroy(); }

	virtual int Process( float dt ) {
		if( m_Timer.IsOver() )
			SetbDestroy( TRUE );
		return XWndView::Process( dt );
	}
	virtual void Draw( void );
};