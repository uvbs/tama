#pragma once
//#include "XSceneBase.h"
#include "XSceneBase.h"
//#include "XFramework/client/XLayout.h"
#include "XFramework/client/XLayout.h"
#include "XPropHero.h"
#include "XSceneEquip.h"

class XGame;
class XWndInvenHeroElem;
class XAppLayout;
class XHero;
class XSceneUnitOrg : public XSceneBase
{
private:
	static XSceneEquip::xSPM_EQUIP s_EquipParam;
private:
	enum xTab { xTAB_NONE,
			xTAB_SKILL,
			xTAB_EQUIP };
	//선택한 영웅
	//노란색 테두리를 on off 해주기위해 XWndInvenHeroElem로 저장
	XWndInvenHeroElem *m_pSelHero;
	_tstring m_strIdSoulStone;	//소울 스톤을 선택한 경우에 여기다가 식별자 저장
	XLayoutObj m_Layout;
	XHero *m_pKeepHero;				//경험치책 먹이고 레벨 오르면 변동스텟 비교해줄 히어로
	BOOL m_bEquip;
	static xTab s_tabCurr;		// 현재 선택된 탭
private:
	//
	void Init()  {
		m_pSelHero = NULL;
//		m_pLayout = NULL;
		m_pKeepHero = nullptr;
		m_bEquip = FALSE;
	}
	void Destroy();
	void SetSprSquad(XWnd *pWnd, XHero *pHero);

	//sort용 메소드
	static bool CompClan(XHero *p1, XHero *p2);
	static bool CompGrade(XHero *p1, XHero *p2);
	static bool CompParty(XHero *p1, XHero *p2);

protected:
public:
	XSceneUnitOrg(XGame *pGame, XSPSceneParam& spBaseParam);
	virtual ~XSceneUnitOrg(void) { Destroy(); }
	///< 
	//
	int OnBack(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnClickTabSkill(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnClickTabEquipment(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnClickInventory(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnClickLegionSelect(XWnd *pWnd, DWORD p1, DWORD p2);	//유닛 선택 버튼
	int OnClickListLegion(XWnd *pWnd, DWORD p1, DWORD p2);	//유닛 디테일
	int OnSelectLegion(XWnd *pWnd, DWORD p1, DWORD p2);	//유닛 디테일에서 선택버튼
	int OnSelectExit(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnSelectDetailExit(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnClickLevelupHero(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnClickProvideSkill(XWnd *pWnd, DWORD p1, DWORD p2);
	//int OnClickLevelupPassive(XWnd *pWnd, DWORD p1, DWORD p2);
//	int OnSelectLevelupHero(XWnd *pWnd, DWORD p1, DWORD p2);
	void OnRecvAddHeroExp(ID idCallback, XHero *pHero, BOOL bLevelup);
	void OnRecvChangeHeroLegion(ID idWnd);
	int OnFinishLevelup(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnDoExitLegion(XWnd* pWnd, DWORD p1, DWORD p2);
	int OnClickSquadUpgrade( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickCheatHeroLvUp( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnSelectSkillLevelup(XWnd *pWnd, DWORD p1, DWORD p2);
// 	int OnSelectSquadLevelup(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnClickUnitInfo(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnClickEquipSlot(XWnd *pWnd, DWORD p1, DWORD p2);
	// virtual
	virtual void Create( void );
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );

	void Update(void) override;
	void OnAutoUpdate() override;
	void UpdateHeroInven();
	void UpdateHeroInfo(void);
	void UpdateHeroBasic();
	void UpdateHeroLevelUpgrade( XHero *pHero );
	void UpdateSquadInfo();
	void UpdateSquadUpgradeButton( XHero *pHero );
	void UpdateSkillInfo();
// 	void UpdateSkillInfo(void);
	void UpdateSkillScroll( XHero *pHero, XGAME::xtIdxSkilltype typeSkill, XGAME::xtTrain typeTrain, const char *cSuffix );
	void UpdateSkillUpgradeButton( XHero *pHero, XGAME::xtIdxSkilltype typeSkill, XGAME::xtTrain typeTrain, const char *cSuffix );
	void UpdateTabEquipment(void);
	void UpdateTrainingCenterButton( XHero *pHero );
	int OnClickReleaseHero( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickReleaseHeroOk(XWnd *pWnd, DWORD p1, DWORD p2);
	void OnRecvReleaseHero( ID snHero );
	void OnRecvLevelupSquad(ID snHero);
//	void DoCreateLevelupReady( XHero *pHero, XGAME::xtTrain type );
//	int OnClickProvideMedal( XWnd* pWnd, DWORD p1, DWORD p2 );
// 	int OnClickLevelupConfirm( XWnd* pWnd, DWORD p1, DWORD p2 );
	
	void SetpKeepHero(XHero *pHero);
	int OnClickInfoSquad( XWnd* pWnd, DWORD p1, DWORD p2 );
//	int OnClickNewLevelupHero( XWnd* pWnd, DWORD p1, DWORD p2 );
//	int OnClickTrainAccept( XWnd* pWnd, DWORD p1, DWORD p2 );
//	void OnRecvLevelupConfirm( XHero *pHero, XGAME::xtTrain type );
	int OnClickTrainingCenter( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickLvupComplete( XWnd* pWnd, DWORD p1, DWORD p2 );
// 	int OnClickActiveupComplete( XWnd* pWnd, DWORD p1, DWORD p2 );
// 	int OnClickPassiveupComplete( XWnd* pWnd, DWORD p1, DWORD p2 );
// 	int OnClickSquadupComplete( XWnd* pWnd, DWORD p1, DWORD p2 );
	//void OnRecvLevelupSkill(void);
	int OnEnterScene( XWnd*, DWORD, DWORD ) override;

	int OnClickPromotion(XWnd* pWnd, DWORD p1, DWORD p2);
};

extern XSceneUnitOrg *SCENE_UNITORG;

