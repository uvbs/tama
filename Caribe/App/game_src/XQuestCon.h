/********************************************************************
	@date:	2015/01/02 16:27
	@file: 	C:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XQuest.h
	@author:	xuzhu
	
	@brief:	퀘스트 시스템
*********************************************************************/
#pragma once

// #if defined(_CLIENT) || defined(_GAME_SERVER)
// #ifdef _XQUEST
#include "XQuest.h"

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/01/02 17:39
*****************************************************************/
class XQuestCon
{
//	XQuestProp::xCond2 *m_pProp = nullptr;	// 프로퍼티 원본
	XQuestProp::xCond2 m_Prop;
	int m_Num;
	bool m_bClear;
	bool m_bDoNotDestroyItemEvent = false;	// DispatchEventByDestroyItem이벤트를 처리하지 말것.
	void Init() {
// 		m_pCond = nullptr;
		m_Num = 0;
		m_bClear = false;
	}
	void Destroy();
public:
//	XQuestCon();
	XQuestCon( XQuestProp::xCond2 *pCond );
	virtual ~XQuestCon() { Destroy(); }
	//
//	GET_ACCESSOR_CONST( const XQuestProp::xCond2*, pCond );
//	GET_ACCESSOR_CONST( XQuestProp::xCond2*, pProp );
	const XQuestProp::xCond2* GetpProp() const {
		return &m_Prop;
	}
	GET_SET_ACCESSOR( bool, bDoNotDestroyItemEvent );
	bool IsClear() const {
		return m_bClear;
	}
	void _SetClear( bool bClear ) {
		m_bClear = bClear;
	}	
	int Serialize( XArchive& ar );
	int DeSerialize( XArchive& ar, int ver );
	//
	void InitQuestCon();
	bool IsWhat( ID idEvent, const xQuest::XEventInfo& infoEvent );
	bool IsWhere( const xQuest::XEventInfo& infoEvent );
	bool IsHow( const xQuest::XEventInfo& infoEvent );
	bool DispatchEvent( XQuestMng* pQuestMng, XQuestObj *pQuestObj, ID idEvent, const xQuest::XEventInfo& infoEvent );
	bool DispatchEventByDestroyItem( ID idItem, int numDel );
	void GetQuestObjective( _tstring *pOutStr );
	void GetQuestObjectiveNum( _tstring *pOutStr );
	static void sReplaceToken( const XQuestCon *pCond, _tstring& str, 
						bool (*pFunc)( const XQuestProp::xCond2 *pCond, LPCTSTR szToken, TCHAR *pOutWord, int lenOut ) );
	static void sReplaceToken( const XQuestCon *pCond, _tstring& str );
	bool IsOpenAreaType();
	bool IsReconType();
	bool IsSpotClearType();
	bool IsQuestType( XGAME::xtQuestCond type ) {
		return ( m_Prop.idWhat == type );
	}
	bool IsQuestTypeWithParam( XGAME::xtQuestCond type, XGAME::xParam param1, XGAME::xParam param2 );
	XGAME::xtQuestCond GetQuestType() {
		return (XGAME::xtQuestCond)m_Prop.idWhat;
	}
	bool IsRewardAfterDelItem();
	ID GetCondItem();
	ID GetidAreaByCond();
	ID GetidSpotbyCond();
	XGAME::xtSpot GettypeSpotByCond( ID *pOutIDArea );
	ID GetCodeSpotByCond();
	void Update();
	bool IsGetStarType();
// 	bool IsEnterSceneCond();
}; // class XQuestCon


// #endif // _XQUEST
// #endif // defined(_CLIENT) || defined(_GAME_SERVER)
