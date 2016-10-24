#pragma once
// 게임서버<->클라이언트간 패킷
#include "VerPacket.h"

// enum이므로 중간에 끼워넣어도 됨
enum xtPacket {
	xCL2GS_NONE=0,
	xCL2GS_SAMPLE,

	xCL2GS_ACCOUNT = 10000,
	xCL2GS_ACCOUNT_NEW,
	xCL2GS_ACCOUNT_NEW_FBID,
	xCL2GS_ACCOUNT_NAME_DUPLICATE,
	xCL2GS_ACCOUNT_NAME_REGIST,
	xCL2GS_ACCOUNT_NICKNAME_DUPLICATE,
	xCL2GS_ACCOUNT_NICKNAME_CHANGE,
	xCL2GS_ACCOUNT_INFO_SAVE,
//	xCL2GS_ACCOUNT_LOGIN_FROM_ID,
	xCL2GS_ACCOUNT_LOGIN_FROM_UUID,	
	xCL2GS_ACCOUNT_REQ_LOGIN,	
	xCL2GS_ACCOUNT_SESSION_ID,
	xCL2GS_ACCOUNT_SESSION_UUID,
	xCL2GS_ACCOUNT_LOGIN,
//	xCL2GS_DIFF_PACKET_VER,
	xCL2GS_ACCOUNT_NOT_EXIST,
	xCL2GS_ACCOUNT_FAILED_LOGIN,
	xCL2GS_ACCOUNT_SUCCESS_LOGIN,				// 게임서버에 로그인성공
	xCL2GS_ACCOUNT_LOGOUT,
	xCL2GS_ACCOUNT_RECONNECT_TRY,				//중복 접속 이니까 다시 접속 시도 하라고 알림.
	xCL2GS_ACCOUNT_RECONNECT_UUID,				//서버 요청으로 클라이언트가 UUID 로 재접속 시도 sessionkey 필요
	xCL2GS_ACCOUNT_RECONNECT_ID,				//서버 요청으로 클라이언트가 ID 로 재접속 시도 sessionkey 필요
	xCL2GS_ACCOUNT_DUPLICATE_LOGOUT,			//중복 접속 이니까 접속을 종료 하라고 알림.
	xCL2GS_ACCOUNT_REGISTER_FACEBOOK,			// 페북 계정 연결
	xCL2GS_ACCOUNT_PUBLIC_KEY,
	xCL2GS_ACCOUNT_RECONNECT_TO_GAMESVR,		// x초후 게임서버접속을 다시 시도하게 함.
	xCL2GS_LOGIN_FAILED_GAMESVR,
//	xCL2GS_BATTLE_INFO,							// 공격을 요청했을때 상대편 진영정보를 포함한 전쟁정보를 내려받는다.
  xCL2GS_SPOT_INFO,               // 정찰이나 전투를 요청했을때 그 스팟의 군단정보등을 받는다.
	xCL2GS_CANCEL_KILL,							// 소탕이 취소됨.
	xCL2GS_CHANGE_HELLO_MSG,
	xCL2GS_INIT_ABIL,								// 특성초기화
	xCL2GS_UNLOCK_TRAINING_SLOT,		

	xCL2GS_LOBBY =  xCL2GS_ACCOUNT + 1000,
	//xCL2GS_LOBBY_BUY_CASHITEM_BY_IAP,
	
	// 로비(월드씬)
	xCL2GS_LOBBY_USER_SPOT_RECON,				// 유저스팟 정찰
	xCL2GS_LOBBY_USER_SPOT_ATTACK,				// 유저스팟 공격
	xCL2GS_LOBBY_SPOT_RECON,					// 자원스팟 정찰
	xCL2GS_LOBBY_SPOT_ATTACK,					// 자원스팟 공격
	xCL2GS_LOBBY_SPOT_TOUCH,					// 스팟 방문
	xCL2GS_LOBBY_ATTACKED_SPOT_RESULT,			// 자원지가 공격당했고 결과가 도착.
	xCL2GS_LOBBY_ATTACKED_CASTLE,				// 성이 점령당했다.
	xCL2GS_LOBBY_SIMUL_ATTACKED_HOME,		// 본성이 침공당한 시뮬레이션
//	xCL2GS_LOBBY_BATTLE_RESULT,					// 전투 결과
	xCL2GS_LOBBY_JEWEL_MATCH_RESULT,			// 보석광산 매칭결과
	xCL2GS_LOBBY_JEWEL_SPOT_RECON,				// 광산 정찰 정보
	xCL2GS_LOBBY_JEWEL_BATTLE_INFO,				// 광산공격 정보
	xCL2GS_LOBBY_SULFUR_SPAWN,					// 유황 스팟이 리스폰되었다.
	xCL2GS_LOBBY_NPC_SPAWN,						// npc 스팟이 리스폰되었다.
	xCL2GS_LOBBY_SPOT_COLLECT,					// 스팟의 지역창고 자원을 수거
	xCL2GS_LOBBY_MANDRAKE_MATCH_RESULT,			// 만드레이크 매칭 결과
	xCL2GS_LOBBY_MANDRAKE_SPOT_RECON,			// 만드레이크 정찰정보
	xCL2GS_LOBBY_MANDRAKE_BATTLE_INFO,			// 만드레이크 공격정보
	xCL2GS_LOBBY_OPEN_CLOUD,					// 사용자가 구름의 가격레이블을 눌렀다.
	xCL2GS_LOBBY_CHANGE_SCALP_TO_BOOK,			///< 징표를 소모해서 책으로 교환한다.
	xCL2GS_LOBBY_REGEN_SPOT,					// 스팟 리젠(캐쉬기능)
	xCL2GS_LOBBY_REMATCH_SPOT,					// 다른 상대로 교체
	xCL2GS_LOBBY_WITHDRAW_MANDRAKE,				// 만드레이크 도전 중지
  xCL2GS_LOBBY_REQ_CAMPAIGN_GUILD_RAID,   // 길드레이드 캠페인정보 요청

	// 부대편성씬
//	xCL2GS_LOBBY_ADD_HERO_EXP,
	xCL2GS_LOBBY_CHANGE_HERO_LEGION,
	xCL2GS_LOBBY_CHANGE_HERO_EQUIPMENT,
	xCL2GS_LOBBY_SUMMON_HERO,		
	xCL2GS_LOBBY_NEW_SQUAD,						///< 새 부대를 군단에 생성한다.
	xCL2GS_LOBBY_MOVE_SQUAD,					///< 군단내 부대의 위치를 서로 바꾼다.
	xCL2GS_LOBBY_CHANGE_SQUAD,					///< 군단 정보 변경
//	xCL2GS_LOOBY_UPGRADE_SQUAD,					///< 부대 업그레이드 요청
	xCL2GS_LOBBY_RELEASE_HERO,
// 	xCL2GS_LOBBY_LEVELUP_SKILL,					///< 스킬레벨업
//	xCL2GS_TRAIN_HERO,					///< 영웅 렙업 훈련
	xCL2GS_CHECK_TRAIN_COMPLETE,						///< 훈련소 슬롯의 훈련이 끝났다.
	xCL2GS_TRAIN_COMPLETE_TOUCH,				///< 훈련완료후 터치
//	xCL2GS_PROVIDE_BOODY,						///< 전리품제공(메달/보옥)
//	xCL2GS_LEVELUP_CONFIRM,						///< 레벨업 확인
	xCL2GS_PROMOTION_HERO,						///< 영웅 승급
	xCL2GS_SUMMON_HERO_BY_PIECE,				///< 영혼석으로 영웅소환
	xCL2GS_TRAIN_HERO_BY_GOLD,					///< 금화로 영웅훈련

	// 창고
	xCL2GS_LOBBY_INVENTORY_EXPAND,				///< 인벤토리 확장

	// 무역상
	xCL2GS_LOBBY_TRADE,							///< 무역상을 통해 자원교환을 요청함.
	
	xCL2GS_LOBBY_CASH_TRADE_CALL,				// 캐쉬로 무역상 호출.
	xCL2GS_LOBBY_TIME_TRADE_CALL,				// 시간으로 무역상 호출.

	// 무기상
	xCL2GS_LOBBY_ITEM_SHOP_LIST_CASH,			///< 아이템 상점 판매 목록(캐쉬)
	xCL2GS_LOBBY_ITEM_SHOP_LIST_TIME,			///< 아이템 상점 판매 목록(시간)
	xCL2GS_LOBBY_ITEM_BUY,						///< 아이템 구매
	xCL2GS_LOBBY_ITEM_SELL,						///< 아이템 상점에 판매	
	xCL2GS_LOBBY_ITEM_SPENT,					///< 아이템 사용(소모 or??)
	
	// 귀중품 상점
// #ifdef _INAPP_GOOGLE
	xCL2GS_LOBBY_CASH_ITEM_BUY_PAYLOAD,			//구글 결제 Payload 요청
// #endif
	xCL2GS_LOBBY_CASH_ITEM_BUY_IAP,			// Inapp_Google

	//xCL2GS_LOBBY_OPTION_SOUND,					// 소리 옵션 변경
	//xCL2GS_LOBBY_OPTION_INVITE,					// 초대 옵션 변경

	// POST(우편함 팝업)
	xCL2GS_LOBBY_POST_ADD,						//우편물 추가됨.
	xCL2GS_LOBBY_POST_INFO,						//클라이언트 우편함 정보 요청
	//xCL2GS_LOBBY_POST_READ,						//클라이언트 우편함 읽음.(상대편에게 알려 줄까 말까?)
	//xCL2GS_LOBBY_POST_GETITEM,					//클라이언트 우편물 수령요청
	xCL2GS_LOBBY_POST_GETITEM_ALL,				//클라이언트 우편물 수령요청		
	xCL2GS_LOBBY_POST_DELETE,					//클라이언트 우편물 삭제 요청

	// 랭킹
	xCL2GS_LOBBY_RANKING_INFO,					//클라이언트 랭킹정보 요청.

	// 길드
	xCL2GS_LOBBY_GUILD_CREATE,					//클라이언트 연방 생성	
	xCL2GS_LOBBY_GUILD_INFO,					// 클라 플레이어의 길드 정보 요청
//	xCL2GS_LOBBY_GUILD_ALL_INFO,					// 모든 길드리스트 요청
//	xCL2GS_LOBBY_GUILD_UPDATE,					//클라이언트 연방 정보 갱신.
	xCL2GS_LOBBY_GUILD_EVENT,					// 유저의 길드관련 정보를 업데이트
	xCL2GS_LOBBY_GUILD_CHANGE_MEMBER,			// 연방원 정보 변경
	xCL2GS_LOBBY_GUILD_CHANEGE_OPTION,			// 연방 Option 정보 변경
	xCL2GS_LOBBY_GUILD_CHANEGE_CONTEXT,			// 연방 설명 변경
	xCL2GS_LOBBY_GUILD_JOIN_REQ,				//클라이언트 연방 합류 요청.
	xCL2GS_LOBBY_GUILD_JOIN_ACCEPT,				//클라이언트 연방 합류 승인.
	xCL2GS_LOBBY_GUILD_JOIN_RESULT,				//클라이언트 연방 합류
	xCL2GS_LOBBY_GUILD_OUT,						//클라이언트 연방 연방 탈퇴.
	xCL2GS_LOBBY_GUILD_KICK,					//클라이언트 연방 연방 추방.
//	xCL2GS_LOBBY_GUILD_KICK_RESULT,
	xCL2GS_LOBBY_GUILD_BUY_ITEM,				// 길드샵 아이템 구입

	xCL2GS_GAME = xCL2GS_LOBBY + 1000,	
	
	// battle
	xCL2GS_INGAME_REQ_FINISH_BATTLE,			// 전투종료후 플레이데이타를 보내고 전투결과를 받는다.
  xCL2GS_INGAME_CANCEL_BATTLE,          // 전투 취소
//	xCL2GS_INGAME_OCCUR_ENCOUNTER,				///< 유황스팟에서 인카운터가발생했다.
	xCL2GS_INGAME_SULFUR_RETREAT,				///< 유황 인카운터 에서 도주
	xCL2GS_INGAME_BATTLE_START,					///< 인게임 부대배치씬에서 시작 누름.
	xCL2GS_INGAME_ADD_BATTLE_LOG,				///< 추가된 전투기록을 보내준다.
	xCL2GS_IS_ONLINE,                   ///< 상대 유저가 온라인이냐
	xCL2GS_PRIVATE_RAID_ENTER_LIST,			///< 개인레이드. 출전영웅 리스트.
	xCL2GS_SPOT_UPDATE_FOR_VIEW,				///< 스팟정보 업데이트( 뷰어용 )
	xCL2GS_SPOT_UPDATE_FOR_BATTLE,			///< 스팟정보 업데이트( 전투용 )
	xCL2GS_ENTER_READY_SCENE,						///< 레디씬 진입요청/승인
	
	// tech
	xCL2GS_TECH_RESEARCH,						///< 특성치를 찍음.
	xCL2GS_TECH_LOCK_FREE,						///< 특성 잠금을 해제
	xCL2GS_TECH_RESEARCH_COMPLETE_NOW,			///< 캐시로 연구완료
	xCL2GS_TECH_UNLOCK_UNIT,					///< 유닛 잠금 해제
	
	// etc
	xCL2GS_SPOT_SYNC,							// 스팟 동기화
	xCL2GS_LEVEL_SYNC,							// 레벨,경험치 동기화
	xCL2GS_SYNC,										// 각종파라메터 동기화
	xCL2GS_UPDATE_SPECIAL_SPOT,					// 스페셜 스팟의 업데이트
	xCL2GS_ADD_ITEM,		
	xCL2GS_ADD_ITEM_LIST,						///< 여러개의 아이템을 한꺼번에 전송
	xCL2GS_ITEM_INVEN_SYNC,						///< 아이템 인벤 동기화
	xCL2GS_RESEARCH_COMPLETE,					///< 연구중인 특성의 연구가 끝남.
	xCL2GS_OCCUR_BUFF,							///< 계정버프 발생
	xCL2GS_FINISH_BUFF,							///< 계정버프 종료
	xCL2GS_CREATE_HERO,							///< 영웅생성
	xCL2GS_AP_CHECK_TIMEOVER,					///< 행동력 타이머 체크
	xCL2GS_ACCEPT_LEVELUP,						///< 계정레벨업을 확인함.
	xCL2GS_INVALID_VERIFY,						///< 서버와 데이타 비동기남.
	xCL2GS_SYNC_TRAIN_SLOT,						///< 훈련소 동기화
	xCL2GS_BIT_FLAG_TUTORIAL,					///< 튜토리얼 플래그
	xCL2GS_FILL_AP,								///< ap채우기
	xCL2GS_PAYMENT_ASSET,					///< 각종 asset을 젬이나 아이템으로 지불
	xCL2GS_TRAIN_COMPLETE_QUICK,				///< 훈련 즉시 완료
	xCL2GS_UNLOCK_MENU,							///< 기능잠금해제
	xCL2GS_CLICK_FOG_SQUAD,						///< 
	xCL2GS_CHECK_UNLOCK_UNIT,				///< 언록한 유닛을 클라에서 확인함.
	xCL2GS_TOUCH_SQUAD_IN_READY,		///< 전투준비씬에서 아군부대를 터치함.
	xCL2GS_CONTROL_SQUAD_IN_BATTLE,		//  전투씬에서 부대를 수동조작함.
	xCL2GS_CHECK_ENCOUNTER_LOG,			
	xCL2GS_GOTO_HOME,									// 홈버튼으로 앱에서 잠시 빠져나감.
	xCL2GS_SYNC_HELLO,								// 스팟의 인사말 메시지 갱신
	xCL2GS_SHOW_ADS,									// 광고비디오 시청
	xCL2GS_FINISH_SHOW_ADS,						// 광고시청 끝
	// quest
	xCL2GS_QUEST_OCCUR,							// 새 퀘스트 발생
	xCL2GS_QUEST_COMPLETE,						// 퀘스트 조건 모두 부합.
	xCL2GS_QUEST_REQUEST_REWARD,				// 퀘보상 수령
	xCL2GS_QUEST_UPDATE,						// 퀘스트 업데이트
	// campaign
	xCL2GS_CAMP_CLICK_STAGE,					///< 캠페인UI에서 스테이지를 선택했다.
	xCL2GS_RESULT_ENTER_GUILD_RAID,   ///< 길드레이드 진입요청
	xCL2GS_OPEN_GUILD_RAID,				///< 길드레이드 오픈
	// tutorial & cutscene
	xCL2GS_END_SEQ,						///< 컷씬의 시퀀스가 끝남.
	xCL2GS_UI_ACTION,					///< 씬에 진입
	xCL2GS_CAMP_REWARD,					///< 캠페인 보상요청

	xCL2GS_SYSTEM = xCL2GS_GAME + 2000,

	// push	
	xCL2GS_PUSH_GCM_REGIST_ID,						// 디바이스에서 생성된 regid를 서버에 저장한다.
	xCL2GS_REGIST_PUSHMSG,						// 푸쉬 보내기
	xCL2GS_UNREGIST_PUSHMSG,					// 푸쉬 지우기
	
	xCL2GS_SYSTEM_GAME_NOTIFY,					// 운영자 공지.
	xCL2GS_SYNC_BASE_INFO,						// 기본값 싱크
	xCL2GS_SYNC_RESOURCE,
	xCL2GS_PROP_SERIALIZE,				// 주요 프로퍼티 시리얼라이즈

	xCL2GS_MAX,

	// 여기서부터는 패킷아니고 타임아웃용임
	xTIMEOUT = xCL2GS_GAME + 9000,				// 서버로부터 접속이 끊어지면 발생하는 타임아웃
	xTIMEOUT_BUY_IAP,
	// cheat
#ifdef _CHEAT
	xCL2GS_CHEAT = xTIMEOUT + 1000,
	xCL2GS_CHEAT_CREATE_ITEM,
#endif // _CHEAT
};

enum xErrorCode {
	xEC_OK = 0,
	xEC_QUICK_OK,
	xEC_ERROR = 0x1000,
	xEC_IS_NOT_RESEARCHING,					// 연구중이 아님.
	xEC_NOT_YET_FINISH,			// 아직 연구 안끝남.
	xEC_NOT_ENOUGH_CASH,					// 캐쉬부족함.
	xEC_NOT_ENOUGH_GOLD,					// 금화부족함.
	xEC_IS_NOT_TRAINING,					// 훈련중이 아님
	xEC_NO_MORE_PROVIDE,					// 더이상 제공안됨
};

