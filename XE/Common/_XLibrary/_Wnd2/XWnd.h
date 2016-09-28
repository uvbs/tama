#pragma once 

#include "etc/XSurfaceDef.h"
#include "etc/xGraphics.h"
#include "etc/InputMng.h"
#include "etc/Timer.h"
//#include "XSoundMng.h"
#include "XFont.h"
#include "XFontMng.h"
#include "XLua.h"
#include "../XInterpolationObj.h"
//#include "XFramework/client/XLayout.h"
//#include "XImageMng.h"
//#include "../XDrawGraph.h"
#include "XBaseDelegate.h"
#include "XWndH.h"

class XWndButton;
class XDelegateWnd;
#pragma warning( disable : 4250 )	//  warning C4250: 'XWndButtonHexNumber' : 우위에 따라 'XWndButton::XWndButton::Process'을(를) 상속합니다.

#if defined(_DEV) && defined(WIN32)
//#define _XWIN_DEBUG			1		// XWnd 디버깅 모드
#endif

#define XWM_STOP_ROLLING			1
#define XWM_DESTROY				2
#define XWM_OK						3		// 다이얼로그에서 OK눌렀을때
#define XWM_SLIDING					4		// 슬라이더 컨트롤을 드래그할때 발생
#define XWM_CLICKED					5		// 버튼 클릭
#define XWM_TOUCH_UP			5			// XWM_CLICKED랑 같은거
#define XWM_SELECT_MENU			6		// 메뉴 선택(이거쓰지말고 elem쓸것
#define XWM_SELECT_ELEM			6		// 선택
#define XWM_DESELECT_MENU		7		// 메뉴 선택 해제
#define XWM_DESELECT_ELEM		7		// 선택 해제
#define XWM_BOUND_BOTTOM		8		// 스크롤뷰에서 아래쪽 한계선까지 스크롤이 되면 발생한다.
#define XWM_BOUND_TOP			9		// 스크롤뷰에서 위쪽 한계선까지 스크롤이 되면 발생한다.
#define XWM_CREATE					10		// 버튼,다이얼로그 등이 생성될때 발생한다.
//#define XWM_DESTROY_AFTER		11		// 어떤객체가 파괴되고 난후 발생될 이벤트
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
#define XWM_SCROLL				21		// 스크롤뷰의 스크롤이 일어날때 이벤트
#define XWM_TOUCH_DOWN			22		// 터치다운
#define XWM_DRAG				23		// drag & drop
#define XWM_DROP				24		// drag & drop
/**
 @brief 윈도우의 파일로딩이 모두 끝나면 호출된다.
 윈도우를 리소스를 실시간으로 읽기때문에 파일로딩이 모두 끝났다는 시점은 Add=>Update()가 호출된 직후로 한다.
*/
#define XWM_FINISH_LOADED		25	//
#define XWM_TOOLTIP					26
// scene
#define XWM_FINISH_SLIDE			200		// XSceneSlide에서 슬라이딩이 끝나면 호출됨
// help
#define XWM_HELP_CLICKED			300		// 버튼을 누르면 헬프객체로 이벤트를 던져준다.
#define XWM_END_HELP_SEQ			301		// 헬프 시퀀스가 끝나면 호출
// Quest
#define XWM_QUEST_CLICKED		400		// 버튼이 눌리면 퀘스트매니저로 이벤트가 전달된다.
// DownloadTask
#define XWM_ALL_COMPLETE			500		// 모든 다운로드가 끝남
#define XWM_EACH_COMPLETE			501		// 각각의 다운로드가 끝남
#define XWM_ERROR_DOWNLOAD			502		
// etc
#define XWM_FINISH_NUMBER_COUNTER		1000	// XWnnTextNumberCount의 카운터가 끝나면 호출.

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
	~XWND_RES_FRAME() { Destroy(); }
	void Destroy();
};

//{
class XWndButton;
class XWndButtonString;
class XWndImage;
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
class XEToolTip;
struct XWND_MESSAGE_MAP
{
	int (XWnd::*pHandler)( XWnd*, DWORD dwParam1, DWORD dwParam2 );
	DWORD msg;
	DWORD param;
	XWnd *pOwner;
	XWND_MESSAGE_MAP() {
		msg = 0;
		param = 0;
		pOwner = NULL;
		pHandler = NULL;
	}
};

class XWndButton;

/****************************************************************
* @brief 
* @author xuzhu
* @date	2014/12/23 15:29
*****************************************************************/
//
class XWnd
{
	struct xCallback {
		ID m_idEvent = 0;
		std::function<void(XWnd*)> m_funcCallback;
		xCallback() {}
		xCallback( ID idEvent, std::function<void( XWnd* )> funcCallback ) 
		: m_idEvent(idEvent), m_funcCallback(funcCallback) {	}
	};
	XList4<xCallback> m_listCallback;
public:
	static BOOL s_bDebugMode;
	enum xtAlert { xALERT_NONE, 
					xOK			= 0x80000001, 
					xOKCANCEL	= 0x80000002, 
					xYESNO		= 0x80000003,
					xCANCEL		= 0x80000004, 
					xCLOSE		= xCANCEL, 
					xNO			= 0x80000005,
					xYES		= 0x80000006, 
				};
	enum xtAppear { xAP_NONE, 
							xAP_ENTER_INIT,
							xAP_ENTER_ING,
							xAP_ENTER_END,
							xAP_LEAVE_INIT,
							xAP_LEAVE_ING,
							xAP_LEAVE_END,
	};
	enum xtFlags {
		xFG_NONE = 0,
		xFG_DRAG = 0x01,	///< 드래그&드롭에서 드래그가 가능한 윈도우인가.
		xFG_DROP = 0x02,	///< 드래그&드롭에서 드롭이 가능한 윈도우인가.
		xFG_NC_EVENT = 0x04,	///< 윈도우 영영밖의 터치를 감지하게 하기위한 플래그
	};
	static XWnd *s_pFocusWindow;
	static ID s_idLDownWnd;		// LDown한 윈도우의 아이디. LUp이 이뤄지려면 LDown이 되어야 한다.
#ifdef WIN32
	struct xWinDepth {
		XWnd* m_pWnd = nullptr;
		std::string m_str;
		int m_Depth = 0;
		xWinDepth( XWnd* pWin, int depth ) 
			: m_pWnd(pWin), m_Depth(depth) {}
	};
	static XVector<xWinDepth>& sGetStringByMouseOverWins();
	static XWnd *s_pMouseOver;		// 현재 마우스가 올라가있는 가장 위 윈도우
	static int s_depthMouseMove;	// 마우스오버된 윈도우의 뎁스
	static bool s_bDrawOutline;		// 디버깅 모드. 모든 윈도우들의 외곽선을 그린다.
	static bool s_bDrawMouseOverWins;		// 디버깅 모드. 현재 마우스위치에 오버된 모든윈도우들의 리스트를 출력한다.
	static XVector<xWinDepth> s_aryMouseOver;		// 현재 마우스가 올라가있는 모든 윈도우의 포인터를 받는다.
	static XWnd* sFindWinMouseOver( ID idWnd );
#endif // _DEBUG
	void SetFocus( XWnd *pWindow ) { s_pFocusWindow = pWindow; }
	XWnd* GetFocus() { return s_pFocusWindow; }
	static XBaseFontDat *s_pFontDat;
//	static XWnd *s_pDropWnd;			// drag & drop 윈도우
//	static void sAdjustDynamicPos( XWnd *pWnd );
	void AddListAllowClick( std::string strKey ) {
		XBREAK( strKey.empty() == true );
		s_listAllowClick.Add( strKey );
	}
	XList<std::string>& GetAllowList() {
		return s_listAllowClick;
	}
	static XWnd* sFindCaptureWnd( ID snWnd );
	static void sSetDragWnd( XWnd *pWnd ) {
		s_pDragWnd = pWnd;
	}
	static XWnd* sGetDragWnd() {
		return s_pDragWnd;
	}
private:
	static XList<std::string> s_listAllowClick;		// 이게 세팅되어 있으면 이 윈도우들외에 다른 윈도우는 입력을 막아야 한다.
	static BOOL IsAllowClickWnd( XWnd *pCompare );
	static XList<XWnd*> s_listCapture;		// 캡쳐한 윈도우 리스트
	static XList<XWnd*> s_listCaptureDel;	
	static XWnd *s_pDragWnd;			// drag & drop 윈도우
	static XWnd *s_pLastLUp;		// 가장 마지막으로 LButtonUp이 호출된윈도우
private:
	void Init() {
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
//		m_pModalDlg = NULL;
		m_Destroy = 0;
		m_prefToolTip = NULL;
		m_bToolTip = FALSE;
		m_pwndLastLDown = NULL;
		m_pwndLastLUp = NULL;
		m_bModal = FALSE;
//		m_pCapture = NULL;
		m_bProcess = TRUE;
//		m_bEnter = FALSE;
		m_bAnimationEnterLeave = FALSE;
		m_stateAppear = xAP_NONE;
		m_secAppear = 0;
		m_Priority = 0;
		m_pAfterDestroyEvent = NULL;
#ifdef WIN32
		m_nDepth = 0;
#endif // WIN32
		m_idGroup = 0;
		m_bTouchable = FALSE;
		m_bitFlags = 0;
		m_vTouchDown.Set(-1.f);		// TouchUp시에 -1로 초기화 해야 한다.
		m_bUpdate = FALSE;
		m_pDelegate = nullptr;
	}
	void Destroy();
	// protected로 만들지 말것 반드시 GetScaleLocal/Final을 구분해서 쓸것
	XE::xtWnd m_wtWnd;	// 윈도우 타입.
	XE::VEC2 m_vPos;		// local
	XE::VEC2 m_vScale, _m_vstackScale;	// local
	float m_dRotateZ;
	int m_Destroy;		// self destroy. 0:live 1:죽고있는중 2:이번턴에죽음
	float m_fAlpha;		
	XEToolTip *m_prefToolTip;
	BOOL m_bToolTip;
	CTimer m_timerPush;		// 누른시간
	XWnd *m_pwndLastLDown;		// LButtonDown한 윈도우
	XWnd *m_pwndLastLUp;		// 마지막으로 LButtonUp한 윈도우
//	XWnd *m_pCapture;		// 마우스이벤트 캡쳐
	CTimer m_timerAppear;
	CTimer m_timerAutoUpdate;
//	BOOL m_bEnter;
	xtAppear m_stateAppear;		// 현재 등장상태
	BOOL m_bAnimationEnterLeave;	// 등장이나 퇴장시 애니메이션을 할건지
	float m_secAppear;
	XWnd *m_pAfterDestroyEvent;	// this가 파괴되고 난 직후에 이벤트처리를 해줘야할 윈도우
	std::string m_strIdentifier;		// 식별 문자열
	BOOL m_bTouchable;			// 터치이벤트를 받을 수 있는가 못받는가(못받으면 지나친다)
	BIT m_bitFlags;				///< 각종 플래그
	XE::VEC2 m_vTouchDown;		///< 각 윈도우내에서 LButtonDown시 좌표를 받아둔다. 
	BOOL m_bUpdate;				///< 이번 프로세스에서 Update를 호출한다.Invalidate()와 같은 역할. 
	XDelegateWnd *m_pDelegate;	///< 다용도 델리게이트
	XE::xtBlendFunc m_blendFunc = XE::xBF_MULTIPLY;
	XE::VEC2 m_vAdjDraw;				///< draw시에 추가로 보정할 좌표. 이것은 윈도우자체의 좌표를 변화시키지 않으면서 그리는 위치만 살짝 조정한다)
	XE::VEC2 m_vAdjPos;				///< m_vPos에 추가로 보정될 좌표. 윈도우좌상귀의 최종좌표는 m_vPos + m_vAdjust가 된다.
	XE::xAlign m_Align = XE::xALIGN_NONE;
	XVector<xnWnd::xClickEvent> m_aryClickEvent;
	xnWnd::xTooltip m_datTooltip;		// 툴팁 정보
#ifdef WIN32
	int m_nDepth;				// 윈도우 트리에서의 깊이
#endif
	//
	void PrivateDestroyChild();
	GET_SET_ACCESSOR( XWnd*, pFirstPush );
#ifdef WIN32
	GET_SET_ACCESSOR( int, nDepth );
#endif // WIN32
protected:
	std::list<XWND_MESSAGE_MAP> m_listMessageMap;
 	XE::VEC2 m_vSize;		// local fuck 잠시 프로텍트로 옮겨옴
	ID	m_id;
	XWnd *m_pParent;
	XList4<XWnd*> m_listItems;
	XWnd *m_pFirstPush;		// 최초클릭됐을때 컨트롤의 포인터( m_pwndLastLDown과 중복되므로 통합시킬것 );
	BOOL m_bShow;			// show/hide
	BOOL m_bEnable;
	BOOL m_bActive;			// 마우스클릭등의 작동을 멈추게 한다(외부사용자용)
	bool _m_bActiveInternal = true;	// 시스템 내부에서만 쓰는 bActive. m_bActive가 true여도 이것이 false면 false로 인식한다. m_bActive가 false일때 이것이 true면 m_bActive가 우선된다.
	BOOL m_bModal;			// 모달형태인지.
	BOOL m_bProcess;			// process()를 임의로 멈추게 할때
//	XWnd* m_pModalDlg;		// 모달다이얼로그가 떠있는지
	int m_Priority;				// 찍기 우선순위. 같이 높을수록 위에 찍힌다.
	_tstring m_strDebug;			// 디버깅용
	ID m_idGroup;			// 그룹아이디. this가 속한 그룹의 아이디. 보통 라디오 버튼에 사용.
	int m_Debug = 0;
	//
	SET_ACCESSOR( XE::xtWnd, wtWnd );
public:
	//////////////////////////////////////////////////////////////////////////
	XWnd() { Init(); }
	template<typename T>
	XWnd( T x, T y, T w, T h ) {
		Init();
#ifdef _DEBUG
//		XBREAK( w == 0 || h == 0 );		// 이제 0사이즈 윈도우도 생길수 있다.
#endif
		m_vPos.Set( x, y ); 
		m_vSize.Set( w, h );
		SetScaleLocal( 1.0f );
		if( m_vSize.IsZero() == FALSE )
			SetbTouchable( TRUE );
	}
	template<typename T>
	XWnd( T x, T y ) {
		Init();
		m_vPos.Set( x, y );
		SetScaleLocal( 1.0f );
	}
	XWnd( const XE::VEC2& vPos ) {
		Init();
		m_vPos = vPos;
		SetScaleLocal( 1.f );
	}
	XWnd( const XE::VEC2& vPos, const XE::VEC2& vSize ) {
		Init();
		m_vPos = vPos;
		m_vSize = vSize;
		SetScaleLocal( 1.f );
	}
	XWnd( XE::xtWnd typeWin ) {
		Init();
		m_wtWnd = typeWin;
		SetScaleLocal( 1.f );
	}
	XWnd( XLayout *pLayout, const char *cNodeName );
	virtual ~XWnd() { Destroy(); }

	GET_ACCESSOR( XE::xtWnd, wtWnd );
	GET_ACCESSOR( XWnd*, pParent );
	// 비공식API이므로 엔진개발자 외엔 사용하지 말것.
	void _SetpParent( XWnd* pParent ) {
		m_pParent = pParent;
	}
	ID GetID() const { return m_id; }
	ID getid() const { return m_id; }
	void SetID( ID id ) { m_id = id; }
	GET_SET_ACCESSOR( BOOL, bShow );
	inline void SetbShow( bool bFlag ) {
		SetbShow( (bFlag)? TRUE : FALSE );
	}
//	GET_SET_ACCESSOR( BOOL, bEnable );
	SET_ACCESSOR( BOOL, bEnable );
	GET_SET_ACCESSOR( ID, idGroup );
	GET_SET_ACCESSOR( BOOL, bTouchable );
	inline void SetbTouchable( bool bTouchable ) {
		m_bTouchable = (bTouchable)? TRUE : FALSE;
	}
	GET_SET_ACCESSOR( BOOL, bUpdate );
	GET_SET_ACCESSOR( XDelegateWnd*, pDelegate );
	GET_ACCESSOR_CONST( XE::xAlign, Align );
	GET_SET_ACCESSOR_CONST( int, Debug );
	GET_SET_ACCESSOR_CONST( const XE::VEC2&, vAdjPos );
	GET_SET_ACCESSOR_CONST( const xnWnd::xTooltip&, datTooltip );
// 	GET_SET_ACCESSOR_CONST( const _tstring&, xmlClick );
// 	GET_SET_ACCESSOR_CONST( const std::string&, strNodeClick );
// 	GET_SET_ACCESSOR_CONST( const XE::VEC2&, vPopupByLayout );
// 	GET_SET_ACCESSOR_CONST( const XE::xAlign&, AlignClick );
	void AddClickEvent( const xnWnd::xClickEvent& clickEvent ) {
		m_aryClickEvent.Add( clickEvent );
	}
	const xnWnd::xClickEvent& FindClickEvent( const std::string& strKey ) const;
	virtual void SetAlign( XE::xAlign align ) {
		m_Align = align;
	}
	void SetbUpdate( bool flag ) {
		m_bUpdate = (flag)? TRUE : FALSE;
	}
	BOOL GetbEnable() const {
		if( m_bEnable == FALSE )		// 부모가 disable이면 자식들도 모두 disable이다. enable은 그렇지 않음.
			return FALSE;
		if( m_pParent )
			return m_pParent->GetbEnable();
		else
			return m_bEnable;
	}
//	GET_SET_ACCESSOR( BOOL, bDestroy );
	// 죽고있는상태/죽을상태 모두를 포함.
	BOOL GetDestroy() const {
		return (m_Destroy) ? TRUE : FALSE;
	}
	// 파괴중인 윈도우인가
	bool IsDestroy() const {
		return (m_Destroy != 0);
	}
	// 죽고있는 상태인가
	BOOL GetDestroying() const {
		return (m_Destroy == 1) ? TRUE : FALSE;
	}
	// 이번턴에 죽을넘인가
	BOOL GetDestroyFinal() const {
		return (m_Destroy == 2) ? TRUE : FALSE;
	}
	void SetbDestroy( BOOL bFlag ) {
		if( m_bAnimationEnterLeave ) {
			m_Destroy = 1;		// 죽고있는 상태로
			m_stateAppear = xAP_LEAVE_INIT;		// 사라지는 애니메이션 시작
		} else
			m_Destroy = (bFlag) ? 2 : 0;
	}
	void SetbDestroy( bool bFlag ) {
		SetbDestroy( (bFlag)? TRUE : FALSE );
	}
	void SetDestroyFinal() {
		m_Destroy = 2;
	}
	SET_ACCESSOR( BOOL, bActive );
	void SetbActive( bool bFlag ) {
		m_bActive = (bFlag)? TRUE : FALSE;
	}
	// 부모중에 하나라도 deActive상태면 자식도 모두 deActive 상태가 된다.
	BOOL GetbActive() {
		if( m_bActive == FALSE )
			return FALSE;
		if( m_pParent )
			return m_pParent->GetbActive();
		else
			return m_bActive;
	}
protected:
	bool GetbActiveInternal() {
		BOOL bActive = GetbActive();
		if( bActive && _m_bActiveInternal == false )
			return _m_bActiveInternal;		// 이경우는 내부용이 우선
		else if( bActive == FALSE && _m_bActiveInternal )
			return m_bActive != FALSE;				// 이경우는 사용자용이 우선
		return _m_bActiveInternal;
	}
	void SetbActiveInternal( bool bFlag ) {
		_m_bActiveInternal = bFlag;
	}
public:
	// 부모나 혹은 자신이 모달속성인가
	BOOL GetbModalAttr() {
		if( m_bModal )
			return TRUE;
		if( m_pParent )
			return m_pParent->GetbModalAttr();
		else
			return m_bModal;
	}
	XE::VEC2 GetAdjust();
//	GET_ACCESSOR( XWnd*, pModalDlg );
	GET_SET_ACCESSOR( XEToolTip*, prefToolTip );
	GET_SET_ACCESSOR( BOOL, bToolTip );
	GET_ACCESSOR( CTimer&, timerPush );
	GET_ACCESSOR( XWnd*, pwndLastLDown );
	GET_ACCESSOR( XWnd*, pwndLastLUp );
	GET_SET_ACCESSOR( BOOL, bModal );
	GET_SET_ACCESSOR( BOOL, bProcess );
	GET_ACCESSOR( BOOL, bAnimationEnterLeave );
	GET_SET_ACCESSOR( int, Priority );
	GET_ACCESSOR( const _tstring&, strDebug );
	GET_ACCESSOR_CONST( const XList4<XWnd*>&, listItems );
	// Enter/Leave Animation
	// 윈도우 등장이나 퇴장시 애니메이션을 할건지
	void SetbAnimationEnterLeave( BOOL bFlag, float sec=0.5f ) {
		XBREAK( bFlag && sec == 0 );
		m_bAnimationEnterLeave = bFlag;
		m_secAppear = sec;
	} 
	// 현재 등장 애니메이션 중인가.
	virtual bool IsAnimation() {
		return IsEnterLeaveAnimation() != FALSE;
	}
	BOOL IsEnterAnimation() {
		return (m_stateAppear == xAP_ENTER_ING || 
				m_stateAppear == xAP_ENTER_INIT );
	}
	// 현재 퇴장 애니메이션 중인가
	BOOL IsLeaveAnimation() {
		return (m_stateAppear == xAP_LEAVE_ING || 
			m_stateAppear == xAP_LEAVE_INIT );	// END상태는 끝나고도 계속 유지되므로 넣지말것.
	}
	// 현재 등장 혹은 퇴장 애니메이션 중인가
	BOOL IsEnterLeaveAnimation() {
		return IsEnterAnimation() || IsLeaveAnimation();
	}

	virtual BOOL ProcessEnterLeaveAnimation( float dt );
	virtual void ProcessAnimationLerp( BOOL bEnter, float timeLerp, float aniLerp );
	virtual int OnProcessEnterLeaveAnimation( BOOL bEnter, float dt );
	virtual BOOL OnInitEnterLeaveAnimation( BOOL bEnter ) { return TRUE; }
	virtual void OnFinishEnterLeave( BOOL bEnter ) {}
	//////////////////////////////////////////////////////////////////////////
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
	GET_ACCESSOR_CONST( XWnd*, pAfterDestroyEvent );
public:
	SET_ACCESSOR( XWnd*, pAfterDestroyEvent );
//	GET_ACCESSOR( BOOL, bEnter );
	GET_ACCESSOR( std::string&, strIdentifier );
	void SetstrIdentifier( const char *cIdentifier ) {
		m_strIdentifier = cIdentifier;
	}
	void SetstrIdentifierf( const char *cIdentifierFormat, ... );
	void SetstrIdentifier( const std::string& strIdentifier ) {
		SetstrIdentifier( strIdentifier.c_str() );
	}
	const char* GetszIdentifier() {
		return m_strIdentifier.c_str();
	}
	// 터치가 윈도우의 영역을 벗어나더라도 이벤트를 받을 수 있게 한다.
	void SetCapture();
	// 캡쳐 속성을 해제한다.
	void ReleaseCapture();
	//
private:
public:
	void DestroyChildAll();
	// 접미사 규칙: Final(m_pWndMng로부터 상속누적된 최종값. Scale이나 Rotate도 모두 반영된 트랜스폼된 값이다), Local(상속되지 않은 this의 지역값)
	// transform x, y
	const XE::VEC2 GetPosLocal() const ;
	void SetPosLocal( const XE::VEC2& vPos ) { 
		m_vPos = vPos; 
	}
	void SetY( float y ) {
		m_vPos.y = y;
	}
	void SetX( float x ) {
		m_vPos.x = x;
	}
	template<typename T>
	XE::VEC2 AddPosLocal( T x, T y ) {
		XE::VEC2 v = GetPosLocal();
		v.x += x;
		v.y += y;
		SetPosLocal( v );
		return v;
	}
	XE::VEC2 AddPosLocal( const XE::VEC2& vAdd ) {
		return AddPosLocal( vAdd.x, vAdd.y );
	}
	template<typename T>
	void SetPosLocal( T x, T y ) { 
		m_vPos.Set( x, y ); 
	}
	// 정체성이 모호한 함수.
	// 값을 곱하는게 있어서 레퍼런스로 못넘기는데 만약 이게 속도문제가 된다면 WndMng.CalcTransform()정도를 만들어서 Final을 한번에 계산해놓고 쓰기만 해야할듯
	virtual XE::VEC2 GetPosFinal() const;
	// 뷰 좌상귀의 화면좌표를 얻는다. 스크롤뷰의 경우 스크롤이 아래로 내려가있는상태라면(m_vAdjust < 0) 뷰는 창 위쪽을 벗어나서 위쪽에 있을것이므로 그 곳의 화면좌표를 얻는다./
	virtual XE::VEC2 GetPosScreen();
	// 창의 좌상귀의 화면좌표를 얻는다. 스크롤뷰의 경우 스크롤 상태(m_vAdjust)와 관계없이 뚫린곳(창)의 좌상귀 좌표를 리턴한다.
	virtual XE::VEC2 GetPosWindow();
	/// pBase로부터의 this,의 좌표를 구한다.
	XE::VEC2 GetPosFromBase( XWnd *pBase );
	virtual XE::VEC2 GetPosAdjust() {
		return XE::VEC2(0);
	}
	const XE::VEC2 GetvAdjDrawLocal() const;
	const XE::VEC2 GetvAdjDrawFinal() const;
	inline void SetvAdjDrawLocal( const XE::VEC2& vAdjDraw ) {
		m_vAdjDraw = vAdjDraw;
	}
	// transform width, height
	inline float GetWidthLocal() { 
		return m_vSize.x * GetScaleLocal().x; 
	}
	inline float GetHeightLocal() { 
		return m_vSize.y * GetScaleLocal().y; 
	}
	inline XE::VEC2 GetSizeLocal() { 
		return m_vSize * GetScaleLocal(); 
	}
	inline XE::VEC2 GetSizeLocalNoTrans() {
		return m_vSize;
	}
	inline XE::VEC2 GetSizeFinal() { 
		return m_vSize * GetScaleFinal(); 
	}
	float GetWidthFinalValid();
	float GetHeightFinalValid();
// 	virtual XE::VEC2 GetSizeWindow() {
// 		return m_vSize * GetScaleFinal();
// 	}
	void PushScale() { _m_vstackScale = m_vScale; }
	void PopScale() { m_vScale = _m_vstackScale; }
//	const XE::VEC2& GetSizeOrig() { return m_vSize; }		// 트랜스폼하지 않은 원래 크기...로 쓰려고 했으나 부모로부터 스케일링을 상속받고 하는등 복잡하기때문에 이 값을 직접 쓸일은 없을듯하다
	template<typename T>
	void SetSizeLocal( T w, T h ) { 
		SetSizeLocal( XE::VEC2( w, h ) ); 
	}
	virtual void SetAutoSize();
	/**
	윈도우의 크기를 강제로 계산해서 돌려준다. 일반적인 경우엔 GetSizeFinal()이 동작하지만
	폰트가 사용되는 XWndTextString류의 윈도우는 폰트레이아웃 구하는 시간이 오래걸려 사이즈를 구해놓지 않으므로
	GetSizeFinal()만으론 사이즈를 구할수 없다. 그러므로 GetSizeForceFinal()을 상속받은
	XWndTextString은 자신의 폰트레이아웃 크기를 반드시 구해서 돌려줘야 한다.
	*/
// 	virtual XE::VEC2 GetSizeForceFinal() {
// 		return GetSizeFinal();
// 	}
#ifdef _DEBUG
	BOOL IsSizeZero() {
		if( m_vSize.IsZero() )
			return TRUE;
		if( m_pParent )
			return m_pParent->IsSizeZero();
		return FALSE;
	}
	void SetSizeLocal( const XE::VEC2& vSize );
#else
	void SetSizeLocal( const XE::VEC2& vSize );
#endif
	void SetHeight( float h );
	void SetWidth( float w );
	bool IsValidSize() const {
		if( this == nullptr )
			return false;
		return !m_vSize.IsZero() && !m_vSize.IsMinus();
	}
	// transform scale
	const XE::VEC2& GetScaleLocal() const { 
		return m_vScale; 
	}
	XE::VEC2 GetScaleFinal() const { 
		if( m_pParent )
			return m_pParent->GetScaleFinal() * m_vScale;
		else
		return m_vScale;
	}	// 상속된 최종 스케일값 parent.scale * localScale
	void SetScaleLocal( const XE::VEC2& vScale ) { 
		SetScaleLocal( vScale.x, vScale.y );	
	}
	void SetScaleLocal( float scalexy ) { 
		SetScaleLocal( scalexy, scalexy ); 
	}
	virtual void SetScaleLocal( float sx, float sy );
	// transform alpha
	float GetAlphaLocal() const { return m_fAlpha; }	// 로컬 알파값(원본알파값)
	void SetAlphaLocal( float alpha ) { m_fAlpha = alpha; }	// 로컬 알파값(원본알파값)
	float GetAlphaFinal() {	// 최종 화면에 그려질 알파값
		if( m_pParent ) {
			return m_pParent->GetAlphaFinal() * GetAlphaLocal();	// 부모의 알파외 local알파를 곱한다
		} else
			return m_fAlpha;	
	}
	void SetRotateLocal( float rotZ );
	float GetRotateLocal() const {
		return m_dRotateZ;
	}
	float GetRotateFinal() { 
		return (m_pParent)? m_pParent->GetRotateFinal() + m_dRotateZ : m_dRotateZ; 
	}
	/// m_vSize와는 다르게 실제 화면에 렌더되는 크기의 바운드박스를 얻는다 보통 클리핑에 사용된다
	XE::xRECT GetBoundBoxByVisibleFinal();
// 	inline XE::xRECT GetBoundBoxByVisibleFinal() const {
// 		return GetBoundBoxByVisible();
// 	}
	// this의 트랜스폼이 적용된 바운딩박스 크기를 돌려준다. 기준은 this의 (0,0)이다.
	inline XE::xRECT GetBoundBoxByVisibleLocal() {
		return GetBoundBoxByVisibleNoTrans() * m_vScale;
// 	// 코드가 괴상하군 한번 손봐야 할듯.
// 		XE::xRECT rect;
// 		rect.vRB = rect.vLT + GetSizeFinal() - XE::VEC2( 1 );
// 		if( rect.vRB.IsMinus() )
// 			rect.vRB = rect.vLT;	// 사이즈가 없는 윈도우는 RB가 -1이 되면 XWnd::Draw 클리핑에서 걸러지므로 LT랑 같게 한다.,
// 		return rect;
	}
	/// this윈도우의 실제 그림이 있는 영역의 트랜스폼되지 않은 크기바운딩박스를 돌려준다. 기준은 this의 (0,0)이다.
	/// 일반적으로는 0,0 - m_vSize가 바운딩 박스지만 텍스트나 스프라이트의 경우는 옵션이나 그림모양에 따라 다르다.
	virtual XE::xRECT GetBoundBoxByVisibleNoTrans() {
		return XE::xRECT( XE::VEC2(0), m_vSize );
	}
	// virtual
	virtual void Disappear() {}
	virtual BOOL IsDisappear() { return TRUE; }
	virtual BOOL IsDisappearing() { return FALSE; }
	virtual void SendKeyMsg( TCHAR keyCode ) {}
	XWnd* Add( XWnd *pChild, bool bFront = false ) { 
		if( pChild->GetID() )
			return Add( pChild->GetID(), pChild, bFront ); 
		else
			return Add( XE::GenerateID(), pChild, bFront );
	}
	XWnd* Add( int id, XWnd *pChild, bool bFront = false );
	XWnd* AddOnly( int id, XWnd *pChild, bool bFront = false );
	XWnd* AddWndTop( XWnd *pWnd ) {
		return AddWndTop( XE::GenerateID(), pWnd );
	}
	XWnd* AddWndTop( ID idWnd, XWnd *pWnd );
	XWnd* Insert( const char *cKey, XWnd *pChild );
	inline XWnd* Insert( const std::string& idsWnd, XWnd *pChild ) {
		return Insert( idsWnd.c_str(), pChild );
	}
	XWnd* Insert( ID idBase, XWnd *pChild );
	XWnd* GetWndTop() {
		if( m_pParent )
			return m_pParent->GetWndTop();
		return this;
	}
	void SortPriority();
	ID DestroyID( int id ) {
		if( id == 0 )
			return 0;
		XWnd *pWnd = Find( id );
		if( pWnd ) {
			pWnd->SetbDestroy( TRUE );
			return pWnd->getid();
		}
		return 0;
	}
	int GetListWnds( XArrayLinearN<XWnd*, 256> *pOut );
	void DestroyWnd( XWnd *pWnd ) {
		if( pWnd == NULL )
			return;
		if( Find( pWnd ) )
			pWnd->SetbDestroy( TRUE );
	}
	void DestroyWndByIdentifier( const char *cIdentifier );
	void DestroyWndByIdentifierf( const char *cIdentifierFormat, ... );
	void DestroyWndByIdentifier( const std::string& strIdentifier ) {
		DestroyWndByIdentifier( strIdentifier.c_str() );
	}

	void DestroyAllWnd() {
		DestroyChildAll();
//		LIST_DESTROY( m_listItems, XWnd* );
	}
	int CallEventHandler( ID msg, DWORD dwParam2=0 );
	virtual int Process( float dt );
protected:
	virtual void Update();
public:
	// 일반사용자 사용금지.
	inline void _Update() {
		Update();
	}
	virtual void Draw();
	virtual void Draw( const XE::VEC2& vParent );
	virtual void OnDrawBefore() {}
	virtual void OnDrawAfter() {}
	virtual void OnUpdateBefore() {}
	virtual void OnUpdateAfter() {}
	virtual void OnProcessBefore() {}
	virtual void OnProcessAfter() {}
	virtual void OnDestroy() {}
	virtual void CallEvent( const char *cFunc );
	void DrawFrame( const XWND_RES_FRAME& frame );
	void GenerateLoopEvent();
	XWnd* Find( ID id );
	inline XWnd* FindByID( ID id ){
		return Find( id );
	}
	XWnd* Find( const char *cIdentifier ) const; 
	inline XWnd* FindByIds( const char* cIds ) const {
		return Find( cIds );
	}
	inline XWnd* FindByIds( const std::string& ids ) const {
		return Find( ids.c_str() );
	}
	XWnd* Findf( const char *cIdentifierFormat, ... ) const;
	XWnd* Find( const std::string& strIdentifier ) const {
		return Find( strIdentifier.c_str() );
	}
	XWnd* FindWithPrefix( const std::string& strIdsPrefix ) const;
	XWnd* Find( XWnd *_pWnd ) const;
	inline XWnd* FindByWnd( XWnd *_pWnd ) const {
		return Find( _pWnd );
	}
	int GetNumChild() {
		return m_listItems.size();
	}
	// lx, ly좌표가 this Wnd영역의내부에 있는가 // lx, ly: this를기준으로한 로컬 마우스좌표
	virtual BOOL IsWndAreaIn( float lx, float ly );
	BOOL IsWndAreaIn( const XE::VEC2& vLocal ) { 
		return IsWndAreaIn( vLocal.x, vLocal.y ); 
	}
	virtual bool IsWndAreaInByScreen( float x, float y );
	inline bool IsWndAreaInByScreen( const XE::VEC2& vPos ) {
		return IsWndAreaInByScreen( vPos.x, vPos.y );
	}
	//
	static BOOL LoadResFrame( LPCTSTR szFile, XWND_RES_FRAME *pFrame, int surfaceBlockSize=0 );
	// event handler
//	void SetEvent( DWORD msg, XBaseScene *pOwner, void (XBaseScene::*pFunc)( XWnd* ) ) {
	// param2는 버튼에서 특정용도로 사용되므로 사용하지 말것.
	template<typename T>
	void SetEvent( DWORD msg, XWnd *pOwner, T func, DWORD param1=0, DWORD param2=0 ) {
		typedef int (XWnd::*CALLBACK_FUNC)( XWnd *, DWORD dwParam1, DWORD dwParam2 );
		for( auto& msgMap2 : m_listMessageMap ) {
			if( msgMap2.msg == msg ) {
				//XWND_MESSAGE_MAP *pMsgMap = &msgMap;
				msgMap2.msg = msg;
				msgMap2.param = param1;
				msgMap2.pOwner = pOwner;
				msgMap2.pHandler = static_cast<CALLBACK_FUNC>( func );
				return;
			}
		}
		XWND_MESSAGE_MAP msgMap;
		msgMap.msg = msg;
		msgMap.param = param1;
		msgMap.pOwner = pOwner;
		msgMap.pHandler = static_cast<CALLBACK_FUNC>( func );
		m_listMessageMap.push_back( msgMap );
	}
	void ClearEvent( DWORD msg ) {
		LIST_MANUAL_LOOP( m_listMessageMap, XWND_MESSAGE_MAP, itor, msgMap )	{
			if( msgMap.msg == msg )
				m_listMessageMap.erase( itor++ );
			else
				++itor;
		} END_LOOP;
	}
	XWND_MESSAGE_MAP FindMsgMap( DWORD msg ) {
		LIST_LOOP( m_listMessageMap, XWND_MESSAGE_MAP, itor, msgMap )	{
			if( msgMap.msg == msg )
				return msgMap;
		} END_LOOP;
		XWND_MESSAGE_MAP msgMap;
		return msgMap;
	}
	BOOL IsHaveEvent( DWORD msg ) {
		LIST_LOOP( m_listMessageMap, XWND_MESSAGE_MAP, itor, msgMap )	{
			if( msgMap.msg == msg )
				return TRUE;
		} END_LOOP;
		return FALSE;
	}
// 	void GetNextClear( std::list<XWnd*>::iterator *pItor );
// 	XWnd* GetNext(std::list<XWnd*>::iterator& itor);
// 	void GetNextClear2( XList<XWnd*>::Itor *pOutItor );
// 	XWnd* GetNext2( XList<XWnd*>::Itor& itor );
	template<typename T, int N>
	int GetUnderChildListToArray( XArrayLinearN<XWnd*, N> *pOutAry ) {
		for( auto pWnd : m_listItems ) {
			pOutAry->Add( pWnd );
		}
		return pOutAry->size();
	}
	void AdjustDynamicPos();
	// 부모들에게 터치다운 이벤트 했던거 취소하라고 알려줌.
	void SendParentsLButtonDownCancel();
	void SendChildLButtonDownCancel();
	// handler
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnLButtonDownCancel();
	virtual void OnNCLButtonDown( float lx, float ly ) {}
	virtual void OnNCMouseMove( float lx, float ly ) {}		// 윈도우 영역밖에서 움직일때 호출되는 핸들러
	virtual void OnNCLButtonUp( float lx, float ly ) {}
	virtual void OnRButtonDown( float lx, float ly );	// 치트용으로만 쓴다.
	virtual void OnRButtonUp( float lx, float ly );	// 치트용으로만 쓴다.
	virtual void OnZoom( float scale, float lx, float ly );			// 확대/축소 동작
	virtual BOOL OnKeyDown( int keyCode );
	virtual BOOL OnKeyUp( int keyCode );
	virtual void OnPlaySound( ID idSnd ) {}
	virtual XEToolTip* CreateToolTip() { return NULL; }		// 툴팁을 실시간으로 생성시키고 싶다면 이것을 재정의 할것.
	virtual BOOL OnCreate() { return TRUE; }	/// 윈도우가 생성되고 Add()가 된후 호출된다.
	virtual void OnStartChangeSizeOfChild( XWnd *pChild, const XE::VEC2& vSize, float secSizing ) {}
	virtual void PreCalcLayoutSize();
	// 하드웨어 자원 및 일반 자원을 복구시킨다.
	virtual BOOL RestoreDevice();
	// 하드웨어 자원만 날린다.
	virtual void DestroyDevice();
	// 홈버튼
	virtual void OnPause();
	/**
	 패치클라이언트 작동으로 게임리소스(프로퍼티,spr,txt등 파일형태로 되어있는 것들)
	 가 삭제되었다 다시 생성되었다는것을 의미함. 이 메소드를 받은 객체는 자신이 게임리소스의
	 포인터를 가지고 있었다면 포인터를 다시 복구시켜야 함.
	*/
	virtual BOOL RestoreGameResource();
	virtual void OnFinishAppear() {}
	// 레이아웃에서 생성될때 this의 자식들이 모두 생성이 끝나면 호출된다.
	virtual void OnFinishCreatedChildLayout( XLayout *pLayout ) {}
	//
	void LuaDestroy() {
		SetbDestroy( TRUE );
	}
	ID LuaGetID() {
		return m_id;
	}
	BOOL LuaGetEnable() {
		return m_bEnable;
	}
	const char* LuaGetKey() {
		return m_strIdentifier.c_str();
	}
	XWnd* LuaFindWnd( const char *cKey ) {
		return Find( cKey );
	}
//	XE::VEC2 GetMaxSizeByChild();
	void GetMaxLTPosByChild( XE::VEC2 *pOutLT, XE::VEC2 *pOutSize );
	virtual XE::VEC2 GetSizeNoTransLayout();
	XE::VEC2 GetSizeNoTransLayoutWithAry( const XVector<XWnd*>& aryWnd ) const;
	XE::VEC2 GetSizeFinalLayout();
	XE::VEC2 GetSizeValidNoTrans();
	float GetSizeValidNoTransWidth();
	float GetSizeValidNoTransHeight();
	void AutoLayoutHCenter( XWnd *pParent );
	void AutoLayoutBottom( XWnd *pParent );
	void AutoLayoutRight( XWnd *pParent );
	void AutoLayoutVCenter( XWnd *pParent );
	void AutoLayoutCenter( XWnd *pParent );
	void AutoLayoutHCenter() {
		AutoLayoutHCenter( m_pParent );
	}
	void AutoLayoutVCenter() {
		AutoLayoutVCenter( m_pParent );
	}
	void AutoLayoutCenter() {
		AutoLayoutCenter( m_pParent );
	}
	void AutoLayoutHCenterWithAry( XArrayLinearN<XWnd*, 256>& aryChilds, float marginLR );
	void AutoLayoutHCenterWithAry( XVector<XWnd*>& aryWnd, float marginLR );
	void AutoLayoutVCenterWithAry( XVector<XWnd*>& aryChilds, float marginTB );
	void AutoLayoutVCenterByChilds( float marginTB );
	void AutoLayoutHCenterByChilds( float marginLR );
	virtual void AutoLayoutByAlign( XWnd *pParent, XE::xAlign align );
	inline void AutoLayoutByAlign( XE::xAlign align ) {
// 		auto pParent = GetpParentValid();
// 		if( pParent )
// 			AutoLayoutByAlign( pParent, align );
		AutoLayoutByAlign( m_pParent, align );		// 내부에서 부모의 유효성을 가로세로 구분해서 검사하기때문에 원래 부모로 넘겨주는게 맞다.
	}
	virtual void Release();
	virtual void DrawDrag( const XE::VEC2& vMouse ) {}
	static XWnd* sGetInsideWnd( XWnd *pRoot, float lx, float ly );
	/// drag&drop에서 drop이 가능하도록 체크
	void SetDropWnd( BOOL bFlag ) {
		if( bFlag )
			m_bitFlags |= xFG_DROP;
		else
			m_bitFlags &= ~xFG_DROP;
	}
	/// drag&drop에서 drag 가능/불가능 세팅
	void SetDragWnd( BOOL bFlag ) {
		if( bFlag )
			m_bitFlags |= xFG_DRAG;
		else
			m_bitFlags &= ~xFG_DRAG;
	}
	/// drop이 가능한 윈도우 인가
	BOOL IsDropWnd() {
		return (m_bitFlags & xFG_DROP)? TRUE : FALSE;
	}
	/// drag이 가능한 윈도우 인가
	BOOL IsDragWnd() {
		return (m_bitFlags & xFG_DRAG)? TRUE : FALSE;
	}
	virtual void ProcessDrag( const XE::VEC2& vDistMove );
	///
	/**
	 @brief 윈도우 영역밖의 터치 이벤트를 감지하기 위한 플래그
	*/
	void SetEnableNcEvent( BOOL bFlag ) {
		if( bFlag )
			m_bitFlags |= xFG_NC_EVENT;
		else
			m_bitFlags &= ~xFG_NC_EVENT;
	}
	BOOL IsEnableNcEvent() {
		return (m_bitFlags & xFG_NC_EVENT)? TRUE : FALSE;
	}
	virtual void GetMatrix( MATRIX *pOut );
	template<typename T>
	T GetCtrl( const char *cKey ) {
		XWnd *pWnd = Find(cKey);
		if( pWnd == nullptr )
			return nullptr;
		return SafeCast<T, XWnd*>( pWnd );
	}
	void ConvertChildPosToLocalPos( const XE::VEC2& vBase );
	void SetAutoUpdate( float sec );
	void ClearAutoUpdate() {
		m_timerAutoUpdate.Off();
	}
	bool IsAutoUpdate() {
		return m_timerAutoUpdate.IsOn() == TRUE;
	}
	// 만약 핸들러만 사용하고 Update()는 호출하지 않길 바란다면 재정의해서 쓴다.
	virtual void OnAutoUpdate() { 
		SetbUpdate( true ); 
	}
	virtual void DrawDebugInfo( float x, float y, XCOLOR col = XCOLOR_WHITE, XBaseFontDat *pFontDat = nullptr);
// 	template<typename T>
// 	XWndButton* SetButtHander( XWnd *pKeyRoot, const char *cKey, T funcCallback, DWORD param1 = 0 );
	template<typename T>
	XWnd* SetButtHander( XWnd *pKeyRoot, const char *cKey, T funcCallback, DWORD param1 = 0)	{
		typedef int ( XWnd::*CALLBACK_FUNC )( XWnd *, DWORD, DWORD );
		if( XBREAK( pKeyRoot == NULL ) )
			return nullptr;
		auto pButt = pKeyRoot->Find( cKey );
		if( pButt )
			pButt->SetEvent( XWM_CLICKED, this, funcCallback, param1 );
		return pButt;
	}
	XWnd* SetClickHander( const char *cKey,
												ID idEvent,
												std::function<void( XWnd* )> func );

	void SetbTouchableWithChild( bool bFlag );
	virtual void GetDebugString( _tstring& strOut );
	void SetbUpdateChilds();
	// Get control
//	XWndImage* GetImageCtrl( const char *cKey ) const;
	XWnd* GetpParentValid();
	GET_SET_ACCESSOR( XE::xtBlendFunc, blendFunc );
// 	inline void SetBlendFunc( XE::xtBlendFunc func ) {
// //		m_Effect = XE::ConvertBlendFuncDMTypeDmType( func );
// 		m_blendFunc = func;
// 	}
	// // 
	virtual void OnDestroyAfterByWnd( ID idWndChild, const std::string& idsChild ) {}
	virtual void Reload();
	virtual void DestroyForReload();
	virtual void ProcessMsg( const std::string& strMsg ) {}
	void SendMsgToChilds( const std::string& strMsg );
	int __OnClickLayoutEvent( XWnd* pWnd, DWORD, DWORD );
	XWnd* GetpParentValidWidth();
	XWnd* GetpParentValidHeight();
	XWnd* GetpParentHaveAlignV();
	XWnd* GetpParentHaveAlignH();
#if defined(_CHEAT) && defined(WIN32)
	void UpdateMouseOverWins( const XE::VEC2& vMouse, int depth, XVector<xWinDepth>* pOutAry );
#endif // defined(_CHEAT) && defined(WIN32)
	bool IsMouseOver( const XE::VEC2& vMouse );
	void SetEvent2( ID idEvent, std::function<void(XWnd*)> func );
private:
	virtual void OnNCModal() {}
	virtual bool IsAbleAlign() const { return true; }
	virtual XE::VEC2 GetvChildLocal( const XE::VEC2& vLocal, XWnd* pParent, XWnd* pGrandParent ) const;
}; // class XWnd

#pragma warning ( default : 4250 )

