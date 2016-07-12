#pragma once


class XWndList;
//////////////////////////////////////////////////////////////////////////
class XWndListElemDelegate
{
	void Init() {}
	void Destroy() {}
public:
	XWndListElemDelegate() { Init(); }
	virtual ~XWndListElemDelegate() { Destroy(); }
	//
	virtual void SetListSelect( BOOL bFlag ) {};
	/**
	 @brief 스크롤이 멈추면 호출된다.
	 @param vCurrLT 전체 스크롤영역중 현재 좌상귀의 좌표
	 @param sizeScrollWindow 스크롤이 보여지는 창문의 크기
	*/
	virtual void DelegateStopScroll( XWndList* pList, const XE::VEC2& vCurrLT, const XE::VEC2& sizeScrollWindow ) {}
};

class _XWndListDelegate
{
	void Init() {}
	void Destroy() {}
public:
	_XWndListDelegate() { Init(); }
	virtual ~_XWndListDelegate() { Destroy(); }
	//
	/**
	 @brief 스크롤이 멈추면 호출된다.
	 @param vCurrLT 전체 스크롤영역중 현재 좌상귀의 좌표
	 @param sizeScrollWindow 스크롤이 보여지는 창문의 크기
	*/
	virtual void DelegateStopScroll( XWndList* pList, const XE::VEC2& vCurrLT, const XE::VEC2& sizeScrollWindow ) {}
};
