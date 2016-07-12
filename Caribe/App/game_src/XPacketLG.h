﻿#pragma once
// 로긴서버<->게임서버간 패킷
#include "VerPacket.h"

// enum이므로 중간에 끼워넣어도 됨
enum xtLGPacket {
	xLS2GS_NONE=0,
	xLS2GS_ACCOUNT_IS_ALREADY_CONNECT,
	xLS2GS_NEW_ACCOUNT,
	xLS2GS_NEW_DUMMY_ACCOUNT,	// 개발용 패킷
	xLS2GS_LOGIN_ACCOUNT_INFO,
	xLS2GS_DID_FINISH_LOAD,
	xLS2GS_SAVE_DUMMY_COMPLETE,
  xLS2GS_LOGIN_UNLOCK_FOR_BATTLE,
	xLS2GS_GAMESVR_INFO,
	xLS2GS_EXIT_GAMESVR,
	xLS2GS_RELOAD,
};
