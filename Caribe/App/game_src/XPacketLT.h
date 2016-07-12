#pragma once
// 게임서버<->클라이언트간 패킷
#include "VerPacket.h"

// enum이므로 중간에 끼워넣어도 됨
enum xtPacket {
	xLS2ST_NONE=0,
	xLS2ST_SAMPLE,

	xLS2ST_ACCOUNT = 50000,
	xLS2ST_SERVER_INFO,
	xLS2ST_SERVER_EVENT,
	xLS2ST_SERVER_IAPINFO,
	xLS2ST_MAX
};
