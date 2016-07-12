#pragma once
#if defined(_SERVER) || defined(_XBOT)
#include <WinSock2.h>
#endif // defined(WIN32) && defined(_XBOT)

XE_NAMESPACE_START( XE )
//
enum xtServerLog {
	xSLOG_NONE,
	xSLOG_OK,
	xSLOG_DESTROY_CONNECT,		// 커넥션이 삭제되기전
	xSLOG_DESTROY_USER,				// 유저 삭제되기전
};
// 시스템이 예약한 패킷
enum xtPacketReserved {
	xXEPK_CONNECT_COMPLETE=0x80000001,
	xXEPK_REQUEST_HB = 0x7FFF0000,		// 커넥션이 살아있는지 확인하는 요청 패킷.
	xXEPK_RESPONSE_HB = 0x7FFF0001,	// 커넥션이 살아있음을 알리는 응답 패킷
//		xXEPK_NOW_WORKING = 0x7FFF0002,		// 현재 클라가 잠시 하는일이 있으니 허트비트로 인한 연결을 끊지 말아달라.
//		xXEPK_RELEASE_WORKING = 0x7FFF0003,		// 클라로부터 일 끝났다는 메시지
};
// XNetWorkDelegate 에러 코드
enum xtNError {
	xN_OK,
	xN_TIMEOUT,
	xN_FAILED_CREATE_SOCKET,
	xN_FAILED_BIND_SOCKET

};

#if defined(_SERVER) || defined(_XBOT)
struct xOVERLAPPED : public	WSAOVERLAPPED {
	int typeEvent;		// 1:send 2:recv
};
#endif // defined(WIN32) && defined(_XBOT)
//
XE_NAMESPACE_END; // XE

