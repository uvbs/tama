
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