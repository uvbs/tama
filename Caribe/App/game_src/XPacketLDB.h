#pragma once
// 로긴서버<->게임서버간 패킷
#include "VerPacket.h"

// enum이므로 중간에 끼워넣어도 됨
enum xtLDBAPacket {
	xLS2DBA_NONE=0,	
	xLS2DBA_FIND_ACCOUNT_UUID,	
	xLS2DBA_FIND_ACCOUNT_SESSION_UUID,
	xLS2DBA_FIND_ACCOUNT_USERID,			//계정.
	xLS2DBA_FIND_ACCOUNT_SESSION_USERID,	//계정.Session ID
};
