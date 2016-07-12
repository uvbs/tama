#pragma once

#define _DB_SERVER
//#define _DEV
#define _DEV_SERVER
//#define MAX_READY_CONNECT		(MAX_CONNECT) 		// 접속대기열 수
#define MAX_ACCOUNT_CONNECT				10000	// 최대 계정 생성수
#define MAX_GAMESVR_CONNECT				256	// 최대 접속 게임서버 수

#define _XNEW_DBUACC			// XDBUAccount가 XAccount를 상속받는 버전
#define _XDBMNG2			// XDBMng2 를 쓰는 버전
// #define _XPACKET2			// XPacket클래스 포맷 바뀜
// #define _XHEXA

//#ifdef _DEBUG
//#endif

#ifdef _XUZHU
//	#define _DUMMY_GENERATOR	// 이제 이걸로 하지말고 빌드솔루션으로 선택해서 할것.
//#define _FAKE_PAYLOAD		// 임시 페이로드값을 사용함.
#define _XLOW_SERVER		// 개발중엔 시스템이 느려지지 않게 이것을 사용함.
#endif

#ifdef _KAREISSMA
//	#define _XMNGTOOL
#endif