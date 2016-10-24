
// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// Caribe.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"


#if defined(_VER_DX)
#pragma message("==================================d3d version")
#elif defined(_VER_OPENGL)
#pragma message("==================================opengl version")
#else
#error "_VER_DX나 _VER_OPENGL중 하나는 있어야 함."
#endif


#ifdef _XPROP_SERIALIZE
#pragma message("==================================_XPROP_SERIALIZE")
#else
#pragma message("==================================not _XPROP_SERIALIZE")
#endif

#if _DEV_LEVEL == DLV_LOCAL
#pragma message("=================DLV_LOCAL==============================================")
#elif	_DEV_LEVEL == DLV_DEV_PERSONAL
#pragma message("================DLV_DEV_PERSONAL==============================================")
#elif	_DEV_LEVEL == DLV_DEV_CREW
#pragma message("================DLV_DEV_CREW==============================================")
#elif	_DEV_LEVEL == DLV_DEV_EXTERNAL
#pragma message("("================DLV_DEV_EXTERNAL==============================================")
#elif	_DEV_LEVEL == DLV_OPEN_BETA
#pragma message("(================DLV_OPEN_BETA==============================================")
#else
#error "DLV_XXX중 뭐라도 되어있어야 함"
#endif 
