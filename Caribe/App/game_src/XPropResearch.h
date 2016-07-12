/********************************************************************
	@date:	2015/02/14 11:38
	@file: 	C:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XPropResearch.h
	@author:	xuzhu
	
	@brief:	특성 연구시 필요한 금화/자원/시간
*********************************************************************/
#pragma once
#include "XEBaseProp.h"

class XPropResearch : public XEBaseProp
{
public:
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
private:
	std::map<_tstring, xPROP*> m_mapData;			///< 프로퍼티 맵(식별자)
	std::map<ID, xPROP*> m_mapID;					///< 프로퍼티 맵(ID)
	std::map<ID, xPROP*>::iterator m_itorID;
	
	XArrayLinear<xPROP*> m_aryProp;				///< 전체 프로퍼티 배열

	void Init() {}
	void Destroy();

public:
	XPropResearch();
	virtual ~XPropResearch() { Destroy(); }

	GET_ACCESSOR( XArrayLinear<xPROP*>&, aryProp );
	
	void GetNextClear( void ) { m_itorID = m_mapID.begin(); }		///< 반복자 초기화
	BOOL ReadProp( CToken& token );					///< txt 파싱
	
	xPROP* GetpProp( LPCTSTR szIdentifier );
	/** strIdentifier는 모두 소문자여야 함. */
	xPROP* GetpProp( _tstring& strIdentifier ) { 
		XBREAK( strIdentifier.empty() == TRUE ); 
		return GetpProp( strIdentifier.c_str() ); 
	}
	xPROP* GetpProp( ID idType );
	
	void Add( _tstring& strIdentifier, xPROP *pProp );
	
	virtual int GetSize( void ) { return m_mapData.size(); }

	LPCTSTR GetszSpr( ID idProp ) {
		XPropResearch::xPROP *pProp = GetpProp( idProp );
		if( pProp == NULL )
			return NULL;
		return pProp->strSpr.c_str();
	}

	virtual void OnDidFinishReadProp( void );

	xPROP* GetpPropFromName( LPCTSTR szName );
	xPROP* GetPropRandom( void ) {
		return m_aryProp.GetFromRandom();
	}
};

extern XPropResearch *PROP_HERO;