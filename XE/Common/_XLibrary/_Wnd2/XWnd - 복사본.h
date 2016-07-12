#pragma once 

#include "XGraphics.h"
#include "InputMng.h"
#include "Timer.h"
#include "XSoundMng.h"
#include "XFont.h"
#include "XFontMng.h"
#include "XLua.h"
#include "XInterpolationObj.h"
#include "XLayout.h"
#pragma warning( disable : 4250 )	//  warning C4250: 'XWndButtonHexNumber' : 우위에 따라 'XWndButton::XWndButton::Process'을(를) 상속합니다.

#define XWM_STOP_ROLLING			1
#define XWM_DESTROY				2
#define XWM_OK						3		// 다이얼로그에서 OK눌렀을때
#define XWM_SLIDING					4		// 슬라이더 컨트롤을 드래그할때 발생
#define XWM_CLICKED					5		// 버튼 클릭
#define XWM_SELECT_MENU			6		// 메뉴 선택
#define XWM_DESELECT_MENU		7		// 메뉴 선택
#define XWM_BOUND_BOTTOM		8		// 스크롤뷰에서 아래쪽 한계선까지 스크롤이 되면 발생한다.
#define XWM_BOUND_TOP			9		// 스크롤뷰에서 위쪽 한계선까지 스크롤이 되면 발생한다.
#define XWM_CREATE					10		// 버튼,다이얼로그 등이 생성될때 발생한다.
#define XWM_DESTROY_AFTER		11		// 어떤객체가 파괴되고 난후 발생될 이벤트
#define XWM_ENTER					12		// 에디트컨트롤에서 글자입력하고 엔터(OK)쳤을때 이벤트.
#define XWM_LBUTTONDOWN		13		// 버튼을 LButtonDown한 최초 순간에 발생.
// 
#define XWM_SOUND_DOWN		14		// 버튼을 누를때 소리
#define XWM_SOUND_UP				15		// 버튼을 뗄때 소리
#define XWM_CANCEL					16
#define XWM_YES						17
#define XWM_NO						18

#define XWM_FINISH_APPEAR		19		// 애니메이션 등장되는 윈도우에서 애니메이션이 끝난 순간.
#define XWM_LOOP_EVENT			20		// 하위 윈도우들을 루프돌면서 지정된 핸들러를 호출해줌
// scene
#define XWM_FINISH_SLIDE			21		// XSceneSlide에서 슬라이딩이 끝나면 호출됨
// help
#define XWM_HELP_CLICKED			22		// 버튼을 누르면 헬프객체로 이벤트를 던져준다.
#define XWM_END_HELP_SEQ			23		// 헬프 시퀀스가 끝나면 호출
// Quest
#define XWM_QUEST_CLICKED		24		// 버튼이 눌리면 퀘스트매니저로 이벤트가 전달된다.
// DownloadTask
#define XWM_ALL_COMPLETE			30		// 모든 다운로드가 끝남
#define XWM_EACH_COMPLETE		31		// 각각의 다운로드가 끝남
#define XWM_ERROR_DOWNLOAD	32		
// etc
#define XWM_FINISH_NUMBER_COUNTER		100	// XWnnTextNumberCount의 카운터가 끝나면 호출.

// flag
namespace XE {
	enum {	xID_OK=1,
				xID_CANCEL=2,
				xID_YES=3,
				xID_NO=4
			};
};

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
class XLayout;
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
	enum xtAlert { xOK=1, xOKCANCEL, xYESNO };
	enum xtAppear { xAP_NONE, 
							xAP_ENTER_INIT,
							xAP_ENTER_ING,
							xAP_ENTER_END,
							xAP_LEAVE_INIT,
							xAP_LEAVE_ING,
							xAP_LEAVE_END,
	};
	static XWnd *s_pFocusWindow;
	void SetFocus( XWnd *pWindow ) { s_pFocusWindow = pWindow; }
	XWnd* GetFocus( void ) { return s_pFocusWindow; }
	static XBaseFontDat *s_pFontDat;
	static XWnd *s_pDropWnd;			// drag & drop 윈도우
	void AddListAllowClick( ID idAllow ) {
		XBREAK( idAllow == 0 );
		s_listAllowClick.Add( idAllow );
	}
	XList<ID>& GetAllowList( void ) {
		return s_listAllowClick;
	}
//	static ID s_idAllowClick;			// 이게 세팅되어 있으면 이 윈도우외에 다른 윈도우는 입력을 막아야 한다.
//	static XWnd *s_pAllowClick;		// 속도땜에 사전에 찾아두는 포인터
private:
	static XList<ID> s_listAllowClick;		// 이게 세팅되어 있으면 이 윈도우들외에 다른 윈도우는 입력을 막아야 한다.
	static BOOL IsAllowClickWnd( XWnd *pCompare );
private:
	void Init( void ) {
		m_wtWnd = XE::WT_NONE;
		m_id = 0;
		m_vScale.x = m_vScale.y = 1.0f;
		m_dRotateZ = 0;
		m_fAlpha = 1.0f;
//		SetScaleLocal( 1.0f );
		m_pParent = NULL;
//		m_pWndMng = NULL;
		m_pFirstPush = NULL;
		m_bShow = TRUE;
		m_bEnable = TRUE;
		m_bActive = TRUE;
		m_pModalDlg = NULL;
		m_Destroy = 0;
		m_prefToolTip = NULL;
		m_bToolTip = FALSE;
		m_pwndLastLDown = NULL;
		m_pwndLastLUp = NULL;
		m_bModal = FALSE;
		m_pCapture = NULL;
		m_bProcess = TRUE;
		m_bEnter = FALSE;
		m_bAnimationEnterLeave = FALSE;
		m_stateAppear = xAP_NONE;
		m_secAppear = 0;
		m_Priority = 0;
		m_pAfterDestroyEvent = NULL;
	}
	void Destroy( void );
	// protected로 만들지 말것 반드시 GetScaleLocal/Final을 구분해서 쓸것
	XE::xtWnd m_wtWnd;	// 윈도우 타입.
	XE::VEC2 m_vPos;		// local
	XE::VEC2 m_vScale, _m_vstackScale;	// local
	float m_dRotateZ;
	int m_Destroy;		// self destroy. 0:live 1:죽고있는중 2:이번턴에죽음
	float m_fAlpha;		
	XToolTip *m_prefToolTip;
	BOOL m_bToolTip;
	CTimer m_timerPush;		// 누른시간
	XWnd *m_pwndLastLDown;		// LButtonDown한 윈도우
	XWnd *m_pwndLastLUp;		// 마지막으로 LButtonUp한 윈도우
	GET_SET_ACCESSOR( XWnd*, pFirstPush );
	XWnd *m_pCapture;		// 마우스이벤트 캡쳐
	CTimer m_timerAppear;
	BOOL m_bEnter;
	xtAppear m_stateAppear;		// 현재 등장상태
	BOOL m_bAnimationEnterLeave;	// 등장이나 퇴장시 애니메이션을 할건지
	float m_secAppear;
	XWnd *m_pAfterDestroyEvent;	// this가 파괴되고 난 직후에 이벤트처리를 해줘야할 윈도우
    void PrivateDestroyChild( void );
	std::string m_strIdentifier;		// 식별 문자열
protected:
	list<XWND_MESSAGE_MAP> m_listMessageMap;
	XE::VEC2 m_vSize;		// local fuck 잠시 프로텍트로 옮겨옴
	ID	m_id;
	XWnd *m_pParent;
	XList<XWnd*> m_listItems;
//	XWndMng *m_pWndMng;
	XWnd *m_pFirstPush;		// 최초클릭됐을때 컨트롤의 포인터( m_pwndLastLDown과 중복되므로 통합시킬것 );
	BOOL m_bShow;			// show/hide
	BOOL m_bEnable;
	BOOL m_bActive;			// 마우스클릭등의 작동을 멈추게 한다
	BOOL m_bModal;			// 모달형태인지.
	BOOL m_bProcess;			// process()를 임의로 멈추게 할때
	XWnd* m_pModalDlg;		// 모달다이얼로그가 떠있는지
	int m_Priority;				// 찍기 우선순위. 같이 높을수록 위에 찍힌다.
	SET_ACCESSOR( XE::xtWnd, wtWnd );
public:
	XWnd() { Init(); }
	template<typename T>
	XWnd( XWndMng *pWndMng, T x, T y, T w, T h ) {
		Init();
//		m_pWndMng = pWndMng;
#ifdef _DEBUG
		XBREAK( w == 0 || h == 0 );
#endif
		m_vPos.Set( x, y ); 
		m_vSize.Set( w, h );
		SetScaleLocal( 1.0f );
	}
	template<typename T>
	XWnd( XWndMng *pWndMng, T x, T y ) {
		Init();
//		m_pWndMng = pWndMng;
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

	GET_ACCESSOR( XE::xtWnd, wtWnd );
	GET_ACCESSOR( XWnd*, pParent );
	ID		GetID( void ) { return m_id; }
	void	SetID( ID id ) { m_id = id; }
	GET_SET_ACCESSOR( BOOL, bShow );
//	GET_SET_ACCESSOR( BOOL, bEnable );
	SET_ACCESSOR( BOOL, bEnable );
	BOOL GetbEnable( void ) {
		if( m_bEnable == FALSE )		// 부모가 disable이면 자식들도 모두 disable이다. enable은 그렇지 않음.
			return FALSE;
		if( m_pParent )
			return m_pParent->GetbEnable();
		else
			return m_bEnable;
	}
//	GET_SET_ACCESSOR( BOOL, bDestroy );
	// 죽고있는상태/죽을상태 모두를 포함.
	BOOL GetDestroy( void ) {
		return (m_Destroy) ? TRUE : FALSE;
	}
	// 죽고있는 상태인가
	BOOL GetDestroying( void ) {
		return (m_Destroy == 1) ? TRUE : FALSE;
	}
	// 이번턴에 죽을넘인가
	BOOL GetDestroyFinal( void ) {
		return (m_Destroy == 2) ? TRUE : FALSE;
	}
	void SetbDestroy( BOOL bFlag ) {
		if( m_bAnimationEnterLeave ) {
			m_Destroy = 1;		// 죽고있는 상태로
			m_stateAppear = xAP_LEAVE_INIT;		// 사라지는 애니메이션 시작
		} else
			m_Destroy = (bFlag) ? 2 : 0;
	}
	void SetDestroyFinal( void ) {
		m_Destroy = 2;
	}
	SET_ACCESSOR( BOOL, bActive );
	// 부모중에 하나라도 deActive상태면 자식도 모두 deActive 상태가 된다.
	BOOL GetbActive( void ) {
		if( m_bActive == FALSE )
			return FALSE;
		if( m_pParent )
			return m_pParent->GetbActive();
		else
			return m_bActive;
	}
	XE::VEC2 GetAdjust( void );
//	GET_ACCESSOR( XWnd*, pModalDlg );
	GET_SET_ACCESSOR( XToolTip*, prefToolTip );
	GET_SET_ACCESSOR( BOOL, bToolTip );
	GET_ACCESSOR( CTimer&, timerPush );
	GET_ACCESSOR( XWnd*, pwndLastLDown );
	GET_ACCESSOR( XWnd*, pwndLastLUp );
	GET_SET_ACCESSOR( BOOL, bModal );
	GET_SET_ACCESSOR( BOOL, bProcess );
	GET_ACCESSOR( BOOL, bAnimationEnterLeave );
	GET_SET_ACCESSOR( int, Priority );
	// 윈도우 등장이나 퇴장시 애니메이션을 할건지
	void SetbAnimationEnterLeave( BOOL bFlag, float sec=0.5f ) {
		m_bAnimationEnterLeave = bFlag;
		m_secAppear = sec;
	} 
//	BOOL GetbAnimationEnterLeave( void ) {
//		return m_timerAppear.IsOn();
//	}
	// pCompare윈도우가 this윈도우의 계열중에 하나인지 검사
	// 계열이란. this의 부모들과 자기자신까지 포함.
	BOOL IsHierarchyWnd( XWnd *pCompare ) {
		if( pCompare == this )
			return TRUE;
		if( m_pParent )
			return m_pParent->IsHierarchyWnd( pCompare );
		return FALSE;
	}
protected:
	GET_SET_ACCESSOR( XWnd*, pAfterDestroyEvent );
public:
	GET_ACCESSOR( BOOL, bEnter );
	void SetstrIdentifier( const char *cIdentifier ) {
		m_strIdentifier = cIdentifier;
	}
	const char* GetszIdentifier( void ) {
		return m_strIdentifier.c_str();
	}
	void SetCapture( XWnd *pWnd ) {
		if( m_pParent )
			return m_pParent->SetCapture( pWnd );
		m_pCapture = pWnd;		// 최상위 윈도우에 세팅한다.
	}
	// 부모를 거슬러 올라가며 캡쳐 윈도우를 찾음.
	XWnd* GetCapture( void ) {
		if( m_pCapture )
			return m_pCapture;
		if( m_pParent )
			return m_pParent->GetCapture();
		return NULL;
	}
	void ReleaseCapture( void ) {
		if( m_pParent )
			return m_pParent->ReleaseCapture();
		m_pCapture = NULL;
	}
	//
private:
	void SetModalDlg( XWnd* pModalDlg ) {
		SAFE_DELETE( m_pModalDlg );	// 먼저있던건 삭제
		m_pModalDlg = pModalDlg;
	}  
	void DestroyModalDlg( void ) { SetModalDlg( NULL ); }
public:
	void DestroyChildAll( void );
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
	void SetSizeLocal( T w, T h ) { SetSizeLocal( XE::VEC2( w, h ) ); }
#ifdef _DEBUG
	BOOL IsSizeZero( void ) {
		if( m_vSize.IsZero() )
			return TRUE;
		if( m_pParent )
			return m_pParent->IsSizeZero();
		return FALSE;
	}
	void SetSizeLocal( const XE::VEC2& vSize ) { 
		m_vSize.x = vSize.x; m_vSize.y = vSize.y; 
		// 디버그모드일땐 부모의 사이즈가 0인지 체크한다.
		// 자식의 사이즈가 정해졌는데 부모의 사이즈가 0이면 뭔가 이상한것이라 판단한다.
		if( m_pParent && (vSize.IsZero() == FALSE && vSize.IsMinus() == FALSE) )
			XBREAK( m_pParent->IsSizeZero() == TRUE );
	}
#else
	void SetSizeLocal( const XE::VEC2& vSize ) { 
		m_vSize.x = vSize.x; m_vSize.y = vSize.y; 
	}
#endif
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
	XWnd* Add( XWnd *pChild ) { 
		return Add( XE::GenerateID(), pChild ); 
	}
	XWnd* Add( int id, XWnd *pChild );
	XWnd* AddWndTop( XWnd *pWnd ) {
		return AddWndTop( XE::GenerateID(), pWnd );
	}
	XWnd* AddWndTop( ID idWnd, XWnd *pWnd );
	XWnd* GetWndTop( void ) {
		if( m_pParent )
			return m_pParent->GetWndTop();
		return this;
	}
	void SortPriority( void );
	void DestroyID( int id )
	{
		XWnd *pWnd = Find( id );
		if( pWnd )
			pWnd->SetbDestroy( TRUE );
	}
	void DestroyWnd( XWnd *pWnd )
	{
		if( pWnd == NULL )
			return;
		if( Find( pWnd ) )
			pWnd->SetbDestroy( TRUE );
	}
	void DestroyAllWnd( void ) {
		DestroyChildAll();
//		LIST_DESTROY( m_listItems, XWnd* );
	}
	int CallEventHandler( ID msg, DWORD dwParam2=0 );
	virtual int Process( float dt );
	virtual BOOL ProcessEnterLeaveAnimation( void );
	virtual void Draw( void );
	virtual void Draw( const XE::VEC2& vParent );
	virtual void OnDestroy( void ) {}
	virtual void CallEvent( const char *cFunc );
	void DrawFrame( const XWND_RES_FRAME& frame );
	void GenerateLoopEvent( void );
	XWnd* Find( ID id );
	XWnd* Find( const char *cIdentifier );
	XWnd* Find( XWnd *_pWnd );
	int GetNumChild( void ) {
		return m_listItems.size();
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
		// fuck
		LIST_LOOP( m_listMessageMap, XWND_MESSAGE_MAP, itor, msgMap )
		{
			if( msgMap.msg == msg )
			{
				XWND_MESSAGE_MAP *pMsgMap = &(*itor);
				pMsgMap->msg = msg;
				pMsgMap->param = param1;
				pMsgMap->pOwner = pOwner;
				pMsgMap->pHandler = static_cast<CALLBACK_FUNC>( func );
				return;
			}
		} END_LOOP;
		XWND_MESSAGE_MAP msgMap;
		msgMap.msg = msg;
		msgMap.param = param1;
		msgMap.pOwner = pOwner;
		msgMap.pHandler = static_cast<CALLBACK_FUNC>( func );
		m_listMessageMap.push_back( msgMap );
	}
	void ClearEvent( DWORD msg ) {
		LIST_MANUAL_LOOP( m_listMessageMap, XWND_MESSAGE_MAP, itor, msgMap )
		{
			if( msgMap.msg == msg )
				m_listMessageMap.erase( itor++ );
			else
				++itor;
		} END_LOOP;
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
	BOOL IsHaveEvent( DWORD msg ) {
		LIST_LOOP( m_listMessageMap, XWND_MESSAGE_MAP, itor, msgMap )
		{
			if( msgMap.msg == msg )
				return TRUE;
		} END_LOOP;
		return FALSE;
	}
	// handler
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnNCMouseMove( float lx, float ly ) {}		// 윈도우 영역밖에서 움직일때 호출되는 핸들러
	virtual void OnNCLButtonUp( float lx, float ly ) {}
	virtual BOOL OnKeyDown( int keyCode );
	virtual BOOL OnKeyUp( int keyCode );
	virtual void OnPlaySound( ID idSnd ) {}
	virtual XToolTip* CreateToolTip( void ) { return NULL; }		// 툴팁을 실시간으로 생성시키고 싶다면 이것을 재정의 할것.
	virtual void ProcessAnimationLerp( BOOL bEnter, float timeLerp, float aniLerp );
	virtual void OnFinishAppear( void ) {}
	virtual void OnAddWnd( void ) {}				// 윈도우가 생성되고 AddWnd()가 될때 불려진다.
	//
	void LuaDestroy( void ) {
		SetbDestroy( TRUE );
	}
	ID LuaGetID( void ) {
		return m_id;
	}
	BOOL LuaGetEnable( void ) {
		return m_bEnable;
	}
}; // class XWnd

#include "XFontSpr.h"

//-------------------------------------------------------------------------------------
class XWndView : public XWnd
{
	XLua *m_prefLua;
	char m_cDrawHandler[ 32 ];	// lua draw handler
	float m_dt;
	BOOL m_bDrawFrame;
	CTimer m_timerAppear;	// 나타날때 타이머
	XE::VEC2 m_vLT;				// 스케일 1.0일때 좌상귀 좌표
	XCOLOR m_colBg;
	XCOLOR m_colBorder;
#ifdef _CHEAT
	CTimer m_timerSpeed;		// 로딩속도 테스트용
	DWORD m_timeLoad;
#endif
	void Init() {
		m_prefLua = NULL;
		XCLEAR_ARRAY( m_cDrawHandler );
		strcpy_s( m_cDrawHandler, "OnDrawView" );		// default handler name
		m_dt = 1.0f;
		m_bDrawFrame = FALSE;
//		m_timerAppear.SetRealTimer( TRUE );
		m_nStateAppear = 0;
		m_nBlockSize = 0;
		m_psfcFrame = NULL;
		m_colBg = 0;
		m_colBorder = 0;
		SetbAnimationEnterLeave( TRUE );		// 기본적으론 등장 애니메이션 있음.
#ifdef _CHEAT
		m_timeLoad = 0;
#endif
	}
	void Destroy() {
		SAFE_RELEASE2( IMAGE_MNG, m_psfcFrame );
	}
protected:
	int m_nBlockSize;
	XWND_RES_FRAME m_frameImage;
	int m_nStateAppear;		// 나타날때 처리중 다용도 상태
	XSurface *m_psfcFrame;	// 뷰 바탕 이미지가 따로 있을때 지정
public:
	XWndView() { Init(); }
	XWndView( float x, float y, float w, float h, const char *cDrawHandler, XLua *pLua )
		: XWnd( NULL, x, y, w, h ) {
		Init(); 
		if( cDrawHandler )
			strcpy_s( m_cDrawHandler, cDrawHandler );
		m_prefLua = pLua;
	}
	XWndView( float x, float y, float w, float h, LPCTSTR szFrame )
		: XWnd( NULL, x, y, w, h ) {
		Init(); 
		LoadRes( szFrame );
	}
	XWndView( float x, float y, LPCTSTR szImg )
		: XWnd( NULL, x, y ) {
		Init(); 
		m_psfcFrame = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, szImg ) );
		XBREAK( m_psfcFrame == NULL );
		SetSizeLocal(m_psfcFrame->GetSize() );
	}
	XWndView( const XE::VEC2& vPos, const XE::VEC2& vSize, XCOLOR colBg, XCOLOR colBorder=XCOLOR_WHITE )
		: XWnd( NULL, vPos.x, vPos.y, vSize.w, vSize.h ) {
		Init();
		m_colBg = colBg;
		m_colBorder = colBorder;
	}
	XWndView( XLayout *pLayout, const char *cKey, XWnd *pParent, const char *cGroup );
	virtual ~XWndView() { Destroy(); }
	GET_SET_ACCESSOR( BOOL, bDrawFrame );
	BOOL IsAnimation( void ) { return m_nStateAppear != 1; }
	float GetSizeFrameBlock( void ) {
		return (float)m_frameImage.nBlockSize;
	}
	//
    BOOL LoadRes( LPCTSTR szRes );

	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void ProcessAnimationLerp( BOOL bEnter, float timeLerp, float aniLerp );
	virtual BOOL ProcessEnterLeaveAnimation( void );
//	virtual void OnAddWnd( void );
//	virtual void OnFinishAppear( void ) {}
};

//}
class XWndDialog : public XWndView
{
public:
	// 예약된 다이얼로그 리턴값
/*	enum xtReturn { xNONE_RETURN=0, 
						xYES=0x7fffff01, // 루아에는 unsigned형이 없는듯? 그래서 -가 되지 않도록 함
						xNO=0x7fffff02, 
						xOK=0x7fffff03, 
						xCANCEL=0xffffff04 }; */
private:
	void Init() {
		m_pFontDat = NULL;
		m_pFont = NULL;
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
	XWndDialog( LPCTSTR szRes, LPCTSTR szFont ) 
		: XWndView() { 
			Init(); 
			LoadRes( szRes );
			m_pFontDat = FONTMNG->Load( szFont, 20.f );
			m_pFont = m_pFontDat->CreateFontObj();
	}
	XWndDialog( LPCTSTR szImg ) 
		: XWndView( 0, 0, szImg ) { 
			Init(); 
			float w = m_psfcFrame->GetWidth();
			float h = m_psfcFrame->GetHeight();
			SetSizeLocal( w, h );
			SetPosLocal( XSCREEN_WIDTH/2-(int)w/2, XSCREEN_HEIGHT/2-(int)h/2 );
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
//	XWndButton* AddButton( ID id, float x, float y, LPCTSTR szText, LPCTSTR szSpr, ID idActUp, ID idActDown=0 );
//	XWndButton* AddButton( ID id, const XE::VEC2& vPos, const XE::VEC2& size, LPCTSTR szText );
	XWndButton* AddButton( ID id, float x, float y, LPCTSTR szSpr, ID idActUp, ID idActDown  );
	XWnd* AddStaticText( ID id, float x, float y, float w, float h, LPCTSTR szText, XBaseFontDat *pFontDat, XE::xAlign align, XCOLOR col=XCOLOR_WHITE, xFONT::xtStyle style=xFONT::xSTYLE_NORMAL );
	XWnd* AddStaticText( float x, float y, LPCTSTR szText, XBaseFontDat *pFontDat, XE::xAlign align, XCOLOR col = XCOLOR_WHITE, xFONT::xtStyle style=xFONT::xSTYLE_NORMAL ) {
		return AddStaticText( XE::GenerateID(), x, y, 0, 0, szText, pFontDat, align, col, style );
	}
	//
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnOk( void ) {}
	virtual void OnCancel( void ) {}
	// lua
	// 삭제
//	XWndButton* LuaAddButton( ID id, float x, float y, ID idText, ID idActUp, ID idActDown ) { 
//		if( XWnd::s_pFont == NULL )
//			XWnd::s_pFont = FONTMNG->Load( _T("NanumGothicBold.ttf"), 20.f );
//		return AddButton( id, x, y, XTEXT( idText )/*XWnd::s_pFont*/, idActUp, idActDown ); 
//	}
//	int LuaDoModal( lua_State *L );

};	// XWndDialog
//-------------------------------------------------------------------------------------
class XWndImage : public XWnd
{
	BOOL m_bCreate;			// 자체 생성한거라 파괴의 책임도 this에 있음을 알림
	XSurface *m_pSurface;
	XE::VEC2 m_vAdjust;
	XE::VEC2 m_vAdjustAxis;		// 회전/확축의 축 설정
	float m_dAngY;
	float m_dAngZ;
	void Init() {
		m_bCreate = FALSE;
		m_pSurface = NULL;
		m_dAngY = 0;
		m_dAngZ = 0;
	}
	void Destroy() {
		if( m_bCreate )
			SAFE_RELEASE2(IMAGE_MNG, m_pSurface );
	}
public:
	template<typename T>
	XWndImage( XWndMng *pWndMng, BOOL bHighReso, LPCTSTR szRes, T x, T y ) 
		: XWnd( pWndMng, x, y )
	{
		Init();
		if( XE::IsHave( szRes ) )
		{
			Create( bHighReso, szRes );
			SetSizeLocal( m_pSurface->GetWidth(), m_pSurface->GetHeight() );
		}
	}
	XWndImage( XWndMng *pWndMng, BOOL bHighReso, float x, float y, float w, float h ) 
		: XWnd( pWndMng, x, y )
	{
		Init();
		SetSizeLocal( w, h );
	}
	XWndImage( XSurface *pSurface, float x, float y ) 
		: XWnd( NULL, x, y ) {
		Init();
		m_pSurface = pSurface;
		SetSizeLocal( pSurface->GetWidth(), pSurface->GetHeight() );
	}
	virtual ~XWndImage() { Destroy(); }
	//
	void SetAdjustAxis( const XE::VEC2& vAxis ) {
		m_vAdjustAxis = vAxis;
	}
	void SetSurface( LPCTSTR szImg ) {
		XBREAK( szImg == NULL );
		if( m_bCreate )
			SAFE_RELEASE2(IMAGE_MNG, m_pSurface );
		Create( TRUE, szImg );
		SetSizeLocal( m_pSurface->GetWidth(), m_pSurface->GetHeight() );
	}
	void SetSurface( XSurface *pSurface ) {
		// pSurface가 널일수도 있다.
		if( m_bCreate )
		{
			SAFE_RELEASE2(IMAGE_MNG, m_pSurface );
			m_bCreate = FALSE;
		}
		m_pSurface = pSurface;
	}
	void SetAlign( XE::xAlign align ) {
		switch( align ) {
		case XE::xALIGN_LEFT:		m_vAdjust = XE::VEC2(0);	break;
		case XE::xALIGN_RIGHT:	m_vAdjust.w -= GetSizeFinal().w;		break;
		case XE::xALIGN_CENTER:	m_vAdjust -= GetSizeFinal() / 2.f;		break;
		case XE::xALIGN_BOTTOM:	m_vAdjust.y = -GetSizeFinal().h;	break;
		}
	}
	// 돌리려면 AdjustAxis로 축도 지정해줘야 한다.
	void SetRotateY( float dAng ) {
		m_dAngY = dAng;
	}
	void SetRotateZ( float dAng ) {
		m_dAngZ = dAng;
	}
	//
	void Create( BOOL bHighReso, LPCTSTR szImg );
	virtual void Draw( void );
	virtual void Draw( XE::VEC2& vPos, xDM_TYPE drawMode/*=xDM_NORMAL*/ );
	virtual void Draw( const XE::VEC2& vPos );
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

	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
//	virtual void OnNCMouseMove( float lx, float ly );
//	virtual void OnNCLButtonUp( float lx, float ly );
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
		SetPosLocal( XSCREEN_WIDTH_HALFSIZE - nw / 2.f, 5.f );
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
	virtual void OnLButtonDown( float lx, float ly ) {}
	virtual void OnLButtonUp( float lx, float ly ) {}
};

class XWndScrollView : public XWnd
{
	XE::VEC2 m_vAdjust;
	XE::VEC2 m_vPrev;
	BOOL m_bLButtonDown;
	BOOL m_bLButtonUp;
	BOOL m_bScroll;
	XE::VEC2 m_vTotalSize;			// 스크롤뷰의 실제 크기(창크기 말고)
	XE::VEC2 m_vScroll;
	XE::VEC2 m_vScrollLast;
	XE::VEC2 m_vMouse;
	float m_Alpha;
	CTimer m_Timer;
public:
	XWndScrollView( float x, float y, float w, float h ) 
		: XWnd( NULL, x, y, w, h )	{
			m_bLButtonUp = FALSE;
			m_bLButtonDown = FALSE;
			m_bScroll = FALSE;
			m_Alpha = 0;
	}
	virtual ~XWndScrollView() {}
	//
	GET_ACCESSOR( XE::VEC2&, vAdjust );
//#ifdef _XVIEWPORT
//	virtual XE::VEC2 GetPosFinal( void ) { 
//		return /*XE::VEC2(0) +*/ m_vAdjust;
//	}
//#endif 
	void Clear( void ) {
		m_vAdjust.Set( 0 );
	}
	void SetViewSize( XE::VEC2 vSize ) {
		m_vTotalSize = vSize;
	}
	void Scroll( void );
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnNCLButtonUp( float lx, float ly );
	virtual void OnNCMouseMove( float lx, float ly );
};

// 리스트 컨트롤
class XWndList : public XWndScrollView
{
public:
	enum xtType { xTYPE_NONE,
					xTYPE_LIST,			// 세로 나열형
					xTYPE_THUMB,		// 썸네일 형
	};
					
private:
	xtType m_Type;
	XWnd *m_pClickedWnd;
	XSurface *m_psfcBg;
	XE::VEC2 m_posBg;
	float m_X, m_Y;
	XE::VEC2 m_vSpace;		// 리스트간 간격
	XE::VEC2 m_vDown;
	void Init() {
		m_Type = xTYPE_NONE;
		m_Y = m_X = 0;
		m_pClickedWnd = NULL;
		m_psfcBg = NULL;
	}
	void Destroy();
protected:
	XList<XWnd*> m_listItem;
public:
//	XWndList( float x, float y, float w, float h, xtType type=xTYPE_LIST );
	XWndList( LPCTSTR szBg, const XE::VEC2& vPos, const XE::VEC2& vLocalxy, const XE::VEC2& vSize, xtType type=xTYPE_LIST );
	virtual ~XWndList() { Destroy(); }
	//
	GET_ACCESSOR( XList<XWnd*>&, listItem );
	GET_SET_ACCESSOR( const XE::VEC2&, vSpace );
	XWnd* AddItem( ID idWnd, XWnd *pWnd );
	// 뷰포트의 최상위 뷰포트 객체는 언제나 자신의 월드좌표를 0으로 돌려준다.
	virtual XE::VEC2 GetPosFinal( void ) { 
		return /*XE::VEC2(0) +*/ GetvAdjust();
	}
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
	void DestroyAllItem( void );
	//
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
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
	void OnLButtonDown( float lx, float ly );
	void OnMouseMove( float lx, float ly );
	void OnLButtonUp( float lx, float ly );
};

class XWndTab : public XWnd
{
protected:
	struct TAB {
		ID idTab;
		_tstring strImg;
		_tstring strLabel;
		float x, y;
		float w, h;
		XSurface *psfcImg;
		TAB() {
			idTab = 0;
			x = y = 0;
			w = h = 0;
			psfcImg = NULL;
		}
	};
private:
	XList<TAB*> m_listTab;
	TAB *m_pSelectedTab;
	XBaseFontObj *m_pFontObj;
	void Init() {
		m_pSelectedTab = NULL;
		m_pFontObj = NULL;
	}
	void Destroy();
public:
	XWndTab( float x, float y, float w, float h, LPCTSTR szFont );
	virtual ~XWndTab() { Destroy(); }
	//
	GET_ACCESSOR( TAB*, pSelectedTab );
	TAB* GetTab( ID idTab );
	void AddTab( ID idTab, LPCTSTR szLabel, LPCTSTR szImg, float x, float y, float w, float h );
	//
	virtual void DrawTabLabel( TAB *pTab );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnClickedTab( TAB *pTab ) {}
};

class XWndTab2 : public XWnd
{
protected:
	struct TAB {
		ID idTab;
		_tstring strImgOn;
		_tstring strImgOff;
		_tstring strLabel;
		float x, y;
		float w, h;
		XSurface *psfcImgOn;		// 탭부분 이미지
		XSurface *psfcImgOff;		// 탭부분 이미지
		TAB() {
			idTab = 0;
			x = y = 0;
			w = h = 0;
			psfcImgOn = NULL;
			psfcImgOff = NULL;
		}
	};
private:
	XList<TAB*> m_listTab;
	TAB *m_pSelectedTab;
	XBaseFontObj *m_pFontObj;
	XSurface *m_psfcBg;
	void Init() {
		m_pSelectedTab = NULL;
		m_pFontObj = NULL;
		m_psfcBg = NULL;
	}
	void Destroy();
public:
	XWndTab2( float x, float y, LPCTSTR szImg, LPCTSTR szFont );
	XWndTab2( float x, float y, float w, float h, LPCTSTR szFont );
	virtual ~XWndTab2() { Destroy(); }
	//
	GET_ACCESSOR( TAB*, pSelectedTab );
	TAB* GetTab( ID idTab );
	void AddTab( ID idTab, LPCTSTR szLabel, LPCTSTR szImgOn, LPCTSTR szImgOff, float x, float y, float w, float h );
	//
	virtual void DrawTabLabel( TAB *pTab );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnClickedTab( TAB *pTab ) {}
	virtual void OnAddWnd( void ) {
		CallEvent( "OnEventAddWnd" );
	}
};

class XWndFillRect : public XWnd
{
	XCOLOR m_Color;
public:
	XWndFillRect( float x, float y, float w, float h, XCOLOR col ) 
		: XWnd( NULL, x, y, w, h )	{
			m_Color = col;
	}
	virtual ~XWndFillRect() {}
	void Draw( void ) {
		XE::VEC2 vPos = GetPosFinal();
		XE::VEC2 vSize = GetSizeFinal();
		GRAPHICS->FillRectSize( vPos, vSize, m_Color );
	}
};
//////////////////////////////////////////////////////////////////////////
class XWndPageForSlide : public XWnd
{
	int m_idxPage;			// 페이지 인덱스
	void Init() {
		m_idxPage = 0;
	}
	void Destroy() {}
public:
	XWndPageForSlide( int idxPage, const XE::VEC2& vPos, const XE::VEC2& vSize ) 
	: XWnd( NULL, vPos.x, vPos.y, vSize.w, vSize.h ) { 
		Init(); 
		m_idxPage = idxPage;
	}
	virtual ~XWndPageForSlide() { Destroy(); }
	//
	GET_ACCESSOR( int, idxPage );
};

//////////////////////////////////////////////////////////////////////////
class XWndPageSlideWithXML : public XWnd
{
	XLayout *m_pLayout;
	TiXmlElement *m_elemSlide;
	XWndPageForSlide *m_pLeft;
	XWndPageForSlide *m_pCurr;
	XWndPageForSlide *m_pRight;
	XList<string> m_listPageKey;		// 페이지 문자열
	XE::xDir m_AutoScrollDir;			// 자동스크롤 방향
	XE::VEC2 m_vStartScroll;	// 자동스크롤 시작위치
	XE::VEC2 m_vPrev;			
	BOOL m_bTouchDown;		// 
	XInterpolationObjDeAccel m_itpScroll;
	XSurface *m_sfcPointOn;
	XSurface *m_sfcPointOff;
	XE::VEC2 m_posPagePoint;		// 페이지포인트 시작위치
	float m_distPagePoint;		// 페이지포인트 간격
	void Init() {
		m_pLayout = NULL;
		m_elemSlide = NULL;
		m_pLeft = m_pCurr = m_pRight = NULL;
		m_AutoScrollDir = XE::xDIR_NONE;
		m_bTouchDown = FALSE;
		m_sfcPointOn = m_sfcPointOff = NULL;
		m_distPagePoint = 25.f;
	}
	void Destroy() {
		SAFE_RELEASE2( IMAGE_MNG, m_sfcPointOn );
		SAFE_RELEASE2( IMAGE_MNG, m_sfcPointOff );
	}
public:
	/**
	szImgPoint: 현재 페이지의 위치를 보여주는 포인트이미지 파일명. 파일명.png의 형식
					으로 넣으면 파일명_on/off.png형태로 자동으로 바꿔준다.
	*/
	XWndPageSlideWithXML( const XE::VEC2& vPos, 
							const XE::VEC2& vSize, 
							XLayout* pLayout, 
							TiXmlElement *elemSlide,
							LPCTSTR szImgPoint=NULL );
	virtual ~XWndPageSlideWithXML() { Destroy(); }
	//
	int GetNextPage( int idxBase );
	int GetPrevPage( int idxBase );
	XWndPageForSlide* GetCurrPage( void ) {
		return m_pCurr;
	}
	XWndPageForSlide* GetRightPage( void ) {
		return m_pRight;
	}
	XWndPageForSlide* GetLeftPage( void ) {
		return m_pLeft;
	}

	int AddPageKey( const char *cKeyPage );
	void SetCurrPage( int idxPage );
	XWndPageForSlide* CreatePage( int idxPage );
	void DoAutoScroll( XE::xDir dir, const XE::VEC2& vStart );
	//
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual int Process( float dt );
	virtual void Draw( void );
};

//////////////////////////////////////////////////////////////////////////
class XWndProgressBar : public XWnd, public XProgressBar
{
	float m_Lerp;
	void Init() {
		m_Lerp = 1.0f;
	}
	void Destroy() {}
public:
	XWndProgressBar( float x, float y, LPCTSTR szImg, LPCTSTR szImgBg );
	XWndProgressBar( float x, float y, float w, float h, XCOLOR col=XCOLOR_GREEN );
	virtual ~XWndProgressBar() { Destroy(); }
	//
	GET_SET_ACCESSOR( float, Lerp );
	//
	void Draw( void );
};

//////////////////////////////////////////////////////////////////////////
class XWndPopup : public XWndView
{
	void Init() {
		XWnd::SetwtWnd( XE::WT_POPUP );
	}
	void Destroy() {}
public:
	XWndPopup( const XE::VEC2& vPos, const XE::VEC2& vSize, LPCTSTR szFrame ) 
	: XWndView( vPos.x, vPos.y, vSize.w, vSize.h, szFrame ) { 
		Init(); 
	}
	virtual ~XWndPopup() { Destroy(); }
	//
	int OnOk() {
		SetbDestroy( TRUE );
		return 1;
	}
	int OnOk( XWnd*, DWORD, DWORD ) {
		SetbDestroy( TRUE );
		return 1;
	}
	int OnCancel( XWnd*, DWORD, DWORD ) {
		SetbDestroy( TRUE );
		return 1;
	}
	BOOL OnKeyUp( int keyCode ) {
		if( keyCode == XE::KEY_BACK ) {
			OnCancel( NULL, 0, 0 );
			return TRUE;
		}
		return FALSE;
	}
};
