#pragma once
#include <algorithm>
#include "XFramework/client/XClientMain.h"

// 게임의 논리적 해상도
#define XRESO_WIDTH		640
#define XRESO_HEIGHT	360

// XCheatOption.h로 옮겨짐

class XGame;
////////////////////////////////////////////////////////////////
class XAppMain : public XClientMain
{
public:
	enum xtTestMode {
		xTEST_NONE,
		xTEST_PARTICLE,
	};
	static XAppMain* sCreate( XE::xtDevice device, int widthPhy, int heightPhy );
#ifdef _CHEAT
//	xtToolMode m_ToolMode = xTM_NONE;			///< 현재 툴모드인가.
	BOOL m_bDebugSquad;		///< 디버그용으로 한부대씩만 나오게 하는 모드
	BOOL m_bDebugViewRefCnt;	///< 유닛들의 레퍼런스 카운트 출력
	BOOL m_bDebugViewSquadRadius;	///< 분대의 근접거리를 표시
	BOOL m_bDebugViewUnitSN;		///< 각 유닛들의 sn값을 표시
	BOOL m_bDebugViewTarget;		///< 타겟의 sn값을 표시
	BOOL m_bDebugRespawnNumRandom;	///< 유닛수 랜덤인가 풀인가
	int m_nDebugRespawnUnit;		///< 0:랜덤 1:소형 2:중형 3:대형 
	BOOL m_bDebugViewBuff;			///< 버프정보 보이기
	BOOL m_bDebugSlowFrame;			///< 프레임수 떨어트리기
	BOOL m_bDebugViewBoundBox;		///< 바운딩박스 보이기
	BOOL m_bDebugViewSquadSN;		///< 부대시리얼번호
	BOOL m_bDebugViewHp;			///< hp보이기
	int m_nToolShowCloud = 2;			///< 구름 표시방법. 0:hide 1:alpha 2:show
	BOOL m_bToolViewHexaLine;		///< 헥사구름 육각외곽선 표시
	XGAME::xtSpot m_modeToolSpotView = XGAME::xSPOT_NONE;	// none은 모두 보이기
	bool m_bDebugQuestList = false;
	bool m_bDebugViewDamage = false;	///< 타격 데미지숫자 보이기
	bool m_bDebugViewAttackedDamage = false;	///< 피격 데미지숫자 보이기(적이 우리편을 친거)
	bool m_bDebugHeroImmortal = false;			///< 영웅들 무적
	bool m_bDebugUnitImmortal = false;			///< 유닛들 무적
	bool m_bDebugViewSquadInfo = false;		///< 전투씬에서 선택한 부대 정보
	bool m_bDebugViewHeroLevel = false;		///< 적 영웅들 레벨보이기
	bool m_bXuzhuMode = false;
	bool m_bDebugViewBoundBoxSpot = false;
	bool m_bDebugViewAreaLabel = false;		///< 구름라벨 모두 보이기
	bool m_bDebugViewSquadsHp = false;		///< 각 부대 hp보이기
	bool m_bStopPassive = false;					///< 패시브스킬 금지
	bool m_bStopActive = false;					///< 액티브스킬 금지
	bool m_bViewCutScene = true;
	bool m_bViewHexaIdx = false;				// 헥사 인덱스
	bool m_bViewAreaCost = false;				// 툴모드시 지역가격을 표시할건지.
	bool m_bViewBgObjBoundBox = true;		// 
	bool m_bViewBgObj = true;
	bool m_bViewSpotInfo = false;			// 각 스팟 디버깅 텍스트 보이기
	bool m_bBattleLogging = false;		// 전투중 일어나는 상황 콘솔에 출력.
	bool m_bUnitOnlyOne = false;			// 디버깅정보 출력할때 부대중 유닛하나만 적용할지.
	BOOL m_bShowFace = TRUE;					// 전투중 초상화 보이기/감추기
	BOOL m_bWaitAfterWin = false;			// 상대부대 전멸 후 대기
	xtTestMode m_ModeTest = xTEST_NONE;
//	bool m_bLow = false;
	int m_idxViewAtlas = -1;
#ifdef _CHEAT
	bool m_bViewMemoryInfo = false;
	std::string m_strLayoutXml;
	std::string m_strLayoutNode;
	std::string m_strReloadCmd;
	bool m_bReloadWhenRetryConstant = FALSE;		// retry나 recreate시 재로드할건지 말건지.
	bool m_bReloadWhenRetryPropUnit = FALSE;
	bool m_bReloadWhenRetryPropSkill = FALSE;
	bool m_bReloadWhenRetryPropLegion = FALSE;
private:
	DWORD m_dwFilter;
	DWORD m_dwNoDraw = 0;
	DWORD m_dwOption = 0;
public:
	GET_SET_BIT_ACCESSOR( Filter );
	GET_SET_BIT_ACCESSOR( Option );
	GET_SET_BIT_ACCESSOR( NoDraw );
	
#else
	bool m_bViewMemoryInfo = false;
#endif
#endif // cheat
private:
//	_tstring m_strUUIDFile;				// uuid파일명

	void Init() {
//		m_strUUIDFile = _T("login.txt");
#ifdef _CHEAT
		m_bDebugSquad = FALSE;
		m_bDebugViewRefCnt = FALSE;
		m_bDebugViewSquadRadius = FALSE;
		m_bDebugViewUnitSN = FALSE;
		m_bDebugViewTarget = FALSE;
		m_bDebugRespawnNumRandom = 0;
		m_nDebugRespawnUnit = 0;
		m_bDebugViewBuff = FALSE;
		m_bDebugSlowFrame = FALSE;
		m_bDebugViewBoundBox = FALSE;
		m_bDebugViewSquadSN = FALSE;
		m_bDebugViewHp = FALSE;
		m_bToolViewHexaLine = FALSE;
#endif
	}
	void Destroy();
public:
	XAppMain();
	virtual ~XAppMain() { Destroy(); }
	//
//	GET_TSTRING_ACCESSOR( strUUIDFile );
	//
	void DidFinishCreate( void );
	void DidFinishInitEngine( void );
	XEContent* CreateGame( void );
	void Draw( void );
	void ConsoleMessage( LPCTSTR szMsg );
	void SaveCheat( FILE *fp );
	void LoadCheat( CToken& token );
	void OnError( XE::xtError codeError, DWORD p1, DWORD p2 );
	BOOL DoEditBox( TCHAR *pOutText, int lenOut );
	BOOL OnSelectLoadType( XE::xtLoadType typeLoad ) override;
private:
	bool RequestCheatAuth() override;
	void FrameMove() override;
};


extern XAppMain *XAPP;