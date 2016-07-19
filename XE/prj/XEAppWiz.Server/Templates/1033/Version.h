#pragma once

#define _GAME_SERVER
#ifdef _XDEBUG
#define _XCONSOLE
#endif
//#define _DEV
#define _DEV_SERVER
#define MAX_READY_CONNECT		(MAX_CONNECT) 		// 접속대기열 수
#define MAX_CONNECT				MAIN->GetmaxConnect()		// 최대 동접자수
#define _XPACKET2			// XPacket클래스 포맷 바뀜

#ifdef _DEBUG
#define _XLOW_SERVER		// 개발중엔 시스템이 느려지지 않게 이것을 사용함.
#endif
