﻿#pragma once

#define VER_ACCOUNT		1

#define VER_CGPK		140		// 클라이언트<->게임서버 간 패킷 버전
#define VER_CLPK		103		// 클라이언트<->로긴서버 간 패킷버전
#define VER_CPPK		100		// 클라이언트<->패치서버 간 패킷버전
#define VER_LGPK		102		// 게임서버 <->로긴서버간 패킷버전

// 프로토콜 바뀐 내역 아래다 기록 남길것.
/**
	VER_CGPK 103
	VER_CLPK 101
		102	크래시덤프 전송
		103 로그인과정 변경
	VER_LGPK
		101 스팟 시리얼라이즈에 score추가.
		102 로그인 과정 변경
	VER_CGPK 105	스페셜스팟 관련 변경
	VER_CGPK 106	드랍장비
	VER_CGPK 107	캠페인
			108		부대레벨업 변경
			109		접속시 클라버전 전송
			111		maxap
			112		훈련소 idxSlot -> snSlot으로 교체
			114		SendResearchComplete변경
			115		로그인
			116-1	훈련완료
			116		유닛잠금
			117		유저특성반영
			118		드랍아이템 변경
			123		보석광산 관련
			127   만드레이크 전투정보에 전투세션 추가.
			128		특성초기화 & 연구 관련 패킷변경
			129		유닛언락 지역에 바인딩
			130		월드데이터 압축저장.
			131		걍 올림.
			132		인앱수정
			133		softnyx 결재
			134		????
			135   전투결과 데이타의 렙업영웅 정보 추가
			136		길드수정
			137		프로퍼티 시리얼라이즈
			139		구름깔때등등 지불방법 추가.
			140		요일스팟 리뉴얼
*/
