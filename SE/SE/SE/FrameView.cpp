// FrameView.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "SE.h"
#include "MainFrm.h"
#include "FrameView.h"
#include "AnimationView.h"
#include "ConsoleView.h"
#include "DlgDrawProperty.h"
#include "DlgEditAni.h"
#include "XGraphicsD3DTool.h"
#include "Tool.h"
#include "SprObj.h"
//#include "d3dView.h"
//#include "Graphic2D.h"
#include "UndoMng.h"
#include "XSoundMng.h"
#include "XList.h"
#include "WndTimeline.h"
#include "XAniAction.h"
#include "XLayerBase.h"
// #include "XKeyBase.h"
// #include "XKeyPos.h"
// #include "XKeyCreateObj.h"
#include "XKeyAll.h"
#include "XLayerObject.h"
#include "XLayerSound.h"
#include "XActObj.h"
#include "Select.h"

using namespace xSpr;

// CFrameView
static CFrameView *s_pFrameView = nullptr;
CFrameView *GetFrameView() { return s_pFrameView; }
CFrameView* CFrameView::s_pInstance = nullptr;

IMPLEMENT_DYNCREATE(CFrameView, CView)

CFrameView::CFrameView()
{
	s_pFrameView = this;
	s_pInstance = this;
//	
//	GetDlgBarTimeline()->m_nKeyEditMode = 0;
	_m_DragMode = xDRAG_NONE;
	_m_DragState = xDS_NONE;
	m_bPanningMode = FALSE;
//	m_bMaxFrameMove = FALSE;
//	m_bMoveLayer = FALSE;
	m_fDragKeyScale = 1.0f;
	m_pPushKey = nullptr;
	m_fStartFrame = 0;
//	m_bTrackScroll = FALSE;
	m_bTrackPush = FALSE;
	m_fScale = 0.5f;
	m_GridUnit = xGU_TIME;
}

CFrameView::~CFrameView()
{
//	
}

void CFrameView::DoDataExchange(CDataExchange* pDX)
{
	CView::DoDataExchange(pDX);
//	DDX_Radio(pDX, IDC_RADIO_KEY_MOVE, GetDlgBarTimeline()->m_nKeyEditMode);
//	DDX_Control(pDX, IDC_COMBO1, m_comboPlayMode);
}

BEGIN_MESSAGE_MAP(CFrameView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTT_ADD_LAYER, &CFrameView::OnBnClickedButtAddLayer)
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
//	ON_WM_MOUSEHWHEEL()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_DEL_KEY, &CFrameView::OnDelKey)
	ON_BN_CLICKED(IDC_BUTT_CREATEOBJ, &CFrameView::OnBnClickedButtCreateobj)
//	ON_WM_HSCROLL()
	ON_WM_RBUTTONDOWN()
//	ON_COMMAND(ID_ADD_LAYER_IMAGE, &CFrameView::OnAddLayerImage)
//	ON_COMMAND(ID_ADD_LAYER_OBJECT, &CFrameView::OnAddLayerObject)
	ON_COMMAND(ID_DEL_SEL_LAYER, &CFrameView::OnDelSelLayer)
//	ON_BN_CLICKED(IDC_RADIO_KEY_MOVE, &CFrameView::OnBnClickedRadioKeyMove)
//	ON_BN_CLICKED(IDC_RADIO_KEY_SCALE, &CFrameView::OnBnClickedRadioKeyScale)
	ON_WM_KEYUP()
	ON_UPDATE_COMMAND_UI( IDC_TEXT_POS, &CFrameView::OnUpdateTextPos )
	ON_UPDATE_COMMAND_UI( IDC_TEXT_ROT, &CFrameView::OnUpdateTextRot )
	ON_UPDATE_COMMAND_UI( IDC_TEXT_SCALE, &CFrameView::OnUpdateTextScale )
	ON_UPDATE_COMMAND_UI( IDC_TEXT_ALPHA, &CFrameView::OnUpdateTextAlpha )
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
	ON_COMMAND(ID_EDIT_KEY, &CFrameView::OnEditKey)
	ON_UPDATE_COMMAND_UI(ID_EDIT_KEY, &CFrameView::OnUpdateEditKey)
	ON_COMMAND(ID_EDIT_ANI, &CFrameView::OnEditAni)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ANI, &CFrameView::OnUpdateEditAni)
	ON_UPDATE_COMMAND_UI(ID_DEL_SEL_LAYER, &CFrameView::OnUpdateDelSelLayer)
	ON_COMMAND(ID_ADD_LAYER_SOUND, &CFrameView::OnAddLayerSound)
	ON_BN_CLICKED(IDC_RELOAD_SOUND, &CFrameView::OnBnClickedReloadSound)
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_SET_REPEATMARK, &CFrameView::OnSetRepeatmark)
	ON_COMMAND(ID_MOVE_START, &CFrameView::OnMoveStart)
	ON_COMMAND(ID_MOVE_END, &CFrameView::OnMoveEnd)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CFrameView::OnCbnSelchangeCombo1)
	ON_COMMAND(ID_SHOW_LAYER, &CFrameView::OnShowLayer)
	ON_UPDATE_COMMAND_UI(ID_SHOW_LAYER, &CFrameView::OnUpdateShowLayer)
	ON_COMMAND(ID_CREATE_KEY, &CFrameView::OnCreateKey)
	ON_WM_SETCURSOR()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_COMMAND(ID_GRID_FRAME, &CFrameView::OnGridFrame)
	ON_UPDATE_COMMAND_UI(ID_GRID_FRAME, &CFrameView::OnUpdateGridFrame)
	ON_COMMAND(ID_GRID_TIME, &CFrameView::OnGridTime)
	ON_UPDATE_COMMAND_UI(ID_GRID_TIME, &CFrameView::OnUpdateGridTime)
	ON_COMMAND(ID_SELECT_ALL_KEY, &CFrameView::OnSelectAllKey)
	ON_COMMAND(ID_SET_INTERPOLATION, &CFrameView::OnSetInterpolation)
	ON_COMMAND( IDM_COPY_LAYER, &CFrameView::OnCopyLayer )
	ON_COMMAND( IDM_SHOW_THIS_LAYER_ONLY, &CFrameView::OnShowThisLayerOnly )
	ON_COMMAND( IDM_SHOW_ALL_LAYER, &CFrameView::OnShowAllLayer )
	ON_COMMAND( IDM_CREATE_KEY_CURR_CHANNEL, &CFrameView::OnCreateKeyCurrChannel )
	ON_COMMAND( IDM_CREATE_POS_KEY, &CFrameView::OnCreatePosKey )
	ON_COMMAND( IDM_CREATE_ROT_KEY, &CFrameView::OnCreateRotKey )
	ON_COMMAND( IDM_CREATE_KEY_SCALE, &CFrameView::OnCreateKeyScale )
	ON_COMMAND( IDM_CREATE_KEY_EFFECT, &CFrameView::OnCreateKeyEffect )
	ON_COMMAND( IDM_SELECT_ALL_KEY_AFTER, &CFrameView::OnSelectAllKeyAfter )
	ON_COMMAND( IDM_SELECT_ALL_KEY_AFTER_IN_LAYER, &CFrameView::OnSelectAllKeyAfterInLayer )
END_MESSAGE_MAP()

// ON_COMMAND( ID_SET_PATH, &CFrameView::OnSetPath )
// ON_COMMAND( ID_RELEASE_PATH, &CFrameView::OnReleasePath )
// ON_UPDATE_COMMAND_UI( ID_RELEASE_PATH, &CFrameView::OnUpdateReleasePath )
// ON_UPDATE_COMMAND_UI( ID_SET_PATH, &CFrameView::OnUpdateSetPath )
// ON_COMMAND( ID_PATH_TYPE_LINE, &CFrameView::OnPathTypeLine )
// ON_UPDATE_COMMAND_UI( ID_PATH_TYPE_LINE, &CFrameView::OnUpdatePathTypeLine )
// ON_COMMAND( ID_PATH_TYPE_CATMULLROM, &CFrameView::OnPathTypeCatmullrom )
// ON_UPDATE_COMMAND_UI( ID_PATH_TYPE_CATMULLROM, &CFrameView::OnUpdatePathTypeCatmullrom )


// CFrameView 진단입니다.

#ifdef _DEBUG
void CFrameView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CFrameView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFrameView 메시지 처리기입니다.
int CFrameView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
    RECT rc;
    GetClientRect(&rc);

	return 0;
}
LRESULT CFrameView::OnKickIdle(WPARAM, LPARAM)
{
	UpdateDialogControls( this, FALSE );
	return (LRESULT)0;
}

void CFrameView::OnInitialUpdate()
{
	XMyD3DView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
//	int w, minw, h, minh;		// WndSplitter::OnHScroll에서 자꾸 다운되서 추가한코드
	CMainFrame *pMainFrm = (CMainFrame *)AfxGetMainWnd();
	{
		CSize size;
		size.cx = 0;
		size.cy = 0;
	}
}

void CFrameView::OnSize(UINT nType, int cx, int cy)
{
	XMyD3DView::OnSize(nType, cx, cy);
}
// void CFrameView::UpdateLayersPos()
// {
// 	SPAction spAction = SPROBJ->GetAction();
// 	if( !spAction )	return;
// 	SPROBJ->UpdateLayersPos();
// }
/**
레이어 이름영역과 키 스크롤영역을 그린다.
*/
void CFrameView::DrawTimeline( const XE::VEC2& vLT )
{

}
/**
 키 스크롤영역을 그린다.
*/
void CFrameView::DrawKeyWindow( XSPAction spAction )
{
	if( XBREAK( spAction == nullptr ) )
		return;
}
void CFrameView::Draw()
{
	auto spActObj = SPROBJ->GetspActObjCurr();
	XSPAction spAction = SPROBJ->GetspAction();
	float fScale = m_fScale;
	float fSpeed = (spAction) ? spAction->GetSpeed() : 0.2f;
	float fGridPerFrame = 1.0f / fSpeed;			// 1프레임당 작은눈금수 = 1.0 / ani speed;
	float fWidthPerFrame = (KEY_WIDTH * fGridPerFrame);		// 1.0프레임당 가로길이
	float fDotPerFrame = KEY_WIDTH / fSpeed;						// 1Frame당 도트수 fSpeed : KEY_WIDTH = 1.0 : ?  결국 fWidthPerFrame과 같군 -_-;;;
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	RECT rect;
	GetClientRect( &rect );
//	rect.top += (int)TOOLBAR_HEIGHT;				// 툴바가 위치할곳은 비워둔다.
	const XE::VEC2 vClient( rect.left, rect.top );
	const XE::VEC2 vRB( rect.right, rect.bottom );
	SPROBJ->UpdateLayersYPos( GetSizeKey() );
	//
	int wClient = (rect.right - rect.left) + 1;
	int hClient = (rect.bottom - rect.top) + 1;
	XE::VEC2 sizeClient( wClient, hClient );
	if( SPROBJ->IsPlaying() ) {
		// 현재프레임위치가 프레임뷰영역을 벗어났다면 자동으로 스크롤시킴
		float frameRightLimit = m_fStartFrame + ((rect.right - TRACK_X) * 0.90f) / fWidthPerFrame;	// 트랙영역중 3/4위치의 프레임번호 계산
		float frameLeftLimit = m_fStartFrame + ((rect.right - TRACK_X) * 0.10f) / fWidthPerFrame;	// 트랙영역중 1/4위치의 프레임번호 계산
		if( SPROBJ->GetFrmCurr() > frameRightLimit )		// 오른쪽 한계를 벗어남
			m_fStartFrame += SPROBJ->GetFrmCurr() - frameRightLimit;
		if( SPROBJ->GetFrmCurr() < frameLeftLimit ) {
			m_fStartFrame -= frameLeftLimit - SPROBJ->GetFrmCurr();
			if( m_fStartFrame < 0 )
				m_fStartFrame = 0;
		}
	}
	// 기본 바탕색
	XE::FillRectangle( (float)rect.left, (float)rect.top, (float)rect.right, (float)rect.bottom, XCOLOR_GRAY );
	if( GetFocus() == this )
		GRAPHICS->DrawRect( 0, 1, (float)rect.right-1, (float)rect.bottom-1, XCOLOR_RED );
	// MaxFrame 영역
	if( spAction ) {
		float fViewMaxFrame = spAction->GetfMaxFrame() - m_fStartFrame;
		XE::FillRectangle( (float)rect.left+TRACK_X, (float)rect.top+KEY_HEIGHT
											, (float)rect.left+TRACK_X+fViewMaxFrame*fWidthPerFrame, (float)rect.bottom
											, XCOLOR_LIGHTGRAY );
	}
	// 선택된 레이어 표시
	int yDraw = rect.top + m_ofsY;
	XViewport vp( 0, (DWORD)( rect.top + KEY_HEIGHT ), rect.right, rect.bottom	);
	if( spAction ) {
		if( TOOL->GetspSelLayer() ) {
			float top = (float)yDraw + KEY_HEIGHT + TOOL->GetspSelLayer()->Getscry();
			float sh = TOOL->GetspSelLayer()->GetHeight();
			XE::FillRectangle( (float)rect.left+TRACK_X, top, (float)rect.right, top+sh, XCOLOR_WHITE );
		}
	}
	// 그리드 라인
	// 타임라인의 가로 라인
	DrawHorizLine( yDraw, sizeClient );
	// 타임라인의 세로라인과 시간 라벨
	DrawGrid( yDraw, vClient, sizeClient, fSpeed );
	// 왼쪽 레이어 이름들..
	DrawLayerName( spActObj, yDraw );
	// key draw
	if( spAction )
		DrawKeyArea( spAction, yDraw, rect, fSpeed );
}
/**
 세로선과 프레임 라벨을 그림
*/
// void CFrameView::DrawVertByFrame( float speedAction, float y, const XE::VEC2& sizeClient )
// {
// 	float fScale = m_fScale;
// 	float fGridPerFrame = 1.0f / speedAction;			// 1프레임당 작은눈금수 = 1.0 / ani speed;
// 	float fDotPerFrame = KEY_WIDTH / speedAction;						// 1Frame당 도트수 fSpeed : KEY_WIDTH = 1.0 : ?  결국 fWidthPerFrame과 같군 -_-;;;
// 	float fRemainFrame = m_fStartFrame - (int)m_fStartFrame;		// == m_fStartFrame % 1.0f
// 	// 1.0 프레임단위 검은세로줄
// 	for( float x = (float)TRACK_X+(fDotPerFrame * -fRemainFrame); x < sizeClient.w; x += KEY_WIDTH * fGridPerFrame ) {		
// 		if( x < TRACK_X )	continue;
// 		XE::DrawLine( x, y, x, y + sizeClient.h, XCOLOR_BLACK );
// 	}
// 	// 1.0프레임단위 숫자표시
// 	float fFrame = (float)((int)(m_fStartFrame + 0.99999f));			// start에 1.0을 더하고 소수점을 버린값을 쓴다
// 	CString str;
// 	for( float x = (float)TRACK_X+(fDotPerFrame * -fRemainFrame); x < sizeClient.w; x += KEY_WIDTH * fGridPerFrame ) {
// 		if( x < TRACK_X )	continue;
// 		str.Format( _T("%3.1f"), fFrame );
// 		SE::g_pFont->DrawString( x+3, y+5, (LPCTSTR)str );
// 		fFrame += 1.0f;
// 	}
// }
void CFrameView::DrawVertByFrame( float speedAction, float y, const XE::VEC2& sizeClient )
{
	float fScale = m_fScale;
	float fGridPerFrame = 1.0f / speedAction;			// 1프레임당 작은눈금수 = 1.0 / ani speed;
	float fDotPerFrame = KEY_WIDTH / speedAction;						// 1Frame당 도트수 fSpeed : KEY_WIDTH = 1.0 : ?  결국 fWidthPerFrame과 같군 -_-;;;
	float fRemainFrame = m_fStartFrame - (int)m_fStartFrame;		// == m_fStartFrame % 1.0f
	// 1.0 프레임단위 검은세로줄
	for( float x = (float)TRACK_X + ( fDotPerFrame * -fRemainFrame ); x < sizeClient.w; x += KEY_WIDTH * fGridPerFrame ) {
		if( x < TRACK_X )	continue;
		XE::DrawLine( x, y, x, y + sizeClient.h, XCOLOR_BLACK );
	}
	// 1.0프레임단위 숫자표시
//	float fFrame = (float)( (int)( m_fStartFrame + 0.99999f ) );			// start에 1.0을 더하고 소수점을 버린값을 쓴다
	float fFrame = (m_fStartFrame - fRemainFrame) * fGridPerFrame;
	CString str;
	for( float x = (float)TRACK_X + ( fDotPerFrame * -fRemainFrame ); 
																									x < sizeClient.w; 
																		x += KEY_WIDTH * fGridPerFrame ) {
		if( x >= TRACK_X ) {
			str.Format( _T( "%d" ), (int)fFrame );
			SE::g_pFont->DrawString( x + 3, y + 5, (LPCTSTR)str );
		}
		fFrame += fGridPerFrame;
	}
}


/**
 세로선과 시간 라벨을 그림
*/
void CFrameView::DrawVertByTime( float speedAction, float y, const XE::VEC2& sizeClient )
{
	float fScale = m_fScale;
	float fGridPerFrame = 1.0f / speedAction;			// 1프레임당 작은눈금수 = 1.0 / ani speed;
	float fDotPerFrame = KEY_WIDTH / speedAction;						// 1Frame당 도트수 fSpeed : KEY_WIDTH = 1.0 : ?  결국 fWidthPerFrame과 같군 -_-;;;
	// grid unit time
	// 0.1초단위 검은색 세로선
	// 0.01666초마다 0.15frame씩 frame이 움직인다.
	// 1초마다 0.15frame * 60 = 9frame
	float widthSec = KEY_WIDTH * XFPS;		// 1초당 가로도트수
	// 0.1초단위 검은세로줄
	for( float x = (float)TRACK_X+(fDotPerFrame * -m_fStartFrame); x < sizeClient.w; x += (widthSec * 0.1f) ) {		
		if( x < TRACK_X )	continue;
		XE::DrawLine( x, y, x, y + sizeClient.h, XCOLOR_BLACK );
	}
	// 0.1초단위 숫자표시
	float sec = 0;
	CString str;
	for( float x = (float)TRACK_X+(fDotPerFrame * -m_fStartFrame); x < sizeClient.w; x += (widthSec * 0.1f) ) {
		if( x < TRACK_X ) {
			sec += 0.1f;
			continue;
		}
		str.Format( _T("%1.1f"), sec );
		SE::g_pFont->DrawString( x + 3, y + 5, str );
		sec += 0.1f;
	}
}
/**
 타임라인의 가로라인
*/
void CFrameView::DrawHorizLine( int yDraw, const XE::VEC2& sizeClient )
{
	// 가로라인
	int numLayer = 20;
	int hTimeLine = numLayer * ( (int)KEY_HEIGHT * 3 );
	for( int y = yDraw; y < yDraw + hTimeLine; y += (int)KEY_HEIGHT )
		XE::DrawLine( 0, (float)y, sizeClient.w, (float)y, XCOLOR_DARKGRAY );
}
void CFrameView::DrawGrid( int yDraw, 
						const XE::VEC2& vClient, 
						const XE::VEC2& sizeClient, 
						float speedAction )
{
	const float fScale = m_fScale;
	const float fGridPerFrame = 1.0f / speedAction;			// 1프레임당 작은눈금수 = 1.0 / ani speed;
	const float fDotPerFrame = KEY_WIDTH / speedAction;						// 1Frame당 도트수 fSpeed : KEY_WIDTH = 1.0 : ?  결국 fWidthPerFrame과 같군 -_-;;;
	//
	XViewport vp( 0, 0, sizeClient );
	// 한칸단위 옅은 세로선
	float fRemainFrame = m_fStartFrame - ( (int)( m_fStartFrame / speedAction ) * speedAction );
	for( int x = (int)( TRACK_X + ( fDotPerFrame * ( speedAction - fRemainFrame ) ) ); x < sizeClient.w; x += (int)KEY_WIDTH )
		XE::DrawLine( (float)x, vClient.y + KEY_HEIGHT, (float)x, vClient.y + KEY_HEIGHT + sizeClient.h, XCOLOR_DARKGRAY );
	if( m_GridUnit == xGU_FRAME )
		DrawVertByFrame( speedAction, vClient.y, sizeClient );
	else
		DrawVertByTime( speedAction, vClient.y, sizeClient );
}
/**
 키 스크롤영역을 그린다.
*/
void CFrameView::DrawKeyArea( XSPAction spAction, int yDraw, const RECT& rectClient, float speedAction )
{
	const float fScale = m_fScale;
	const XE::VEC2 vClient( TRACK_X, (float)rectClient.top + KEY_HEIGHT );
	const XE::VEC2 vRBClient( rectClient.right, rectClient.bottom );
	const XE::VEC2 sizeClient = vRBClient - vClient;
	XViewport vp( vClient, sizeClient );
	//
	DrawKeys( spAction, yDraw, speedAction, rectClient );
	//
	float fGridPerFrame = 1.0f / speedAction;			// 1프레임당 작은눈금수 = 1.0 / ani speed;
	float fWidthPerFrame = ( KEY_WIDTH * fGridPerFrame );		// 1.0프레임당 가로길이
	// 뷰포트 설정을 바꾸기때문에 여기 위로 dx line그리기 등의 함수는 사용하지 말것
	// 현재 프레임위치를 나타내는 파란선
	{
		float x1 = TRACK_X + ( ( SPROBJ->GetFrmCurr() - m_fStartFrame ) * fWidthPerFrame );
		XE::FillRectangle( x1, (float)yDraw, x1 + KEY_WIDTH, vRBClient.y, XCOLOR_RGBA( 0, 0, 255, 128 ) );
	}
	// 도돌이표 표시
	if( spAction->GetRepeatMark() > 0 ) {
		float x1 = TRACK_X + ( ( spAction->GetRepeatMark() - m_fStartFrame ) * fWidthPerFrame );
		XE::DrawLine( x1, (float)yDraw, x1, vRBClient.y, XCOLOR_RGBA( 255, 0, 0, 128 ), 3.0f );
	}
	// 키 선택 드래그 영역 표시
	if( IsDragMode( xDRAG_BLOCK ) ) {
		XE::DrawRectangle( m_vDragStart, m_vDragEnd, XCOLOR_RED, 2.0f );
	}
	// 툴팁 표시
	DrawToolTipByKey();
}
void CFrameView::DrawToolTipByKey()
{
	const float fScale = m_fScale;
	if( TOOL->GetSelToolTipKey().GetNum() > 0 ) {
		const float space = 8;		// 테두리 여백크기
		XE::VEC2 sizeTip = XE::VEC2( space * 2, 0 );
		float nMaxW = 0;
		float l = 0, top = 0;					// 툴팁 사각형의 좌상귀
		XBaseKey *pKey;
		TOOL->GetSelToolTipKey().GetNextClear();
		sizeTip.y += space / 2;
		while( pKey = (XBaseKey *)TOOL->GetSelToolTipKey().GetNext() ) {
			float w, h;
			w = h = 0;
			sizeTip.y += space / 2;		// 구분선이 있을경우 여백이 작아야 하기때문.
			pKey->GetToolTipSize( &w, &h );				// 이 키가 표시될 툴팁의 최소 크기를 구한다
			h *= 1.15f;		// 세로로 약간 더크게 함.
			sizeTip.y += h;									// 툴팁의 전체크기에 더한다
			if( w > nMaxW )
				nMaxW = w;
			l = pKey->GetvDrawPos().x;					// 툴팁 사각형의 좌상귀
			top = pKey->GetvDrawPos().y - sizeTip.y - space;
			if( top < TOOLBAR_HEIGHT ) {
				top = TOOLBAR_HEIGHT;
				l += (GetSizeKey().w * 2.f);
			}
			if( TOOL->GetSelToolTipKey().GetLast() == pKey )
				sizeTip.y += space;
			else
				sizeTip.y += space / 2;		// 구분선을 그려야 할때는 여벽을 조금만준다
		}
		sizeTip.x = nMaxW;		// 가장 넓은 툴팁의 가로크기를 기준으로 한다
		if( top < 0 ) {		// 화면을 벗어나면 조정
			top = 0;
			l += KEY_WIDTH;
		}
		XE::FillRectangle( l, top, l + sizeTip.x, top + sizeTip.y, XCOLOR_RGBA( 64, 64, 64, 200 ) );		// 반투명으로 툴팁 바탕 사각형을 그림
		XE::DrawRectangle( l, top, l + sizeTip.x, top + sizeTip.y, XCOLOR_BLACK );		// 외곽선
		// 툴팁 텍스트를 차례로 그림
		float _t = top + space;
		TOOL->GetSelToolTipKey().GetNextClear();
		while( pKey = (XBaseKey *)TOOL->GetSelToolTipKey().GetNext() ) {
			auto spLayer = SPROBJ->GetspLayer( pKey );
			SE::g_pFont->DrawString( l + 8, _t, pKey->GetToolTipString(spLayer) );				// 툴팁텍스트 그림
			// 그외 툴팁 virtual draw를 호출
			pKey->DrawToolTip( SPRDAT, l + sizeTip.w, _t );
			float w, h;
			pKey->GetToolTipSize( &w, &h );
			_t += h + space / 2;		// 텍스트밑에 도트를 좀더 내려서 선을 그림
			if( TOOL->GetSelToolTipKey().GetLast() != pKey )		// 마지막 키는 구분선 그리지 않음
				XE::DrawLine( l, _t, l + sizeTip.x, _t, XCOLOR_BLACK );		// 툴팁간 구분선
			_t += space / 2;
		}
	}
}
/**
 키스크롤영역의 키들을 그린다.
*/
void CFrameView::DrawKeys( XSPAction spAction, int yDraw, float speedAction, const RECT& rectClient )
{
	if( spAction == nullptr )
		return;
	const float fScale = m_fScale;
	const float fGridPerFrame = 1.0f / speedAction;			// 1프레임당 작은눈금수 = 1.0 / ani speed;
	const float fWidthPerFrame = ( KEY_WIDTH * fGridPerFrame );		// 1.0프레임당 가로길이
	const float fDotPerFrame = KEY_WIDTH / speedAction;						// 1Frame당 도트수 fSpeed : KEY_WIDTH = 1.0 : ?  결국 fWidthPerFrame과 같군 -_-;;;
	// 진짜 키들 그리기
	float wx = m_fStartFrame * fDotPerFrame;			// m_fStartFrame을 도트수로 환산
	// 각 키들의 화면좌표를 먼저 업데이트 한다.
	spAction->GetNextKeyClear();
	while( XBaseKey *pKey = spAction->GetNextKey() ) {
		pKey->UpdateDrawPos( SPROBJ, speedAction, TRACK_X, (float)yDraw + KEY_HEIGHT, wx, fScale );
	}
	// 업데이트된 화면좌표로 각 키를 그린다
// 	spAction->GetNextKeyClear();
// 	while( XBaseKey *pKey = spAction->GetNextKey() ) {
	for( auto pKey : spAction->GetlistKey() ) {
		const auto typeMain = pKey->GetType();
		if( typeMain == xSpr::xKT_IMAGE ) {
		} else
		if( typeMain == xSpr::xKT_CREATEOBJ ) {
			const auto typeChannel = pKey->GetSubType();
			if( typeChannel == xKTS_MAIN ) {
				auto pKeyObj = SafeCast<XKeyCreateObj*>( pKey );
				if( pKeyObj ) {
//					auto pLayer = SafeCast<XLayerObject*>( pKey->GetspLayer().get() );
					auto spLayer 
						= SPROBJ->GetspLayerByidLayer<XLayerObject>( pKey->GetidLayer() );
					if( spLayer /*&& spLayer->GetpSprObjCurr() */) {
//						auto pSprObj = spLayer->GetpSprObjCurr();
						auto pSprObj = SPROBJ->GetpSprObjChild( pKeyObj->GetidSprObj() );
						if( pSprObj ) {
							auto spActObj = pSprObj->GetspActObjCurr();
							if( spActObj ) {
								auto speedChild = spActObj->GetSpeed();
								const float fGridPerFrame = 1.0f / speedChild;			// 1프레임당 작은눈금수 = 1.0 / ani speed;
								const float fWidthPerFrame = ( KEY_WIDTH * fGridPerFrame );		// 1.0프레임당 가로길이
//								const float fDotPerFrame = KEY_WIDTH / speedChild;						// 1Frame당 도트수 fSpeed : KEY_WIDTH = 1.0 : ?  결국 fWidthPerFrame과 같군 -_-;;;
								const float fViewMaxFrame = spLayer->GetmaxFrame();
								auto vLT = pKey->GetvDrawPos() + XE::VEC2( 0, 5 );
								const XE::VEC2 vSize( fViewMaxFrame * ( fWidthPerFrame )
																		, KEY_HEIGHT - 10.f );
								XE::FillRectangleSize( vLT, vSize, XCOLOR_RGBA( 0, 0, 255, 128 ) );
							}
						}
					}
				} // pKeyObj
			} // KTS_MAIN
		} // KT_CREATEOBJ
		// pos/rot/scale일경우 키 보간선을 그린다
		const auto cnType = pKey->GetSubType();
		if( cnType == xSpr::xKTS_POS ) {
			XKeyPos *pPosKey = SafeCast<XKeyPos*>( pKey );
			if( pPosKey ) {
				// 포지션 키는 패스를 쓰고 있어서 따로 만듬.
				DrawInterpolationLineByPosKey( pPosKey );
			}
		} else
		if( cnType == xSpr::xKTS_SCALE ) {
			DrawInterpolationLine<XKeyScale>( pKey );
// 			auto pScaleKey = SafeCast<XKeyScale*>( pKey );
// 			if( pScaleKey ) {
// //				DrawInterpolationLineByScaleKey( pScaleKey );
// 			}
		} else
		if( cnType == xSpr::xKTS_ROT ) {
			DrawInterpolationLine<XKeyRot>( pKey );
		} else
		if( cnType == xSpr::xKTS_EFFECT ) {
			DrawInterpolationLine<XKeyEffect>( pKey );
		}
		// 이 키가 선택된 키인가.
		BOOL bSelected = TOOL->GetSelectKey().Find( pKey );
		// 키(네모)를 그림
		XE::VEC2 vLT( TRACK_X, KEY_HEIGHT );
		pKey->DrawKey( XCOLOR_BLACK, fScale, bSelected != FALSE );
	} // for key
	// 쉐도우 키들 그리기
// 	TOOL->GetShadowKey().Clear();
// 	while( XBaseKey *pKey = static_cast<XBaseKey*>( TOOL->GetShadowKey().GetNext() ) ) {
	for( auto pElem : TOOL->GetShadowKey().GetlistElem() ) {
		auto pKey = static_cast<XBaseKey*>( pElem );
		float speed = spAction->GetSpeed();
		pKey->UpdateDrawPos( SPROBJ, speed, TRACK_X, (float)yDraw + KEY_HEIGHT, wx, fScale );
		pKey->DrawKey( XCOLOR_BLACK, fScale );
	}
// 	}
}

/**
 타임라인 왼쪽 각 레이어의 이름을 출력한다.
*/
void CFrameView::DrawLayerName( XSPActObj spActObj, int yDraw )
{
	if( spActObj == nullptr )
		return;
	if( spActObj->GetNumLayers() == 0 )
		return;
	// 레이어 리스트의 레이블을 모두 그린다
	DrawLayerName( spActObj, 0, (float)yDraw + KEY_HEIGHT, m_spPushLayer );
	float t = 9;
	if( m_spMoveLayerCurr )		// 레이어이동모드에서 삽입될 위치에 파란선두개를 긋는다
		t = (float)yDraw + KEY_HEIGHT + m_spMoveLayerCurr->Getscry();
	else {
//		if( SPROBJACT->GetLastLayer() )
		// 가장마지막레이어의 아래쪽좌표를 구함
		auto spLastLayer = spActObj->GetLastLayer();
		if( spLastLayer )
			t = (float)yDraw + KEY_HEIGHT + spLastLayer->Getscry() + spLastLayer->GetHeight();
	}
	// 레이어 이동중이면 파란선 표시
	if( IsDragMode( xDRAG_LAYER_MOVE ) ) {
		XE::DrawLine( 0, t, TRACK_X, t, XCOLOR_BLUE );  t += 1.0f;
		XE::DrawLine( 0, t, TRACK_X, t, XCOLOR_BLUE );
	}
}

/**
@brief
*/
void CFrameView::DrawLayerName( XSPActObj spActObj, float left, float top, XSPBaseLayer spPushLayer )
{
	D3DCOLOR colWhite = XCOLOR_WHITE;
	auto spSelected = TOOL->GetspSelLayer();
	for( auto& spLayer : spActObj->GetlistLayer() ) {
		bool bHide = spLayer->GetbShow() == FALSE;
		XCOLOR col = XCOLOR_WHITE;
		// 레이어가 눌린상태면 바탕색 다르게
		if( spPushLayer &&spPushLayer->getid() == spLayer->getid() ) {
			XE::FillRectangle( 0, top + spLayer->Getscry() + 1, TRACK_X, top + spLayer->Getscry() + 1 + KEY_HEIGHT, XCOLOR_WHITE );
		}
		// + 표시
		if( spLayer->GetbAbleOpen() ) {
			float top2 = top + spLayer->Getscry() + 5;
			XE::DrawRectangle( 0, top2, 8, top2 + 8, XCOLOR_WHITE );							// 박스
			XE::DrawLine( 0, top2 + 4, 8, top2 + 4, XCOLOR_WHITE );		// 가로선
			if( spLayer->GetbOpen() == FALSE ) {	// 닫혀있는상태 +표시
				XE::DrawLine( 0 + 4, top2, 0 + 4, top2 + 8, XCOLOR_WHITE );		// 세로선
			}
		}
		// label 표시
		if( spPushLayer && spPushLayer->getid() == spLayer->getid() )
			col = XCOLOR_BLACK;
		else {
			if( bHide )
				col = XCOLOR_LIGHTGRAY;
			else
				if( spSelected && spLayer->getid() == spSelected->getid() )
					col = XCOLOR_YELLOW;
		}
		spLayer->DrawLabel( left, top, col );
	}
}

void CFrameView::OnBnClickedButtAddLayer()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	PopupKeyFrame();
}

void CFrameView::OnBnDoubleClicked()
{

}

void CFrameView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( nFlags & MK_SHIFT ) 
	{
		int old = GetDlgBarTimeline()->m_nKeyEditMode;
		GetDlgBarTimeline()->m_nKeyEditMode = KEYEDIT_SCALE;
		GetDlgBarTimeline()->UpdateData( FALSE );		// 변수를 컨트롤로
		if( old != GetDlgBarTimeline()->m_nKeyEditMode )
			Invalidate( FALSE );
	} else
	if( nFlags & MK_ALT )
	{
		m_bPanningMode = TRUE;		// 패닝모드로 전환
	}
// 	if( nChar == VK_RETURN ) {
// 		if( TOOL->GetSelectKey().GetNum() > 0 ) {
// 			auto pKeySelected = TOOL->GetSelectKey().GetFirst();
// 			if( pKeySelected ) {
// 				TOOL->GetSelectKey().Clear();
// 				TOOL->GetSelectKey().Add( pKeySelected );
// 				OnEditKey();
// 			}
// 		}
// 	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
void CFrameView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( nFlags & MK_SHIFT ) 
	{
		GetDlgBarTimeline()->m_nKeyEditMode = KEYEDIT_MOVE;
		GetDlgBarTimeline()->UpdateData( FALSE );	// 변수를 컨트롤로
		Invalidate( FALSE );
	}
	if( nFlags & MK_ALT ) 
		m_bPanningMode = FALSE;

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

/**
 Key윈도우 영역내 로컬좌표를 건네주면 m_pPushKey가 
*/
// BOOL CFrameView::ProcSelectKey( UINT nFlags, const XE::VEC2& vLocal )
// {
// 	SPAction spAction = SPROBJ->GetAction();
// // 	BOOL bSelect = TRUE;
// 	if( m_pPushKey ) {
// 		XBaseKey *pKey = m_pPushKey;
// 		// m_pPushKey위에서 손을 뗏는가
// 		bool bInSide = pKey->IsInside( vLocal, m_fScale ) != FALSE;
// 		if( bInSide ) {
// 			spAction->GetNextKeyClear();
// 			if( nFlags & MK_CONTROL ) {
// 				if( TOOL->GetSelectKey().Find( pKey ) )		// 이미 선택되어있는키에 컨트롤클릭하면 선택해제한다
// 					TOOL->GetSelectKey().Del( pKey );
// 				else
// 					TOOL->GetSelectKey().Add( pKey );			// 컨트롤키 누르고 선택하면 셀렉트목록체 추가시킨다
// 			} else
// 			if( nFlags & MK_SHIFT ) {
// 				if( TOOL->GetSelectKey().GetNum() == 0 )
// 					TOOL->GetSelectKey().Add( pKey );			// 선택된 키가 하나도 없는데 쉬프트누르고 선택했다면 그냥 평범하게 추가시킨다
// 				else {
// 					// 쉬프트를 누르고 선택하면 먼저 선택했던 키에서부터 같은레이어에 있는 선택한 키까지 모두 선택된다
// 					if( static_cast<XBaseKey*>( TOOL->GetSelectKey().GetLast() )->IsSameLayer( SPROBJ, pKey ) )	{	// 일단 두키가 같은 레이어여야 쉬프트 선택이 된다
// 						XBaseKey *pSKey;
// 						spAction->GetRangeKeyClear();
// 						while( pSKey = spAction->GetRangeKey( SPROBJ, static_cast<XBaseKey*>( TOOL->GetSelectKey().GetLast() ), pKey ) )
// 							TOOL->GetSelectKey().Add( pSKey );
// 					}
// 				}
// 				TOOL->SetspSelLayer( pKey->GetspLayer() );
// 			} else {
// 				// 그냥 클릭
// 				TOOL->GetSelectKey().Clear();				// 그냥 키를 선택했을때는 선택되었던 키들을 삭제하고
// 				TOOL->GetSelectKey().Add( pKey );			// 새로선택한 키를 추가한다
// 				TOOL->SetspSelLayer( pKey->GetspLayer() );
// 			}
// 		} else {// bInside
// // 			bSelect = FALSE;
// 		}
// 	} else {
// 		TOOL->GetSelectKey().Clear();
// // 		bSelect = FALSE;
// 	}
// // 	return bSelect;
// }
/**
 LButtonUp이벤트때 클릭한 키를 ctrl, shift키 유무에 따라 처리한다.
*/
void CFrameView::ProcSelectKeyWhenButtUp( XBaseKey *pClickKey, UINT nFlags )
{
	if( XBREAK( pClickKey == nullptr ) )
		return;
	auto spActObj = SPROBJ->GetspActObjCurr();
	if( spActObj )
		return;
	auto spLayer = spActObj->GetspLayer( pClickKey );
	if( nFlags & MK_CONTROL ) {
		if( TOOL->GetSelectKey().Find( pClickKey ) )		// 이미 선택되어있는키에 컨트롤클릭하면 선택해제한다
			TOOL->GetSelectKey().Del( pClickKey );
		else
			TOOL->GetSelectKey().Add( pClickKey );			// 컨트롤키 누르고 선택하면 셀렉트목록체 추가시킨다
	} else
	if( nFlags & MK_SHIFT ) {
		if( TOOL->GetSelectKey().GetNum() == 0 )
			TOOL->GetSelectKey().Add( pClickKey );			// 선택된 키가 하나도 없는데 쉬프트누르고 선택했다면 그냥 평범하게 추가시킨다
		else {
			// 쉬프트를 누르고 선택하면 먼저 선택했던 키에서부터 같은레이어에 있는 선택한 키까지 모두 선택된다
			auto pKeyLast = static_cast<XBaseKey*>( TOOL->GetSelectKey().GetLast() );
			//if( pKeyLast->IsSameLayer( pClickKey ) 
			//	&& pKeyLast->GetSubType() == pClickKey->GetSubType() ) {	// 일단 두키가 같은 레이어여야 쉬프트 선택이 된다
			if( spActObj ) {
				// 같은채널의 키인가.
				if( spActObj->IsSameLayerByKeyWithOpenCheck( pKeyLast, pClickKey ) ) {
					XSPAction spAction = SPROBJ->GetspAction();
					spAction->GetRangeKeyClear();
					while( XBaseKey *pSKey = spAction->GetRangeKey( SPROBJ, static_cast<XBaseKey*>(TOOL->GetSelectKey().GetLast()), pClickKey ) )
						TOOL->GetSelectKey().Add( pSKey );
				}
			}
		}
		TOOL->SetspSelLayer( spLayer );
	} else {
		// 그냥 클릭
		TOOL->GetSelectKey().Clear();				// 그냥 키를 선택했을때는 선택되었던 키들을 삭제하고
		TOOL->GetSelectKey().Add( pClickKey );			// 새로선택한 키를 추가한다
		TOOL->SetspSelLayer( spLayer );
	}
}
//-------------------------------------------------------------------------------------------------------------
/**
 CFrameView윈도우내(툴바영역제외)좌표 vMouse를 Key가 표시되는 스크롤영역의 좌표로 환산한다.
*/
XE::VEC2 CFrameView::GetPosInKeyWindow( const XE::VEC2& vMouse ) 
{
	return vMouse - XE::VEC2( TRACK_X, KEY_HEIGHT + m_ofsY );
}
/**
 레이어 이름영역 기준 좌표로 변환한다.
*/
XE::VEC2 CFrameView::GetPosInLayerWindow( const XE::VEC2& vMouse )
{
	return vMouse - XE::VEC2( 0, KEY_HEIGHT + m_ofsY );
}

/**
 @brief vMouse부분의 레이어를 얻는다.
*/
XSPBaseLayer CFrameView::GetspLayerCurrMouse( const XE::VEC2& vMouse )
{
	XSPAction spAction = SPROBJ->GetspAction();
	if( !spAction ) 
		return XSPBaseLayer();
	auto v = GetPosInLayerWindow( vMouse );
	return CheckPushLayer( v );
}

void CFrameView::OnLButtonDblClk(UINT nFlags, CPoint _point)
{
	static int s_idx = 0;
//	CONSOLE( "double clicked=%d", s_idx++ );
	const XE::VEC2 vMouse = _point;
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	auto spActObj = SPROBJ->GetspActObjCurr();
	if( !spActObj )	return;
	XSPAction spAction = spActObj->GetspAction();
	if( !spAction ) return;
	// 왼쪽 레이어 이름 영역의 터치 처리
	if( vMouse.x < TRACK_X ) {
	} else {
		float fScale = m_fScale;
		// 키 타임라인 스크롤 영역 클릭
		auto v = GetPosInKeyWindow( vMouse );
		if( m_bPanningMode ) {
		} else {
			// 키 선택 / 맥스 프레임조절 모드
			if( SPROBJ->GetPause() == TRUE ) {		// 애니메이션이 멈춰있을때만 처리
				// 키를 눌렀는지 검사
				auto pKey = SPROBJ->GetInsideKey( vMouse, m_fScale );		// 클릭한 지점의 키가 있는지 얻음
				//
				if( pKey ) {
					if( pKey->IsInside( vMouse, m_fScale ) ) {
						TOOL->GetSelectKey().Clear();
						TOOL->GetSelectKey().Add( pKey );
						auto spLayer = spActObj->GetspLayer( pKey );
						if( XASSERT(spLayer) )
							TOOL->SetspSelLayer( spLayer );
						//
						OnEditKey();
					}
				}
			} // is pause
		}
	}
	Invalidate( FALSE );
	//s_prevPoint = _point;
	m_vPrev = vMouse;
	__super::OnLButtonDblClk(nFlags, _point);
}

void CFrameView::OnLButtonDown(UINT nFlags, CPoint _point)
{
	SetCapture();
	XE::VEC2 vMouse = _point;
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	auto spActObj = SPROBJ->GetspActObjCurr();
	if( !spActObj )
		return;
	XSPAction spAction = spActObj->GetspAction();
	if( !spAction ) return;
	// 왼쪽 레이어 이름 영역의 터치 처리
	if( vMouse.x < TRACK_X ) {
		m_areaLDown = xAT_LABEL;
		// +영역 눌렀을때
		if( vMouse.x < 8 ) {
			auto v = GetPosInLayerWindow( vMouse );
			auto spLayer = CheckPushLayer( v, (int)KEY_HEIGHT );
			if( spLayer ) {
				spLayer->SetbOpen( !spLayer->GetbOpen() );
				SPROBJ->UpdateLayersYPos( GetSizeKey() );				// +플 눌러서 펼져졌으므로 y좌표를 다시 계산해야 한다
			}
		} else 
		// 레이어 이름부분 클릭
		if( vMouse.x < TRACK_X ) {
			auto spLayer = GetspLayerCurrMouse( vMouse );
// 			auto v = GetPosInLayerWindow( vMouse );
// 			auto spLayer = spAction->CheckPushLayer( v );
			if( spLayer ) {
				m_spPushLayer = spLayer;
				// 레이어 바꾸기에서 버그가 있어서 클릭에서 셀렉트 하는 방식으로 바꿈
				TOOL->SetspSelLayer( spLayer );
				TOOL->GetSelectKey().Clear();
				//  그라인에 있는 모든 키를 한꺼번에 셀렉트 한다
				spAction->GetNextKeyClear();
				while( XBaseKey *pKey = spAction->GetNextKey() ) {
//					if( pKey->IsSameLayer( spLayer ) )	
					if( spLayer->IsSameLayer(pKey) )
						TOOL->GetSelectKey().Add( pKey );
				}
				if( nFlags & MK_CONTROL ) {
					m_bModeLayerCopy = true;
				}
			}
		}
		// 레이어 이름영역 
	} else {
		// 타임라인 영역
		m_areaLDown = xAT_TIME_LINE;
		float fScale = m_fScale;
		// 키 타임라인 스크롤 영역 클릭
		auto v = GetPosInKeyWindow( vMouse );
		if( m_bPanningMode ) {
			StartDragMode( xDRAG_PANNING );		// 패닝모드에서 좌클릭시작하면 무조건 패닝모드 시작
		} else {
			// 키 선택 / 맥스 프레임조절 모드
			if( SPROBJ->GetPause() == TRUE ) {		// 애니메이션이 멈춰있을때만 처리
				// 키를 눌렀는지 검사
				m_pPushKey = SPROBJ->GetInsideKey( vMouse, m_fScale );		// 클릭한 지점의 키가 있는지 얻음
				//
				if( m_pPushKey ) {
					if( !TOOL->GetSelectKey().Find( m_pPushKey ) ) {
						// 현재 선택되어있는 키가 아닐경우에만 LDown시 바로 셀렉트 된다.
						TOOL->GetSelectKey().Clear();
						TOOL->GetSelectKey().Add( m_pPushKey );
					}
				} else {
					// maxFrame 조절
					float fSpeed = (spAction) ? spAction->GetSpeed() : 0.2f;
					float fGridPerFrame = 1.0f / fSpeed;			// 1프레임당 작은눈금수 = 1.0 / ani speed;
					float fWidthPerFrame = (KEY_WIDTH * fGridPerFrame);		// 1.0프레임당 가로길이
					// a = maxframe을 도트로환산
					// 마우스 포인터가 a의 근처에서 클릭됐는지 검사
					// 클릭됐으면 maxframe 조절 모드로 전환해서 mousemove에서 처리
					float fMaxPos = (spAction->GetfMaxFrame() - m_fStartFrame) * fWidthPerFrame + TRACK_X;		// maxframe의 픽셀좌표
					// 맥스프레임영역 클릭
					if( vMouse.x >= (int)fMaxPos - 5 && vMouse.x <= (int)fMaxPos + 5 ) {
						StartDragMode( xDRAG_MAX_FRAME );
						m_fClickMaxFrame = spAction->GetfMaxFrame();
						m_pLastUndoAction = new XUndoActionChange( SPROBJ, spAction );
						UNDOMNG->PushUndo( m_pLastUndoAction );
					}
				} // if( m_pPushKey ) {
			} // is pause
		}
	}



	// 키를 누른것도 아니고 맥스프레임라인을 누른것도 아니면
	if( !m_pPushKey && IsNotDragMode( xDRAG_MAX_FRAME ) ) {
		m_bTrackPush = TRUE;		// 트랙부분을 클릭했다
		m_vDragStart = m_vDragEnd = _point;
	}
	Invalidate( FALSE );
	//s_prevPoint = _point;
	m_vPrev = vMouse;
	CView::OnLButtonDown(nFlags, _point);
}

void CFrameView::OnMouseMove(UINT nFlags, CPoint point)
{
	XE::VEC2 vMouse = point;
	if( GetFocus() != this ) {
		SetFocus();
	}
	float fScale = m_fScale;
	auto vDist = m_vPrev - vMouse;
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	XSPAction spAction = SPROBJ->GetspAction();
	if( spAction ) {
		float fSpeed = (spAction) ? spAction->GetSpeed() : 0.2f;
		float fGridPerFrame = 1.0f / fSpeed;			// 1프레임당 작은눈금수 = 1.0 / ani speed;
		float fFramePerDot = fSpeed / KEY_WIDTH;						// 1 픽셀당 Frame크기
		int nPrevNumToolTip = TOOL->GetSelToolTipKey().GetNum();		// 이전 턴에서 툴팁키 개수
		TOOL->GetSelToolTipKey().Clear();
		if( SPROBJ->GetPause() ) {
			if( m_spPushLayer ) {
				OnMouseMoveByPushLayer( nFlags, vMouse );
			}
		}
		// 툴팁 검사
		// 마우스 오버되는 모든 키들을 골라낸다
		if( point.x > TRACK_X && IsClearDragMode() ) {			// 키 드래그중엔 툴팁을 표시하지 않는다
			spAction->GetNextKeyClear();
			while( XBaseKey *pKey = spAction->GetNextKey() ) {
				if( pKey->IsInside( vMouse, m_fScale ) )		// 마우스포지션이 키위에 있는가
					TOOL->GetSelToolTipKey().Add( pKey );
			}
			// 이전프레임에 툴팁이 표시됐었다면 화면을 갱신해야한다. 
			if( nPrevNumToolTip > 0 ||  TOOL->GetSelToolTipKey().GetNum() > 0 )		
				Update();
		}
		if( SPROBJ->GetPause() == TRUE ) {		// 애니메이션이 멈춰있을때만 처리
			if( m_spPushLayer ) {
//				OnMouseMoveByPushLayer( nFlags, vMouse );
			} else
			if( m_pPushKey ) {
				OnMouseMoveByPushKey( nFlags, vMouse, fSpeed );
			} else {
				// max frame영역 드래깅
				if( IsDragMode( xDRAG_MAX_FRAME ) ) {
					float distFrame = ( vMouse.x - m_vPrev.x ) * fFramePerDot;		// 이동한 프레임양
					spAction->SetfMaxFrame( spAction->GetfMaxFrame() + distFrame );
					Invalidate( FALSE );
				} else
				// 블럭지정 시작 처리
				if( m_bTrackPush ) {
					auto vAbs = vDist;
					vAbs.Abs();
					if( vAbs.x > 2 || vAbs.y > 2 ) {
						StartDragMode( xDRAG_BLOCK );	// GetDlgBarTimeline()->m_nKeyEditMode = KEYEDIT_DRAG_BLOCK;
					} else {
						int a = 0;
					}
				}
			}
		} // if pause
		// 트랙 스크롤
		if( IsDragMode( xDRAG_PANNING ) ) {
			if( m_alScrollDir == XE::xAL_NONE ) {
				auto vAbs = vDist.Abs();
				if( vAbs.x >= 2 || vAbs.y >= 2 ) {
					if( vAbs.x >= vAbs.y ) {
						m_alScrollDir = XE::xAL_X;
					} else
						m_alScrollDir = XE::xAL_Y;
				}
			}
			if( m_alScrollDir == XE::xAL_X ) {
				float distFrame = vDist.x * fFramePerDot;		// 이동한 도트수만큼을 프레임으로 환산
				m_fStartFrame += distFrame;
				if( m_fStartFrame < 0 )		m_fStartFrame = 0;
				if( m_fStartFrame > spAction->GetfMaxFrame() )
					m_fStartFrame = spAction->GetfMaxFrame();
			} else 
			if( m_alScrollDir == XE::xAL_Y ) {
				m_ofsY -= (int)vDist.y;
				if( m_ofsY > 0 )
					m_ofsY = 0;
			}
			Invalidate( FALSE );
		}
	} // if action
	// 블럭 드래깅
	if( IsDragMode( xDRAG_BLOCK ) ) {
		m_vDragEnd = point;
		Invalidate(0);
	}
	//s_prevPoint = point;
	m_vPrev = point;
	CView::OnMouseMove(nFlags, point);
}

//-------------------------------------------------------------------------------------------------------------
void CFrameView::OnLButtonUp( UINT nFlags, CPoint _point )
{
	ReleaseCapture();
	const XE::VEC2 vMouse = _point;
	SetFocus();		// 클릭하면 프레임뷰로 포커스가 맞춰진다
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	// 드래그앤드롭중에 이곳에서 드롭되었다면
	XSPAction spAction = SPROBJ->GetspAction();
	if( SPROBJ->GetPause() == TRUE ) {		// 애니메이션이 멈춰있을때만 처리
		if( spAction ) {
//		if( vMouse.x < TRACK_X ) {			// 레이어 이름부분 클릭
			if( m_areaLDown == xAT_LABEL ) {
				OnLButtonUpInLabel( nFlags, vMouse );
			} else
//		if( point.x >= TRACK_X )		블럭을 그리고 레이어이름 영역에서 뗄수도있기땜에 이거 있음 안됨
			if( m_areaLDown == xAT_TIME_LINE ) {
				OnLButtonUpInTimeline( nFlags, vMouse );
			} // timeline
		} // if action
	}	else { // if pause
		CONSOLE( "플레이중에는 키를 선택할 수 없습니다" );
	}
	// clear drag mode
	ClearDragMode();
	m_bTrackPush = FALSE;
	m_pPushKey = nullptr;
	m_spPushLayer = nullptr;
	m_areaLDown = xAT_NONE;
	Invalidate( FALSE );
	CView::OnLButtonUp( nFlags, _point );
}

void CFrameView::OnMouseMoveByPushLayer( UINT nFlags, const XE::VEC2& vMouse )
{
	if( XBREAK(m_spPushLayer == nullptr) )
		return;
	if( vMouse.x < TRACK_X ) {
		if( IsClearDragMode() ) {
			StartDragMode( xDRAG_LAYER_MOVE );		// 레이어 드래그시작
		}
	}
	// Layer move -------------------------------------------------------------------------
	if( IsDragMode( xDRAG_LAYER_MOVE ) ) {
		// TRACK_X영역 밖에서도 움직일수 있게 하기위해 밖으로 뺌
		float ly = vMouse.y - KEY_HEIGHT - m_ofsY;
		m_spMoveLayerCurr = CheckPushLayer( vMouse );		// 드래그중에 현재커서가 위치한곳의 레이어
		Invalidate( FALSE );
	}
}

void CFrameView::OnMouseMoveByPushKey( UINT nFlags, const XE::VEC2& vMouse, float speedAction )
{
	XASSERT( m_pPushKey != nullptr );
	float fScale = m_fScale;
	float fFramePerDot = speedAction / KEY_WIDTH;						// 1 픽셀당 Frame크기
	if( IsClearDragMode() ) {		// 아무 드래깅 상태도 아닌상태에서....
		if( TOOL->GetSelectKey().Find( m_pPushKey ) == FALSE ) {	// 셀렉트되지 않은 키를 드래그 했는가
			TOOL->GetSelectKey().Clear();
			TOOL->GetSelectKey().Add( m_pPushKey );	// 새로 드래그하려고 하는 키만 셀렉트 시킴
		}
		if( nFlags & MK_CONTROL ) {	// 컨트롤키 누른상태에서 키를 드래그한순간! 제명이 된게 아니고 키카피가 시작된다
			StartDragMode( xDRAG_KEY_COPY );		 // m_nDragKeyCopy = 1;
			TOOL->CreateShadowKey();			// 현재선택된 키들을 쉐도우키로 복사한다
		} else {
			switch( GetDlgBarTimeline()->m_nKeyEditMode )	{					// 키를 클릭하고 마우스를 움직이는 순간에 드래그모드가 활성화 된다
			case KEYEDIT_MOVE:
				StartDragMode( xDRAG_KEY_MOVE );	// m_nDragKeyStart = 1;			
				break;
			case KEYEDIT_SCALE:
				StartDragMode( xDRAG_KEY_SCALE );	//	m_nDragKeyScale = 1;			
				m_fDragKeyScale = 1.0f;					// 초기값
				TOOL->CreateShadowKey();				// 현재선택된 키들을 쉐도우키로 복사한다
				break;
			}
		}
	} else
	// 드래깅중 ..... ------------------------------------------------------
	// key move -------------------------------------------------------------------------
	if( IsDragMode( xDRAG_KEY_MOVE ) )	{
		float distFrame = (vMouse.x - m_vPrev.x) * fFramePerDot;		// 이동한 프레임양
		BOOL bAble = TRUE;
		SELECT_LOOP( TOOL->GetSelectKey(), XBaseKey*, pKey ) {
			if( pKey->GetfFrame() + distFrame < 0 )	{	// 키를 옮겨보려고 하니 영역을 벗어날땐 키무빙동작 취소시킨다
				bAble = FALSE;
				break;
			}
		} END_LOOP;
		if( bAble ) {
			BOOL bAppend = FALSE;
			SELECT_LOOP( TOOL->GetSelectKey(), XBaseKey*, pKey ) {
				if( GetDragState() == xDS_PUSH ) {		// 드래그 최초진입
					auto pUndo = new XUndoKeyChange( SPROBJ, SPROBJ->GetspActObjCurr(), pKey );
					UNDOMNG->PushUndo( pUndo, bAppend );		// 최초한번만 
					TOOL->GetSelUndoKeys().Add( pUndo );		// 언두액션들을 리스트에 넣는다
					bAppend = TRUE;
				}
				float fFrame = pKey->GetfFrame() + distFrame;
				pKey->SetfFrame( fFrame );		// 키프레임값을 조절
			} END_LOOP;
			SetDragMode( xDRAG_KEY_MOVE, xDS_DRAGGING );
		}
		Invalidate( FALSE );
	} else
	// key scale -------------------------------------------------------------------------
	if( IsDragMode( xDRAG_KEY_SCALE ) )
	{
		m_fDragKeyScale += (vMouse.x - m_vPrev.x) * 0.01f;			// 1도트이동에 1%씩 스케일링 한다
		SELECT_LOOP( TOOL->GetShadowKey(), XBaseKey*, pKey ) {
			float fScaleFrame = pKey->GetpShadowParent()->GetfFrame() * m_fDragKeyScale;		// 원본키의 프레임에 스케일링을 해서
			pKey->SetfFrame( fScaleFrame );																	// 쉐도우키에 적용한다
		} END_LOOP;
		Invalidate( FALSE );
	} else
	// key copy -------------------------------------------------------------------------
	if( IsDragMode( xDRAG_KEY_COPY ) ) {
		float distFrame = (vMouse.x - m_vPrev.x) * fFramePerDot;		// 이동한 프레임양
		BOOL bAble = TRUE;
		SELECT_LOOP( TOOL->GetShadowKey(), XBaseKey*, pKey ) {
			if( pKey->GetfFrame() + distFrame < 0 ) {		// 키를 옮겨보려고 하니 영역을 벗어날땐 키무빙동작 취소시킨다
				bAble = FALSE;
				break;
			}
		} END_LOOP;
		if( bAble ) {
			SELECT_LOOP( TOOL->GetShadowKey(), XBaseKey*, pKey ) {
				float fFrame = pKey->GetfFrame() + distFrame;
				pKey->SetfFrame( fFrame );		// 쉐도우 키프레임값을 조절
			} END_LOOP;
			SetDragMode( xDRAG_KEY_COPY, xDS_DRAGGING );
		}
		Invalidate( FALSE );
	}
} // OnMouseMoveByPushKey

/**
 @brief LButtonUp이벤트중 레이어이름부분 클릭처리
*/
void CFrameView::OnLButtonUpInLabel( UINT nFlags, const XE::VEC2& vMouse )
{
	auto spActObj = SPROBJ->GetspActObjCurr();
	if( !spActObj )
		return;
	// 레이어 순서바꿈
	if( IsDragMode( xDRAG_LAYER_MOVE ) ) {
		if( m_spPushLayer != m_spMoveLayerCurr )		// 드래그모드로 바뀌었는데 제자리에서 손을 뗀경우가 아닐때
			spActObj->SwapLayer( TOOL->GetspSelLayer(), m_spMoveLayerCurr, GetSizeKey() );
		m_spMoveLayerCurr = nullptr;
	}
}

/**
 @brief LButtonUp이벤트중 레이어이름부분 클릭처리
*/
void CFrameView::OnLButtonUpInTimeline( UINT nFlags, const XE::VEC2& vMouse )
{
	auto spActObj = SPROBJ->GetspActObjCurr();
	if( !spActObj )
		return;
	// 키 이동
	const auto dragMode = GetDragMode();
	switch( dragMode )
	{
	case xDRAG_KEY_MOVE: OnLButtonUpInTimelineByDragKeyMove( nFlags, vMouse ); break;
	case xDRAG_KEY_SCALE: OnLButtonUpInTimelineByDragKeyScale( nFlags, vMouse ); break;
	case xDRAG_KEY_COPY: OnLButtonUpInTimelineByDragKeyCopy( nFlags, vMouse ); break;
	case xDRAG_BLOCK: OnLButtonUpInTimelineByDragBlockSelectKey( nFlags, vMouse ); break;
	case xDRAG_MAX_FRAME: OnLButtonUpInTimelineByDragMaxFrame( nFlags, vMouse ); break;
	// 드래깅 없이 ButtonUp
	case xDRAG_NONE: OnLButtonUpInTimelineByNotDrag( nFlags, vMouse ); break;
	case xDRAG_PANNING: break;
	case xDRAG_LAYER_MOVE: XBREAK(1); break;
	default: XBREAK(1); break;
	}
}

/**
 @brief 타임라인 키이동 드래그 처리
*/
void CFrameView::OnLButtonUpInTimelineByDragKeyMove( 
																					UINT nFlags
																				, const XE::VEC2& vMouse )
{
	auto spActObj = SPROBJ->GetspActObjCurr();
	if( !spActObj )
		return;
	// 키를 옮겨다 놓았을때 언두 정보를 푸쉬한다
	TOOL->GetSelUndoKeys().GetNextClear();
	// 선택된 모든키들을 루프돈다
	SELECT_LOOP( TOOL->GetSelectKey(), XBaseKey*, pKey ) {		
		// 키프레임단위로 반올림 조정
		const auto speed = spActObj->GetSpeed();
		float fFrame 
			= (int)((pKey->GetfFrame() + ( speed / 2 )) / speed ) * speed;
		pKey->SetfFrame( fFrame );		// 키프레임값을 조절
		auto pUndo = SafeCast<XUndoKeyChange*>( TOOL->GetSelUndoKeys().GetNext() );
		if( pUndo )		// 키가 0프레임에 있어서 드래그를 했으나 안움직인경우는 이게 널이다
			pUndo->SetKeyAfter( pKey );		// 이동이 끝난후의 키상태를 언두에 갱신한다
	} END_LOOP;
	if( TOOL->GetSelectKey().GetNum() == 1 ) {
		auto pKey = TOOL->GetSelectKey().GetFirst();
		if( pKey ) {
			auto spSelLayer = GetspLayerCurrMouse( vMouse );
			if( spSelLayer )
				TOOL->SetspSelLayer( spSelLayer );
		}
	}
	TOOL->GetSelUndoKeys().Clear();
	spActObj->SortKey();						// 키리스트를 프레임순서에 따라 다시 소트한다
	// 클릭으로 프레임 이동
	MoveFrame( vMouse );			// 현재 마우스 좌표위치로 현재프레임을 이동시킴
}

/**
 @brief 타임라인 키 scale 드래그 처리
*/
void CFrameView::OnLButtonUpInTimelineByDragKeyScale( 
																					UINT nFlags
																				, const XE::VEC2& vMouse )
{
	auto spActObj = SPROBJ->GetspAction();
	if( !spActObj )
		return;
	TOOL->ApplyShadowKey();		// 변화된 쉐도우키의 정보를 원본에 반영한다
	TOOL->GetShadowKey().Destroy();
}
/**
 @brief 타임라인 키 copy 드래그 처리
*/
void CFrameView::OnLButtonUpInTimelineByDragKeyCopy( 
																					UINT nFlags
																				, const XE::VEC2& vMouse )
{
	auto spActObj = SPROBJ->GetspActObjCurr();
	if( !spActObj )
		return;
	BOOL bAppend = FALSE;
	// 쉐도우키의 원본키를 카피해서 새키로 추가한다
	SELECT_LOOP( TOOL->GetShadowKey(), XBaseKey*, pKey ) {
		XBREAK( pKey->GetpShadowParent() == nullptr );
//		XBaseKey *pCopyKey = pKey->GetpShadowParent()->CopyDeep();	// 쉐도우키의 원본을 카피뜸
// 		pCopyKey->SetfFrame( pKey->GetfFrame() );	// 쉐도우키의 프레임값으로 교체
// 		spActObj->AddKey( pCopyKey );
		auto pCopyKey = spActObj->CreateNewInstanceKeyBySameLayer( pKey->GetpShadowParent() );
		pCopyKey->SetfFrame( pKey->GetfFrame() );	// 쉐도우키의 프레임값으로 교체
		// undo push
		UNDOMNG->PushUndo( new XUndoCreateKey( SPROBJ, spActObj->GetspAction(), pCopyKey, _T( "키 카피" ) ), bAppend );
		bAppend = TRUE;
	} END_LOOP;
	TOOL->GetShadowKey().Destroy();
	spActObj->SortKey();						// 키리스트를 프레임순서에 따라 다시 소트한다
}

/**
 @brief 타임라인 키 maxframe 드래그 처리
*/
void CFrameView::OnLButtonUpInTimelineByDragMaxFrame( 
																					UINT nFlags
																				, const XE::VEC2& vMouse )
{
	auto spAct = SPROBJ->GetspAction();
	if( !spAct )
		return;
	float frame = (int)( spAct->GetfMaxFrame() / spAct->GetSpeed() ) * spAct->GetSpeed();		// 최소키단위로 정렬
	if( nFlags & MK_SHIFT ) {
		float fToScale = frame / m_fClickMaxFrame;		// 최초클릭했을때 맥스프레임에서 얼마나 커지거나 작아졌나
		spAct->ScaleKey( fToScale );									// 전체키를 스케일링 한다
	}
	spAct->SetfMaxFrame( frame );
	m_pLastUndoAction->SetActionAfter( spAct );
}

/**
 @brief 타임라인 키 maxframe 드래그 처리
*/
void CFrameView::OnLButtonUpInTimelineByDragBlockSelectKey( 
																					UINT nFlags
																				, const XE::VEC2& vMouse )
{
	auto spAct = SPROBJ->GetspAction();
	if( !spAct )
		return;
	// 영역드래그 하고 버튼을 떼면 영역내 모든 키가 셀렉트 된다
	XBaseKey *pKey = nullptr;
	spAct->GetNextKeyClear();
	if( nFlags & MK_CONTROL ) {
		while( pKey = spAct->GetNextKey() ) {
			if( pKey->IsInside( m_vDragStart, m_vDragEnd, m_fScale ) ) {
				if( TOOL->GetSelectKey().Find( pKey ) )		// 이미 선택되어 있는 키는 선택 해제한다
					TOOL->GetSelectKey().Del( pKey );
				else
					TOOL->GetSelectKey().Add( pKey );		// 현재 선택키 목록에 추가시킨다
			}
		}
	} else {
		// CTRL을 누르지않고 영역 드래그 했다
		TOOL->GetSelectKey().Clear();	// 선택되어있던키 모두 클리어 
		while( pKey = spAct->GetNextKey() ) {
			if( pKey->IsInside( m_vDragStart, m_vDragEnd, m_fScale ) ) {
				TOOL->GetSelectKey().Add( pKey );
			}
		}
	}
	GetDlgBarTimeline()->m_nKeyEditMode = KEYEDIT_MOVE;	// 영역 드래그 해제
}
/**
 @brief 타임라인 영역에서 드래그 없이 Up이벤트 처리
*/
void CFrameView::OnLButtonUpInTimelineByNotDrag( 
																					UINT nFlags
																				, const XE::VEC2& vMouse )
{
	auto spAct = SPROBJ->GetspAction();
	if( !spAct )
		return;
	// PushKey
	XSPAction spAction = SPROBJ->GetspAction();
	spAction->GetNextKeyClear();		// ?? 쓰는데가 있음?
	if( m_pPushKey ) {
		if( m_pPushKey->IsInside( vMouse, m_fScale ) )
			ProcSelectKeyWhenButtUp( m_pPushKey, nFlags );
	} else
		TOOL->GetSelectKey().Clear();
	auto spSelLayer = GetspLayerCurrMouse( vMouse );
	if( spSelLayer )
		TOOL->SetspSelLayer( spSelLayer );
	MoveFrame( vMouse );			// 현재 마우스 좌표위치로 현재프레임을 이동시킴
}


void CFrameView::MoveFrame( const CPoint& point )
{
	if( point.x < TRACK_X )	return;
	float fScale = m_fScale;
	XSPAction spAction = SPROBJ->GetspAction();
	if( !spAction ) return;
	float fSpeed = (spAction) ? spAction->GetSpeed() : 0.2f;
	float fGridPerFrame = 1.0f / fSpeed;			// 1프레임당 작은눈금수 = 1.0 / ani speed;
	float fWidthPerFrame = (KEY_WIDTH * fGridPerFrame);		// 1.0프레임당 가로길이

	float fFrame = m_fStartFrame + ((point.x - TRACK_X) / fWidthPerFrame);
	fFrame = (int)(fFrame / spAction->GetSpeed()) * spAction->GetSpeed();		// speed단위로 정렬되도록 한다. 스피드가 0.2일때 fFrame이 0.5라면 0.4로 맞춤
	SPROBJ->MoveFrame( fFrame );
	//
// 	auto spLayer = TOOL->GetspSelLayer();
// 	if( spLayer ) {
// 		auto pLayerMove = SafeCast<XLayerMove*>( spLayer.get() );
// 		if( pLayerMove ) {
// 			auto vPos = pLayerMove->GetPos();
// 			m_strPos.Format( _T( "pos:\n%.1f,%.1f" ), vPos.x, vPos.y );
// 		}
// 	}
	UpdateKeyInfo();
}

// frame위치가 트랙뷰의 중앙에 오도록 자동스크롤시킨다
void CFrameView::MoveFrame( float frame )
{
	float fScale = m_fScale;
	XSPAction spAction = SPROBJ->GetspAction();
	if( !spAction ) return;
	float fSpeed = (spAction) ? spAction->GetSpeed() : 0.2f;
	float fGridPerFrame = 1.0f / fSpeed;			// 1프레임당 작은눈금수 = 1.0 / ani speed;
	float fWidthPerFrame = (KEY_WIDTH * fGridPerFrame);		// 1.0프레임당 가로길이
	CRect rect;
	GetClientRect( &rect );

	float frameCenter = ((rect.right - TRACK_X) * 0.5f) / fWidthPerFrame;		// 트랙의 중앙지점의 프레임번호
	m_fStartFrame = frame - frameCenter;
	if( m_fStartFrame < 0 )
		m_fStartFrame = 0;
	SPROBJ->MoveFrame( frame );
	UpdateKeyInfo();
}

void CFrameView::UpdateKeyInfo()
{
	auto spLayer = TOOL->GetspSelLayer();
	if( spLayer && spLayer->IsTypeLayerMove() ) {
		auto pLayerMove = SafeCast<XLayerMove*>( spLayer.get() );
		if( pLayerMove ) {
			const auto& cnPos = pLayerMove->GetcnPos();
			const auto& cnRot = pLayerMove->GetcnRot();
			const auto& cnScale = pLayerMove->GetcnScale();
			const auto& cnEff = pLayerMove->GetcnEffect();
			m_strPos.Format(_T("pos:\n%.1f, %.1f"), cnPos.m_vPos.x, cnPos.m_vPos.y );
			m_strRot.Format(_T("rot:\n%.1f"), cnRot.fAngle );
			m_strScale.Format(_T("scale:\n%.2f, %.2f"), cnScale.vScale.x, cnScale.vScale.y );
			m_strAlpha.Format(_T("alpha:\n%.2f"), cnEff.fAlpha );
		}
	} else {
		m_strPos.Empty();
		m_strRot.Empty();
		m_strScale.Empty();
		m_strAlpha.Empty();
	}
}

BOOL CFrameView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( zDelta < 0 ) {
		m_nScale -= 5;
		if( m_nScale < 10 )
			m_nScale = 10;
	} else {
		m_nScale += 5;
		if( m_nScale > 200 )
			m_nScale = 200;
	}
	m_fScale = m_nScale / 100.f;
	CWnd *pWnd = GetDlgBarTimeline()->GetDlgItem( IDC_TEXT_SCALE );
	if( pWnd ) {
		CString str;
		str.Format( _T("%d%%"), (int)(m_fScale*100.0f) );
		pWnd->SetWindowText( (LPCTSTR)str );
	}
	Invalidate(0);

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CFrameView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg->message == WM_KEYDOWN )
	{
	}
	return CView::PreTranslateMessage(pMsg);
}

void CFrameView::OnDelKey()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XSPAction spAction = SPROBJ->GetspAction();
	if( !spAction ) return;
	if( TOOL->GetSelectKey().GetNum() <= 0 )
		return;
	BOOL bAppend = FALSE;
	XBaseKey *pToolTipKey;
	TOOL->GetSelectKey().GetNextClear();
	while( auto pKey = static_cast<XBaseKey*>( TOOL->GetSelectKey().GetNext() ) )		// 선택된 모든키들을 루프돈다
	{
		TOOL->GetSelToolTipKey().GetNextClear();
		while( pToolTipKey = (XBaseKey *)TOOL->GetSelToolTipKey().GetNext() )
		{
			if( pToolTipKey == pKey ) {
				TOOL->GetSelToolTipKey().Del( pToolTipKey );		// 삭제하려는 키가 툴팁키리스트에 있는지 보고 있으면 삭제
				break;
			}
		}
		UNDOMNG->PushUndo( new XUndoDelKey( SPROBJ, spAction, pKey ), bAppend );
		bAppend = TRUE;
		SPROBJ->DelKey( spAction, pKey );								// 키를 삭제하고
	}
	TOOL->GetSelectKey().Clear();								// 선택했던 키를 지웠으니 클리어 시켜야함
	SPROBJ->MoveFrame();					// 현재프레임까지 다시 실행시킴
	Invalidate( FALSE );
	
}

// 현재 프레임에 XSprObj객체를 생성하는 키를 삽입한다
void CFrameView::OnBnClickedButtCreateobj()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	XSPAction spAction = SPROBJ->GetspAction();
	if( !spAction ) return;
	// 레이어의 유효성 검사
	auto spBaseLayer = TOOL->GetspSelLayer();
	if( spBaseLayer && spBaseLayer->IsTypeLayerObj() ) {
		XSPLayerObject spSelLayer = std::static_pointer_cast<XLayerObject>( spBaseLayer );
		if( spSelLayer ) {
			if( XASSERT( spSelLayer->IsSameType( xSpr::xLT_OBJECT ) ) ) {
				//
				CFileDialog	dlg( true, _T( "spr" ), nullptr, OFN_FILEMUSTEXIST,
					_T( "spr Files(*.spr)|*.spr|AllFiles(*.*)|*.*||" ), nullptr );
				if( dlg.DoModal() == IDOK ) {
					spAction->AddKeyCreateObj( XE::GenerateID(),
												(LPCTSTR)dlg.GetFileName(), 
												0, 
												xRPT_1PLAY,		// idAct를 0으로 넣어도 키추가한후 MoveFrame()을 하기때문에 key::execute()가 실행되서 m_nAction값은 제대로 채워진다
												spSelLayer,
												SPROBJ->GetFrmCurr(), 0, 0 );		// 0, 0은 부모로부터의 로컬좌표다
					SPROBJ->MoveFrame( SPROBJ->GetFrmCurr() );
					Invalidate( FALSE );
					GetAnimationView()->Update();
					GetFrameView()->Update();
				}
// 				spSelLayer = SPROBJ->GetLayer( XBaseLayer::xOBJ_LAYER, 0 );
// 				if( spSelLayer == nullptr ) {
// 					XALERT( "object layer가 없습니다" );
// 					return;
// 				}
			}
		}
// 		else {
// 			spSelLayer = SPROBJ->GetLayer( XBaseLayer::xOBJ_LAYER, 0 );
// 		}
	} else {
		CONSOLE("오브젝트 레이어를 선택하십시오.");
	}
}
/*
void CFrameView::UpdateSlider()
{
	CSliderCtrl *pSlider = (CSliderCtrl *)GetDlgItem( IDC_SLIDER_SPEED );
	if( pSlider )
	{
		pSlider->SetRange( 10, 100 );
		pSlider->SetPos( (int)(SPROBJ->GetSpeedCurrentAction() * 100) );
		pSlider->SetTicFreq( 5 );
		TCHAR szBuff[16];
		_stprintf_s( szBuff, _T("%1.2f"), SPROBJ->GetSpeedCurrentAction() );
		GetDlgItem( IDC_TEXT_SPEED )->SetWindowText( szBuff );
	}
}
*/
void CFrameView::Update()
{
	GetDlgBarTimeline()->Update();
	//
// 	XSPAction spAction = SPROBJ->GetspAction();
// 	if( spAction ) {
// // fuck		m_comboPlayMode.SetCurSel( (int)spAction->GetPlayMode() );
// 	}
	
	Invalidate( FALSE );
}

void CFrameView::UpdateOfsYBySelectedLayer()
{
	XSPAction spAction = SPROBJ->GetspAction();
	if( spAction ) {
		// 현재 선택된 레이어가 화면을 벗어났다면 자동으로 세로로 스크롤시켜준다.
		RECT rectClient;
		GetClientRect( &rectClient );
		auto spSelLayer = TOOL->GetspSelLayer();
		if( spSelLayer ) {
			float bottomLayer = TOOLBAR_HEIGHT + spSelLayer->Getscry() + spSelLayer->GetHeight();
			if( bottomLayer > rectClient.bottom ) {
				m_ofsY = -( ((int)bottomLayer - (rectClient.bottom - (int)TOOLBAR_HEIGHT)) + 40 );
			} 
#pragma message("FocusLayer개념으로 만들어야함.")
			/*
			 선택된 레이어가 타임라인에 보이는게 목적이고 선택된 레이어를 우선 타임라인뷰의 중간에 오도록 하고
			 만약 0번레이어나 마지막레이어라면 보정하는 방식으로
			*/
		}
	}
	Update();
}
/*
void CFrameView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.&
	if( nSBCode == SB_THUMBTRACK )
	{
		if( pScrollBar == GetDlgItem( IDC_SLIDER_SPEED ) )
		{
			CSliderCtrl *pSliderCtrl = (CSliderCtrl *)pScrollBar;
			SPROBJ->SetSpeedCurrentAction( pSliderCtrl->GetPos() / 100.0f );
			TCHAR szBuff[16];
			_stprintf_s( szBuff, _T("%1.2f"), SPROBJ->GetSpeedCurrentAction() );
			GetDlgItem( IDC_TEXT_SPEED )->SetWindowText( szBuff );
			Invalidate( FALSE );
		}
	}

	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}
*/
void CFrameView::PopupKeyFrame( CPoint point )
{
	if( SPROBJ->IsPlaying() ) {
		SPROBJ->SetPause( TRUE );			// 플레이중이었으면 플레이 멈춤
		return;
	}
	CMenu		menu, *popup;
	POINT	p;
	
	menu.LoadMenu( IDR_POPUP_KEYFRAME );
	popup = menu.GetSubMenu(0);
	
	GetCursorPos( &p );
	popup->TrackPopupMenu( TPM_CENTERALIGN | TPM_LEFTBUTTON, 
		p.x, p.y, AfxGetMainWnd() );
	
	menu.DestroyMenu();
}
void CFrameView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	// 우클릭한 위치로 프레임 이동
	m_vRMouse = point;	// 우클릭한 마우스 위치
	// 맨위 눈금부분 우클릭
	if( point.y <= (int)KEY_HEIGHT ) {
		CMenu		menu, *popup;
		POINT	p;
		menu.LoadMenu( IDR_POPUP_GRID );
		popup = menu.GetSubMenu(0);
		GetCursorPos( &p );
		popup->TrackPopupMenu( TPM_CENTERALIGN | TPM_LEFTBUTTON, 
			p.x, p.y, AfxGetMainWnd() );
		menu.DestroyMenu();
	} else {
	// 타임라인 부분 우클릭
		MoveFrame( point );
		PopupKeyFrame( point );
	}
	CView::OnRButtonDown(nFlags, point);
}

void CFrameView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	__super::OnRButtonUp(nFlags, point);
}

/*
void CFrameView::OnAddLayerImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	SPAction spAction = SPROBJ->GetAction();
	if( spAction == nullptr )	return;
	SPBaseLayer spLayer = SPROBJACT->AddImgLayer();
//	Invalidate( FALSE );
//	GetDlgBarTimeline()->Invalidate( FALSE );
//	GetWndTimeline()->Invalidate( FALSE );
//	TOOL->SetspSelLayer( spLayer );		// 새로생긴 레이어를 자동으로 선택해준다
}

void CFrameView::OnAddLayerObject()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	SPAction spAction = SPROBJ->GetAction();
	if( spAction == nullptr )	return;
	SPBaseLayer spLayer = SPROBJACT->AddObjLayer();
	Invalidate( FALSE );
	TOOL->SetspSelLayer( spLayer );		// 새로생긴 레이어를 자동으로 선택해준다
}
*/	
void CFrameView::OnAddLayerSound()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	auto spActObj = SPROBJ->GetspActObjCurr();
	if( !spActObj )
		return;
	XSPAction spAction = SPROBJ->GetspAction();
	if( spAction == nullptr )	return;
// 	SPBaseLayer spLayer = SPROBJACT->AddSndLayer();
	auto spLayer = spActObj->AddLayerAuto<XLayerSound>();
	UpdateOfsYBySelectedLayer();
	Invalidate( FALSE );
	TOOL->SetspSelLayer( spLayer );		// 새로생긴 레이어를 자동으로 선택해준다
}

// 선택한 레이어 삭제
void CFrameView::OnDelSelLayer()
{
	XSPAction spAction = SPROBJ->GetspAction();
	if( spAction == nullptr )	return;
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( SPROBJ->IsPlaying() )		return;
	XSPBaseLayer spLayer = TOOL->GetspSelLayer();
	if( spLayer == nullptr )		return;
	int sel = XALERT_YESNO( "선택한 레이어 [%s]를 삭제하시겠습니까?", spLayer->GetTypeString() );
	if( sel == IDYES ) {
		BOOL bAppend = FALSE;
		BOOL bDelOnlyLayer = FALSE;
		if( bDelOnlyLayer == FALSE ) {
			// 레이어삭제로 인해 사라지게될 키들의 언두를 푸쉬시킨다
			GET_NEXT_KEY_LOOP( spAction, pKey ) {
				if( spLayer->IsSameLayer( pKey ) ) {
					UNDOMNG->PushUndo( new XUndoDelKey( SPROBJ, spAction, pKey ), bAppend );
					bAppend = TRUE;
				}
			}
		}
		UNDOMNG->PushUndo( new XUndoDelLayer( SPROBJ, spLayer ), bAppend );
		SPROBJ->DelLayer( TOOL->GetspSelLayer(), bDelOnlyLayer );
		TOOL->SetspSelLayer( nullptr );			// 셀렉트 되었던 레이어를 클리어시킴
		TOOL->GetSelectKey().Clear();
	}
	Invalidate( FALSE );

}
void CFrameView::OnUpdateDelSelLayer(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}

void CFrameView::OnCopyLayer()
{
	// 현재 선택된 레이어를 복사해서 추가로 레이어를 만든다.
	if( TOOL->GetspSelLayer() )
		TOOL->AddLayerFromCopy( TOOL->GetspSelLayer() );
#pragma message("여기하다가 키 복사가 제대로 안되서 멈춤.")
}

/*
void CFrameView::OnBnClickedRadioKeyMove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgBarTimeline()->m_nKeyEditMode = KEYEDIT_MOVE;
}

void CFrameView::OnBnClickedRadioKeyScale()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgBarTimeline()->m_nKeyEditMode = KEYEDIT_SCALE;
}
*/
// 키 수정 다이얼로그
void CFrameView::OnEditKey()
{
	VALID_ACTOBJ( spActObj );
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( TOOL->GetSelectKey().GetNum() == 0 )		// 선택된 키가 없으면 취소
		return;
	XBaseKey *pKey = static_cast<XBaseKey*>( TOOL->GetSelectKey().GetLast() );		// 마지막으로 선택된 키를 받는다
	auto pUndo = new XUndoKeyChange( SPROBJ, SPROBJ->GetspActObjCurr(), pKey );
	UNDOMNG->PushUndo( pUndo );
	if( pKey->EditDialog() )			// 각 키에 맞는 수정 다이얼로그를 띄움
	{
		if( pKey->GetpcLua() )
		{
			// 키에서 편집한 코드를 하나로 합친다
			char cBuff[ 0xffff ] = {0,};
			SPRDAT->CompositLuaCodes( cBuff, 0xffff );
			SPRDAT->SetLua( cBuff );		// 합쳐진 코드를 SprDat,에 넣는다
		}
		pUndo->SetKeyAfter( pKey );
		auto spLayer = spActObj->GetspLayer( pKey );
		if( XASSERT(spLayer) )
			pKey->Execute( spActObj, spLayer );
		Update();
	}
}

void CFrameView::OnUpdateTextPos(CCmdUI *pCmdUI)
{
	pCmdUI->SetText( m_strPos );
}
void CFrameView::OnUpdateTextRot( CCmdUI *pCmdUI )
{
	pCmdUI->SetText( m_strRot );
}
void CFrameView::OnUpdateTextScale( CCmdUI *pCmdUI )
{
	pCmdUI->SetText( m_strScale );
}
void CFrameView::OnUpdateTextAlpha( CCmdUI *pCmdUI )
{
	pCmdUI->SetText( m_strAlpha );
}
void CFrameView::OnUpdateEditKey(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( TOOL->GetSelectKey().GetNum() > 0 )
		pCmdUI->Enable(1);
	else
		pCmdUI->Enable(0);
}

void CFrameView::OnEditAni()
{
	XSPAction spAction = SPROBJ->GetspAction();
	if( !spAction ) return;
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDlgEditAni dlg;
	dlg.m_fMaxFrame = spAction->GetfMaxFrame();
	dlg.m_scaleX = spAction->GetvScale().x;
	dlg.m_scaleY = spAction->GetvScale().y;
	dlg.m_rotateZ = spAction->GetvRotate().z;
	dlg.m_RotateY = spAction->GetvRotate().y;
	if( dlg.DoModal() == IDOK )
	{
		spAction->SetfMaxFrame( dlg.m_fMaxFrame );
		spAction->SetvScale( XE::VEC2( dlg.m_scaleX, dlg.m_scaleY ) );
		spAction->SetvRotate( XE::VEC3( 0, dlg.m_RotateY, dlg.m_rotateZ ) );
		Update();
	}
}

void CFrameView::OnUpdateEditAni(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	XSPAction spAction = SPROBJ->GetspAction();
	if( spAction ) 
		pCmdUI->Enable(1);
	else
		pCmdUI->Enable(0);
}



void CFrameView::OnBnClickedReloadSound()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SOUNDMNG->RemoveAll();		// 플레이를 멈추면 사운드 모두 삭제. 에디팅중에 사운드 파일이 바뀔수도 있기때문에 항상 새로갱신하기 위해 삭제함
	CONSOLE( "사운드 Reload 완료" );
}


void CFrameView::OnSetRepeatmark()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XSPAction spAction = SPROBJ->GetspAction();
	if( !spAction )		return;
	spAction->SetRepeatMark( SPROBJ->GetFrmCurr() );		// 클릭한 위치에 도돌이표를 찍는다
	Invalidate(FALSE);
}
// 첫프레임으로 이동
void CFrameView::OnMoveStart()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XSPAction spAction = SPROBJ->GetspAction();
	if( !spAction ) return;
//	SPROBJ->MoveFrame( 0 );
	MoveFrame( 0 );
}
// 마지막 프레임으로 이동
void CFrameView::OnMoveEnd()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XSPAction spAction = SPROBJ->GetspAction();
	if( !spAction ) return;
	SPROBJ->MoveFrame( spAction->GetfMaxFrame() );
}

void CFrameView::OnCbnSelchangeCombo1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	XSPAction spAction = SPROBJ->GetspAction();
	if( !spAction ) return;
//fuck	spAction->SetPlayMode( (xRPT_TYPE)m_comboPlayMode.GetCurSel() );
	SPROBJ->SetPlayType( spAction->GetPlayMode() );

}
// 레이어 보이기/감추기
void CFrameView::OnShowLayer()
{
	if( TOOL->GetspSelLayer() ) {
		TOOL->GetspSelLayer()->SetbShow( !TOOL->GetspSelLayer()->GetbShow() );	// 선택된 레이어를 보이거나 감춤
		Update();
	}
}
void CFrameView::OnUpdateShowLayer(CCmdUI *pCmdUI)
{
	if( TOOL->GetspSelLayer() )
		pCmdUI->Enable(1);
	else
		pCmdUI->Enable(0);
}
/**
 @brief 선택한 레이어를 제외한 나머지 레이어를 감춘다.
*/
void CFrameView::OnShowThisLayerOnly()
{
	// 선택한 레이어를 제외한 나머지 레이어를 감춘다.
	auto spSelActObj = TOOL->GetSelActObj();
	if( spSelActObj ) {
		spSelActObj->SetShowLayerExcept( TOOL->GetspSelLayer() );
		Update();
	}
}
/**
 @brief 모든 레이어를 보이게 한다.
*/
void CFrameView::OnShowAllLayer()
{
	auto spSelActObj = TOOL->GetSelActObj();
	if( spSelActObj ) {
		spSelActObj->SetShowLayerAll();
		Update();
	}
}

// 우클릭한 위치에 키를 생성한다
void CFrameView::OnCreateKey()
{
// 	// 우클릭한 위치가 어떤 레이어, 어떤 서브 레이어인지 파악
// 	XSPAction spAction = SPROBJ->GetspAction();
// 	if( !spAction ) return;
// 	if( SPROBJ->GetPause() ) {		// 애니메이션이 멈춰있을때만 처리
// 		if( m_vRMouse.x >= TRACK_X ) {		// 키프레임 영역 클릭
// 			// 우클릭지점의 레이어를 선택
// 			xSpr::xtLayerSub subType;		// 클릭한곳의 구체적인 서브레이어
// 			auto spLayer = spAction->CheckPushLayer( m_vRMouse.x, (m_vRMouse.y - KEY_HEIGHT), 0, &subType );
// 			if( spLayer ) {
// 				TOOL->SetspSelLayer( spLayer );
// 				// 각 서브레이어에 맞는 다이얼로그를 띄워 키를 생성
// 				auto pNewKey = spLayer->DoCreateKeyDialog( spAction, SPROBJ->GetFrmCurr(), subType );
// 				if( pNewKey )
// 					pNewKey->Execute( SPROBJ );
// 			}
// 		}
// 	} 
// 	Update();
}

void CFrameView::OnCreateKeyCurrChannel()
{
	VALID_ACTOBJ( spActObj );
	// 우클릭한 위치가 어떤 레이어, 어떤 서브 레이어인지 파악
	XSPAction spAction = SPROBJ->GetspAction();
	if( !spAction ) return;
	if( SPROBJ->GetPause() ) {		// 애니메이션이 멈춰있을때만 처리
		if( m_vRMouse.x >= TRACK_X ) {		// 키프레임 영역 클릭
			// 우클릭지점의 레이어를 선택
			xSpr::xtLayerSub subType;		// 클릭한곳의 구체적인 서브레이어
			auto spLayer = CheckPushLayer( m_vRMouse.x, ( m_vRMouse.y - KEY_HEIGHT ), 0, &subType );
			if( spLayer ) {
				TOOL->SetspSelLayer( spLayer );
				// 각 서브레이어에 맞는 다이얼로그를 띄워 키를 생성
				auto pNewKey = spLayer->DoCreateKeyDialog( spAction, SPROBJ->GetFrmCurr(), subType );
				if( pNewKey )
					pNewKey->Execute( spActObj, spLayer );
			}
		}
	}
	Update();
}


void CFrameView::OnCreatePosKey()
{
	CreateKeyByTypeChannel( xSpr::xLTS_POS );
}


void CFrameView::OnCreateRotKey()
{
	CreateKeyByTypeChannel( xSpr::xLTS_ROT );
}


void CFrameView::OnCreateKeyScale()
{
	CreateKeyByTypeChannel( xSpr::xLTS_SCALE );
}


void CFrameView::OnCreateKeyEffect()
{
	CreateKeyByTypeChannel( xSpr::xLTS_EFFECT );
}
/**
 @brief 현재 마우스 위치의 레이어를 선택하고 typeChannel의 키를 생성한다.
*/
XBaseKey* CFrameView::CreateKeyByTypeChannel( xSpr::xtLayerSub typeChannel )
{
	auto spActObj = SPROBJ->GetspActObjCurr();
	if( !spActObj )
		return nullptr;
	XBaseKey *pNewKey = nullptr;
	XSPAction spAction = SPROBJ->GetspAction();
	if( !spAction ) 
		return nullptr;
	if( SPROBJ->GetPause() ) {		// 애니메이션이 멈춰있을때만 처리
		if( m_vRMouse.x >= TRACK_X ) {		// 키프레임 영역 클릭
			// 우클릭지점의 레이어를 선택
			auto spLayer = CheckPushLayer( m_vRMouse.x
																		 , (m_vRMouse.y - KEY_HEIGHT)
																		 , 0
																		 , nullptr );
			if( spLayer ) {
				TOOL->SetspSelLayer( spLayer );
				// typeChannel키를 생성한다.
				pNewKey = spLayer->DoCreateKeyDialog( spAction
																								, SPROBJ->GetFrmCurr()
																								, typeChannel );
				if( pNewKey )
					pNewKey->Execute( spActObj, spLayer );
			}
		}
	}
	Update();
	return pNewKey;
}

BOOL CFrameView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( m_bPanningMode )
		SetCursor( AfxGetApp()->LoadCursor( IDC_HAND1 ) );
	else
		SetCursor( AfxGetApp()->LoadStandardCursor( IDC_ARROW ) );

	return TRUE;
//	return __super::OnSetCursor(pWnd, nHitTest, message);
}

// 휠버튼 누르면 패닝
void CFrameView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	SetCapture();
	m_bPanningMode = TRUE;
	StartDragMode( xDRAG_PANNING );
	Invalidate(0);
	__super::OnMButtonDown(nFlags, point);
}

void CFrameView::OnMButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bPanningMode = FALSE;
	if( IsDragMode( xDRAG_PANNING ) )
		ClearDragMode();
	m_bTrackPush = FALSE;
	m_alScrollDir = XE::xAL_NONE;
	__super::OnMButtonUp(nFlags, point);
}

// 맨위 눈금 표시 단위 프레임으로
void CFrameView::OnGridFrame()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_GridUnit = xGU_FRAME;
	TOOL->SaveINI();
	Invalidate(0);
}

void CFrameView::OnUpdateGridFrame(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( m_GridUnit == xGU_FRAME )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}

// 맨위 눈금 표시 단위 0.1초 단위로
void CFrameView::OnGridTime()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_GridUnit = xGU_TIME;
	TOOL->SaveINI();
	Invalidate(0);
}

void CFrameView::OnUpdateGridTime(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( m_GridUnit == xGU_TIME )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}
// 전체키를 선택한다
void CFrameView::OnSelectAllKey()
{
	if( SPROBJ == nullptr )	return;
	if( SPROBJ->GetspAction() == nullptr )	return;

	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	TOOL->GetSelectKey().Clear();
	XBaseKey *pKey = nullptr;
	SPROBJ->GetspAction()->GetNextKeyClear();
	while( pKey = SPROBJ->GetspAction()->GetNextKey() )
	{
		TOOL->GetSelectKey().Add( pKey );
	}
	Invalidate(0);
}

/**
 @brief 현재 위치 이후의 모든 키 선택
*/
void CFrameView::OnSelectAllKeyAfter()
{
	if( !SPROBJ )
		return;
	auto spActDat = SPROBJ->GetspAction();
	if( !spActDat )
		return;
	TOOL->GetSelectKey().Clear();
	const auto frameCurr = SPROBJ->GetFrmCurr();
	auto& listKey = spActDat->GetlistKey();
	for( auto pKey : listKey ) {
		if( pKey->GetfFrame() >= frameCurr )
			TOOL->GetSelectKey().Add( pKey );
	}
	GetFrameView()->Update();
}

/**
 @brief 현재 레이어의 현재위치 이우희 모든 키 선택
*/
void CFrameView::OnSelectAllKeyAfterInLayer()
{
	if( !SPROBJ )
		return;
	auto spActDat = SPROBJ->GetspAction();
	if( !spActDat )
		return;
	auto spLayerDat = TOOL->GetspSelLayer();
	if( !spLayerDat )
		return;
	TOOL->GetSelectKey().Clear();
	const auto frameCurr = SPROBJ->GetFrmCurr();
	auto& listKey = spActDat->GetlistKey();
	for( auto pKey : listKey ) {
		if( spLayerDat->GetidLayer() == pKey->GetidLayer()
			&& pKey->GetfFrame() >= frameCurr )
				TOOL->GetSelectKey().Add( pKey );
	}
	GetFrameView()->Update();
}

bool compFrame( XBaseElem *pKey1, XBaseElem *pKey2 )
{
	XBaseKey *pBaseKey1 = SafeCast<XBaseKey*, XBaseElem*>( pKey1 );
	XBaseKey *pBaseKey2 = SafeCast<XBaseKey*, XBaseElem*>( pKey2 );
	return pBaseKey1->GetfFrame() < pBaseKey2->GetfFrame();
}

// void CFrameView::OnSetPath()
// {
// 	// TODO: 여기에 명령 처리기 코드를 추가합니다.
// 	// 셀렉트된 키들을 일단 소트시킴
// 	TOOL->GetSelectKey().Sort( compFrame );
// 	// 선택된 키의 시작키와 끝키사이 키중 pos키만 찾아낸다
// 	XList<XKeyPos*> listPathKey;
// 	{
// 		// 시작 KeyPos타입 키를 찾는다
// 		XBaseKey *pStartKey = nullptr; //SafeCast<XBaseKey*, XBaseElem*>( TOOL->GetSelectKey().GetFirst() );
// 		SELECT_LOOP( TOOL->GetSelectKey(), XBaseKey*, pKey )
// 		{
// 			if( pKey->GetSubType() == xSpr::xKTS_POS )
// 			{
// 				pStartKey = pKey;
// 				break;
// 			}
// 		} END_LOOP;
// 		XBaseKey *pEndKey = SafeCast<XBaseKey*, XBaseElem*>( TOOL->GetSelectKey().GetLast() );		// 선택한 키중 마지막 키
// 		XBaseKey *pCurrKey = pStartKey;
// 		XKeyPos* pCurrPosKey = nullptr;
// 		if( pCurrKey )
// 		{
// 			do
// 			{
// 				pCurrPosKey = SafeCast<XKeyPos*, XBaseKey*>( pCurrKey );
// 				if( pCurrPosKey )
// 					listPathKey.Add( pCurrPosKey );		// 패스에 속하는 키들을 모두 찾아낸다
// 				if( pCurrKey == pEndKey )
// 					break;
// 			} while( pCurrKey = pCurrKey->GetNextKey() );
// 		}
// 	}
// 	//
// 	if( listPathKey.Size() == 0 )
// 	{
// 		CONSOLE( "PosKey를 찾을 수 없었습니다." );
// 		return;
// 	}
// 	if( listPathKey.Size() == 1 )	// 키를 하나만 설정한경우
// 	{
// 		CONSOLE( "패스는 최소 2개키가 필요합니다. 패스명령을 취소합니다" );
// 		return;
// 	}
// 	// 선택한 키중 이미 패스로 지정된 키가 있는지 찾아서 처리한다
// 	// 다른패스의 키지만 그패스의 끝키면 공유가 된다
// 	XLIST_LOOP( listPathKey, XKeyPos*, pPosKey ) {
// 		if( pPosKey->GetidPath() )		// 이미 패스로 지정된 키다
// 		{
// 			if( pPosKey->GetpPrevPathKey() && pPosKey->GetpNextPathKey() )		// 앞뒤에 연결되어 있다면 헤드나 꼬리가 아니다
// 			{
// 				XALERT( "%.1f프레임위치의 키가 이미 다른패스로 지정되어 있습니다.", pPosKey->GetfFrame() );
// 				return;
// 			}
// 		}
// 	} END_LOOP;
// 	// 패스에 속하는 키들을 앞뒤로 연결시키고 패스설정을 한다.
// 	{
// 		ID idPath = XE::GenerateID();
// 		BYTE r = (BYTE)xRandom( 128, 255 );
// 		BYTE g = (BYTE)xRandom( 128, 255 );
// 		BYTE b = (BYTE)xRandom( 128, 255 );
// 		XCOLOR colPath = XCOLOR_RGBA( r, g, b, 255 );
// 		listPathKey.GetNextClear();
// 		XKeyPos *pStartKey = listPathKey.GetNext();
// 		pStartKey->SetidPrevPathKey( 0 );		// 시작키의 prev는 널이다
// 		XKeyPos *pCurrKey = pStartKey;
// 		XKeyPos *pNextPosKey = nullptr;
// 		while( pNextPosKey = listPathKey.GetNext() )
// 		{
// 			pCurrKey->SetidNextPathKey( pNextPosKey->GetidKey() );	
// 			if( pNextPosKey->IsHeadPath() == FALSE )		// 다음키가 어떤 패스의 헤드가 아닐때만
// 				pNextPosKey->SetidPrevPathKey( pCurrKey->GetidKey() );	// 다음키의 '앞'에 currkey를 연결시킨다
// 			pCurrKey->SetidPath( idPath );
// 			pCurrKey->SetPathType( SPR::xLINE_SPLINE_CATMULLROM );
// 			pCurrKey->SetcolPath( colPath );
// //			pCurrKey->SetInterpolation( xSpr::xNONE );
// 			pCurrKey = pNextPosKey;
// 		}
// 		if( pCurrKey->GetpNextPathKey() == nullptr )	// 패스의 마지막키가 다른키가 연결되어 있지 않을때만
// 		{
// 			pCurrKey->SetidNextPathKey( 0 );		// 맨 마지막키의 '뒤'는 없다
// 			pCurrKey->SetidPath( idPath );			// 패스아이디를 지정
// 			pCurrKey->SetcolPath( colPath );
// 		}
// 		// 마지막키가 다른패스의 헤드라면 헤드처리를 한다
// //		pCurrKey->SetidPrevPathKey( 0 );		// 
// //		pCurrKey->SetPathType( SPR::xLINE_SPLINE_CATMULLROM );
// //		pCurrKey->SetInterpolation( xSpr::xNONE );
// //		pStartKey->SetInterpolation( xSpr::xACCEL );		// 시작키에 보간방법을 넣어줌
// //		pStartKey->SetPathType( SPR::xLINE_SPLINE_CATMULLROM );
// 	}
// 	Invalidate(0);
// }

// void CFrameView::OnUpdateSetPath(CCmdUI *pCmdUI)
// {
// 	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
// 	if( TOOL->GetSelectKey().GetNum() > 0 )	// 일단은 키선택한게 있을때만 메뉴가 뜨도록
// 		pCmdUI->Enable(1);
// 	else
// 		pCmdUI->Enable(0);
// }
// // 선택한 키에 연결된 패스를 해제한다
// void CFrameView::OnReleasePath()
// {
// 	// TODO: 여기에 명령 처리기 코드를 추가합니다.
// 	if( TOOL->GetSelectKey().GetNum() <= 0 )		return;
// 	//
// 	XBaseKey *pKey = SafeCast<XBaseKey*, XBaseElem*>( TOOL->GetSelectKey().GetFirst() );
// 	if( pKey && pKey->GetSubType() == xSpr::xKTS_POS )		// 선택한키가 POS채널이고
// 	{
// 		XKeyPos *pPosKey = SafeCast<XKeyPos*, XBaseKey*>( pKey );
// 		if( pPosKey && pPosKey->GetidPath() )		// 패스로 설정된 키여야 해제가 가능하다
// 		{
// 			// 선택한 키가 속한 패스의 선두를 찾는다.
// 			XKeyPos *pCurrKey = pPosKey;
// 			while(1)
// 			{
// 				if( pCurrKey->GetpPrevPathKey() == nullptr )		// 헤드를 찾았다
// 					break;
// 				pCurrKey = pCurrKey->GetpPrevPathKey();			// 연결된 앞키를 따라간다
// 			}
// 			// 선두에서 부터 차례로 패스를 해제시킨다
// 			while( pCurrKey )
// 			{
// 				XKeyPos *pNextKey = pCurrKey->GetpNextPathKey();
// 				pCurrKey->SetidPath( 0 );
// 				pCurrKey->SetidNextPathKey( 0 );
// 				pCurrKey->SetidPrevPathKey( 0 );
// 				pCurrKey->SetPathType( SPR::xLINE_NONE );
// 				pCurrKey->SetInterpolation( xSpr::xLINEAR );		// 패스를 해제하면 일반적인 선형보간으로 바꿔준다
// 				pCurrKey = pNextKey;
// 			}
// 			Invalidate(0);
// 		}
// 	}
// }
// 
// void CFrameView::OnUpdateReleasePath(CCmdUI *pCmdUI)
// {
// 	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
// 	if( TOOL->GetSelectKey().GetNum() > 0 )		// 선택한 키가 있고
// 	{
// 		XBaseKey *pKey = SafeCast<XBaseKey*, XBaseElem*>( TOOL->GetSelectKey().GetFirst() );
// 		if( pKey && pKey->GetSubType() == xSpr::xKTS_POS )		// 선택한키가 POS채널이고
// 		{
// 			XKeyPos *pPosKey = SafeCast<XKeyPos*, XBaseKey*>( pKey );
// 			if( pPosKey && pPosKey->GetidPath() )		// 패스로 설정된 키여야 메뉴가 나타난다
// 			{
// 				pCmdUI->Enable(1);
// 			}
// 		}
// 	} else
// 		pCmdUI->Enable(0);
// 
// }
// 
// // 패스타입-라인 선택
// void CFrameView::OnPathTypeLine()
// {
// 	// TODO: 여기에 명령 처리기 코드를 추가합니다.
// 	SELECT_LOOP( TOOL->GetSelectKey(), XBaseKey*, pKey )
// 	{
// 		if( pKey->GetSubType() == xSpr::xKTS_POS )
// 		{
// 			XKeyPos *pKeyPos = XKeyPos::Cast( pKey );
// 			if( pKeyPos )
// 				pKeyPos->SetPathType( SPR::xLINE_LINE );
// 		}
// 	} END_LOOP;
// }
// 
// void CFrameView::OnUpdatePathTypeLine(CCmdUI *pCmdUI)
// {
// 	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
// 	if( TOOL->GetSelectKey().GetNum() > 0 )
// 	{
// 		XBaseKey *pKey = SafeCast<XBaseKey*, XBaseElem*>( TOOL->GetSelectKey().GetFirst() );
// 		if( pKey )
// 		{
// 			XKeyPos *pKeyPos = XKeyPos::Cast( pKey );
// 			if( pKeyPos && pKeyPos->GetPathType() == SPR::xLINE_LINE )
// 				pCmdUI->SetCheck(1);
// 			else
// 				pCmdUI->SetCheck(0);
// 		}
// 	} else
// 		pCmdUI->SetCheck(0);
// }
// 
// void CFrameView::OnPathTypeCatmullrom()
// {
// 	// TODO: 여기에 명령 처리기 코드를 추가합니다.
// 	SELECT_LOOP( TOOL->GetSelectKey(), XBaseKey*, pKey )
// 	{
// 		if( pKey->GetSubType() == xSpr::xKTS_POS )
// 		{
// 			XKeyPos *pKeyPos = XKeyPos::Cast( pKey );
// 			if( pKeyPos )
// 				pKeyPos->SetPathType( SPR::xLINE_SPLINE_CATMULLROM );
// 		}
// 	} END_LOOP;
// }
// 
// void CFrameView::OnUpdatePathTypeCatmullrom(CCmdUI *pCmdUI)
// {
// 	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
// 	if( TOOL->GetSelectKey().GetNum() > 0 )
// 	{
// 		XBaseKey *pKey = SafeCast<XBaseKey*, XBaseElem*>( TOOL->GetSelectKey().GetFirst() );
// 		if( pKey )
// 		{
// 			XKeyPos *pKeyPos = XKeyPos::Cast( pKey );
// 			if( pKeyPos && pKeyPos->GetPathType() == SPR::xLINE_SPLINE_CATMULLROM )
// 				pCmdUI->SetCheck(1);
// 			else
// 				pCmdUI->SetCheck(0);
// 		}
// 	} else
// 		pCmdUI->SetCheck(0);
// }
// 단구간 및 다구간의 보간구역 설정
// 선택된키의 시작/끝키를 보간구역으로 지정한다
void CFrameView::OnSetInterpolation()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	// 선택된 키들을 프레임순으로 소트한다
	TOOL->GetSelectKey().Sort( compFrame );
	// 시작키와 끝키를 지정한다
	// PosKey타입만 모두 골라낸다
	XList<XKeyPos*> listPosKey;
	SELECT_LOOP( TOOL->GetSelectKey(), XBaseKey*, pBaseKey )
	{
		if( pBaseKey->GetSubType() != xSpr::xKTS_POS )	continue;
		XKeyPos *pPosKey = XKeyPos::Cast( pBaseKey );
		listPosKey.Add( pPosKey );
	} END_LOOP;
	//
	if( listPosKey.Size() == 0 )
	{
		CONSOLE( "PosKey를 찾을 수 없었습니다." );
		return;
	}
	if( listPosKey.Size() == 1 )	// 키를 하나만 설정한경우
	{
		CONSOLE( "보간은 최소 2개키가 필요합니다. 보간설정 명령을 취소합니다" );
		return;
	}
	XKeyPos *pStartKey = listPosKey.GetFirst();
	XKeyPos *pEndKey = listPosKey.GetLast();
	//
	XDlgEditInterpolation dlg;
	dlg.m_Interpolation = pStartKey->GetInterpolation();
	if( dlg.DoModal() == IDOK )
	{
		pStartKey->SetInterpolation( dlg.m_Interpolation );
// 		pStartKey->SetpInterpolationEndKey( pEndKey );		// 시작키엔 보간끝키를 지정
		XLIST_LOOP( listPosKey, XKeyPos*, pKey )
		{
			if( pKey != pStartKey && pKey != pEndKey )
			{
				pKey->SetInterpolation( pStartKey->GetInterpolation() );		// 보간구간의 중간키는 시작키의 보간값으로 채움
// 				pKey->SetpInterpolationEndKey( nullptr );		// 보간구간 중간키는 끝키를 모두 없앰
			}
		} END_LOOP;
	}

}

// pKey1과 2사이를 잇는 보간 선을 그린다
void CFrameView::DrawInterpolationLinePos( XKeyPos *pKey1, XKeyPos *pKey2, const XE::VEC2& vOffset )
{
	if( pKey1 == nullptr )	return;
	if( pKey2 == nullptr )	return;
	float frameLength = pKey2->GetfFrame() - pKey1->GetfFrame();		// 보간구간 프레임길이
	if( frameLength == 0 )
		return;
	frameLength *= 10.0f;
	float i = 0;
	while(1)
	{
		if( i > frameLength )
			i = frameLength;
		float fTimeLerp = i / frameLength;		// 프레임길이의 프레임수만큼 쪼개서 보간한다
		i += 1.0f;
		if( fTimeLerp >= 1.0f )
			break;
		float fSpeedLerp;
		if( XLayerMove::sCalcInterpolation( &fSpeedLerp, fTimeLerp, pKey1->GetInterpolation() ) ) {
			XE::VEC2 vLength = pKey2->GetvDrawPos() - pKey1->GetvDrawPos();
			XE::VEC2 vCurr = vOffset + pKey1->GetvDrawPos() + vLength * fSpeedLerp;
			GRAPHICS->DrawPoint( vCurr, 1, XCOLOR_RED );
			GRAPHICS->DrawPoint( vCurr.x, vCurr.y-1.0f, 1, XCOLOR_RED );
			GRAPHICS->DrawPoint( vCurr.x, vCurr.y+1.0f, 1, XCOLOR_RED );
			GRAPHICS->DrawPoint( vCurr.x-1.0f, vCurr.y, 1, XCOLOR_RED );
			GRAPHICS->DrawPoint( vCurr.x+1.0f, vCurr.y, 1, XCOLOR_RED );
		}
	}

}
// void CFrameView::DrawInterpolationLine( XKeyPos *pKey1, XKeyPos *pKey2, const XE::VEC2& vOffset )
// {
// 	if( pKey1 == nullptr )	return;
// 	if( pKey2 == nullptr )	return;
// 	float frameLength = pKey2->GetfFrame() - pKey1->GetfFrame();		// 보간구간 프레임길이
// 	if( frameLength == 0 )
// 		return;
// 	frameLength *= 10.0f;
// 	float i = 0;
// 	while(1)
// 	{
// 		if( i > frameLength )
// 			i = frameLength;
// 		float fTimeLerp = i / frameLength;		// 프레임길이의 프레임수만큼 쪼개서 보간한다
// 		i += 1.0f;
// 		if( fTimeLerp >= 1.0f )
// 			break;
// 		float fSpeedLerp;
// 		if( XLayerMove::CalcInterpolation( &fSpeedLerp, fTimeLerp, pKey1->GetInterpolation() ) )
// 		{
// 			XE::VEC2 vLength = pKey2->GetvDrawPos() - pKey1->GetvDrawPos();
// 			XE::VEC2 vCurr = vOffset + pKey1->GetvDrawPos() + vLength * fSpeedLerp;
// 			GRAPHICS->DrawPoint( vCurr, 1, XCOLOR_RED );
// 			GRAPHICS->DrawPoint( vCurr.x, vCurr.y-1.0f, 1, XCOLOR_RED );
// 			GRAPHICS->DrawPoint( vCurr.x, vCurr.y+1.0f, 1, XCOLOR_RED );
// 			GRAPHICS->DrawPoint( vCurr.x-1.0f, vCurr.y, 1, XCOLOR_RED );
// 			GRAPHICS->DrawPoint( vCurr.x+1.0f, vCurr.y, 1, XCOLOR_RED );
// 		}
// 	}
// 
// }

// void CFrameView::DrawInterpolationLineByScale( XKeyScale *pKey1, XKeyScale *pKey2
// 																							, const XE::VEC2& vOffset )
// {
// 	if( pKey1 == nullptr )	return;
// 	if( pKey2 == nullptr )	return;
// 	float frameLength = pKey2->GetfFrame() - pKey1->GetfFrame();		// 보간구간 프레임길이
// 	if( frameLength == 0 )
// 		return;
// 	frameLength *= 10.0f;
// 	float i = 0;
// 	while( 1 ) {
// 		if( i > frameLength )
// 			i = frameLength;
// 		float fTimeLerp = i / frameLength;		// 프레임길이의 프레임수만큼 쪼개서 보간한다
// 		i += 1.0f;
// 		if( fTimeLerp >= 1.0f )
// 			break;
// 		float fSpeedLerp;
// 		if( XLayerMove::CalcInterpolation( &fSpeedLerp, fTimeLerp, pKey1->GetInterpolation() ) ) {
// 			XE::VEC2 vLength = pKey2->GetvDrawPos() - pKey1->GetvDrawPos();
// 			XE::VEC2 vCurr = vOffset + pKey1->GetvDrawPos() + vLength * fSpeedLerp;
// 			GRAPHICS->DrawPoint( vCurr, 1, XCOLOR_RED );
// 			GRAPHICS->DrawPoint( vCurr.x, vCurr.y - 1.0f, 1, XCOLOR_RED );
// 			GRAPHICS->DrawPoint( vCurr.x, vCurr.y + 1.0f, 1, XCOLOR_RED );
// 			GRAPHICS->DrawPoint( vCurr.x - 1.0f, vCurr.y, 1, XCOLOR_RED );
// 			GRAPHICS->DrawPoint( vCurr.x + 1.0f, vCurr.y, 1, XCOLOR_RED );
// 		}
// 	}
// 
// }

// void CFrameView::DrawInterpolationLineByBase( XBaseKey* pKey1, XBaseKey* pKey2
// 																			, const XE::VEC2& vOffset )
// {
// 	if( pKey1 == nullptr )	return;
// 	if( pKey2 == nullptr )	return;
// 	float frameLength = pKey2->GetfFrame() - pKey1->GetfFrame();		// 보간구간 프레임길이
// 	if( frameLength == 0 )
// 		return;
// 	frameLength *= 10.0f;
// 	float i = 0;
// 	while( 1 ) {
// 		if( i > frameLength )
// 			i = frameLength;
// 		float fTimeLerp = i / frameLength;		// 프레임길이의 프레임수만큼 쪼개서 보간한다
// 		i += 1.0f;
// 		if( fTimeLerp >= 1.0f )
// 			break;
// 		float fSpeedLerp;
// 		if( XLayerMove::CalcInterpolation( &fSpeedLerp, fTimeLerp, pKey1->GetInterpolation() ) ) {
// 			XE::VEC2 vLength = pKey2->GetvDrawPos() - pKey1->GetvDrawPos();
// 			XE::VEC2 vCurr = vOffset + pKey1->GetvDrawPos() + vLength * fSpeedLerp;
// 			GRAPHICS->DrawPoint( vCurr, 1, XCOLOR_RED );
// 			GRAPHICS->DrawPoint( vCurr.x, vCurr.y - 1.0f, 1, XCOLOR_RED );
// 			GRAPHICS->DrawPoint( vCurr.x, vCurr.y + 1.0f, 1, XCOLOR_RED );
// 			GRAPHICS->DrawPoint( vCurr.x - 1.0f, vCurr.y, 1, XCOLOR_RED );
// 			GRAPHICS->DrawPoint( vCurr.x + 1.0f, vCurr.y, 1, XCOLOR_RED );
// 		}
// 	}
// 
// }

/**
 @brief 타임트랙에 pPosKey의 보간라인을 그린다.
*/
void CFrameView::DrawInterpolationLineByPosKey( XKeyPos* pPosKey )
{
	// 패스로 연결되어 있는 다음키가 있으면 선을 그려준다
//	XKeyPos *pNextPosKey = pPosKey->GetpNextPathKey();
	XKeyPos *pNextPosKey = pPosKey->GetpNextPosKey();
	if( pNextPosKey ) {
		XE::VEC2 v1 = pPosKey->GetvDrawPos();		// 뷰포트 좌표를 빼줌
		XE::VEC2 v2 = pNextPosKey->GetvDrawPos();
//		XE::VEC2 vh( KEY_WIDTH / 3, KEY_HEIGHT / 3 );
		const auto vh = GetSizeKey(m_fScale) / 3;
		// 다음키와 선연결
		XE::VEC2 vl1 = v1 + vh;
		XE::VEC2 vl2 = v2 + vh;
		if( pNextPosKey->GetpPrevPosKey() != pPosKey )	// 뒷키의 '앞'이 현재키가 아니면 끊어진 선을 그림										
			vl2 -= ( vl2 - vl1 ) / 2.0f;
// 		XCOLOR col = pPosKey->GetcolPath();
// 		if( col == XCOLOR_WHITE )
// 			col = XCOLOR_BLACK;
		XCOLOR col = XCOLOR_BLACK;
		XE::DrawLine( vl1, vl2, col, 2 );
		// 보간연결선 그림
		XKeyPos *pEndKey = pPosKey->GetpInterpolationEndKey();
		DrawInterpolationLinePos( pPosKey, pEndKey, vh * 2.0f );	// vh * 2.0 = KEY_WIDTH*0.66(2/3)
	}
}
// void CFrameView::DrawInterpolationLineByScaleKey( XKeyScale* pKey )
// {
// 	// 보간으로 연결되어 있는 다음키가 있으면 선을 그려준다
// 	if( pKey->GetInterpolation() ) {
// 		auto pNextKey = SafeCast<XKeyScale*>( pKey->GetNextKey() );
// 		if( pNextKey ) {
// 			const auto v1 = pKey->GetvDrawPos();		// 뷰포트 좌표를 빼줌
// 			const auto v2 = pNextKey->GetvDrawPos();
// 			const auto vh = GetSizeKey(m_fScale) / 3;
// 			// 다음키와 선연결
// 			const auto vl1 = v1 + vh;
// 			const auto vl2 = v2 + vh;
// // 			const auto col = XCOLOR_BLACK;
// // 			XE::DrawLine( vl1, vl2, col, 2 );
// 			// 보간연결선 그림
// 			DrawInterpolationLineByScale( pKey, pNextKey, vh * 2.0f );	// vh * 2.0 = KEY_WIDTH*0.66(2/3)
// 		}
// 	}
// }
// void CFrameView::DrawInterpolationLineByBase( XKeyScale* pKey )
// {
// 	// 보간으로 연결되어 있는 다음키가 있으면 선을 그려준다
// 	if( pKey->GetInterpolation() ) {
// //		auto pNextKey = SafeCast<XKeyScale*>( pKey->GetNextKey() );
// 		auto pNextKey = pKey->GetNextKey();
// 		if( pNextKey ) {
// 			const auto v1 = pKey->GetvDrawPos();		// 뷰포트 좌표를 빼줌
// 			const auto v2 = pNextKey->GetvDrawPos();
// 			const auto vh = GetSizeKey( m_fScale ) / 3;
// 			// 다음키와 선연결
// 			const auto vl1 = v1 + vh;
// 			const auto vl2 = v2 + vh;
// // 			const auto col = XCOLOR_BLACK;
// // 			XE::DrawLine( vl1, vl2, col, 2 );
// 			// 보간연결선 그림
// 			DrawInterpolationLine( pKey, pNextKey, vh * 2.0f );	// vh * 2.0 = KEY_WIDTH*0.66(2/3)
// 		}
// 	}
// }

/**
@brief 주어진 좌표로 어느레이어를 눌렀는지 얻는다.
@param ly TOOLBAR_HEIGHT+KEY_HEIGHT를 뷰좌표에서 뺀 로컬좌표다
@param h TOOLBAR_HEIGHT+KEY_HEIGHT를 뷰좌표에서 뺀 로컬좌표다
보너스로 어느 서브레이어를 선택한건지도 pOutSubType에 담아준다
*/
XSPBaseLayer CFrameView::CheckPushLayer( float lx, float ly
																				, float h
																				, xSpr::xtLayerSub *pOutSubType )
{
	if( pOutSubType )
		*pOutSubType = xSpr::xLTS_NONE;
	float ori_h = h;
	auto& listLayer = SPROBJ->GetspActObjCurr()->GetlistLayer();
	for( auto& spLayer : listLayer ) {
		h = ori_h;
		if( h == 0 ) {
			// 레이어가 열려있으면
			if( spLayer->GetbOpen() ) {
				h = KEY_HEIGHT * 2;			// 기본으로 두줄을 검사하고
				if( spLayer->GetBitDrawChannel( CHA_ROT ) )
					h += KEY_HEIGHT;
				if( spLayer->GetBitDrawChannel( CHA_SCALE ) )
					h += KEY_HEIGHT;
				if( spLayer->GetBitDrawChannel( CHA_EFFECT ) )		// 이펙트 채널을 그리고 있으면
					h += KEY_HEIGHT;			// 한줄 더 늘인다
			} else
				h = KEY_HEIGHT;			// 레이어가 닫혀있으면 한줄만 검사하면 된다
		}
		if( ly >= spLayer->Getscry() && ly < spLayer->Getscry() + h ) {
			if( pOutSubType && ori_h == 0 ) {
				if( spLayer->GetbOpen() ) {
					int nArea = (int)((ly - spLayer->Getscry()) / KEY_HEIGHT);
					switch( nArea ) {
					case 0:	*pOutSubType = xSpr::xLTS_MAIN;	break;		// 레이어의 첫번째줄
					case 1:	*pOutSubType = xSpr::xLTS_POS;	break;
					case 2:	*pOutSubType = xSpr::xLTS_ROT;	break;
					case 3:	*pOutSubType = xSpr::xLTS_SCALE;	break;
					case 4:	*pOutSubType = xSpr::xLTS_EFFECT;	break;
					}
				}
			}
			return spLayer;
		}
	}
	return nullptr;
}

/**
 @brief 현재 선택된 키를 바꾼다.
 @param vDir x는 좌우, y는 상하. 
*/
void CFrameView::SetKeyCursor( const XE::POINT& vDir )
{
	auto& selectKeys = TOOL->GetSelectKey();
	if( selectKeys.GetNum() == 0 )
		return;
	auto pKey = SafeCast<XBaseKey*>( selectKeys.GetFirst() );
	if( pKey ) {
		XSPAction spAction = SPROBJ->GetspAction();
		auto spActObj = SPROBJ->GetspActObjCurr();
		if( spActObj ) {
			// pKey를 기준으로 vDir이 가리키는 오프셋만큼 떨어진 키를 얻는다.
			auto pKeyNew = spActObj->GetKeyByKeyWithDir( pKey, vDir );
			if( pKeyNew ) {
				selectKeys.Clear();
				selectKeys.Add( pKeyNew );
				auto rect = GetRectTimeline();
				float fScale = m_fScale;
				float fSpeed = ( spAction ) ? spAction->GetSpeed() : 0.2f;
				float fGridPerFrame = 1.0f / fSpeed;			// 1프레임당 작은눈금수 = 1.0 / ani speed;
				float fWidthPerFrame = ( KEY_WIDTH * fGridPerFrame );		// 1.0프레임당 가로길이
				float frameRightLimit = m_fStartFrame + ( ( rect.GetWidth() ) * 0.90f ) / fWidthPerFrame;	// 트랙영역중 3/4위치의 프레임번호 계산
				float frameLeftLimit = m_fStartFrame + ( ( rect.GetWidth() ) * 0.10f ) / fWidthPerFrame;	// 트랙영역중 1/4위치의 프레임번호 계산
				if( pKeyNew->GetfFrame() > frameRightLimit ) {
					m_fStartFrame += pKeyNew->GetfFrame() - frameRightLimit;
				} else
				if( pKeyNew->GetfFrame() < frameLeftLimit )
					m_fStartFrame -= frameLeftLimit - pKeyNew->GetfFrame();
				if( m_fStartFrame < 0 )
					m_fStartFrame = 0;
					// 포커스 이동
				Update();
			}
		}
	}
}

/**
 @brief 타임라인영역의 크기를 얻는다.
*/
XE::xRECT CFrameView::GetRectTimeline()
{
	RECT rect;
	GetClientRect( &rect );
	XE::xRECT rectArea;
	rectArea.vLT = XE::VEC2( TRACK_X, TOOLBAR_HEIGHT );
	rectArea.vRB = XE::VEC2( rect.right, rect.bottom );
	return rectArea;
}

