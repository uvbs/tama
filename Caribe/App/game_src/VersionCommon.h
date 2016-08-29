#pragma once

#define _CARIBE				// 프로젝트명 디파인
#define _XVERIFY

//#define _XACCOUNT_REF			// VersionXE에 이미 있음.어카운트 레퍼런트 카운트 버전
#define _NEW_INAPP			// payload방식의 새로운 결제 프로세스
#define _XUNIT_HSL					// 상대유닛 hsl로 바꾸는 버전

#ifndef _XSINGLE
#define _XPROP_SERIALIZE		// 프로퍼티 시리얼라이즈 버전
#endif

#ifdef _XUZHU
//#define _XPATCH
//	#define _XSINGLE			// 서버 안띄워도 되는 버전
//	#define _XABIL_TEST			// 특성 테스트
//#define _XMEM_POOL				// 메모리풀 사용 버전
//#define _UNIT_LOCK	
#ifdef WIN32
	#define _xIN_TOOL	// 구름 찍기 툴이 포함됨.
//	#define _XPROP_GRID
#endif
#endif

#ifdef _XCREW
#ifdef WIN32
#define _xIN_TOOL	// 툴이 포함됨.
#endif

#endif // _XCREW

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
	#define _XLOGIN_FACEBOOK
	#define _NICK_FIRST
	#define _XHEXA				// 6각형구름
	#define _XQUEST				// 퀘스트 시스템
	#define _XREADY			// 인게임 전투배치 씬
	#ifdef _DEBUG
		#define _DEV_PATCH	// 사내 개발용 패치서버 접속
	#endif
#endif

#define _XHOME_PROC			// 홈으로 갈때 접속 끊었다가 돌아오면 첨부터 다시 시작하도록 (전투중 제외)
#define _XCAMP_HERO2		// 영웅던전 리뉴얼버전
