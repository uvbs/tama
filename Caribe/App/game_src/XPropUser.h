/********************************************************************
	@date:	2015/09/24 16:30
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XPropUser.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once

#if defined(_CLIENT) || defined(_GAME_SERVER)

#include "XFramework/XEBaseProp.h"
#include "XPropUserH.h"

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/09/24 16:08
*****************************************************************/
class XPropUser;
class XPropUser : public XEBaseProp
{
public:
	static std::shared_ptr<XPropUser>& sGet();
public:
	XPropUser() : m_aryProp( XGAME::MAX_HERO_LEVEL + 1 ) { 
		Init(); 
	}
	virtual ~XPropUser() { Destroy(); }
	GET_ACCESSOR( const std::vector<xPropUser::xPROP*>&, aryProp );
	int GetSize( void ) override { return m_aryProp.size(); }
	DWORD GetMaxExp( int level ) {
		return (DWORD)m_aryProp[ level ]->expMax;
	}
	const xPropUser::xPROP* GetTable( int lvAcc ) {
		return m_aryProp[ lvAcc ];
	}
	//
private:
	static std::shared_ptr<XPropUser> s_spInstance;
	std::vector<xPropUser::xPROP*> m_aryProp;				///< 전체 프로퍼티 배열
private:
	void Init() {}
	void Destroy();
	BOOL ReadProp( CToken& token, DWORD dwParam ) override;					///< txt 파싱
	void Add( int lv, xPropUser::xPROP *pProp );

}; // class XPropUser


#endif // #if defined(_CLIENT) || defined(_GAME_SERVER)
