#pragma once

#include "XWnd.h"
#include "XWndScrollView.h"

class XWndAlert;
class XWndButtonString;
class XWndProgressBar2;
class XParticleMng;
class XParticleDelegate;
class XLayout;
class _XWndListDelegate;


// 리스트 컨트롤
class XWndList : public XWndScrollView
{
public:
	enum xtType {
		xTYPE_NONE,
		xLT_VERT,			// 세로 나열형
		xLT_HORIZ,			// 가로 나열형
		xLT_THUMB,		// 썸네일 형
	};
	enum {
		xID_EMPTY_SLOT = 0xFFFFFFFF,	// 빈슬롯의 윈도우아이디는 이걸로할것.
	};
	struct xINTERPOLATION_WND {
		XE::VEC2 vBefore;		// 원래 자리
		XE::VEC2 vAfter;			// 밀려나야 할 자리
		XWnd *pWnd;			// 애니메이션 될 대상.
	};
private:
	xtType m_Type;
//	XWnd *m_pClickedWnd;
	ID m_idClickedWnd = 0;
	float m_X, m_Y;
	XE::VEC2 m_vSpace;		// 리스트간 간격
	XE::VEC2 m_vDown;
	XList4<ID> m_listSelect;		// 멀티셀렉션 옵션일때.
	BOOL m_bMultiSelect;		// 여러개 선택가능한 형태
	bool m_bSelectedUI;			// 선택시 선택UI를 표시할건지
	int m_State;					// 0:가만있음. 1:크기변화로 재배열되는중. 
	XE::VEC2 m_sizeResizingChild;	// 크기가 변하는 자식의 변하는 양
	XInterpolationObjDeAccel m_ipoRearrange;		// 재배열될때 사용.
	float m_secRearrange;			// 재배열되는 애니메이션의 시간.
	XWnd *m_pResizingChild;		// 크기가 변하는 자식.
	XList4<xINTERPOLATION_WND> m_listRearrange;	// 재배열 애니메이션 대상들.
	std::string m_strItemLayoutForXML;		// 각각의 아이템의 레이아웃을 정의한 노드이름.
	BOOL m_bScrollExclusive;			// 스크롤 독점모드.
	BOOL m_bTouchDown;
	XE::VEC2 m_vAdjustByViewport;		// 왼쪽이나 윗쪽으로 넘어가는 뷰포트때문에 생기는 좌표 보정값.
	XE::VEC2 m_sizeFixed;				// 리스트elem의 각항목의 크기를 이 값으로 고정시킨다.
	_XWndListDelegate *m_pDelegate = nullptr;
	void Init() {
		m_Type = xTYPE_NONE;
		m_Y = m_X = 0;
//		m_pClickedWnd = NULL;
//		m_psfcBg = NULL;
		m_bMultiSelect = FALSE;
		m_State = 0;
		m_secRearrange = 0;
		m_pResizingChild = NULL;
		m_bScrollExclusive = FALSE;
		m_bTouchDown = FALSE;
	}
	void Destroy();
protected:
	XList4<XWnd*> m_listElem;
public:
	//	XWndList( float x, float y, float w, float h, xtType type=xTYPE_LIST );
//	XWndList( LPCTSTR szBg, const XE::VEC2& vPos, const XE::VEC2& vLocalxy, const XE::VEC2& vSize, xtType type = xLT_VERT );
	XWndList( const XE::VEC2& vPos, const XE::VEC2& vSize, xtType type = xLT_VERT );
	virtual ~XWndList() { Destroy(); }
	//
	GET_ACCESSOR_CONST( const XList4<XWnd*>&, listElem );
	GET_SET_ACCESSOR_CONST( const XE::VEC2&, vSpace );
	GET_SET_ACCESSOR_CONST( BOOL, bMultiSelect );
	GET_SET_ACCESSOR_CONST( const std::string&, strItemLayoutForXML );
	GET_SET_ACCESSOR_CONST( const XE::VEC2&, sizeFixed );
	GET_SET_ACCESSOR( _XWndListDelegate*, pDelegate );
	GET_SET_BOOL_ACCESSOR( bSelectedUI );
	void SetstrItemLayoutForXML( const char *cNodeName ) {
		m_strItemLayoutForXML = cNodeName;
	}
	XWnd* AddItem( ID idWnd, XWnd *pWnd );
	inline XWnd* AddItem( XWnd *pWnd ) {
		return AddItem( XE::GenerateID(), pWnd );
	}
	// 뷰포트의 최상위 뷰포트 객체는 언제나 자신의 월드좌표를 0으로 돌려준다.
	XE::VEC2 GetPosFinal( void ) const {
		return /*XE::VEC2(0) +*/ GetvAdjScroll() + m_vAdjustByViewport;
	}
	void Clear( void ) {
		XWndScrollView::Clear();
		m_listElem.Clear();
		DestroyAllWnd();
//		m_pClickedWnd = NULL;
		m_idClickedWnd = 0;
		m_Y = 0;
	}
	// 	void Del( XWnd* pWnd );
	// 	void DelFromID( ID idWnd ) ;
	int GetNumItem( void ) {
		return m_listElem.size();
	}
	void DelItem( ID idWnd );
	void DestroyAllItem( void );
	XWnd* GetElemByIndex( int idx );
private:
	void Update( void );
public:
	void RecalcElem( XWnd *pWnd );
	//
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnLButtonDownCancel( void );
	virtual void OnStartChangeSizeOfChild( XWnd *pChild, const XE::VEC2& vSize, float secSizing );
	virtual void ProcessDrag( const XE::VEC2& vDistMove );
	template<typename T>
	void Sort( T func ) {
		m_listElem.sort( func );
	}
	void SetType( xtType type ) {
		m_Type = type;
		switch( type ) {
		case XWndList::xLT_VERT:
			XScroll::SetScrollVertOnly();
			break;
		case XWndList::xLT_HORIZ:
			XScroll::SetScrollHorizOnly();
			break;
		case XWndList::xLT_THUMB:
			// 썸네일 타입의 경우 어느한쪽으로만 스크롤방향을 잠그길 원한다면 외부에서 추가로 지정해야 한다.
			XScroll::SetScrollAllDir();
			break;
		default:
			break;
		}
	}
	void SetTypeHoriz() {
		SetType( xLT_HORIZ );
	}
	void SetTypeVert() {
		SetType( xLT_VERT );
	}
	void SetTypeThumbnail() {
		SetType( xLT_THUMB );
	}
	void OnStopScroll() override;
	void DoMoveTop();
	void DoMoveBottom();
	XWnd* GetpClickedWnd() {
		if( !m_idClickedWnd )
			return nullptr;
		return GetWndTop()->FindByID( m_idClickedWnd );
	}
	XE::VEC2 GetSizeNoTransLayout() override;
	XWnd* GetpFirst() {
		return m_listElem.GetFirst();
	}
}; // class XWndList


XWndList* xGET_LIST_CTRL( XWnd *pRoot, const char *cKey );
