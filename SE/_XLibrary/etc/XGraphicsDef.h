/*
 *  XGraphicsDef.h
 *  Game
 *
 *  Created by xuzhu on 11. 3. 29..
 *  Copyright 2011 LINKS CO.,LTD. All rights reserved.
 *
 */
#ifndef __XGRAPHICSDEF_H__
#define __XGRAPHICSDEF_H__

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

#endif 
