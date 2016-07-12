/********************************************************************
	@date:	2015/06/22 16:33
	@file: 	C:\xuzhu_work\Project\iPhone_may\Caribe\App\game_src\XPropSeq.h
	@author:	xuzhu
	
	@brief:	명령 시퀀스들의 프로퍼티
*********************************************************************/
#pragma once

#include "XXMLDoc.h"

XE_NAMESPACE_START( xHelp )
class XPropOrder;
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/06/22 16:34
*****************************************************************/
class XPropSeq
{
public:
	XVector<PropOrderPtr> m_aryOrder;
	std::string m_strIdentifier;
	std::string m_idsPrev;		// 어떤 시퀀스로부터 이어지는 시퀀스라면 이전시퀀스의 ids
	std::string m_idsCompletedSeq;	// 특정 시퀀스를 완료해야만 나오는 시퀀스
	std::string m_idsFinishededQuest;	// 조건: 특정 퀘스트의 목표를 달성했을때(퀘완료아님) 나오는 시퀀스
	XE::VEC2 m_vStopPos;						// 스탑버튼 위치
// 	std::string m_strCondition;				// 컷씬발생 조건으로 쓰려고 했는데 아직 하드코딩용으로만 사용.
	XGAME::xtActionEvent m_SeqEvent = XGAME::xAE_NONE;		// 이 시퀀스의 발생조건
//	XGAME::xtScene m_Scene = XGAME::xSC_NONE;
	DWORD m_dwEventParam = 0;	// 시퀀스발생 이벤트의 파라메터
	DWORD m_dwEventParam2 = 0;	// 시퀀스발생 이벤트의 파라메터
	std::string m_strEventParam;	// 스트링형태의 파라메터
	std::string m_strEventParam2;	// 스트링형태의 파라메터
#ifdef _xIN_TOOL
	_tstring m_strOrigEventParam;	// 저장용. event_param이 xml상에 어떻게 저장되어있었는지 문자열로 보관.
	_tstring m_strOrigEventParam2;	
	bool m_bSavePos = false;
#endif // _xIN_TOOL
public:
	XPropSeq() { Init(); }
	virtual ~XPropSeq() { Destroy(); }
	//
	void AddOrder( PropOrderPtr& spPropOrder );
	PropOrderPtr GetspOrderProp( const char *cidsOrder );
	inline PropOrderPtr GetspOrderProp( const std::string& idsOrder ) {
		return GetspOrderProp( idsOrder.c_str() );
	}
	inline bool IsHaveParam() {
		return m_dwEventParam || m_dwEventParam2 || !m_strEventParam.empty();
	}
	inline bool IsNotHaveParam() {
		return !IsHaveParam();
	}
	bool Save( XEXmlNode& nodeRoot );
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
private:
	void Init() {}
	void Destroy() {}
}; // class XPropSeq


XE_NAMESPACE_END;
