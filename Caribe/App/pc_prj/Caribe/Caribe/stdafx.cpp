
// stdafx.cpp : ǥ�� ���� ���ϸ� ��� �ִ� �ҽ� �����Դϴ�.
// Caribe.pch�� �̸� �����ϵ� ����� �˴ϴ�.
// stdafx.obj���� �̸� �����ϵ� ���� ������ ���Ե˴ϴ�.

#include "stdafx.h"


#if defined(_VER_DX)
#pragma message("==================================d3d version")
#elif defined(_VER_OPENGL)
#pragma message("==================================opengl version")
#else
#error "_VER_DX�� _VER_OPENGL�� �ϳ��� �־�� ��."
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
#pragma message("================DLV_OPEN_BETA==============================================")
#else
#error "DLV_XXX�� ���� �Ǿ��־�� ��"
#endif 
