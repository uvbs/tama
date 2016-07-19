#pragma once

#define _[!output UPPERCASE_SAFE_PROJECT_NAME]				// 프로젝트명 디파인
#define _XVERIFY

#define _XACCOUNT_REF			// 어카운트 레퍼런트 카운트 버전


#ifdef _XUZHU
//#define _XPATCH
#endif

#if defined(_XTHAILAND)
  // 국가별 설정을 넣으시오
  //#define _NICK_FIRST
  //#define _XLOGIN_FACEBOOK
  //#define _CONTENTS_BILLING
#else
  // 유니버셜 버전 설정을 넣으시오
  #define _XKOREA
  #define _XUNIVERSAL
  #define _XLOGIN_UUID		// UUID로그인 방식
  #define _NICK_FIRST
  #ifdef _DEBUG
    #define _DEV_PATCH	// 사내 개발용 패치서버 접속
  #endif
#endif