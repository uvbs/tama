#pragma once
namespace XGAME {
struct xSquad;
struct xLegion;
}
class XArchive;
class XAccount;
class XSquadron;
class XHero;

#define RATE_ATK_DEFAULT_ELITE		0.2f
#define RATE_HP_DEFAULT_ELITE		5.f
#define RATE_ATK_DEFAULT_RAID		0.5f
#define RATE_HP_DEFAULT_RAID		100.f


XE_NAMESPACE_START( xnLegion )
//

// 군단을 객체화 시키지않고 데이터형태로만 저장할수 있는 버전. 영웅은 snHero만 사용하므로 PC전용
struct xSquadDat {
	int m_idxPos = -99;
	ID m_snHero = 0;
	xSquadDat() {}
	xSquadDat( int idxPos, ID snHero ) 
		: m_idxPos(idxPos)
		, m_snHero( snHero ) {}
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, DWORD ver );
	inline ID getid() const {
		return m_snHero;
	}
};
struct xLegionDat {
	XList4<xSquadDat> m_listSquad;
	ID m_snLeader = 0;
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, DWORD ver );
	inline bool IsValid() const {
		return !m_listSquad.empty();
	}
	inline bool IsInvalid() const {
		return !IsValid();
	}
	inline const xSquadDat* GetpcSquadBySN( ID snHero ) const {
		return m_listSquad.FindpcByIDNonPtr( snHero );
	}
	inline xSquadDat* GetpSquadBySN( ID snHero ) {
		return m_listSquad.FindByIDNonPtr( snHero );
	}
	inline void AddSquad( int idxPos, ID snHero ) {
		if( GetpcSquadBySN( snHero ) )
			return;
		xSquadDat sq( idxPos, snHero );
		m_listSquad.push_back( sq );
	}
	inline void Clear() {
		m_listSquad.clear();
		m_snLeader = 0;
	}
};

//
XE_NAMESPACE_END; // xnLegion

