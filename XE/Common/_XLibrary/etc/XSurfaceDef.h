/*
 *  XSurfaceDef.h
 *  Game
 *
 *  Created by xuzhu on 11. 3. 29..
 *  Copyright 2011 LINKS CO.,LTD. All rights reserved.
 *
 */
//#pragma once
#ifndef _XSURFACEDEF_H__
#define _XSURFACEDEF_H__

// draw mode
enum xDM_TYPE {
	xDM_ERROR = -1,
	xDM_NONE = 0,		// 그림을 찍지 않음
	xDM_NORMAL,
	xDM_SCREEN,
	xDM_MULTIPLY,
	xDM_SUBTRACT,
	xDM_GRAY,
};

namespace XE {
// 블렌딩 함수(xDM_TYPE을 쓰지말고 이걸쓸것)
enum xtBlendFunc : int {
	xBF_NONE = 0,		
	xBF_NO_DRAW,		// 그림을 찍지 않음
	xBF_MULTIPLY,
	xBF_ADD,
	xBF_SUBTRACT,
	xBF_GRAY,
};

// xDM_NONE = 0,		// 그림을 찍지 않음
// xDM_NORMAL,
// xDM_ADD,
// xDM_MULTIPLY,		// 겹치면 더 어두워짐.
// xDM_SUBTRACT,
// xDM_LIGHTEN,
// xDM_DARKEN,
// xDM_SCREEN,
// xDM_GRAY,

inline xDM_TYPE ConvertBlendFuncDMTypeDmType( xtBlendFunc func ) {
	switch( func ) {
	case xBF_NO_DRAW:	return xDM_NONE;	break;
	case xBF_ADD:		return xDM_SCREEN;	break;
	case xBF_GRAY:		return xDM_GRAY; break;
	case xBF_SUBTRACT:
	case xBF_MULTIPLY:
		return xDM_NORMAL;
	default:
		XBREAK(1);
		break;
	}
	return xDM_NORMAL;
}

inline xtBlendFunc ConvertDMTypeToBlendFunc( xDM_TYPE drawMode ) {
	switch( drawMode ) {
	case xDM_NORMAL:
	case xDM_MULTIPLY:		
		return XE::xBF_MULTIPLY;
	case xDM_SCREEN:		
		return XE::xBF_ADD;
	case xDM_SUBTRACT:		
		return XE::xBF_SUBTRACT;
	case xDM_GRAY:		
		return XE::xBF_GRAY;
		break;
	default:
		XBREAK(1);
		break;
	}
	return xBF_NONE;
}
}; // namespace XE


#define EFF_FLIP_HORIZ		0x01		// 좌우 플립
#define EFF_FLIP_VERT			0x02		// 상하 플립

#endif
