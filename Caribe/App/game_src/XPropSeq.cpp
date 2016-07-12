#include "stdafx.h"
#include "XPropSeq.h"
#include "XPropOrder.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

XE_NAMESPACE_START( xHelp )
	
void XPropSeq::AddOrder( PropOrderPtr& spPropOrder )
{
	spPropOrder->m_idx = m_aryOrder.size();
	m_aryOrder.push_back( spPropOrder );
}

PropOrderPtr XPropSeq::GetspOrderProp( const char *cidsOrder )
{
	for( auto& spOrder : m_aryOrder ) {
		if( spOrder->m_strIdentifer == cidsOrder )
			return spOrder;
	}
	return PropOrderPtr();
}

bool XPropSeq::Save( XEXmlNode& nodeRoot )
{
#ifdef _xIN_TOOL
	auto nodeSeq = nodeRoot.AddNode( m_strIdentifier.c_str() );
// 	nodeSeq.AddAttribute( "condition", m_strCondition );
	nodeSeq.AddAttribute( "prev_seq", m_idsPrev );
	nodeSeq.AddAttribute( "complete_seq", m_idsCompletedSeq );
	nodeSeq.AddAttribute( "complete_quest", m_idsFinishededQuest );
	nodeSeq.AddAttribute( "event", XGAME::GetstrEnumActionEvent( m_SeqEvent ) );
	if( !m_strOrigEventParam.empty() )
		nodeSeq.AddAttribute( "event_param", m_strOrigEventParam );
	if( !m_strOrigEventParam2.empty() )
		nodeSeq.AddAttribute( "event_param2", m_strOrigEventParam2 );
	// orders
	m_bSavePos = false;
	for( auto& spPropOrder : m_aryOrder ) {
		auto nodeOrder = nodeSeq.AddNode( spPropOrder->m_strXmlIds.c_str() );
		spPropOrder->Save( this, nodeOrder );
	}
	m_bSavePos = false;
#endif // _xIN_TOOL
	return true;
}

void XPropSeq::Serialize( XArchive& ar ) const
{
	ar << (int)m_aryOrder.size();
	for( auto spOrder : m_aryOrder ) {
		ar << (int)spOrder->m_Type;
		spOrder->Serialize( ar );
	}
	ar << m_strIdentifier << m_idsPrev << m_idsCompletedSeq << m_idsFinishededQuest;
	ar << m_vStopPos;
	ar << (char)m_SeqEvent;
	ar << (char)0;
	ar << (char)0;
	ar << (char)0;
	ar << m_dwEventParam << m_dwEventParam2;
	ar << m_strEventParam << m_strEventParam2;
}
void XPropSeq::DeSerialize( XArchive& ar, int ver )
{
	m_aryOrder.clear();
	int num;
	ar >> num;
	for( int i = 0; i < num; ++i ) {
		int i0;
		ar >> i0;		auto type = (xtOrder)i0;
		PropOrderPtr spPropOrder;
		switch( type ) {
		case xHelp::xHO_DIALOG:
			spPropOrder = std::make_shared<XPropDialog>();
			break;
		case xHelp::xHO_INDICATE:
			spPropOrder = std::make_shared<XPropIndicate>();
			break;
		case xHelp::xHO_DELAY:
			spPropOrder = std::make_shared<XPropDelay>();
			break;
		case xHelp::xHO_TOUCH:
			spPropOrder = std::make_shared<XPropTouch>();
			break;
		case xHelp::xHO_CAMERA:
			spPropOrder = std::make_shared<XPropCamera>();
			break;
		case xHelp::xHO_READY_TOUCH:
			spPropOrder = std::make_shared<XPropTouch>();
			break;
		case xHelp::xHO_MODE_CHANGE:
			spPropOrder = std::make_shared<XPropMode>();
			break;
		default:
			XBREAK(1);
			break;
		}
		if( spPropOrder ) {
			spPropOrder->DeSerialize( ar, ver );
			m_aryOrder.Add( spPropOrder );
		}
	}
	ar >> m_strIdentifier >> m_idsPrev >> m_idsCompletedSeq >> m_idsFinishededQuest;
	ar >> m_vStopPos;
	char c0;
	ar >> c0;		m_SeqEvent = (xtActionEvent)c0;
	ar >> c0 >> c0 >> c0;
	ar >> m_dwEventParam >> m_dwEventParam2;
	_tstring strTemp;
	ar >> strTemp;		m_strEventParam = SZ2C( strTemp );
	ar >> strTemp;		m_strEventParam2 = SZ2C( strTemp );
}


XE_NAMESPACE_END;


