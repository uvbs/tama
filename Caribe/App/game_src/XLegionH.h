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

// 군단객체를 만들수 있는 순수 데이터
struct xSquadDat {
	int m_idxPos = -99;
	ID m_snHero = 0;
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, DWORD ver );
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
};

//
XE_NAMESPACE_END; // xnLegion

