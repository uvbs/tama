#pragma once
#include "XList.h"
#include "Select.h"

class XDragSelect
{
public:
	enum xtDrag { xDRAG_NONE, xDRAG_ONLY, xDRAG_CONTROL, xDRAG_SHIFT, xDRAG_ALT };		// 드래그할때 ctrl, shift등을 누르고 있는지 상태
private:
	int m_nActive;
	XList4<ID> m_listSelectObj;		// 선택된 오브젝트
	XE::VEC2 m_vStart, m_vEnd;
	XE::VEC2 m_sizeObj;		// 각 오브젝트의 크기
	ID m_idPushObj;		// 셀렉트오브젝트를 LDown만 한상태
	int m_nDragState;	// L down:1 L down+draging:2
	xtDrag m_DragState;
	void Init() {
		m_nActive = 0;
		m_idPushObj = 0;
		m_nDragState = 0;
		m_DragState = xDRAG_NONE;
	}
public:
	XDragSelect() { Init(); }
	virtual ~XDragSelect() {}
	//
	GET_SET_ACCESSOR( const XE::VEC2&, sizeObj );
	GET_ACCESSOR( XList4<ID>&, listSelectObj );
	//
	void Draw();
	int ProcessObjLButtonUp( UINT nFlags, const XE::VEC2& vMouse );
	int ProcessDragAreaLButtonUp( UINT nFlags, const XE::VEC2& vMouse );

	int OnLButtonDown( UINT nFlags, const XE::VEC2& vMouse );
	int OnLButtonUp( UINT nFlags, const XE::VEC2& vMouse );
	int OnMouseMove( UINT nFlags, const XE::VEC2& vMouse );

	// virtual 
	virtual BOOL SelectObjInArea( XBaseElem *pObj, const XE::VEC2& vScr );	// 이 메소드를 상속받은 클래스는 vScr좌표가 pObj를 선택했는지 검사해야한다.
	virtual void SelectOnDragOnly( XBaseElem *pObj, const XE::VEC2& vObjDist );	// pObj에게 vObjDist만큼 이동명령을 전달
	// 하위클래스에서 구현해야 할 메소드 pure virtual
	virtual XE::VEC2 Projection( const XE::VEC2& vPos )=0;	// 이 메소드를 상속받은 클래스는 주어진 좌표를 해당뷰에 그릴때 화면좌표를 계산해서 돌려줘야 한다
	virtual XE::VEC2 UnProjection( const XE::VEC2& vScr )	=0;		// Projection과 반대. 화면좌표를 오브젝트좌표로 변환한다
	virtual void SelectGetNextClear()=0;	// 이 메소드를 상속받은 클래스는 셀렉트대상의 루프를 돌기위해 초기화를 해줘야 한다.
	virtual XBaseElem* SelectGetNext()=0;		// 이 메소스를 상속받은 클래스는 셀렉트대상의 루프를 차례대로 돌수있도록 구현해야 한다.
	virtual XBaseElem* GetSelectObj( ID idObj )=0;
};
