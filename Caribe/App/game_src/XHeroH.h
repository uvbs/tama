#pragma once
#include "XFramework/Game/XFLevel.h"

struct xSquadStat {
	float meleePower = 0;
	float rangePower = 0;
	float def = 0;
	int hp = 0;
	float speedAtk = 0;
	float speedMoveForMeter = 0;
	float GetStat( int idxStat ) {
		switch( idxStat ) {
		case 0:	return meleePower;
		case 1: return rangePower;
		case 2: return def * 100.f;
		case 3: return (float)hp;
		case 4: return speedAtk * 100.f;
		case 5: return speedMoveForMeter * 100.f;
		default:
			XBREAK(1);
			break;
		}
		return 0;
	}
};

XE_NAMESPACE_START( xnHero )

struct xUpgrade {
	XFLevel m_Level;
	bool m_bTrainComplete = false;	// 영웅 훈련완료 여부
	//
	XFLEVEL_ACCESSOR( m_Level );
	int Serialize( XArchive& ar ) const {
		m_Level.Serialize( ar );
//		ar << xboolToByte( m_bLevelupReady );
		ar << xboolToByte( m_bTrainComplete );
		ar << (BYTE)0;
		ar << (BYTE)0;
		ar << (BYTE)0;
		return 1;
	}
	int DeSerialize( XArchive& ar, int ) {
		BYTE b0;
		m_Level.DeSerialize( ar );
//		ar >> b0;	m_bLevelupReady = xbyteToBool( b0 );
		ar >> b0;	m_bTrainComplete = xbyteToBool( b0 );
		ar >> b0 >> b0 >> b0;
		return 1;
	}
};

// namespace xHero
XE_NAMESPACE_END;