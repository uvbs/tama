#pragma once
#include "d3dView.h"


class XD3DWnd : public CDockablePane, public XD3DView
{
	DECLARE_DYNAMIC(XD3DWnd)
public:
	XD3DWnd() {}
	virtual ~XD3DWnd() {}

	void Update( void ) { Invalidate(0); }
protected:
	virtual void Draw( void )=0;
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
};


