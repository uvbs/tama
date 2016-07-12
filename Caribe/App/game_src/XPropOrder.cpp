#include "stdafx.h"
#include "XPropOrder.h"
#include "XPropSeq.h"
#include "XPropHelp.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

XE_NAMESPACE_START( xHelp )

bool XPropOrder::Save( XPropSeq *pSeq, XEXmlNode& nodeOrder )
{
#ifdef _xIN_TOOL
	if( m_typeEnd == xFIN_END_ORDER ) {
		nodeOrder.AddAttribute( "end_order", m_idsEnd );
	} else {
		nodeOrder.AddAttribute( "end", m_strXmlTypeEnd );
		nodeOrder.AddAttribute( "ids_end", m_idsEnd );
	}
	nodeOrder.AddAttribute( "ids", m_strIdentifer );
	std::string strIdsTarget;
	int idx = 0;
	for( auto& str : m_aryIdsTarget ) {
		strIdsTarget += str;
		if( idx++ < (int)m_aryIdsTarget.size() - 1 ) {
			strIdsTarget += ", ";
		}
	}
	nodeOrder.AddAttribute( "ids_target", strIdsTarget );
#endif // _xIN_TOOL
	return true;
}

void XPropOrder::Serialize( XArchive& ar ) const
{
	ar << m_strIdentifer << m_idsEnd;
	ar << m_aryIdsTarget << m_aryIdsAllow;
	ar << (char)m_Type;
	ar << (char)m_typeEnd;
	ar << (char)0;
	ar << (char)0;
	ar << m_idx;
}
void XPropOrder::DeSerialize( XArchive& ar, int )
{
	_tstring strTemp;
	ar >> strTemp;		m_strIdentifer = SZ2C(strTemp);
	ar >> strTemp;		m_idsEnd = SZ2C(strTemp);
	int num;
	ar >> num;
	for( int i = 0; i < num; ++i ) {
		ar >> strTemp;		m_aryIdsTarget.push_back( SZ2C( strTemp ) );
	}
	ar >> num;
	for( int i = 0; i < num; ++i ) {
		ar >> strTemp;		m_aryIdsAllow.push_back( SZ2C( strTemp ) );
	}
	char c0;
	ar >> c0;		m_Type = (xtOrder)c0;
	ar >> c0;		m_typeEnd = (xtFinish)c0;
	ar >> c0;		
	ar >> c0;		
	ar >> m_idx;
}
//////////////////////////////////////////////////////////////////////////
void XPropIndicate::Serialize( XArchive& ar ) const {
	XPropOrder::Serialize( ar );
	ar << m_vPos << m_dAng;
	ar << m_strSpr;
	ar << m_idAct;
	ar << m_idsRoot;
	ar << m_secLife;
}
void XPropIndicate::DeSerialize( XArchive& ar, int ver ) {
	XPropOrder::DeSerialize( ar, ver );
	ar >> m_vPos >> m_dAng;
	ar >> m_strSpr;
	ar >> m_idAct;
	_tstring strTemp;
	ar >> strTemp;		m_idsRoot = SZ2C(strTemp);
	ar >> m_secLife;
}

bool XPropIndicate::Save( XPropSeq *pSeq, XEXmlNode& nodeOrder )
{
#ifdef _xIN_TOOL
	XPropOrder::Save( pSeq, nodeOrder );
	//
	if( m_aryIdsTarget.size() == 0 || m_vPos.x > 0 || m_vPos.y > 0 ) {
		nodeOrder.AddAttribute( "x", m_vPos.x );
		nodeOrder.AddAttribute( "y", m_vPos.y );
	}
	if( m_dAng == 0 )
		nodeOrder.AddAttribute( "dir", "right" );
	else if( m_dAng == 180.f )
		nodeOrder.AddAttribute( "dir", "left" );
	else if( m_dAng == 270.f )
		nodeOrder.AddAttribute( "dir", "up" );
	else if( m_dAng == 90.f )
		nodeOrder.AddAttribute( "dir", "down" );
	else
		nodeOrder.AddAttribute( "dir", (int)m_dAng );
	nodeOrder.AddAttribute( "root", m_idsRoot );
	nodeOrder.AddAttribute( "spr", m_strSpr );
	if( m_idAct )
		nodeOrder.AddAttribute( "id_act", m_idAct );
	std::string strIdsAllow;
	int idx = 0;
	for( auto& str : m_aryIdsAllow ) {
		strIdsAllow += str;
		if( idx++ < (int)m_aryIdsAllow.size() - 1 ) {
			strIdsAllow += ", ";
		}
	}
	nodeOrder.AddAttribute( "ids_allow", strIdsAllow );
#endif // _xIN_TOOL
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool XPropDialog::Save( XPropSeq *pSeq, XEXmlNode& nodeOrder )
{
#ifdef _xIN_TOOL
	XPropOrder::Save( pSeq, nodeOrder );
	//
	if( !_m_strText.empty() ) {
		XASSERT( m_idText == 0 );
//		nodeOrder.AddAttribute( "text", _m_strText );
		// 텍스트형태로 있었으면 텍스트테이블에 넣고 그 아이디만 저장한다.
		ID idNew = TEXT_TBL->AddTextAutoID( _m_strText.c_str(), 70000, 10000 );
		m_idText = idNew;
		_m_strText.clear();
		XPropHelp::s_bTextAdded = true;
	}
	if( m_idText ) {
		nodeOrder.AddAttribute( "id_text", m_idText );
		XASSERT( _m_strText.empty() );
	}
	nodeOrder.AddAttribute( "talker", m_strTalker );
	if( m_vPos.x >= 0 )
		nodeOrder.AddAttribute( "x", m_vPos.x );
	if( m_vPos.y >= 0 )
		nodeOrder.AddAttribute( "y", m_vPos.y );
	if( m_bSavePos ) {
		nodeOrder.AddAttribute( "save_pos", true );		// on만 저장한다.
	} 
#endif // _xIN_TOOL
	return true;
}

void XPropDialog::Serialize( XArchive& ar ) const
{
	XPropOrder::Serialize( ar );
	ar << m_strTalker;
	ar << m_idText;
	ar << m_vPos;
	ar << (char)xboolToByte(m_bSavePos);
	ar << (char)0;
	ar << (short)0;
//	m_aryIndicate.SerializePtrElem<std::shared_ptr<XPropIndicate>>( ar );
	ar.SerializePtr( m_aryIndicate );
}
void XPropDialog::DeSerialize( XArchive& ar, int ver )
{
	XPropOrder::DeSerialize( ar, ver );
	m_aryIndicate.clear();
	char c0;
	ar >> m_strTalker;
	ar >> m_idText;
	ar >> m_vPos;
	ar >> c0;		m_bSavePos = xbyteToBool(c0);
	ar >> c0 >> c0 >> c0;
//	m_aryIndicate.DeSerializeSharedPtrElem<XPropIndicate>( ar, ver );
	ar.DeSerializeSharedPtr( m_aryIndicate, ver );
}


bool XPropCamera::Save( XPropSeq *pSeq, XEXmlNode& nodeOrder )
{
#ifdef _xIN_TOOL
	XPropOrder::Save( pSeq, nodeOrder );
	//
	if( m_aryIdsTarget.size() == 0 ) {
		nodeOrder.AddAttribute( "x", m_vwDst.x );
		nodeOrder.AddAttribute( "y", m_vwDst.y );
		nodeOrder.AddAttribute( "push_pos", m_bPushPos );
		nodeOrder.AddAttribute( "pop_pos", m_bPopPos );
	}
#endif // _xIN_TOOL
	return true;
}

bool XPropTouch::Save( XPropSeq *pSeq, XEXmlNode& nodeOrder )
{
#ifdef _xIN_TOOL
	XPropOrder::Save( pSeq, nodeOrder );
	//
	nodeOrder.AddAttribute( "flag", m_bFlag );
#endif // _xIN_TOOL
	return true;
}

bool XPropDelay::Save( XPropSeq *pSeq, XEXmlNode& nodeOrder )
{
#ifdef _xIN_TOOL
	XPropOrder::Save( pSeq, nodeOrder );
	//
	nodeOrder.AddAttribute( "sec", m_secDelay );
#endif // _xIN_TOOL
	return true;
}

bool XPropMode::Save( XPropSeq *pSeq, XEXmlNode& nodeOrder )
{
#ifdef _xIN_TOOL
	XPropOrder::Save( pSeq, nodeOrder );
	//
	nodeOrder.AddAttribute( "mode", m_strMode );
	nodeOrder.AddAttribute( "flag", m_bFlag );
#endif // _xIN_TOOL
	return true;
}

XE_NAMESPACE_END;
