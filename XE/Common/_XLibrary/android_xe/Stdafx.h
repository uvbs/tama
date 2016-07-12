// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.

#pragma once

#define XE_ANDROID_VALID_STDAFX					// 안드로이드에서 이 stdafx.h파일을 제대로 인클루드 했는지 확인하기 위한 용도.

// xe엔진을 위한 stdafx입니다. ios나 안드로이드도 이 파일을 써야 합니다
#ifdef __cplusplus
#include "xLibGame.h"
#endif // c++
