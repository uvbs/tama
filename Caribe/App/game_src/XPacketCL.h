#pragma once

// 게임서버<->클라이언트간 패킷
#include "VerPacket.h"

// enum이므로 중간에 끼워넣어도 됨
enum xtCLPacket {
	xCL2LS_NONE=0,
	xCL2LS_DIFF_PACKET_VER,
	xCL2LS_ACCOUNT_SESSION_UUID,		// 세션키와함께 UUID로 접속
	xCL2LS_ACCOUNT_SESSION_ID,			// 세션키와함께 등록ID로 접속
	xCL2LS_ACCOUNT_LOGIN_FROM_UUID,		// 세션키없이 UUID로 접속
	xCL2LS_ACCOUNT_LOGIN_FROM_ID,		// 세션키없이 등록계정으로 접속
	xCL2LS_ACCOUNT_LOGIN_FROM_FACEBOOK,		// FACEBOOK으로 로그인
//	xCL2LS_ACCOUNT_NOT_EXIST,			// 접속을 시도했으나 계정이 없음
	xCL2LS_ACCOUNT_NEW,					// 새계정을 생성할것
	xCL2LS_ACCOUNT_REQ_LOGIN_INFO,		// 요청한 로그인계정 정보를 전달
	xCL2LS_NO_ACCOUNT,
	xCL2LS_ACCOUNT_RECONNECT_TRY,		// 이미 접속한 계정이므로 재접속을 해주세요.
	xCL2LS_ACCOUNT_NICKNAME_DUPLICATE,	// 닉네임 중복
	xCL2LS_WRONG_PASSWORD,				// 패스워드 실패
	xCL2LS_CLOSED_SERVER,				// 서버 점검중.
	
	xCL2LS_LOGIN_LOCK_FOR_BATTLE,   // 현재 공격받고 있어서 로그인이 잠시 막힘
	xCL2LS_LOGIN_LOCK_FREE,         // 로그인락 상태가 해제됨.
	xCL2LS_ERROR_LOG,								// 크래시덤프 파일 보내기
};