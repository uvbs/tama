#include "stdafx.h"
#include "XPostInfo.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


using namespace XGAME;

XPostItem::XPostItem()
{
	Init();
}
XPostItem::XPostItem( XGAME::xtPOSTResource type, int count )
{
	Init();
	m_typeprop = type;
	m_nCount = count;
}
XPostItem::~XPostItem()
{
}


void XPostItem::Init()
{
	m_idprop = 0;
	m_typeprop = xPOSTRES_INIT;
	m_nCount = 1;
}

xReward XPostItem::GetReward()
{
	xReward reward;
	switch( m_typeprop ) {
	case XGAME::xPOSTRES_GOLD:
		reward.SetGold( m_nCount );
		break;
	case XGAME::xPOSTRES_GEM:
		reward.SetCash( m_nCount );
		break;
	case XGAME::xPOSTRES_ITEMS:
		reward.SetItem( m_idprop, m_nCount );
		break;
	case XGAME::xPOSTRES_RESOURCE:
		reward.SetResource( (xtResource)m_idprop, m_nCount );
		break;
	case XGAME::xPOSTRES_GUILD_COIN:
		reward.SetGuildCoin( m_nCount );
		break;
	default:
		XBREAKF( 1, "알수없는 우편아이템 type=%d", m_typeprop );
		break;
	}
	return reward;
}
void XPostItem::Serialize(XArchive& p) const
{
	p << (DWORD)m_typeprop;
	p << m_nCount;
	switch( m_typeprop ) {
	case XGAME::xPOSTRES_GOLD:
	case XGAME::xPOSTRES_GEM:
	case XGAME::xPOSTRES_GUILD_COIN:
		break;
	case XGAME::xPOSTRES_ITEMS:
	case XGAME::xPOSTRES_RESOURCE:
		p << m_idprop;		// 이전버전과 순서가 바뀜
		break;
	default:
		XBREAKF( 1, "알수없는 우편아이템 type=%d", m_typeprop );
		break;
	}
// 	p << (ID)m_typeprop;
// 	if( m_typeprop == xPOSTRES_GOLD ) {
// 		p << m_nCount;
// 	} else if( m_typeprop == xPOSTRES_GEM ) {
// 		p << m_nCount;
// 	} else if( m_typeprop == xPOSTRES_ITEMS ) {
// 		p << m_idprop;
// 		p << m_nCount;
// 	} else if( m_typeprop == xPOSTRES_RESOURCE ) {
// 		p << m_idprop;
// 		p << m_nCount;
// 	} else if( m_typeprop == xPOSTRES_GUILD_COIN ) {
// 		p << m_nCount;
// 	}
}

void XPostItem::DeSerialize(XArchive& p, int verPost )
{
	XBREAK( verPost < 100 );
	DWORD dw1;
	p >> dw1;	m_typeprop = ( XGAME::xtPOSTResource )dw1;
	if( verPost >= 101 ) {
		p >> m_nCount;
		switch( m_typeprop ) {
		case XGAME::xPOSTRES_GOLD:
		case XGAME::xPOSTRES_GEM:
		case XGAME::xPOSTRES_GUILD_COIN:
			break;
		case XGAME::xPOSTRES_ITEMS:
		case XGAME::xPOSTRES_RESOURCE:
			p >> m_idprop;		// 이전버전과 순서가 바뀜
			break;
		default:
			XBREAKF( 1, "알수없는 우편아이템 type=%d", m_typeprop );
			break;
		}
	} else {
		if( m_typeprop == xPOSTRES_GOLD ) {
			p >> dw1;  m_nCount = dw1;
		} else if( m_typeprop == xPOSTRES_GEM ) {
			p >> dw1;  m_nCount = dw1;
		} else if( m_typeprop == xPOSTRES_ITEMS ) {
			p >> dw1;  m_idprop = dw1;
			p >> dw1;  m_nCount = dw1;
		} else if( m_typeprop == xPOSTRES_RESOURCE ) {
			p >> dw1;  m_idprop = dw1;
			p >> dw1;  m_nCount = dw1;
		} else if( m_typeprop == xPOSTRES_GUILD_COIN ) {
			p >> m_nCount;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void XPostInfo::Init()
{
	m_RewardTableid = 0;
	m_PostType = XGAME::xPOSTTYPE_NONE;
//	m_ItemCounts = 0;
	m_Index = 0;				// 시리얼 코드	
}

void XPostInfo::AddlistPostItems( XSPPostItem spItem ) 
{
	m_listPostItems.Add( spItem );
}

XSPPostItem XPostInfo::AddPostGem( int numGem ) 
{
	auto spPost = XSPPostItem( new XPostItem( XGAME::xPOSTRES_GEM, numGem ) );
	AddlistPostItems( spPost );
	return spPost;
}

XSPPostItem XPostInfo::AddPostGold( int numGold )
{
	auto spPost = XSPPostItem( new XPostItem( XGAME::xPOSTRES_GOLD, numGold ) );
	AddlistPostItems( spPost );
	return spPost;
}

XSPPostItem XPostInfo::AddPostGuildCoin( int num )
{
	auto spPost = XSPPostItem( new XPostItem( XGAME::xPOSTRES_GUILD_COIN, num ) );
	AddlistPostItems( spPost );
	return spPost;
}

XSPPostItem XPostInfo::AddPostItem( ID idItem, int num )
{
	auto spPost = XSPPostItem( new XPostItem( XGAME::xPOSTRES_ITEMS, idItem, num ) );
	AddlistPostItems( spPost );
	return spPost;
}


void XPostInfo::DelPostItems()
{
//	XLIST_DESTROY(m_listPostItems, XPostItem*);
	m_listPostItems.clear();
}

/**
 @brief itempropid아이템의 개수를 반환
*/
int XPostInfo::ReceiveItem(int itempos, int itempropid)
{
	for( auto spItems : m_listPostItems ) {
		if( spItems->GetItemID() == itempropid ) {
			return spItems->GetnCount();
		}
// 		return 0;		//??
	}
// 	XLIST_LOOP(m_listPostItems, XPostItem*, pItems)
// 	{
// 		if (pItems->GetItemID() == itempropid )
// 		{				
// 			return pItems->GetnCount();				
// 		}
// 		return 0;		
// 	}END_LOOP;
// 
	return 0;
}

void XPostInfo::Serialize(XArchive& p) const
{
	XBREAK( m_Index > 0xffff );
	p << (BYTE)VER_POST;
	p << (BYTE)m_Index;
	p << (BYTE)m_listPostItems.size();
	p << (BYTE)0;
	p << m_idTextSender << m_strSenderName;
	p << m_strRecvName;
	p << m_idTextTitle << m_strTitle;
	p << m_idTextMsg << m_strMessage;
	for( auto spItem : m_listPostItems ) {
		spItem->Serialize( p );
	}
//	if( m_listPostItems.size() > 0 ) {
// 	XLIST_LOOP( m_listPostItems, XPostItem*, spPostItem ) {
// 		pPostItem->Serialize( p );
// 	}END_LOOP;
//	}
// 	else {
// 		p << (int)0;
// 	}
}

void XPostInfo::DeSerialize(XArchive& p, int verPost )
{
	XBREAK( verPost < 100 );
	if( verPost >= 101 ) {
		BYTE b0;
		p >> b0;	int verPost = b0;
		p >> b0;	m_Index = b0;
		p >> b0;	const int numItem = b0;
		p >> b0;
		p >> m_idTextSender >> m_strSenderName;
		p >> m_strRecvName;
		p >> m_idTextTitle >> m_strTitle;
		p >> m_idTextMsg >> m_strMessage;
		if( numItem > 0 ) {
			DeSerializeItems( p, numItem, verPost );
// 			m_listPostItems.clear();
// 			for( int n = 0; n < numItem; ++n ) {
// 				auto spItem = XSPPostItem( new XPostItem );
// 				spItem->DeSerialize( p, verAcc );
// 				m_listPostItems.Add( spItem );
// 			}
		}
	} else {
		ID index;
		DWORD dw1;
		TCHAR Buff[1024];
		p >> index;		SetIndex(index);
		XBREAK( index > 0xff );
		p.ReadString(Buff);	SetstrSenderName(Buff);
		p.ReadString(Buff);	SetstrRecvName(Buff);
		p.ReadString(Buff);	SetstrTitle(Buff);
		p.ReadString(Buff);	SetstrMessage(Buff);
		p >> dw1;
		if( dw1 > 0 ) {
			XBREAK( dw1 > 0xff );		// 새버전에선 바이트로 저장함.
//			SetWithItemCount( (short)dw1 );
			m_listPostItems.clear();
			for( int n = 0; n < (int)dw1; n++ ) {
				auto spPostItem = XSPPostItem( new XPostItem );
				spPostItem->DeSerialize( p, verPost );
				m_listPostItems.Add( spPostItem );
			}
		}
	}
}

void XPostInfo::SerializeItems( XArchive& ar ) const
{
	for( auto spItem : m_listPostItems ) {
		spItem->Serialize( ar );
	}
}

void XPostInfo::DeSerializeItems( XArchive& ar, int num, int verPost )
{
	m_listPostItems.clear();
	for( int i = 0; i < num; ++i ) {
		auto spItem = XSPPostItem( new XPostItem );
		spItem->DeSerialize( ar, verPost );
		m_listPostItems.Add( spItem );
	}
}
