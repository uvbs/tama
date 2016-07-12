/********************************************************************
	@date:	
	@file: 	
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once

XE_NAMESPACE_START( xPropSample )

struct xPROP {
	ID idProp;					///< 캐릭터 아이디
	_tstring strIdentifier;		///< 식별 아이디 문자열
	_tstring strName;			///< 이름

	// 기본 스탯
	float attack;				///< 공격력
	float defense;				///< 방어력
	float hp;					///< 체력
	float attackSpeed;			///< 공격 속도
	float moveSpeed;			///< 이동 속도

	_tstring strSpr;			///< 스프라이트 파일명
	_tstring strIcon;			///< icon 파일명

	xPROP() {
		idProp = 0;
		attack = 0;
		defense = 0;
		hp = 0;
		attackSpeed = 0;
		moveSpeed = 0;
	}
	~xPROP() { }
};
XE_NAMESPACE_END; // xPropSample

