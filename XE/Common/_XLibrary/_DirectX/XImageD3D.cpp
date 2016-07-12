/*
 *  XImageD3D.cpp
 *
 *  Created by xuzhu on 11. 5. 19..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#include "stdafx.h"
#ifdef _VER_DX
#ifdef _VER_OPENGL
#error "_VER_DX와 _VER_OPENGL은 함께 쓸 수 없음."
#endif
#include "XImage.h"
#include "_DirectX/XGraphicsD3DTool.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
#endif // _VER_DX
