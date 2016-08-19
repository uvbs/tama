#pragma once

#include "resource.h"
#include "afxwin.h"
#include "SprObj.h"
//#include "UndoAction.h"
#include "XMyD3DView.h"
#include "XKeyAll.h"
// CFrameView 폼 뷰입니다.

class XUndoActionChange;
class XBaseKey;
class XKeyImage;
class XKeyPos;
class XKeyRot;
class XKeyScale;
class XKeyEffect;
class XKeyCreateObj;
class XKeyDummy;
class XKeyEvent;
class XKeySound;

#define KEY_WIDTH		(20.0f*fScale)					// 트랙 한눈금 가로크기
#define KEY_WIDTH2	20.0f					// 트랙 한눈금 가로크기
#define KEY_HEIGHT	20.0f

#define KEYEDIT_MOVE		0
#define KEYEDIT_SCALE		1
//#define KEYEDIT_COPY			2
//#define KEYEDIT_PANNING	3		// 타임라인 스크롤
//#define KEYEDIT_DRAG_BLOCK	4	// 영역 드래그

class CFrameView : public XMyD3DView {
public:
	enum xtGridUnit { xGU_TIME, xGU_FRAME };
	static CFrameView* sGet() { return s_pInstance; };
private:
	static CFrameView* s_pInstance;
	// 어떤 드래그 중인가
	enum xtDragMode { 
					xDRAG_NONE,
					xDRAG_KEY_MOVE, 
					xDRAG_KEY_SCALE, 
					xDRAG_KEY_COPY, 
					xDRAG_PANNING, 
					xDRAG_BLOCK, 
					xDRAG_MAX_FRAME, 
					xDRAG_LAYER_MOVE }; 
	// 드래그 상태
	enum xtDragState { xDS_NONE,					// 초기화 상태 
					xDS_PUSH,					// 아직 드래그 안하고 푸쉬만 한상태
					xDS_DRAGGING };				// 드래깅 중
	enum xtArea {
		xAT_NONE,
		xAT_LABEL,				// 레이어 이름 영역
		xAT_TIME_LINE,		// 타임라인영역
		xAT_TIME_BAR,			// 상단 타임바 영역
	};
	DECLARE_DYNCREATE(CFrameView)
//	
	XE::VEC2 m_vMouse, m_vRMouse;
	xtDragMode _m_DragMode;	// 어떤 드래그를 하고 있는가. 
	xtDragState _m_DragState;		// 드래그 상태
	xtArea m_areaLDown = xAT_NONE;		// LDown한 영역
	BOOL m_bPanningMode;		// alt를 누르고 있거나 휠을 누르고 있는경우
	BOOL m_bTrackPush;			// 트랙을 lbuttondown했다
	bool m_bModeLayerCopy = false;
	float m_fDragKeyScale;			// 
	XBaseKey *m_pPushKey;		// OnLButtonDown했을때 누른 키
	XSPBaseLayer m_spPushLayer;	// OnLButtonDown했을때 누른 레이어
	xtGridUnit m_GridUnit;			// 그리드 표시단위
	XSPBaseLayer m_spMoveLayerCurr;		// 레이어드래그중 현재 커서위치(삽입할곳)의 레이어를 받아둠
	float m_fClickMaxFrame;		// 맥스프레임 조절시에 최초클릭했을때 maxframe값을 저장
	float m_fStartFrame;				// 트랙윈도우 시작프레임
	int m_ofsY = 0;			// 타임라인윈도우 세로 스크롤 오프셋
	float m_fScale;					// 트랙윈도우의 확대축소
	int m_nScale = 50;				// m_fScale의 정수형버전
	XUndoActionChange *m_pLastUndoAction;
	XE::VEC2 m_vDragStart, m_vDragEnd;	// 영역 드래그 시작, 끝 위치
	XE::VEC2 m_vPrev;
	XE::xtAL m_alScrollDir = XE::xAL_NONE;		// 스크롤 허용방향
	CTimer m_timerDoubleClick;
	CString m_strPos;
	CString m_strRot;
	CString m_strScale;
	CString m_strAlpha;
	//
	xtDragMode GetDragMode() { 
		return _m_DragMode; 
	}
	void StartDragMode( xtDragMode dragMode ) { 
		_m_DragMode = dragMode; 
		_m_DragState = xDS_PUSH; 
	}
	void SetDragMode( xtDragMode dragMode, xtDragState dragState ) { 
		_m_DragMode = dragMode; 
		_m_DragState = dragState; 
	}
	void ClearDragMode() { 
		_m_DragMode = xDRAG_NONE; 
		_m_DragState = xDS_NONE; 
	}
	xtDragState GetDragState() { 
		return _m_DragState; 
	}
	xtDragState IsDragMode( xtDragMode dragMode ) {	// 현재 파라메터 상태냐
		if( dragMode == _m_DragMode ) {
			XBREAK( _m_DragState == xDS_NONE );		// 이런경우는 없어야 한다
			return _m_DragState;
		}
		return xDS_NONE;
	}
	BOOL IsNotDragMode( xtDragMode dragMode ) {		// 현재 파라메터상태가 아니냐?
		if( _m_DragMode != dragMode )
			return TRUE;
		return FALSE;
	}
	BOOL IsClearDragMode() { return ( _m_DragMode == xDRAG_NONE )? TRUE : FALSE; }	// 아무 드래그 모드가 아닌 초기상태다

	//
//	void UpdateSlider();
	void PopupKeyFrame( CPoint point = CPoint(0,0) );
	void ProcSelectKeyWhenButtUp( XBaseKey *pClickKey, UINT nFlags );
//	void ProcessLayerAreaMouseMove( UINT nFlags, const XE::VEC2& vMouse );
protected:

public:
	CFrameView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFrameView();

	enum { IDD = IDD_FRAMEVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	GET_ACCESSOR( int, ofsY );

	void MoveFrame( const CPoint& point );
	inline void MoveFrame( const XE::VEC2& vMouse ) {
		MoveFrame( vMouse.ToCPoint() );
	}
	void MoveFrame( float frame );
	void Update();
	void DrawInterpolationLinePos( XKeyPos *pKey1, XKeyPos *pKey2, const XE::VEC2& vOffset );
//	void DrawInterpolationLineByBase( XBaseKey* pKey1, XBaseKey* pKey2, const XE::VEC2& vOffset );
//	void DrawInterpolationLineByScale( XKeyScale *pKey1, XKeyScale *pKey2, const XE::VEC2& vOffset );
	template<typename T>
	void DrawInterpolationLineBetweenKey( const T* pKey1, const T* pKey2, const XE::VEC2& vOffset ) {
		if( pKey1 == nullptr )	return;
		if( pKey2 == nullptr )	return;
		float frameLength = pKey2->GetfFrame() - pKey1->GetfFrame();		// 보간구간 프레임길이
		if( frameLength == 0 )
			return;
		frameLength *= 10.0f;
		float i = 0;
		while( 1 ) {
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
				GRAPHICS->DrawPoint( vCurr.x, vCurr.y - 1.0f, 1, XCOLOR_RED );
				GRAPHICS->DrawPoint( vCurr.x, vCurr.y + 1.0f, 1, XCOLOR_RED );
				GRAPHICS->DrawPoint( vCurr.x - 1.0f, vCurr.y, 1, XCOLOR_RED );
				GRAPHICS->DrawPoint( vCurr.x + 1.0f, vCurr.y, 1, XCOLOR_RED );
			}
		}
	}
	template<typename T>
	void DrawInterpolationLine( const XBaseKey* _pKey ) {
		auto pKey = SafeCast<const T*>( _pKey );
		// 보간으로 연결되어 있는 다음키가 있으면 선을 그려준다
		if( XASSERT(pKey) ) {
			if( pKey->GetInterpolation() ) {
				const XBaseKey* pNextKey = pKey->GetNextKeyConst();
				if( pNextKey ) {
					const auto v1 = pKey->GetvDrawPos();		// 뷰포트 좌표를 빼줌
					const auto v2 = pNextKey->GetvDrawPos();
					const auto vh = GetSizeKey( m_fScale ) / 3;
					// 다음키와 선연결
					const auto vl1 = v1 + vh;
					const auto vl2 = v2 + vh;
	// 			const auto col = XCOLOR_BLACK;
	// 			XE::DrawLine( vl1, vl2, col, 2 );
					// 보간연결선 그림
					const auto pNextKeyScale = SafeCast<const T*>( pNextKey );
					DrawInterpolationLineBetweenKey<T>( pKey, pNextKeyScale, vh * 2.0f );	// vh * 2.0 = KEY_WIDTH*0.66(2/3)
				}
			}
		}
	}
	void Draw();
	void UpdateOfsYBySelectedLayer();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
//	virtual void OnDraw(CDC* /*pDC*/);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtAddLayer();
	afx_msg void OnBnDoubleClicked();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnInitialUpdate();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk( UINT nFlags, CPoint point );
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//	afx_msg BOOL OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDelKey();
	afx_msg void OnBnClickedButtCreateobj();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnAddLayerImage();
//	afx_msg void OnAddLayerObject();
	afx_msg void OnDelSelLayer();
//	afx_msg void OnBnClickedRadioKeyMove();
//	afx_msg void OnBnClickedRadioKeyScale();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
//	int m_nKeyEditMode;
	afx_msg LRESULT OnKickIdle(WPARAM, LPARAM);
	afx_msg void OnEditKey();
	afx_msg void OnUpdateEditKey(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTextPos( CCmdUI *pCmdUI );
	afx_msg void OnUpdateTextRot( CCmdUI *pCmdUI );
	afx_msg void OnUpdateTextScale( CCmdUI *pCmdUI );
	afx_msg void OnUpdateTextAlpha( CCmdUI *pCmdUI );
	afx_msg void OnEditAni();
	afx_msg void OnUpdateEditAni(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDelSelLayer(CCmdUI *pCmdUI);
	afx_msg void OnAddLayerSound();
	afx_msg void OnBnClickedReloadSound();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSetRepeatmark();
	afx_msg void OnMoveStart();
	afx_msg void OnMoveEnd();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnShowLayer();
	afx_msg void OnUpdateShowLayer(CCmdUI *pCmdUI);
	afx_msg void OnCreateKey();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnGridFrame();
	afx_msg void OnUpdateGridFrame(CCmdUI *pCmdUI);
	afx_msg void OnGridTime();
	afx_msg void OnUpdateGridTime(CCmdUI *pCmdUI);
	afx_msg void OnSelectAllKey();
	afx_msg void OnSetInterpolation();
	afx_msg void OnCopyLayer();
	afx_msg void OnShowThisLayerOnly();
	afx_msg void OnShowAllLayer();
	afx_msg void OnCreateKeyCurrChannel();
	afx_msg void OnCreatePosKey();
	afx_msg void OnCreateRotKey();
	afx_msg void OnCreateKeyScale();
	afx_msg void OnCreateKeyEffect();
	afx_msg void OnSelectAllKeyAfter();
	afx_msg void OnSelectAllKeyAfterInLayer();
private:
	XE::VEC2 GetPosInKeyWindow( const XE::VEC2& vMouse );
	XE::VEC2 GetPosInLayerWindow( const XE::VEC2& vMouse );
	void DrawTimeline( const XE::VEC2& vLT );
	void DrawKeyWindow( XSPAction spAction );
	XE::VEC2 GetvScrPosKey( XBaseKey *pKey, XSPAction spAction );
	void DrawVertByFrame( float speedAction, float y, const XE::VEC2& sizeClient );
	void DrawVertByTime( float speedAction, float y, const XE::VEC2& sizeClient );
	void DrawGrid( int yDraw, const XE::VEC2& vClient, const XE::VEC2& sizeClient, float speedAction );
	void DrawHorizLine( int yDraw, const XE::VEC2& sizeClient );
	void DrawKeys( XSPAction spAction, int yDraw, float speedAction, const RECT& rectClient );
	void DrawKeyArea( XSPAction spAction, int yDraw, const RECT& rectClient, float speedAction );
	void DrawToolTipByKey();
	void DrawLayerName( XSPActObj spActObj, int yDraw );
	void DrawLayerName( XSPActObj spActObj, float left, float top, XSPBaseLayer spPushLayer );
	void OnMouseMoveByPushLayer( UINT nFlags, const XE::VEC2& vMouse );
	void OnMouseMoveByPushKey( UINT nFlags, const XE::VEC2& vMouse, float speedAction );
	XBaseKey* CreateKeyByTypeChannel( xSpr::xtLayerSub typeChannel );
	XSPBaseLayer GetspLayerCurrMouse( const XE::VEC2& vMouse );
	void OnLButtonUpInLabel( UINT nFlags, const XE::VEC2& vMouse );
	void OnLButtonUpInTimeline( UINT nFlags, const XE::VEC2& vMouse );
	void OnLButtonUpInTimelineByDragKeyMove( UINT nFlags, const XE::VEC2& vMouse );
	void OnLButtonUpInTimelineByDragKeyScale( UINT nFlags , const XE::VEC2& vMouse );
	void OnLButtonUpInTimelineByDragKeyCopy( UINT nFlags, const XE::VEC2& vMouse );
	void OnLButtonUpInTimelineByDragMaxFrame( UINT nFlags, const XE::VEC2& vMouse );
	void OnLButtonUpInTimelineByDragBlockSelectKey( UINT nFlags, const XE::VEC2& vMouse );
	void OnLButtonUpInTimelineByNotDrag( UINT nFlags, const XE::VEC2& vMouse );
	void UpdateKeyInfo();
	void DrawInterpolationLineByPosKey( XKeyPos* pPosKey );
//	void DrawInterpolationLineByBase( XKeyScale* pKey );
//	void DrawInterpolationLineByScaleKey( XKeyScale* pPosKey );
public:
	// 타임트랙에 그려질 키하나의 크기
	float GetWidthKey( float fTransformScale ) const {
		return KEY_WIDTH2 * fTransformScale;
	}
	float GetHeightKey( float fTransformScale ) const {
		return KEY_HEIGHT * fTransformScale;
	}
	// 원래 private에 들어가야 맞는것이나 기존코드와의 호환을 위해 풀어둠.
	XE::VEC2 GetSizeKey( float fTransformScale = 0.f ) const {
		if (fTransformScale == 0)
			fTransformScale = m_fScale;
		return XE::VEC2( KEY_WIDTH2 * fTransformScale, KEY_HEIGHT );
	}
	void SetKeyCursor( const XE::POINT& vDir );
	XE::xRECT GetRectTimeline();
private:
	XSPBaseLayer CheckPushLayer( float lx, float ly
															 , float h
															 , xSpr::xtLayerSub *pOutSubType );
 	inline XSPBaseLayer CheckPushLayer( const XE::VEC2& vLocal, float h = 0, xSpr::xtLayerSub *pOutSubType = nullptr ) {
 		return CheckPushLayer( vLocal.x, vLocal.y, h, pOutSubType );
 	}
};

CFrameView *GetFrameView();

