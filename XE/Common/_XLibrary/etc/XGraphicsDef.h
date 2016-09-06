/*
 *  XGraphicsDef.h
 *  Game
 *
 *  Created by xuzhu on 11. 3. 29..
 *  Copyright 2011 LINKS CO.,LTD. All rights reserved.
 *
 */
#pragma once

#define		_XL		GRAPHICS->GetViewportLeft()
#define		_XR		GRAPHICS->GetViewportRight()
#define		_YT		GRAPHICS->GetViewportTop()
#define		_YB		GRAPHICS->GetViewportBottom()

/*typedef enum tagxPixelFormat {
	xPIXELFORMAT_NONE = -1,
	xPIXELFORMAT_RGB565 = 0,
	xPIXELFORMAT_RGB555,
	xPIXELFORMAT_RGBA8
} xPixelFormat;*/
enum xPixelFormat {
	xPIXELFORMAT_NONE = -1,
	xPIXELFORMAT_RGB565 = 0,
	xPIXELFORMAT_RGB555,
	xPIXELFORMAT_RGBA8
};

enum xAlpha {
	xNO_ALPHA = 0,
	xALPHA
};

XE_NAMESPACE_START(XE)

enum xtPixelFormat : int {
	xPF_NONE,
	xPF_ARGB8888,
	xPF_ARGB4444,
	xPF_ARGB1555,
	xPF_RGB565,
	xPF_RGB555,
	xPF_MAX,
};

inline int GetBpp( xtPixelFormat format ) {
	switch( format ) {
	case XE::xPF_ARGB8888:
		return 4;
	case XE::xPF_ARGB4444:
	case XE::xPF_ARGB1555:
	case XE::xPF_RGB565:
	case XE::xPF_RGB555:
		return 2;
	default: XBREAK( 1 ); break;
	}
	return 0;
}
inline bool IsAlphaFormat( xtPixelFormat format ) {
	switch( format )
	{
	case XE::xPF_ARGB8888:
	case XE::xPF_ARGB4444:
	case XE::xPF_ARGB1555:
		return true;
	case XE::xPF_RGB565:
	case XE::xPF_RGB555:
		return false;
	default: XBREAK( 1 ); break;
	}
	return false;
}

struct xHSL {
	XE::VEC3 m_vHSL;
	XE::VEC2 m_vRange1, m_vRange2;
	void SetHSL( float h, float s, float l ) {
		m_vHSL.Set( h, s, l );
	}
	bool operator == ( const xHSL& rhs ) const {
		return ((int)m_vHSL.x == (int)rhs.m_vHSL.x
				&& (int)m_vHSL.y == (int)rhs.m_vHSL.y
				&& (int)m_vHSL.z == (int)rhs.m_vHSL.z
				&& (int)m_vRange1.v1 == (int)rhs.m_vRange1.v1
				&& (int)m_vRange1.v2 == (int)rhs.m_vRange1.v2
				&& (int)m_vRange2.v1 == (int)rhs.m_vRange2.v1
				&& (int)m_vRange2.v2 == (int)rhs.m_vRange2.v2 );
	}
};

/**
 @brief 파일명(spr혹은 png)과 hsl의 묶음 구조체
*/
struct xHSLFile {
	_tstring m_strFile;
	xHSL m_HSL;
};

XE_NAMESPACE_END; // XE

