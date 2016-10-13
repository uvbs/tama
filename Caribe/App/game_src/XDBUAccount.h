#pragma once
#include "network/XPacket.h"
#include "server/XSAccount.h"
#include "XArchive.h"
#include "XFramework/Game/XFLevel.h"
#include "XPool.h"

class XDBUAccount;
typedef std::shared_ptr<XDBUAccount> XSPDBUAcc;
typedef std::shared_ptr<const XDBUAccount> XSPDBUAccConst;

class XDBUAccount: public XSAccount
								, public XMemPool<XDBUAccount>
{
	void Init() {
	}
	void Destroy();
public:	
	XArchive	m_BinaryResource;						//바이너리 데이터1
	XArchive	m_BinaryWorld;							//바이너리 데이터2
	XArchive	m_BinaryHero;							//바이너리 데이터3
	XArchive	m_BinaryFLevel;							//바이너리 데이터4
	//
	XDBUAccount();
	XDBUAccount(ID idaccount);
	virtual ~XDBUAccount() {
		Destroy();
	}
	//
	BOOL LoadDB( void *dbAccount );
	BOOL SaveDB( void );

	BOOL MakeResourcePacket ( XArchive &p );
	BOOL RestoreResourcePacket ( XArchive &p );
	void MakeLegionPacketFull( XArchive &p, int idxLegion );
	XLegion* RestoreLegionPacketFullToLink( XArchive &ar, int idxLegion );
	XLegion* RestoreLegionPacketFull( XArchive& arFull, int idxLegion );
// 	void MakeArchiveHeros( XArchive& ar );
// 	BOOL RestoreArchiveHeros( XArchive& ar );
private:
	XSPDBUAcc GetThis() {
		return std::static_pointer_cast<XDBUAccount>( XDBAccount::GetThis() );
	}
};

