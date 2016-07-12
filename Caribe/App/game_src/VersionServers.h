#pragma once

// 모든 서버 공통 디파인
#ifdef _XDEBUG
#define _XCONSOLE
#endif
#define _XPACKET2			// XPacket클래스 포맷 바뀜
#define _XHEXA

// ex) _DEV_LEVEL=DLV_DEV_CREW;
// #define DLV_LOCAL					1		// 개발자 개인자리의 개발하는 서버
// #define DLV_DEV_CREW			2		// 192번 개발섭(개발자만 접속)
// #define DLV_DEV_EXTERNAL	3		// 외부공개용 개발 테섭(외부관계자 접속용)
// #define DLV_OPEN_BETA			4		// 오픈베타(일반유저용)
// #define DLV_MASTER				5		// 정식버전

//#define _DEV_LEVEL				DLV_LOCAL
#define _INAPP_GOOGLE
#define _NEW_INAPP				// 새 방식의 결제 프로세스
#define _XPACKET_BUFFER		// 서버의 연결이 끊겼을때 보내야할 패킷을 잠시 보관함.
#define _XCAMP_HERO2			// 영웅캠페인 리뉴얼
