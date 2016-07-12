#pragma once

#define _GAME_SERVER
//#define _DEV
#define _DEV_SERVER
#define MAX_READY_CONNECT		(MAX_CONNECT) 		// 접속대기열 수
#define MAX_CONNECT				XEnv::sGet()->GetmaxConnect()		// 최대 동접자수

#define _XPROP_SERIALIZE		// 프로퍼티 시리얼라이즈 버전

#ifdef _XUZHU
#endif

