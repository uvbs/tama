#include "stdafx.h"
#ifdef _XGLM
#pragma message("XGLM--------------------------------------")
#endif
#ifdef _XGLM
#include "xMathGLM.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

namespace XE 
{
	/*static */glm::mat4 s_mIdentity(1.f);
};

#endif // glm
