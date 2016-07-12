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
#include "XQuestProp.h"
#include "XPropItem.h"

class XBaseItem;
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/01/06 18:17
*****************************************************************/
class XDelegateQuestMng
{
public:
	XDelegateQuestMng() {}
	virtual ~XDelegateQuestMng() {}
	// 
	/// 퀘스트 발생 델리게이트
	virtual void DelegateOccurQuest( XQuestObj *pQuestObj ) {}
	/// 퀘스트발생 델리게이트2. 퀘스트의 각 조건마다 델리게이트가 호출된다.
	virtual void DelegateOccurQuestCond( XQuestObj *pQuestObj, XQuestCon *pCond ) {}
	
	/// 퀘스트 조건 완료
	virtual void DelegateCompleteQuest( XQuestObj *pQuestObj ) {}
	/// 퀘스트 종료(보상까지 끝남)
	virtual void DelegateFinishQuest( XQuestObj *pQuestObj, bool bItemSync ) {}
	/// 퀘스트 업데이트
	virtual void DelegateUpdateQuest( XQuestObj *pQuestObj ) {}
	/// 퀘스트로 인해 아이템드랍이 필요함. 
	virtual void DelegateDropItemFromQuest( XQuestObj *pQuestObj, ID idItem, XArrayLinearN<ItemBox,256> *pOut ) {}
	/// 퀘스트 삭제
	virtual void DelegateDestroyQuest( XQuestObj *pQuestObj ) {}
	virtual void DelegateDestroyQuestCond( XQuestObj *pQuestObj, XQuestCon *pCond ) {}
	

}; // class XQuestMngDelegate
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/01/02 16:29
*****************************************************************/
class XQuestMng;
/**
 @brief 계정당 하나씩 가지고 있는 받은 퀘스트목록 관리자 객체
*/
class XQuestMng
{
public:
private:
	XDelegateQuestMng *m_pDelegate;
	XList4<XQuestObj*> m_listQuestCurr;		// 받은 퀘스트의 인스턴스 객체들
	XList4<ID> m_listQuestsComplete;	// 완료 퀘스트의 아이디
	XList4<XQuestObj*> m_listDelete;	// 삭제될 퀘
	//
	void Init() {
		m_pDelegate = nullptr;
	}
	void Destroy();
public:
	XQuestMng();
	virtual ~XQuestMng() { Destroy(); }
	//
	virtual int Serialize( XArchive& ar );
	virtual int DeSerialize( XArchive& ar );
	//
	void UpdatedQuestObj( XQuestObj *pQuestObj );
	void DispatchEvent( ID idEvent, const xQuest::XEventInfo& infoEvent );
	void DispatchEvent( ID idEvent, DWORD dwParam, DWORD dwParam2 = 0 );
	inline void DispatchEvent( ID idEvent, int nParam, int nParam2 = 0 ) {
		DispatchEvent( idEvent, (DWORD)nParam, (DWORD)nParam2 );
	}
	bool DispatchEventByDestroyItem( ID idItem, int numDel );
	bool IsCondWhen( XQuestProp::xProp *pProp, ID idEvent, const xQuest::XEventInfo& infoEvent );
	bool IsCompleteQuest( ID idProp );
	bool IsFinishedQuest( ID idProp );
	bool IsHaveCurrQuest( ID idProp );
	bool IsHaveCurrQuest( const _tstring& idsQuest );
	void SetpDelegate( XDelegateQuestMng *pDelegate ) {
		XBREAK( pDelegate  == nullptr );
		m_pDelegate = pDelegate;
	}
	XQuestObj* CreateAndAddQuestObjCurr( ID idQuest );
	XQuestObj* CreateAndAddQuestObjCurr( XQuestProp::xProp *pProp );
	///< 
	template<int N>
	void GetQuestsToAry( XArrayLinearN<XQuestObj*, N> *pAryOut ) {
		*pAryOut = m_listQuestCurr;
	}
	void GetQuestsToAry( XVector<XQuestObj*> *pAryOut );
	template<int N>
	void GetQuestsToAryWithNormal( XArrayLinearN<XQuestObj*, N> *pAryOut ) {
		for( auto pObj : m_listQuestCurr ) {
			if( !pObj->IsRepeat()  )
				pAryOut->Add( pObj );
		}
	}
	template<int N>
	void GetQuestsToAryWithRepeat( XArrayLinearN<XQuestObj*, N> *pAryOut ) {
		for( auto pObj : m_listQuestCurr ) {
			if( pObj->IsRepeat()  )
				pAryOut->Add( pObj );
		}
	}
	void GetQuestsCompletedToAry( std::vector<ID> *pOutAry ) {
		auto size = m_listQuestsComplete.size();
		pOutAry->resize( size );
		for( auto id : m_listQuestsComplete ) {
			pOutAry->push_back( id );
		}
	}

	XQuestObj* GetQuestObj( ID idProp );
	XQuestObj* GetQuestObj( LPCTSTR idsQuest );
	inline XQuestObj* GetQuestObj( const _tstring& idsQuest ) {
		return GetQuestObj( idsQuest.c_str() );
	}
	void DestroyQuestObj( XQuestObj *pObj );
	int DoDropFromQuest( XArrayLinearN<ItemBox, 256>& ary, const xQuest::XEventInfo& infoEvent );
	bool IsWhereOperatorAnd( const XQuestProp::xWhere& xwhere, const xQuest::XEventInfo& infoEvent );
	bool IsWhereAttr( const XQuestProp::xWhereAttr& wAttr, const xQuest::XEventInfo& infoEvent );
#ifdef _CHEAT
	void _UpdateQuest();
//	void UpdateOccurQuestCheat();
#endif // _CHEAT
	void SortBysecUpdate();
	void DoExitQuest( XQuestObj *pObj );
#ifdef _SERVER
	void SetsecUpdateNow( XQuestObj *pObj ) {
// 		DWORD secUpdate = XTimer2::sGetTime();
// 		pObj->SetsecUpdate( secUpdate );
		pObj->SetsecUpdate( XTimer2::sGetTime() );
	}
#endif // server
	void OnInitQuest( XQuestObj *pObj );
	void Process( float dt );
	void DispatchEvent( XQuestObj* pQuestObj, XQuestCon *pCond, ID idEvent, const xQuest::XEventInfo& infoEvent );
	void DispatchEvent( XQuestObj* pQuestObj, XQuestCon *pCond, ID idEvent, DWORD dwParam, DWORD dwParam2 = 0 );
	bool DispatchEventFast( ID idEvent, const xQuest::XEventInfo& infoEvent, XQuestObj* pQuestObj, XQuestCon *pCond );
	bool DispatchEventFast( XQuestObj* pQuestObj, XQuestCon *pCond, ID idEvent, DWORD dwParam, DWORD dwParam2 = 0 );
	void _DelQuest( ID idQuest );
	void _DelQuestByCompleted( ID idQuest );
	int GetDropableItems( std::vector<XGAME::xDropItem> *pOutAry, const xQuest::XEventInfo& infoEvent );
	bool IsHaveQuestType( XGAME::xtQuestCond type );
	bool IsHaveQuestTypeWithParam( XGAME::xtQuestCond type
																, XGAME::xParam param1
																, XGAME::xParam param2 );
	bool IsHaveQuestTypeWithParam( XGAME::xtQuestCond type
																, DWORD dwParam1
																, DWORD dwParam2 );
// 	bool IsHaveGetStarQuest() {
// 		return IsHaveQuestType( XGAME::xQC_EVENT_GET_STAR );
// 	}
	bool IsHaveQuestParamType( XGAME::xtParam typeParam, DWORD dwParam );
private:
	// 외부에서 사용할일 없음. 
	bool OccurQuestByDispatchEvent( ID idEvent, const xQuest::XEventInfo& info );
}; // class XQuestMng

// #endif // _XQUEST
// #endif // #if defined(_CLIENT) || defined(_GAME_SERVER)
