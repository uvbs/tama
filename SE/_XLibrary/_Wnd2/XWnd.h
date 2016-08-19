#pragma once
#include "XGraphics.h"
#include "InputMng.h"
#include "Timer.h"
#include "XSoundMng.h"
#include "XFont.h"
#include "XFontMng.h"
#include "XLua.h"
#pragma warning( disable : 4250 )	//  warning C4250: 'XWndButtonHexNumber' : 우위에 따라 'XWndButton::XWndButton::Process'을(를) 상속합니다.

#define XWM_STOP_ROLLING			1
#define XWM_DESTROY				2
#define XWM_OK						3		// 다이얼로그에서 OK눌렀을때
#define XWM_SLIDING					4		// 슬라이더 컨트롤을 드래그할때 발생
#define XWM_CLICKED					5		// 버튼 클릭
#define XWM_SELECT_MENU			6		// 메뉴 선택
#define XWM_BOUND_BOTTOM		7		// 스크롤뷰에서 아래쪽 한계선까지 스크롤이 되면 발생한다.
#define XWM_BOUND_TOP			8		// 스크롤뷰에서 위쪽 한계선까지 스크롤이 되면 발생한다.
//#define XWM_BOUND_LEFT			9		// 스크롤뷰에서 왼쪽 한계선까지 스크롤이 되면 발생한다.
//#define XWM_BOUND_RIGHT			10		// 스크롤뷰에서 오른쪽 한계선까지 스크롤이 되면 발생한다.
#define XWM_ENTER					11		// 에디트컨트롤에서 글자입력하고 엔터(OK)쳤을때 이벤트.


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
//class XBaseScene;
class XToolTip;
struct XWND_MESSAGE_MAP
{
	DWORD msg;
	DWORD param;
	XWnd *pOwner;
	int (XWnd::*pHandler)( XWnd*, DWORD dwParam1, DWORD dwParam2 );
	XWND_MESSAGE_MAP() {
		msg = 0;
		param = 0;
		pOwner = NULL;
		pHandler = NULL;
	}
};	
////////////////////////////////////////////////////////////////////////////////////
class XWnd
{
public:
	static XWnd *s_pFocusWindow;
	void SetFocus( XWnd *pWindow ) { s_pFocusWindow = pWindow; }
	XWnd* GetFocus( void ) { return s_pFocusWindow; }
	static XBaseFontDat *s_pFontDat;
	static XWnd *s_pDropWnd;			// drag & drop 윈도우
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
	virtual XE::VEC2 GetPosFinal( void ) { 
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
	}
	void DestroyAllWnd( void ) {
		LIST_DESTROY( m_listItems, XWnd* );
	}
	int CallEventHandler( ID msg, DWORD dwParam1=0 );
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
	//
	static BOOL LoadResFrame( LPCTSTR szFile, XWND_RES_FRAME *pFrame, int surfaceBlockSize=0 );
	// event handler
//	void SetEvent( DWORD msg, XBaseScene *pOwner, void (XBaseScene::*pFunc)( XWnd* ) ) {
	template<typename T>
	void SetEvent( DWORD msg, XWnd *pOwner, T func, DWORD param1=0, DWORD param2=0 ) {
		typedef int (XWnd::*CALLBACK_FUNC)( XWnd *, DWORD dwParam1, DWORD dwParam2 );
		XWND_MESSAGE_MAP msgMap;
		msgMap.msg = msg;
		msgMap.param = param1;
		msgMap.pOwner = pOwner;
		msgMap.pHandler = static_cast<CALLBACK_FUNC>( func );
		m_listMessageMap.push_back( msgMap );
	}
	XWND_MESSAGE_MAP FindMsgMap( DWORD msg ) {
		LIST_LOOP( m_listMessageMap, XWND_MESSAGE_MAP, itor, msgMap )
		{
			if( msgMap.msg == msg )
				return msgMap;
		} END_LOOP;
		XWND_MESSAGE_MAP msgMap;
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
	XWndView( float x, float y, float w, float h, const char *cDrawHandler, XLua *pLua )
		: XWnd( NULL, x, y, w, h ) {
		Init(); 
		if( cDrawHandler )
			strcpy_s( m_cDrawHandler, cDrawHandler );
		m_prefLua = pLua;
	}
	XWndView( float x, float y, float w, float h, LPCTSTR szRes )
		: XWnd( NULL, x, y, w, h ) {
		Init(); 
		LoadRes( szRes );
	}
	virtual ~XWndView() { Destroy(); }
	GET_SET_ACCESSOR( BOOL, bDrawFrame );
	BOOL IsAnimation( void ) { return m_nStateAppear != 1; }
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
	XWndDialog( T x, T y, T width, T height, LPCTSTR szRes, LPCTSTR szFont ) 
		: XWndView( x, y, width, height ) { 
			Init(); 
			LoadRes( szRes );
			m_pFontDat = FONTMNG->Load( szFont, 20.f );
			m_pFont = m_pFontDat->CreateFontObj();
			SetDialogSize( width, height );
	}
	template<typename T>
	XWndDialog( T width, T height, LPCTSTR szRes, LPCTSTR szFont ) 
		: XWndView() { 
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
	virtual void OnCancel( void ) {}
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

// iOS방식의 노티피케이션 알림창
class XWndTextString;
class XWndNotification : public XWndView
{
	CTimer m_Timer;
	void Init() {
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

// menuitem base class
class XWndMenuItem : public XWnd
{
	void Init() {}
	void Destroy() {}
public:
	XWndMenuItem() {}
	virtual ~XWndMenuItem() {}
	virtual XWnd* OnLButtonDown( float lx, float ly ) { return NULL; }
	virtual ID OnLButtonUp( float lx, float ly ) { return 1; }
};

class XWndScrollView : public XWnd
{
	XE::VEC2 m_vAdjust;
	XE::VEC2 m_vPrev;
	BOOL m_bLButtonDown;
	BOOL m_bScroll;
public:
	XWndScrollView( float x, float y, float w, float h ) 
		: XWnd( NULL, x, y, w, h )	{
			m_bLButtonDown = FALSE;
			m_bScroll = FALSE;
	}
	virtual ~XWndScrollView() {}
	//
	virtual XE::VEC2 GetPosFinal( void ) { 
		return XWnd::GetPosFinal() + m_vAdjust;
	}
	void Clear( void ) {
		m_vAdjust.Set( 0 );
	}
	void Scroll( float sx, float sy );
	virtual XWnd* OnLButtonDown( float lx, float ly ) {
		XE::VEC2 vMouse( lx, ly );
		m_vPrev = vMouse;
		m_bLButtonDown = TRUE;
	return XWnd::OnLButtonDown( -m_vAdjust.x + lx, -m_vAdjust.y + ly );
	}
	virtual XWnd* OnMouseMove( float lx, float ly ) {
		XE::VEC2 vMouse( lx, ly );
		XE::VEC2 vDist = vMouse - m_vPrev;
		XWnd *pWndRet = this;
		if( m_bLButtonDown )
			m_bScroll = TRUE;
		if( m_bScroll )
			XWndScrollView::Scroll( 0, -vDist.y );
		else
			pWndRet = XWnd::OnMouseMove( -m_vAdjust.x + lx, -m_vAdjust.y + ly );
		m_vPrev = vMouse;
		return pWndRet;
	}
	virtual ID OnLButtonUp( float lx, float ly ) {
		m_bLButtonDown = FALSE;
		if( m_bScroll == FALSE )
		{
			m_bScroll = FALSE;
			return XWnd::OnLButtonUp( -m_vAdjust.x + lx, -m_vAdjust.y + ly );
		}
		m_bScroll = FALSE;
		return GetID();
	}
};

// 리스트 컨트롤
class XWndList : public XWndScrollView
{
	XList<XWnd*> m_listItem;
	XWnd *m_pClickedWnd;
	float m_Y;
	void Init() {
		m_Y = 0;
		m_pClickedWnd = NULL;
	}
	void Destroy();
public:
	XWndList( float x, float y, float w, float h );
	XWndList( const XE::VEC2& vPos, const XE::VEC2& vSize );
	virtual ~XWndList() { Destroy(); }
	//
	XWnd* AddItem( ID idWnd, XWnd *pWnd );
	void Clear( void ) {	
		XWndScrollView::Clear();
		m_listItem.Clear();
		DestroyAllWnd();	 
		m_pClickedWnd = NULL;
		m_Y = 0;
	}
	void Del( XWnd* pWnd );
	void DelFromID( ID idWnd ) ;
	int GetNumItem( void ) {
		return m_listItem.size();
	}
	//
	virtual void Draw( void );
	virtual XWnd* OnLButtonDown( float lx, float ly );
	virtual XWnd* OnMouseMove( float lx, float ly );
	virtual ID OnLButtonUp( float lx, float ly );
};

// 팝업 메뉴
class XWndPopupMenu : public XWndView
{
	int (XWnd::*m_pCallBack)( XWnd*, DWORD dwParam1, DWORD dwParam2 );
	XWnd *m_pOwnerHandler;
	void Init() {
		m_ctrlList = NULL;
		m_pCallBack = NULL;
		m_pOwnerHandler = NULL;
	}
	void Destroy() {}
protected:
	XWndList *m_ctrlList;
	XE::VEC2 m_vPrev;
public:
	XWndPopupMenu( const XE::VEC2 vPos, const XE::VEC2 vSize, LPCTSTR szRes );
	virtual ~XWndPopupMenu() { Destroy(); }

/*	template<typename T>
	void SetEvent( DWORD msg, XWnd *pOwner, T func, DWORD param1=0, DWORD param2=0 ) {
		m_ctrlList->SetEvent( msg, pOwner, func, param1, param2 );
	} */
	template<typename T>
	void SetEvent( XWnd *pOwner, T func ) {
		typedef int (XWnd::*CALLBACK_FUNC)( XWnd *, DWORD dwParam1, DWORD dwParam2 );
		m_pCallBack = static_cast<CALLBACK_FUNC>( func );
		m_pOwnerHandler = pOwner;
		m_ctrlList->SetEvent( XWM_SELECT_MENU, this, &XWndPopupMenu::OnSelectMenu );
	}
	XWnd* AddItem( ID idWnd, XWnd *pWnd ) {
		return m_ctrlList->AddItem( idWnd, pWnd );
	}
	int OnSelectMenu( XWnd *pWnd, DWORD param1, DWORD param2 ) {
		int ret = (m_pOwnerHandler->*m_pCallBack)( pWnd, param1, param2 );
		SetbDestroy( TRUE );
		return ret;
	}
	void OnNCLButtonUp( float lx, float ly );
//	virtual void Draw( void );
};

#include "XList.h"
// 심플한 텍스트 팝업 메뉴
class XWndPopupMenuText : public XWndPopupMenu
{
//	XList<XWndTextString*> m_listText;
	XBaseFontDat *m_pFontDat;
//	int m_idxSelected;
	float m_hElem;		// 메뉴 항목의 세로높이
//	float m_DrawYOffset;
//	BOOL m_bScroll;
//	BOOL m_bClicked;
	void Init() {	
		m_pFontDat = NULL;
//		m_idxSelected = -1;
		m_hElem = 0;
//		m_DrawYOffset = 0;
//		m_bScroll = FALSE;
//		m_bClicked = FALSE;
	}
	void Destroy();
public:
	XWndPopupMenuText( const XE::VEC2 vPos, const XE::VEC2 vSize, LPCTSTR szRes, XBaseFontDat *pFontDat, XCOLOR col )
		: XWndPopupMenu( vPos, vSize, szRes ) { 
			Init(); 
			m_pFontDat = pFontDat;
			m_hElem = pFontDat->GetFontHeight() * 1.5f;;
	}
	virtual ~XWndPopupMenuText() { Destroy(); }

	void AddItem( ID idWnd, LPCTSTR szText );
	void Draw( void );
	XWnd* OnLButtonDown( float lx, float ly );
	XWnd* OnMouseMove( float lx, float ly );
	ID OnLButtonUp( float lx, float ly );
};
