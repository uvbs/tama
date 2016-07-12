#include "stdafx.h"
#include "XPacket.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XPacket::XPacket( ID idPacket ) 
{ 
	Init(); 
	SetPacketHeader3( idPacket, XE::GenerateID() );
// 	m_idPacket = idPacket;
// 	*this << idPacket;
// 	m_idKey = XE::GenerateID();
// 	*this << m_idKey;
}
XPacket::XPacket( ID idPacket, ID idKey ) 
{ 
	Init(); 
	SetPacketHeader3( idPacket, idKey );
// 	m_idPacket = idPacket;
// 	*this << idPacket;
// 	m_idKey = idKey;
// 	*this << idKey;
}

XPacket::XPacket( ID idPacket, ID idKey, bool bDynamicBuffer, int sizeMax )
	: XArchive( sizeMax )
{
	Init();
	SetPacketHeader3( idPacket, idKey );
// 	m_idPacket = idPacket;
// 	*this << idPacket;
// 	m_idKey = idKey;
// 	*this << idKey;
}
