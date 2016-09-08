#pragma once
#include "etc/XGraphicsDef.h"

XE_NAMESPACE_START( XE )
//
struct xImg {
	void* m_pDat;
	XE::POINT m_ptSize;
	xtPixelFormat m_Fmt;
	xImg() 
		: m_pDat( nullptr )
		, m_Fmt( xPF_NONE ) {}
	xImg( void* pDat, XE::POINT ptSize, xtPixelFormat fmt )
		: m_pDat( pDat )
		, m_ptSize( ptSize )
		, m_Fmt( fmt ) {}
	~xImg() {
		Destroy();
	}
	void Destroy();
	void Set( void* pDat, int w, int h, xtPixelFormat fmt ) {
		m_pDat = pDat;
		m_ptSize.Set( w, h );
		m_Fmt = fmt;
	}
	void Set( void* pDat, const XE::POINT& ptSize, xtPixelFormat fmt ) {
		m_pDat = pDat;
		m_ptSize = ptSize;
		m_Fmt = fmt;
	}
};

//
XE_NAMESPACE_END; // XE

typedef std::shared_ptr<XE::xImg> XSPImg;


