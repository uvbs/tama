#pragma once

// enum이므로 중간에 끼워넣어도 됨
// 로그인서버와 게임서버간에 사용되는 패킷
enum xtLoginGameSvrPacket {
	xLS2GS_PK_NONE = 0,
	xLS2GS_PK_CONNECT_SUCCESS,
	xLS2GS_DID_FINISH_LOAD,
};