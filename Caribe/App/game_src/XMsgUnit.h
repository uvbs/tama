/********************************************************************
	@date:	2016/07/22 10:53
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XMsgUnit.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once

#include "skill/SkillDef.h"
#include "XBaseUnitH.h"

class XBaseUnit;
XE_NAMESPACE_START( xnUnit )

class XMsgBase;

/****************************************************************
* @brief 메시지큐 객체
* @author xuzhu
* @date	2016/07/22 12:56
*****************************************************************/
class XMsgQ
{
public:
#ifdef _XLEAK_DETECT
	static XList4<XSPMsgBase> s_qMsgAll;			// memory leak 감지용
	static void sDelMsg( ID snMsg );
#endif // _DEBUG
	static void sCheckLeak();
public:
	XMsgQ() { Init(); }
	virtual ~XMsgQ() { Destroy(); }
	// get/setter
	inline int GetSize() const {
		return m_qMsg1.size();
	}
	// public member
	void Process( XBaseUnit* pOwner );
	void Release();
private:
	// private member
	XList4<XSPMsgBase> m_qMsg1;//, m_qMsg2;			// 객체간 전달용 메시지 큐(flip용으로 두개)
private:
	// private method
	void Init() {}
	void Destroy() {}
	void AddMsg( XSPMsgBase spMsg ) {
		m_qMsg1.push_back( spMsg );
#ifdef _XLEAK_DETECT
		s_qMsgAll.push_back( spMsg );
#endif // _XLEAK_DETECT
	}
friend class XBaseUnit;
}; // class XMsgQ

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/07/22 10:54
*****************************************************************/
class XMsgBase
{
public:
	XMsgBase( xtMsg typeMsg ) 
	: m_Type( typeMsg ) { 
		Init(); 
		m_snMsg = XE::GenerateID();
	}
	virtual ~XMsgBase() { Destroy(); }
	// get/setter
	GET_ACCESSOR_CONST( xtMsg, Type );
	GET_ACCESSOR_CONST( ID, snMsg );
	inline ID getid() const {
		return m_snMsg;
	}
	// public member
	virtual void Process( XBaseUnit* pOwner ) = 0;
	virtual void Release() = 0;
	static int sGetnumObj() {
		return s_numObj;
	}
	static void sClearnumObj() {
		s_numObj = 0;
	}
private:
	static int s_numObj;
	// private member
	xtMsg m_Type = xUM_NONE;
	ID m_snMsg = 0;
private:
	// private method
	void Init() { ++s_numObj; }
	void Destroy() { --s_numObj; }
}; // class XMsgBase
/****************************************************************
* @brief 객체에 데미지를 가할때 타겟측에 push하는 메시지
* @author xuzhu
* @date	2016/07/22 10:54
*****************************************************************/
class XMsgDmg : public XMsgBase
{
public:
	XMsgDmg( XSPWorldObj spAtkObj
				, XSPUnit spTarget
				, float dmg
				, float ratioPenet
				, XSKILL::xtDamage typeDmg
				, BIT bitAttrHit
				, XGAME::xtDamageAttr typeDmgAttr  ) 
	: XMsgBase( xUM_DMG ) 
	, m_spAtkObj( spAtkObj )
	, m_spTarget( spTarget )
	, m_Dmg( dmg )
	, m_ratioPenet( ratioPenet )
	, m_typeDmg( typeDmg )
	, m_bitAttrHit( bitAttrHit )
	, m_attrDmg( typeDmgAttr ) { 
		Init(); 
	}
	virtual ~XMsgDmg() { Destroy(); }
	// get/setter
	// public member
	void Process( XBaseUnit* pOwner ) override;
private:
	// private member
	XSPWorldObj m_spAtkObj;
	XSPUnit m_spTarget;
	float m_Dmg = 0;
	float m_ratioPenet = 0.f;
	XSKILL::xtDamage m_typeDmg = XSKILL::xDMG_NONE;
	BIT m_bitAttrHit = 0;
	XGAME::xtDamageAttr m_attrDmg = XGAME::xDA_NONE;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Release() override {
		m_spAtkObj.reset();
		m_spTarget.reset();
	}
}; // class XMsgDmg
//
/****************************************************************
* @brief 타겟을 공격하고 난후 결과에 대한 피드백을 공격자에게 날리는 메시지 
* @author xuzhu
* @date	2016/07/22 13:48
*****************************************************************/
class XMsgDmgFeedback : public XMsgBase
{
public:
	XMsgDmgFeedback( const xDmg& dmgInfo );
	~XMsgDmgFeedback() { Destroy(); }
	// get/setter
	// public member
	void Process( XBaseUnit* pOwner ) override;
private:
	// private member
	xDmg m_dmgInfo;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Release() override {	
		m_dmgInfo.Release();
	}
}; // class XMsgDmgFeedback

/****************************************************************
* @brief 공격자에게 보내는 "타겟을 사살함"메시지
* @author xuzhu
* @date	2016/07/22 14:57
*****************************************************************/
class XMsgKillTarget : public XMsgBase
{
public:
	XMsgKillTarget( const xDmg& dmgInfo );
	~XMsgKillTarget() { Destroy(); }
	// get/setter
	// public member
	void Process( XBaseUnit* pOwner ) override;
private:
	// private member
	xDmg m_dmgInfo;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Release() override {
		m_dmgInfo.Release();
	}
}; // class XMsgKillTarget

/****************************************************************
* @brief 타겟에게 보정값을 보냄
* @author xuzhu
* @date	2016/07/22 14:57
*****************************************************************/
class XMsgAddAdjParam : public XMsgBase
{
public:
	XMsgAddAdjParam( int adjParam, XSKILL::xtValType valType, float adj );
	~XMsgAddAdjParam() { Destroy(); }
	// get/setter
	// public member
	void Process( XBaseUnit* pOwner ) override;
private:
	// private member
	XGAME::xtParameter m_AdjParam = XGAME::xADJ_NONE;
	XSKILL::xtValType m_valType = XSKILL::xNONE_VALTYPE;
	float m_valAdj = 0;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Release() override {	}
}; // class XMsgAddAdjParam

/****************************************************************
* @brief 타겟의 상태를 on/off한다.
* @author xuzhu
* @date	2016/07/22 14:57
*****************************************************************/
class XMsgSetState : public XMsgBase
{
public:
	XMsgSetState( XGAME::xtState idxState, bool bFlag );
	~XMsgSetState() { Destroy(); }
	// get/setter
	// public member
	void Process( XBaseUnit* pOwner ) override;
private:
	// private member
	XGAME::xtState m_idxState;
	bool m_bFlag;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Release() override {	}
}; // class XMsgSetState

XE_NAMESPACE_END // xnUnit
