/********************************************************************
	@date:	2014/09/17 11:46
	@file: 	D:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XSkillMng.h
	@author:	xuzhu
	
	@brief:	XSKILL::XESkillMng를 상속받은 스킬매니저
*********************************************************************/
#pragma once
#include "skill/XESkillMng.h"

/****************************************************************
* @brief 
* @author xuzhu
* @date	2014/09/17 11:47
*****************************************************************/
class XSkillMng : public XSKILL::XESkillMng
{
	void Init() {}
	void Destroy();
public:
	XSkillMng();
	virtual ~XSkillMng() { Destroy(); }
	//
	virtual void OnAddConstant( XConstant *pConstant );
	virtual BOOL CustomParsingEffect( TiXmlAttribute *pAttr,
									const char *cAttrName,
									const char *cParam,
									XSKILL::XSkillDat* pSkillDat,
									XSKILL::EFFECT *pEffect );
	void AdjustEffectParam( XSKILL::XSkillDat* pDat, XSKILL::EFFECT *pEffect ) override;
// 	XSKILL::xCOND_PARAM ParsingCond( TiXmlAttribute *pAttr, 
// 									const char *cAttrName,
// 									const char *cVal,
// 									XSKILL::XSkillDat* pSkillDat,
// 									XSKILL::EFFECT *pEffect ) override;
	bool IsInverseParam( int invokeParam ) const override;
	bool IsRateParam( int idxParam ) const override;
};

extern XSkillMng *SKILL_MNG;
