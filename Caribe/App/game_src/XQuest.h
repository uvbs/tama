/********************************************************************
	@date:	2015/01/02 16:27
	@file: 	C:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XQuest.h
	@author:	xuzhu
	
	@brief:	퀘스트 시스템
*********************************************************************/
#pragma once

//#if defined(_CLIENT) || defined(_GAME_SERVER)
//#ifdef _XQUEST
#include "XQuestProp.h"
#include "XStruct.h"

/*
	ver 2: listComplete추가
	ver 3: secUpdate추가
	ver 4: num 데이타형 바뀜.
	ver 5: 퀘스트 프로퍼티를 저장하던방식 삭제
*/
#define VER_QUEST_SERIALIZE		5

class XLegion;
class XQuestCon;
class XQuestMng;
namespace xQuest {
	class XEventInfo {
		std::vector<DWORD> m_aryParam;
		LegionPtr m_spLegion;		// how용
 		int m_Amount;		// 리소스같은 조건의 양을 설정.
		XGAME::xParam m_Param2;	// 서브 파라메터
		_tstring m_strParam;		// 스트링형태 파라메터
		bool m_bLevelUnder = false;		// m_Level이하 조건의 이벤트를 발생시킬지 말지.
		void Init() {
			XBREAK( XGAME::xQC_EVENT_MAX >= 100 );	// 큰수 쓰지 말것.
			m_Amount = 0;
		}
		void Destroy() {}
	public:
		XEventInfo() : m_aryParam(XGAME::xQC_EVENT_MAX) { Init(); }
		virtual ~XEventInfo() { Destroy(); }
		//
		GET_ACCESSOR_CONST( const LegionPtr&, spLegion );
		SET_ACCESSOR( LegionPtr&, spLegion );
		GET_SET_ACCESSOR_CONST( int, Amount );
		GET_SET_ACCESSOR_CONST( bool, bLevelUnder );
		void SetLevel( int lv ) {
			SetParam( XGAME::xQC_EVENT_LEVEL, lv );
		}
		void SetLevelHero( int lv ) {
			SetParam( XGAME::xQC_EVENT_HERO_LEVEL, lv );
		}
		void SetLevelSquad( int lv ) {
			SetParam( XGAME::xQC_EVENT_HERO_LEVEL_SQUAD, lv );
		}
		void SetLevelSkill( int lv ) {
			SetParam( XGAME::xQC_EVENT_HERO_LEVEL_SKILL, lv );
		}
		void SetNumCash( int numCash ) {
			SetParam( XGAME::xQC_EVENT_BUY_CASH, numCash );
		}
		void SetidArea( ID idArea ) {
			SetParam( XGAME::xQC_EVENT_OPEN_AREA, idArea );
		}
		void SetidSpot( ID idSpot ) {
			SetParam( XGAME::xQC_EVENT_CLEAR_SPOT, idSpot );
			SetParam( XGAME::xQC_EVENT_RECON_SPOT, idSpot );
			SetParam( XGAME::xQC_EVENT_VISIT_SPOT, idSpot );
			SetParam( XGAME::xQC_EVENT_DEFENSE, idSpot );
		}
		void SettypeSpot( XGAME::xtSpot typeSpot ) {
			SetParam( XGAME::xQC_EVENT_CLEAR_SPOT_TYPE, typeSpot );
			SetParam( XGAME::xQC_EVENT_RECON_SPOT_TYPE, typeSpot );
			SetParam( XGAME::xQC_EVENT_VISIT_SPOT_TYPE, typeSpot );
		}
		void SetidHero( ID idHero ) {
			SetParam( XGAME::xQC_EVENT_GET_HERO, idHero );
			SetParam( XGAME::xQC_EVENT_KILL_HERO, idHero );
		}
		void SetidItem( ID idItem ) {
			SetParam( XGAME::xQC_EVENT_GET_ITEM, idItem );
			SetParam( XGAME::xQC_EVENT_HAVE_ITEM, idItem );
		}
		void SetidQuest( ID idQuest ) {
			SetParam( XGAME::xQC_EVENT_CLEAR_QUEST, idQuest );
		}
		void SettypeResource( XGAME::xtResource type ) {
			SetParam( XGAME::xQC_EVENT_GET_RESOURCE, type );
			SetParam( XGAME::xQC_EVENT_HAVE_RESOURCE, type );
		}
		void SetidCode( ID idCode ) {
			SetParam( XGAME::xQC_EVENT_CLEAR_SPOT_CODE, idCode );
		}
		DWORD GetParam( int idxParam ) const {
			if( XBREAK( (size_t)idxParam >= m_aryParam.size() ) )
				return 0;
			return m_aryParam[ idxParam ];
		}
		void SetParam( int idxParam, DWORD dwVal ) {
			if( XBREAK( (size_t)idxParam >= m_aryParam.size()  ) )
				return;
			m_aryParam[ idxParam ] = dwVal;
		}
		DWORD GetParam2() const  {
			return m_Param2.dwParam;
		}
		void SetParam2( DWORD dwParam ) {
			m_Param2.dwParam = dwParam;
		}
		GET_SET_ACCESSOR_CONST( const _tstring&, strParam );
	}; // class XEventInfo
};

/****************************************************************
* @brief 퀘스트 인스턴스의 기본형
* @author xuzhu
* @date	2015/01/02 17:05
*****************************************************************/
class XQuestObj
{
public:
	static XQuestObj* sCreateQuestObj( ID idQuest );
	static XQuestObj* sCreateQuestObj( XQuestProp::xProp *pProp );
	static int sSerialize( XQuestObj *pObj, XArchive& ar );
	static int sDeSerialize( XQuestObj *pObj, XArchive& ar );
	enum xtAlert {
		xMS_NONE,		// 마크없음
		xMS_MUST_HAVE,	// 마크떠아함
		xMS_HAVE,		// 마크떠있음
		xMS_MUST_DEL,	// 마크지워야함.
	};
private:
//	ID m_snQuestObj;	// 퀘 오브젝트는 sn번호가 필요없다 m_pProp->idProp이 같은 퀘를 두개를 받을 수 없기때문에.
  ID m_idProp = 0;
	XQuestProp::xProp *_m_pProp = nullptr;
  ID m_keyProp = 0;
	XArrayLinear<XQuestCon*> m_aryConds;
	xtAlert m_stateAlert = xMS_NONE;		// 느낌표 마크가 떠야 하는지에 대한 상태
	DWORD m_secUpdate = 0;						// 퀘가 업데이트 되면 현재 시간을 쓴다.
	bool m_bDestroy = false;
	bool m_bUpdate = false;
#ifdef _CLIENT
	DWORD m_secClicked = 0;
#endif // _CLIENT
	void Init() {}
	void Destroy();
	void DestroyAllCond();
public:
	XQuestObj( XQuestProp::xProp *pProp ) { 
		Init(); 
		_m_pProp = pProp;
		if( pProp )
			m_idProp = pProp->idProp;
		m_keyProp = XQuestProp::sGet()->GetidKey();
	}
	virtual ~XQuestObj() { Destroy(); }
	//
	XQuestProp::xProp* GetpProp() {
		if( m_keyProp != XQuestProp::sGet()->GetidKey() ) {
			if( m_idProp )
				_m_pProp = XQuestProp::sGet()->GetpProp( m_idProp );
			m_keyProp = XQuestProp::sGet()->GetidKey();
		}
		return _m_pProp;
	}
//	GET_ACCESSOR( XQuestProp::xProp*, pProp );
	GET_SET_ACCESSOR( xtAlert, stateAlert );
	GET_SET_ACCESSOR( bool, bUpdate );
	// 이 퀘스트가 느낌표가 있는 상태인가.
	bool IsUpdateAlert() {
		return m_stateAlert == xMS_MUST_HAVE || m_stateAlert == xMS_HAVE;
	}
//	GET_ACCESSOR( ID, snQuestObj );
	GET_ACCESSOR( const XArrayLinear<XQuestCon*>&, aryConds );
	GET_ACCESSOR( DWORD, secUpdate );
#ifdef _CLIENT
	GET_ACCESSOR( DWORD, secClicked );
	bool IsOverClickedTime() {
		return (XTimer2::sGetTime() - m_secClicked > xMIN_TO_SEC(5));
	}
	void UpdatesecClicked( bool bRandom ) {
		m_secClicked = XTimer2::sGetTime();
		if( bRandom )
			m_secClicked -= xRandom( (int)(xMIN_TO_SEC(5) * 0.5f) );
	}
#endif // _CLIENT
	void SetsecUpdate( DWORD secUpdate ) {
		m_secUpdate = secUpdate;
#ifdef _CLIENT
		UpdatesecClicked( false );
#endif // _CLIENT
	}
	void UpdatesecUpdate() {
		SetsecUpdate( XTimer2::sGetTime() );
	}
	GET_SET_ACCESSOR( bool, bDestroy );
	ID getid() {
		return GetpProp()->idProp;
	}
	inline ID GetidProp() {
		return getid();
	}
	inline _tstring& GetstrIdentifer() {
		return GetpProp()->strIdentifier;
	}
	bool IsRepeat() {
		return GetpProp()->repeat != 0;
	}
	int GetRepeat() {
		return (int)GetpProp()->repeat;
	}
	//
	int Serialize( XArchive& ar );
	int DeSerialize( XArchive& ar, int ver );
	//
	void OnCreate();
	void InitQuest();
	bool DispatchEvent( XQuestMng* pQuestMng, ID idEvent, const xQuest::XEventInfo& infoEvent );
	bool DispatchEventByDestroyItem( ID idItem, int numDel );
	void GetQuestObjective( _tstring *pOutStr, bool bIncludeName );
	bool IsAllComplete();
	int GetRewardToAry( XArrayLinearN<XGAME::xReward*, 128> *pOutAry );
	void SetAllComplete();
	bool IsAreaQuest();
	bool IsReconQuest();
	bool IsQuestType( XGAME::xtQuestCond type );
	bool IsQuestTypeWithParam( XGAME::xtQuestCond type, XGAME::xParam param1, XGAME::xParam param2 );
	// 대표퀘스트 타입을 얻는다. 가장 첫번째 조건목록이 대표타입이 된다.
	XGAME::xtQuestCond GetQuestType();
	ID GetidOpenAreaCondition();
	bool IsSpotClearQuest();
	ID GetCondItem();
	void GetQuestObjectiveNum( _tstring *pOutStr );
	ID GetidSpotOfCond();
	XGAME::xtSpot GettypeSpotByCond();
	void GetQuestDescReplacedToken( _tstring *pOut );
	void ReplaceToken( _tstring *pOut, const _tstring& strToken );
	XQuestCon* GetCondObjByWhat( XGAME::xtQuestCond condType );
	bool IsHaveWhatParamTypeCondWithParam( XGAME::xtParam typeParam, DWORD dwParam );
//	int DoDropFromQuest( XArrayLinearN<XBaseItem*,256>& ary );
//	XQuestCon* GetpCond( ID snCond );
}; // class XQuestObj
//#endif // _XQUEST
//#endif // defined(_CLIENT) || defined(_GAME_SERVER)
