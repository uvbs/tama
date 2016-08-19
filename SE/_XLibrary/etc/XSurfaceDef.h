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
	xDM_ADD,
	xDM_MULTIPLY,		// 겹치면 더 어두워짐.
	xDM_SUBTRACT,
	xDM_LIGHTEN,
	xDM_DARKEN,
	xDM_SCREEN,
	xDM_GRAY,


};

#define EFF_FLIP_HORIZ		0x01		// 좌우 플립
#define EFF_FLIP_VERT			0x02		// 상하 플립

#endif
