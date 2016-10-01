#pragma once
// cpp�ҽ������� ���ǰ� ���ҽ��ε� ���Ǵ� ������Դϴ�.
// ���⼭ ������ ������� ������Ƽ�� ��ũ��Ʈ ��� ����� ���ֽ��ϴ�.
#ifdef __cplusplus
// �Ʒ��� ���ǵ� ������� ��� ��ƿ��� ���� �ִ�.
#define DEFINE_SAMPLE		11


namespace XGAME
{
	///< ��ȭ
	enum xCurrency {
		xCUR_NONE,
		xCUR_WON_KOREA,
		xCUR_US_DOLLAR,
	};

// 	///< 8����
// 	enum xtDir { DIR_E, 
// 				DIR_ES, 
// 				DIR_S, 
// 				DIR_WS, 
// 				DIR_W, 
// 				DIR_WN, 
// 				DIR_N, 
// 				DIR_EN };	
	
	///< ���� ������
	enum xtSize { 
				xSIZE_NONE,
				xSIZE_SMALL,
				xSIZE_MIDDLE,
				xSIZE_BIG,
				xSIZE_MAX,
				};
	// ���� ����
	enum xtUnitFilter {
		xUF_NONE,
		xUF_SMALL   = 0x0001,
		xUF_MIDDLE  = 0x0002,
		xUF_BIG		= 0x0004,
		xUF_SIZE_ALL = 0x0007,
		xUF_TANKER  = 0x0008,
		xUF_RANGE	= 0x0010,
		xUF_SPEED	= 0x0020,
		xUF_ATKTYPE_ALL = 0x0038,
		xUF_ALL = 0xffffffff,
	};
	///< ���� Ÿ��
	enum xtAttack {
					xAT_NONE,
					xAT_TANKER,
					xAT_RANGE,
					xAT_SPEED,
					xAT_MAX,
				};
	enum xtUnitRank {
		xUR_NONE,
		xUR_SOLDIER,		///< ����
		xUR_HERO,			///< ����
	};
	enum xtStat {
		xSTAT_NONE,
		xSTAT_ATK_MELEE,
		xSTAT_ATK_RANGE,
		xSTAT_DEF,
		xSTAT_HP,
		xSTAT_SPEED_ATK,
		xSTAT_SPEED_MOV
	};
// 	enum xtResource {	
// 						xRES_WOOD,
// 						xRES_IRON,
// 						xRES_JEWEL,
// 						xRES_SULFUR,		///< ��Ȳ
// 						xRES_MANDRAKE,
// 						xRES_MAX,
// 						xRES_GOLD = -1,						
// 						xRES_WOOD_IRON = -2,
// 						xRES_CASH = -3,
// 						xRES_GUILD_COIN = -4,
// 						xRES_ALL = -0x7f,
// 	};
	// ������ ���� �ٲ��� ����.
	enum xtResource {	
		xRES_WOOD,
		xRES_IRON,
		xRES_JEWEL,
		xRES_SULFUR,		///< ��Ȳ
		xRES_MANDRAKE,
		xRES_MAX,
		xRES_GOLD = xRES_MAX,
		xRES_CASH,
		xRES_GUILD_COIN,
		xRES_WOOD_IRON,
		xRES_ALL,
		xRES_NONE = xRES_ALL,
	};

	enum xtPOSTResource {
		xPOSTRES_GOLD,
		xPOSTRES_GEM,
		xPOSTRES_ITEMS,
		xPOSTRES_RESOURCE,
		xPOSTRES_GUILD_COIN,
		xPOSTRES_MAX,
		xPOSTRES_INIT = 999,
	};

	enum xtUnit { 
				xUNIT_NONE,
				xUNIT_SPEARMAN,
				xUNIT_ARCHER,
				xUNIT_PALADIN,
				xUNIT_MINOTAUR,
				xUNIT_CYCLOPS,
				xUNIT_LYCAN,
				xUNIT_GOLEM,
				xUNIT_TREANT,
				xUNIT_FALLEN_ANGEL,		///< Ÿõ��
				xUNIT_MAX,
				xUNIT_ANY = -1,
	};

	///< ����Ÿ�� xtAttack�� �Բ� ����.
// 	enum xtLead { xLD_NONE,
// 				xLD_TANKER,		///< �������� ����
// 				xLD_RANGE,		///< ���Ÿ� ���� ����
// 				xLD_SPEED,		///< ���ǵ����� ����
// 				xLD_MAX,
//	};

	///< Ŭ��
// 	enum xtClan { xCL_NONE,
// 				xCL_CROW,			///< ��� 
// 				xCL_IRONLORD,		///< ��ö����
// 				xCL_FREEDOM,		///< ��������
// 				xCL_ANCIENT,		///< ��빮��
// 				xCL_FLAME,			///< �Ҳ�
// 				xCL_MAX,
// 				xCL_RANDOM=-1,		///< ��������
// 	};

	//���� ���
	enum xtUserGrade{
				xUG_NORMAL,			//�Ϲ�
				xUG_PRIMIUM,		//���
				xUG_MAX,			
	};	

	///< Ŭ����
	enum xtClass { xCLS_NONE,
				xCLS_TANKER,		///< ����������
				xCLS_RANGE,			///< ���Ÿ��� ����
				xCLS_HORSE,			///< �⸶��
				xCLS_HEAL,			///< ������
				xCLS_WIZARD,		///< ��������
	};

	///< ���
	enum xtGrade {
				xGD_NONE,
				xGD_COMMON,			///< �Ϲ�
				xGD_VETERAN,		///< ���׶�
				xGD_RARE,			///< ���
				xGD_EPIC,			///< ����
				xGD_LEGENDARY,		///< ����
				xGD_MAX,
	};
	// ���� ���
	enum xtGradeLegion {
		xGL_NONE,
		xGL_NORMAL,		// �Ϲ� �δ�
		xGL_ELITE,		// ���� �δ�(hp�� �Ϲݺδ��� 2��)
		xGL_RAID,		// ���̵� �δ�(hp�� �Ϲݺδ��� 4��)
		xGL_MAX,
	};

	///< ������ Ÿ��
	enum xtItem {
		xIT_NONE,		///< �ʱ�ȭ��
		xIT_EQUIP,		///< ����������
		xIT_SCALP,		///< Ŭ�� ¡ǥ
		xIT_SCROLL,		///< ��ų ����ǰ
		xIT_MEDAL,		///< �δ���ۿ� ����ǰ
		xIT_BOOK,		///< å
		xIT_QUEST,		///< ����
		xIT_CASH,		///< ������
		xIT_EVENT,		///< �̺�Ʈ
		xIT_ETC,		///< ��Ÿ ����
		xIT_SOUL,
	};

	///< ������ �����(�κ����� ���)
	enum xtItemUse {
		xIU_NONE,		///< ���Ұ���
		xIU_USE,		///< �κ����� ��밡��
	};

	///< ���������� ����
	enum xtParts {
		xPARTS_NONE,			///< �ʱ�ȭ��
		xPARTS_HEAD,			///<  �Ӹ�
		xPARTS_CHEST,			///< ����
		xPARTS_HAND,			///< ��
		xPARTS_FOOT,			///< ��
		xPARTS_ACCESSORY,		///< �Ǽ�����
		xPARTS_MAX,
	};

	// �ߵ��Ķ����(����: ADJ�� ���� �Ķ���ʹ� ����ġ�� �ƴϴ�)
	// �̰͵� �������ø��� �������ְ� ��������Ƿ� ���°��� ������.
	enum xtParameter {
		xNONE_PARAM = 0,
		xADJ_NONE = 0,
		xADJ_ATTACK,					// ���ݷº���
		xADJ_ATTACK_MELEE_TYPE,			// �������ݷº���
		xADJ_ATTACK_RANGE_TYPE,			// ���Ÿ����ݷº���
		xADJ_DEFENSE,					// ���º���
		xADJ_MAX_HP,					// �ִ�ü�º���
		xADJ_HP,						// ü�º���
		xADJ_HP_REGEN,					// ü�¸����纸��
		xADJ_HP_REGEN_SEC,				// ü�¸����ӵ�����
		xADJ_MOVE_SPEED,				// �̼Ӻ���
		xADJ_ATTACK_SPEED,				// ���Ӻ���
		xADJ_ATTACK_RANGE,				// ���ݻ�Ÿ�����
		xADJ_HIT_RATE,					// ����������
		xADJ_DAMAGE,					// ������غ���
		xADJ_PHY_DAMAGE,				// �������غ���
		xADJ_MAG_DAMAGE,				// �������غ���
		xADJ_PHY_DAMAGE_RECV,			// �޴¹������غ���
		xADJ_MAG_DAMAGE_RECV,			// �޴¸������غ���
		xADJ_MELEE_DAMAGE_RECV,			// �޴±������غ���
		xADJ_RANGE_DAMAGE_RECV,			// �޴¿��Ÿ����غ���
		xADJ_DAMAGE_RECV,				// �޴¸�����غ���
		xADJ_HEAL,						// ��������
		xADJ_HEAL_RECV,					// �޴���������
		xADJ_SKILL_POWER,				// ��ų�Ŀ�����
		xADJ_SIGHT,						// �þߺ���
		xADJ_PENETRATION_RATE,			// ����������
		xADJ_PENETRATION_PROB,			// ����Ȯ������
		xADJ_EVADE_RATE,				// ȸ��������
		xADJ_EVADE_RATE_MELEE,			// ����ȸ��������
		xADJ_EVADE_RATE_RANGE,			// ���Ÿ�ȸ��������
		xADJ_EVADE_RATE_SMALL,			// ����ȸ��������
		xADJ_EVADE_RATE_MIDDLE,			// ����ȸ��������
		xADJ_EVADE_RATE_BIG,			// ����ȸ��������
		xADJ_BLOCKING_RATE,				// �������������
		xADJ_CRITICAL_RATE,				// ġ��Ÿ������
		xADJ_CRITICAL_POWER,			///< ġ��Ÿ�������.
		xADJ_CRITICAL_RATE_RECV,		///< �޴�ġ��Ÿ������
		xADJ_CRITICAL_RECV,				// �޴¸��ġ��Ÿ���غ���
		xADJ_MELEE_CRITICAL_RECV,		// �޴±���ġ��Ÿ���غ���
		xADJ_RANGE_CRITICAL_RECV,		// �޴¿��Ÿ�ġ��Ÿ���غ���
		xADJ_ADD_DAMAGE_TRIBE,			///< �߰���������������
		xADJ_ADD_DAMAGE_CLASS,			///< �߰���������������
		xADJ_ATTACK_SPEED_SMALL,		///< �������Ӻ���
		xADJ_ATTACK_SPEED_MIDDLE,		///< �������Ӻ���
		xADJ_ATTACK_SPEED_BIG,			///< �������Ӻ���
		xADJ_DAMAGE_SMALL,				///< �������غ���
		xADJ_DAMAGE_MIDDLE,				///< �������غ���
		xADJ_DAMAGE_BIG,				///< �������غ���
		xADJ_DAMAGE_TANKER,				///< ���������غ���
		xADJ_DAMAGE_RANGE,				///< ���Ÿ������غ���
		xADJ_DAMAGE_SPEED,				///< ���ǵ������غ���
		xADJ_DAMAGE_SPEARMAN,			///< â�����غ���
		xADJ_DAMAGE_ARCHER,			///< �ü����غ���
		xADJ_DAMAGE_PALADIN,			///< ������غ���
		xADJ_DAMAGE_MINOTAUR,			///< �̳����غ���
		xADJ_DAMAGE_CYCLOPS,			///< �������غ���
		xADJ_DAMAGE_LYCAN,			///< �������غ���
		xADJ_DAMAGE_GOLEM,			///< �����غ���
		xADJ_DAMAGE_TREANT,			///< ��Ʈ���غ���
		xADJ_DAMAGE_FALLEN_ANGEL,			///< Ÿõ���غ���
		xADJ_DAMAGE_FIRE,			///< ȭ�����غ���
		xADJ_SKILL_COOL,				///< ��Ÿ�Ӻ���/������ð�����
		xADJ_VAMPIRIC,					///< �����纸��

		// �̰����� �߰� �Ͻÿ�
		xMAX_PARAM,
		xADJ_HARDCORD,					///< �ϵ��ڵ���
		// TODO: ����ġ�� �ƴ� �Ķ���ʹ� �̰��� �߰��Ͻÿ�
		xHP=-1,				// ü��(����:�����ƴ�)
		xMP=-2,				// ����(����:�����ƴ�)
// 		xVAMPIRIC_HP=-3,		// ü������
// 		xVAMPIRIC_MP=-4,		// ��������
	};

	enum xtState {
		xST_NONE,
		xST_SLEEP,			///< ����(
		xST_STUN,			///< ����
		xST_HOLD,			///< ����
		xST_BLEEDING,		///< ����
		xST_POISON,			///< �ߵ�(���迭 ��Ʈ�������̸� �ص��� �����ϴ�. �׷��� DOT���������� �������� �� ũ��.)
		xST_BLIND,			///< ����
		xST_SLOW,			///< ��ȭ
		xST_FROZEN,			///< ����
		xST_ICE,			///< ����(������ ������ ������)
		xST_PARALYSIS,	///< ����(����μ� ������ ����)
		xST_BURN,			///< ȭ��(������ ȭ���� ���̴�?)
		xST_CHAOS,			///< ȥ��(�ǾƸ� �ĺ����� ���ϰ� �����Ѵ�.)
		xST_FEAR,			///< ����(Ÿ�ٰ� �ݴ�������� ��������)
		xST_SILENCE,		///< ħ��(��ų �� Ư�����Ұ�)
		xST_INVISIBLE,		///< ����ȭ
		xST_ENTANGLE,		///< �ְ���(�̵��Ұ�)
		xST_TAUNT,			///< ����
		xST_DECAY,			///< ����(���п� �ɸ����� Ÿ���ϸ� �߰��������� ����)
		xST_FURY,			///< �г�(�̵��ӵ� ����ȿ���� �ɸ��� �ʴ´�)
		xST_MAX,
	};

	// ����
	enum xtTribe {
		xTB_NONE,		///< �ʱⰪ Ȥ�� ���
		xTB_HUMAN,
		xTB_BEAST,		///< ����
		xTB_MONSTER,	
		xTB_DEVIL,		///< �Ǹ�
	};
	// ���� �Լ����
	enum xtGet {
		xGET_NONE,
		xGET_GATHA				= 0x001,
		xGET_QUEST				= 0x002,
		xGET_GUILD_RAID		= 0x004,
		xGET_MEDAL_SPOT		= 0x008,
		xGET_EVENT				= 0x010,
		xGET_MAX,
	};
// 	enum xtGetBit {
// 		xGB_NONE = 0,
// 		xGB_GATHA				= 0x001,
// 		xGB_QUEST				= 0x002,
// 		xGB_GUILD_RAID	= 0x004,
// 		xGB_MEDAL_SPOT	= 0x008,
// 		xGB_EVENT				= 0x010,
// 		xGB_MAX,
// 	};
	// ���� Ÿ��
	enum xtSpot {
		xSPOT_NONE,
		xSPOT_CASTLE,		// ��������
		xSPOT_JEWEL,
		xSPOT_SULFUR,
		xSPOT_MANDRAKE,
		xSPOT_NPC,
		xSPOT_DAILY,		// ���Ϻ� �̺�Ʈ ����
		xSPOT_SPECIAL,		// ����� �̺�Ʈ ����
		xSPOT_CAMPAIGN,		///< ķ���� ����
		xSPOT_VISIT,		///< �湮����(������)
		xSPOT_CASH,			///< ĳ������ ����
		xSPOT_GUILD_RAID,     ///< ��巹�̵�
		xSPOT_PRIVATE_RAID,   ///< ���� ���̵�
		xSPOT_COMMON,         ///< ����
		xSPOT_MAX,
		xSPOT_WHEN = xSPOT_MAX,
	};
	// UI �׼�
	enum xtUIAction {
		xUA_NONE = 0,
		xUA_RESEARCH,		// ����
		xUA_UNLOCK_UNIT,	// �����������
		xUA_TRADE,			// ������ �ŷ�
		xUA_VISIT_ARMOR,	// ����� �湮
		xUA_BUY_ARMOR,		// ����� �ŷ�
		xUA_VISIT_CASH,		// ����ǰ���� �湮
		xUA_BUY_CASH,		// ����ǰ�������� ����
		xUA_SELL_ITEM,		// ������ �Ǹ�
		xUA_HIRE_HERO,		// �������
		xUA_PROVIDE_BOOK,	// å ����
		xUA_PROVIDE_SKILL,	// ���� ����
		xUA_PROVIDE_SQUAD,	// �޴� ����
		xUA_REPOSITION,		// �δ� ������ ����
		xUA_TRAINING_LEVEL,		// �������� �Ʒ�
		xUA_TRAINING_SKILL,		// ���� ��ų �Ʒ�
		xUA_TRAINING_SQUAD,		// ���� �δ� �Ʒ�
		xUA_JOIN_GUILD,			// ��� ����/����
		xUA_EQUIP,			// �������
		xUA_EXPAND_INVENTORY,	// â��Ȯ��
		xUA_PROMOTION_HERO,		// ���� �±�
		xUA_SUMMON_HERO,		// ��ȥ������ ��ȯ
		xUA_ENTER_SCENE,		// �� ����
		xUA_HERO_ORGANIZE,		// ������ ���ܿ� ����
		xUA_CONTROL_SQUAD,		// �δ���Ʈ��
		xUA_TOUCH_SQUAD_IN_READY,		// ������ġ������ �δ� ����.
		xUA_EDIT_SQUAD,				// ��������
		xUA_MAX,		// 
	};
	enum xtEvt {
		xEVT_NONE,
		xEVT_TRADE,
		xEVT_SHOP,
	};
	// ���� Ÿ��
	enum xtShopType {
		xSHOP_NONE,
		xSHOP_ARMORY,		// �������
		xSHOP_CASHSHOP,		// ����ǰ(ĳ��)����.
		xSHOP_GUILD,		// ������
		xSHOP_MAX,
	};
	// ����Ÿ��
	enum xtCoin {
		xCOIN_NONE,
		xCOIN_GOLD,			// ��ȭ
		xCOIN_CASH,			// ĳ��
		xCOIN_GUILD,		// �������
		xCOIN_MEDAL,		// �޴�
		xCOIN_MAX,

	};
	enum xtPush {
		xHERO_LEVELUP_TRAINING,		// ���� ���� �Ʒ�
		xSQUAD_LEVELUP_TRAINING,	// �δ� �Ʒ�
		xSKILL_TRAINING_ACTIVE,		// ��ų �Ʒ�(��Ƽ��)
		xSKILL_TRAINING_PASSIVE,	// ��ų �Ʒ�(�нú�)
		xTECH_TRAINING,				// Ư�� ����
		xTRADER_RETURN,				// ������ ����
		xARMORY_RETURN,				// ����� ����
		xATTACT,					// ���ݹ���
		xEVENT,						// �̺�Ʈ
	};
	// ������ �߻� �̺�Ʈ
// 	enum xtSeqEvent {
// 		xSE_NONE,
// 		xSE_ENTER_SCENE,
// 		xSE_UNLOCK_MENU,		// ��� �������
// 	};
	// ���� ��ɵ��� ������������� ���� ��Ʈ
	enum xtMenus {
		XBM_NONE = 0,
		xBM_LABORATORY = 0x01,		// ������
		xBM_CATHEDRAL = 0x02,		// ����(��簡 ��������Ȱ� �ƴ�. ����� ��������� �Ҽ� �ְ� �Ȱ���)
		xBM_TAVERN = 0x04,			// ������
		xBM_ACADEMY = 0x08,			// ��ī����(�б�)
		XBM_UNIT_HANGOUT = 0x10,			// ����
		xBM_EMBASSY = 0x20,			// ����
		xBM_MARKET = 0x40,			// ����
//		xBM_TRAING_CENTER = 0x80,	// �Ʒü�
		xBM_DISPOSITION = 0x100,	// �δ��ġ 
		xBM_EQUIP = 0x200,			// ��񽽷�
		xBM_BARRACK = 0x400,		// �δ����޴�
		xBM_ALL = 0xffffffff,		// ����Ʈ
	};
	// �����̺�Ʈ
	enum xtActionEvent {
		xAE_NONE,
		xAE_UNLOCK_MENU,	// � ��ݱ���� ������
		xAE_OPEN_AREA,		// ������ ����
		xAE_UNLOCK_UNIT,	// ������ ��������Ǿ���.
		xAE_ENTER_SCENE,	// ���� ����
		xAE_POPUP,			// �˾��� ����.
		xAE_CLOSE_POPUP,	// �˾��� ����.
		xAE_END_SEQ,		// �������ϳ��� ����
		xAE_WHERE_QUEST,	// ��â�� where����
		xAE_CONFIRM_QUEST,	// ����Ʈ �а� Ȯ���� ����
		xAE_QUEST_COMPLETE,	// ����Ʈ�� ?���°� ��
		xAE_QUEST_NEW,		// �� ����Ʈ�� ����.
		xAE_QUEST_DESTROY,	// ����Ʈ�� ������� �ް� ���� ������.
		xAE_VISIT_SPOT,		// ���� �湮
		xAE_MAX,
	};
	// ��
	enum xtScene {
		xSC_NONE,
		xSC_START,
//		xSC_MAIN_LOADING,
		xSC_OPENNING,
		xSC_TITLE,
		xSC_WORLD,
		xSC_UNIT_ORG,
		xSC_STORAGE,
		xSC_SHOP,
		xSC_HERO,
		xSC_TRADER,
		xSC_ARMORY,
		xSC_OPTION,
		xSC_ENDING,
		xSC_LEGION,			// ������
		xSC_TECH_SEL,		///< Ư��Ʈ�� ���ּ���
		xSC_TECH,			///< Ư��Ʈ����
		xSC_INGAME,
		xSC_EQUIP,
		xSC_GUILD,
		xSC_GUILD_SHOP,
		xSC_READY,			///< �ΰ��� ������ġ��.
		xSC_PATCH,
		xSC_LOADING,		///< �ε���
		xSC_TEST,				///< �����׽�Ʈ�� empty ��
		xSC_TEST2,				///< �����׽�Ʈ�� empty ��
		xSC_PRIVATE_RAID,
		xSCENE_MAX,
	};
	// �ƾ� �̺�Ʈ �Ķ����
	enum xtEVentParam {
		xEP_NONE,
		xEP_EXTRA_HERO,
		xEP_LOCK_PALADIN,
	};
	/**
	 ���� when�̺�Ʈ�� what�̺�Ʈ�� xQC_EVENT�� �Բ� ���� �ִ�. �׷��� when���� �������� �ʴ� ��� �̺�Ʈ(HAVE_ITEM����)
	 �� when�� ����� ������ ����ϴ�. ���� ��������� when�� what�̺�Ʈ�� �����ؼ� ���� event_leveló�� �����̺�Ʈ��
	 �پ��̴� ���� what�̺�Ʈ�� DispatchQuestEvent()�Ҷ� �ڵ����� when���ε� �ٲ㼭 OccurQuest�� �ϵ��� �ؾ��Ѵ�.
	 �ʹ� ū�۾��� �ɰŰ��� �ϴ��� �̴�� ����.
	*/
	// ����Ǵ°��̹Ƿ� �����ٲ��� ����.(��� �����????)
	enum xtQuestCond {
		xQC_NONE = 0,
		// ����/������(�� ��������)
		xQC_EVENT_LEVEL,				///< Ư�������� �Ǹ�
		xQC_EVENT_OPEN_AREA,			///< Ư�� ������ ����
		xQC_EVENT_CLEAR_SPOT,			///< Ư�� ������ Ŭ����(when���� ������)
		xQC_EVENT_RECON_SPOT,			///< ������ ����(when���� ������)
		xQC_EVENT_VISIT_SPOT,			///< ������ �湮(��ġ)(when���� ������)
		xQC_EVENT_CLEAR_SPOT_CODE,		///< Ư�����̵��� Ŭ����(when���� ������)
		xQC_EVENT_CLEAR_SPOT_TYPE,		///< ���������� Ŭ����(when���� ������)
		xQC_EVENT_RECON_SPOT_TYPE,		///< ���������� ����(when���� ������)
		xQC_EVENT_VISIT_SPOT_TYPE,		///< ���������� �湮(��ġ)(when���� ������)
		xQC_EVENT_GET_HERO,				///< Ư�� ������ ����(when���� ������)
		xQC_EVENT_KILL_HERO,			///< ������ ���(�̹��������� �������� ����.)(when���� ������)
		xQC_EVENT_CLEAR_QUEST,			///< Ư�� ����Ʈ�� ����
		xQC_EVENT_DEFENSE,				///< �� ����(�����ȵ�)(when���� ������)
		xQC_EVENT_GET_RESOURCE,			///< �ڿ��� ȹ��(Ư������ ���ĺ��� ī����)(when���� ������)
		xQC_EVENT_HAVE_RESOURCE,		///< �ڿ��� ����(���� ������ �ڿ������θ�)(���� ��������)(when���� ������)
		xQC_EVENT_GET_ITEM,				///< �������� ȹ��( " )(when���� ������)
		xQC_EVENT_HAVE_ITEM,			///< �������� ����(when���� ������)
		xQC_EVENT_CLEAR_STAGE,			///< ķ���� ���������� Ŭ����(�������̸� ������� ����. ����Ʈ�� ���̱� ���ؼ� ����� ķ������)(when���� ������)
		xQC_EVENT_UI_ACTION,			///< Ư�� UI����� ����
		xQC_EVENT_END_SEQ,				///< �������� ����
		xQC_EVENT_GET_STAR,				///< ���� ȹ��.(when���� ������)
		xQC_EVENT_HERO_LEVEL,			///< �ƹ������̳� x������ ����(when���� ������)
		xQC_EVENT_HERO_LEVEL_SQUAD,			///< �ƹ������̳� �δ뷹�� x�� ����(when���� ������)
		xQC_EVENT_HERO_LEVEL_SKILL,			///< �ƹ������̳� ��ų���� x�� ����(when���� ������)
		xQC_EVENT_TRAIN_QUICK_COMPLETE,	///< �Ʒ� ��� �Ϸ�(when���� ������)
		xQC_EVENT_RESEARCH_ABIL,				///< Ư�� ����.(�Ķ���ͷ� TIER�ܰ� ����)(when���� ������)
		xQC_EVENT_UNLOCK_UNIT,					///< ���� ��� ����(when���� ������)
		xQC_EVENT_HIRE_HERO,						///< ��������(��������)(when���� ������)
		xQC_EVENT_HIRE_HERO_NORMAL,		///< �Ϲݿ�������(��������)(when���� ������)
		xQC_EVENT_HIRE_HERO_PREMIUM,		///< ��޿�������(��������)(when���� ������)
		xQC_EVENT_SUMMON_HERO,					///< ��ȥ������ ��ȯ(when���� ������)
		xQC_EVENT_PROMOTION_HERO,				///< ���� �±�(�Ķ���ͷ� ��� ����)(when���� ������)
		xQC_EVENT_BUY_CASH,							///< ĳ�� ����(when���� ������)
		xQC_EVENT_BUY_ITEM,							///< ����������� ������ ����(when���� ������)
		xQC_EVENT_ATTACK_SPOT,					///< Ư�� ������ ���ݽõ�.(when���� ������)
		xQC_EVENT_CLEAR_GUILD_RAID,			///< ��巹�̵带 ������ Ŭ���� �Ѵ�.(when���� ������)
		xQC_EVENT_MAX,
		// ���
		xQC_WHERE_SPOT = 100,		///< Ư�����̿���
		xQC_WHERE_SPOT_TYPE,		///< Ư��������������
		xQC_WHERE_AREA,				///< Ư����������
		xQC_WHERE_SPOT_CODE,		///< Ư�����̵鿡��(�����ڵ带 ����)
		// ���
		xQC_HOW_LIMITED_SQUAD = 200,		///< ������ ���� �δ��
		xQC_HOW_BAN_UNIT,					///< Ư������ ������
		xQC_HOW_BAN_ARM,					///< Ư������ ������
		xQC_HOW_BAN_CLAN,					///< Ư��Ŭ�� ������
		xQC_HOW_BAN_TRIBE,					///< Ư������ ������
		// enum���� 255�� ���� �ʵ��� �Ұ�.
	};
	// ��������Ʈ Ÿ��
	enum xtBgObj {
		xBOT_NONE,
		xBOT_WATER_SHINE,		// ����¦��
		xBOT_WATER_FALL,		// ����
		xBOT_WATER_STEAM,					// ������
		xBOT_LAVA_BOIL,			// ��ϲ���
		xBOT_LAVA_STEAM,		// �������
		xBOT_LAVA_STREAM,		// �������
		xBOT_JEWEL_SHINE,		// ���� ��¦��
//		xBOT_SOUND,					// ���� ��ü
		xBOT_MAX,
	};

	/**
	 @brief ĳ���� ���Ұ����� ��ҵ�.
	*/
	enum xtPaymentRes {
		xPR_NONE,
		xPR_GOLD,
		xPR_RES,
		xPR_TIME,
		xPR_AP,
		xPR_ITEM,
		xPR_GEM,
		xPR_TRY_DAILY,		// ���Ͻ��� ����Ƚ��
	};
	// ��Ÿ enum��
	enum {
		xQUEST_COMPLETE = 1,
		xQUEST_NOT_COMPLETE = 0,
	};
	

};	// namespace XGAME

#endif