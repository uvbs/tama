#pragma once

#include "d3dView.h"
#include "tool.h"
//#include "UndoMng.h"
#include "MovableInterface.h"
#include "XDragSelect.h"

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
class XUndoKeyChange;

typedef list<XMovableInterface *>				XMI_List;
typedef list<XMovableInterface *>::iterator		XMI_Itor;

// CAnimationView 폼 뷰입니다.
class CAnimationView : public CFormView, public XD3DView, public XDragSelect
{
	DECLARE_DYNCREATE(CAnimationView)

//	CToolBar m_ToolBar;
	XE::VEC2 m_vCenter;
	XE::VEC2 m_vMouse, m_vRMouse;
	XE::VEC2 m_vDragDist, m_vDragStart;		// 드래그할때 움직인 거리(버튼다운에서 부터 기준)
	int m_nDragLayer;			// 레이어 드래그 이동모드. 1은 푸쉬상태, 2는 드래그상태
	int m_nDragRotate;			// 레이어 드래그 회전모드. 상태는 위와 같음
	int m_nDragScale;			// 레이어 드래그 스케일링 모드 (나중에 드래그관련 플래그는 하나로 합치자. 두가지를 동시에 드래그하는 일은 없을것 같기 때문)
	BOOL m_bViewAdjustAxis;		// 회전/스케일링 축 보이기 감추기
	float m_fPrevAngle;				// m_fAngleFromAxis의 이전프레임값
	float m_fPrevDistFromAxis;		// fDistFromAxis의 이전프레임값
	XBaseKey *m_pNewAddKey;	// Drag모드중에 새로추가된 키의 포인터
	XMovableInterface *m_pDragMI;		// 드래그중인 movable interface. m_pMICross등의 포인터가 참조된다
	XMovableInterface *m_pMICross;		// 회전축 십자선 인터페이스
	XMICrossDir *m_pMIEventCreateObj;	// event : create obj의 십자선 인터페이스
	XUndoKeyChange *m_pLastUndoKeyChange;			// 마지막으로 만들어낸 키값변경 언두행동
	XMI_List m_listMI;				// MovableInterface 관리자
	BOOL m_bAlt = FALSE;					// 알트키를 누르면 true가 된다
// 	int m_nDragPosKeyGenerate;	// 
// 	XE::VEC2 m_vPosKeyGenerate;	// 자동 pos키생성시 마우스 위치
// 	float m_speedPosKeyGenerate;	// 자동 pos키생성의 지정된 속도
	XE::VEC2 m_vAdjBackBg;				// 배경이미지 위치 보정값.
	bool m_bModeBackBg = false;		// 배경이미지 이동모드.
	bool m_bPanningMode = false;
protected:
	CAnimationView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CAnimationView();

public:
	enum { IDD = IDD_ANIMATIONVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	GET_BOOL_ACCESSOR( bPanningMode );
//	XE::VEC2 GetMousePosT() { return m_vMouse / GetvScaleView(); }
//	XE::VEC2 GetRMousePosT() { return m_vRMouse / GetvScaleView(); }
	XE::VEC2 GetMouseCoord() { return (m_vMouse - m_vCenter) / GetvScaleView(); }		// 센터로부터의 마우스 좌표인데 확축이 적용되어 리턴된다
	XE::VEC2 GetRMouseCoord() { return (m_vRMouse - m_vCenter) / GetvScaleView(); }		// 센터로부터의 마우스 좌표인데 확축이 적용되어 리턴된다
	void DrawPathLayer();
	void Draw();
	void DrawPathLayer( XSPAction spAction, SPR::xtLINE lineType, XSPLayerMove spLayer, XCOLOR col );
	void Update() { Invalidate( FALSE ); }
	void PopupLayer( CPoint point );
	void RegisterMI( XMovableInterface *pmi ) {
		m_listMI.push_back( pmi );
	}
	void UnRegisterMI( XMovableInterface *pmi ) {
		XMI_Itor itor = find( m_listMI.begin(), m_listMI.end(), pmi );
		if( itor != m_listMI.end() )	// 파일이 최초 로딩될때 이전 액션의 OnSelect()하면서 거짓이 될수 있다
			m_listMI.erase( itor );
	}
	void DestroyMI( XMovableInterface *pmi ) {
		XMI_Itor itor;
		for( itor=m_listMI.begin(); itor!=m_listMI.end(); )
		{
			if( (*itor) == pmi ) {
				m_listMI.erase( itor++ );		// 알맹이 삭제책임은 생성한곳에서 한다
				break;
			} else
				itor++;
		}
	}
	void DrawMICRossDir( XMICrossDir *pmi ) {
		pmi->SetvScale( GetvScaleView() );
		pmi->Draw( m_vCenter, XE::VEC2(0,0), m_vMouse );
	}
	XE::VEC2 PosToViewCoord( const XE::VEC2& vSrc ) {		// 십자선 기준 좌표를 뷰내 스크린 좌표로 변환
		return m_vCenter + vSrc * GetvScaleView();
	}
	XKeyPos* MoveLayer( XSPActObj spActObj, XSPLayerMove spLayerMove, const XE::VEC2& vDist, BOOL bCreateUndo );
	void DrawLayerBefore( XSPBaseLayer spLayer );
	// XDragSelect의 virtual 구현
	virtual XE::VEC2 Projection( const XE::VEC2& vPos ) {	return PosToViewCoord( vPos );	}
	virtual XE::VEC2 UnProjection( const XE::VEC2& vScr )	{	// 화면좌표(vScr)를 오브젝트 좌표로 변환
		XE::VEC2 v = ( vScr - m_vCenter ) / GetvScaleView();
		return v;
	}
	virtual void SelectGetNextClear();
	virtual XBaseElem* SelectGetNext();
	virtual void SelectOnDragOnly( XBaseElem *pObj, const XE::VEC2& vObjDist );
	virtual XBaseElem* GetSelectObj( ID idObj );
	GET_SET_BOOL_ACCESSOR( bModeBackBg );
private:
	void OnMouseMoveByBackBg( const XE::VEC2& vMouse );
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
protected:
	virtual void OnDraw(CDC* /*pDC*/);
public:
	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnDrawProperty();
	afx_msg void OnUpdateDrawProperty(CCmdUI *pCmdUI);
	afx_msg void OnCreateEvent();
	afx_msg void OnUpdateCreateEvent(CCmdUI *pCmdUI);
	afx_msg void OnEditEvent();
	afx_msg void OnUpdateEditEvent(CCmdUI *pCmdUI);
	afx_msg void OnPropertyObj();
	afx_msg void OnUpdatePropertyObj(CCmdUI *pCmdUI);
	afx_msg void OnViewTransAxis();
	afx_msg void OnUpdateViewTransAxis(CCmdUI *pCmdUI);
	afx_msg void OnCreateDummy();
	afx_msg void OnCreateSound();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message );
};


CAnimationView *GetAnimationView();
