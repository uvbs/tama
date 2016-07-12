#pragma once

#define _LOGIN_SERVER
//#define _DEV
#define MAX_READY_CONNECT		(MAX_CONNECT) 		// 접속대기열 수
#define MAX_CONNECT				MAIN->GetmaxConnect()		// 최대 동접자수

#define _XNEW_DBUACC			// XSAccount를 상속받는 XDBAccount 버전
#define _XDBMNG2
// #define _XPACKET2			// XPacket클래스 포맷 바뀜
// #define _XHEXA

//#ifdef _DEBUG
#define _XLOW_SERVER		// 개발중엔 시스템이 느려지지 않게 이것을 사용함.
//#endif
#ifdef _XUZHU
	#define _DEV_SERVER
//	#define _DUMMY_GENERATOR	// 이제 이걸로 하지말고 빌드솔루션으로 선택해서 할것.
#endif

