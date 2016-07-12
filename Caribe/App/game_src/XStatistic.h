/********************************************************************
	@date:	2015/03/17 13:49
	@file: 	C:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XStatistic.h
	@author:	xuzhu
	
	@brief:	전투통계
*********************************************************************/
#pragma once
#include "XHero.h"

class XStatistic
{
public:
	struct xSquad {
		XHero *pHero = nullptr;
		float damageDeal = 0;		// 총 데미지 딜링양
		float damageAttacked = 0;	// 총 맞은 양
		float damageDealByCritical = 0;	// 총 치명타 딜링양
		float damageByEvade = 0;		// 총 회피량
		float heal = 0;						// 총 치료량
		float treated = 0;					// 총 치료받은 양
		float damageDealBySkill = 0;		// 총 스킬공격양
		ID getid() {
			return pHero->GetsnHero();
		}
	};
private:
	XList4<xSquad*> m_listSquads;
	void Init() {}
	void Destroy();
public:
	XStatistic();
	virtual ~XStatistic() { Destroy(); }
	//
	GET_ACCESSOR( XList4<xSquad*>&, listSquads );

	void AddSquad( XHero *pHero );
	float AddDamageDeal( ID snHero, float damage );
	float AddDamageAttacked( ID snHero, float damage );
	float AddDamageDealByCritical( ID snHero, float damage );
	float AddDamageByEvade( ID snHero, float damage );
	float AddHeal( ID snHero, float heal );
	float AddTreated( ID snHero, float heal );
	float AddDamageDealBySkill( ID snHero, float damage );
	float GetMaxDamageDeal();
	float GetMaxDamageAttacked();
	float GetMaxDamageDealByCritical();
	float GetMaxDamageByEvade();
	float GetMaxHeal();
	float GetMaxTreated();
	float GetMaxDamageDealBySkill();
}; // class XStatistic
