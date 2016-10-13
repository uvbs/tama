#include "stdafx.h"
#if defined(_CLIENT) || defined(_GAME_SERVER)
#ifdef _CLIENT
#include "etc/XSurface.h"
#endif // _CLIENT
#include "XPropTech.h"
#include "XSkillMng.h"
#include "skill/XSkillDat.h"
#include "XResObj.h"
#include "XStruct.h"
#include "XSystem.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;
//XPropTech *PROP_TECH = NULL;
ID XPropTech::s_idGlobal = 1;

std::shared_ptr<XPropTech> XPropTech::s_spSingleton; //( new XPropTech );
std::shared_ptr<XPropTech> XPropTech::sGet() 
{
	if( s_spSingleton == nullptr )
    s_spSingleton = std::shared_ptr<XPropTech>( new XPropTech );
	return s_spSingleton;
}
//////////////////////////////////////////////////////////////////////////
XVector<XGAME::xRES_NUM> XPropTech::xtResearch::GetaryNeedRes() const {
	XVector<XGAME::xRES_NUM> ary;
	const int size = aryResourceNeed.size();
	for( int i = 0; i < size; ++i ) {
		ary.Add( XGAME::xRES_NUM( aryResourceNeed[i], aryNumNeed[i] ) );
	}
	return ary;
}

//////////////////////////////////////////////////////////////////////////
XPropTech::XPropTech()
// 	: m_goldTierForUnlock( MAX_TIER + 1 )
	: m_aryGoldUnlock( MAX_ABIL_UNIT + 1 )
	, m_aryCost( MAX_ABIL_UNIT * MAX_POINT + 1)
	, m_aryUnitsAbil( xUNIT_MAX )
	, m_aryRoots( xUNIT_MAX )
{ 
	Init(); 
}

BOOL XPropTech::Load( LPCTSTR szXml )
{
	return XXMLDoc::Load( XE::MakePath( DIR_PROP, szXml ) );
}

void XPropTech::Destroy() 
{
	XLIST4_DESTROY( m_listNode );
//	XARRAYN_DESTROY( m_aryRoots );
	XVECTOR_DESTROY( m_aryRoots );
}

bool XPropTech::OnDidFinishLoad()
{
	XEXmlNode nodeRoot = FindNode( "root" );
	if( nodeRoot.IsEmpty() )
		return false;
	// 각 유닛들을 루프돈다.
	for( int i = 1; i < XGAME::xUNIT_MAX; ++i ) {
		XGAME::xtUnit unit = (XGAME::xtUnit) i;
		// cIds유닛의 특성리스트를 읽는다.
		LoadUnit( nodeRoot, unit );
	}
	return true;
}

/**
 @brief 각 유닛의 특성리스트를 읽는다.
*/
XPropTech::xNodeAbil* XPropTech::LoadUnit( XEXmlNode& nodeRoot, XGAME::xtUnit unit )
{
	const char *cUnit = XGAME::GetUnitIdentifier( unit );
	XEXmlNode nodeUnit = nodeRoot.FindNode( cUnit );
	if( nodeUnit.IsEmpty() )
	{
//		XALERT("<propTech.xml> not found unit node: %s", C2SZ(cUnit) );
		return nullptr;
	}
	XEXmlNode nodeCristic = nodeUnit.GetFirst();
	xNodeAbil *pNodeRoot = nullptr;
	// 루트노드 생성
//	XArrayLinearN<xNodeAbil*, 256> aryTemp;
	XVector<xNodeAbil*> aryTempEachUnit;
	while( !nodeCristic.IsEmpty() )
	{
		if( nodeCristic.GetcstrName() == "abil" )
		{
			xNodeAbil *pNodeAbil = new xNodeAbil;
			aryTempEachUnit.Add( pNodeAbil );
			pNodeAbil->idNode = nodeCristic.GetInt( "id" );
			if( pNodeAbil->idNode == 0 )
				pNodeAbil->idNode = sGenerateID();
			else
			if( pNodeAbil->idNode >= s_idGlobal)
				s_idGlobal = pNodeAbil->idNode + 1;
//			pNodeAbil->strIdentifier = nodeCristic.GetcstrName();	
			pNodeAbil->idName = nodeCristic.GetInt( "id_name" );
			pNodeAbil->idDesc = nodeCristic.GetInt( "id_desc" );
			pNodeAbil->strSkill = C2SZ(nodeCristic.GetString( "skill" ));
			std::transform( pNodeAbil->strSkill.begin(), pNodeAbil->strSkill.end(), pNodeAbil->strSkill.begin(), tolower );
			pNodeAbil->strIcon = C2SZ( nodeCristic.GetString( "icon" ) );
//			pNodeAbil->lvOpenable = nodeCristic.GetInt( "level" );
			pNodeAbil->tier = nodeCristic.GetInt("tier");
			XBREAK( pNodeAbil->tier == 0 || pNodeAbil->tier > 5 );

			pNodeAbil->maxPoint = nodeCristic.GetInt( "max_point" );
			pNodeAbil->vPos.x = (float)nodeCristic.GetInt( "posx" );
			pNodeAbil->vPos.y = (float)nodeCristic.GetInt( "posy" );
			pNodeAbil->unit = unit;
			{
				XEXmlNode nodeParent = nodeCristic.GetFirst();
				while( !nodeParent.IsEmpty() ) {
					if( nodeParent.GetcstrName() == "parent" ) {
						ID idParent = nodeParent.GetInt( "node" );
						if( idParent > 0 )
							pNodeAbil->listParentID.Add( idParent );
					}
					// 노드 포인터 할당은 로딩이 다끝난후 마지막에 한다.
					nodeParent = nodeParent.GetNext();
				}
			}
			// 부모가 없는 특성이면 루트노드의 자식으로 붙인다.
			// 자식리스트 읽음
			{
				XEXmlNode nodeChild = nodeCristic.GetFirst();
				while( !nodeChild.IsEmpty() ) {
					if( nodeChild.GetcstrName() == "child" ) {
						ID idChild = nodeChild.GetInt( "node" );
						if( idChild )
							pNodeAbil->listChildID.Add( idChild );
					}
					// 노드 포인터 할당은 로딩이 다끝난후 마지막에 한다.
					nodeChild = nodeChild.GetNext();
				}
			}
			// 아이콘이 없으면 스킬프로퍼티에서 가져온다.
			if( pNodeAbil->strIcon.empty() ) {
				auto pDat = SKILL_MNG->FindByIds( pNodeAbil->strSkill );
				if( pDat == nullptr ) {
					CONSOLE( "<propTech.xml> id=%d:not found skill:%s", pNodeAbil->idNode, pNodeAbil->strSkill.c_str() );
					pNodeAbil->strIcon = _T( "icon_dummy.png" );
				} else {
					pNodeAbil->strIcon = pDat->GetstrIcon();
					if( pNodeAbil->strIcon.empty() ) {
						CONSOLE( "<propTech.xml> id=%d:not found skill:%s", pNodeAbil->idNode, pNodeAbil->strSkill.c_str() );
						pNodeAbil->strIcon = _T( "icon_dummy.png" );
					}
				}
			}
			XBREAK( pNodeAbil->strIcon == _T("null") );
			Add( unit, pNodeAbil );
		} // if( nodeCristic.GetcstrName() == "abil" )
		//
		nodeCristic = nodeCristic.GetNext();
	} // while( !nodeCristic.IsEmpty() )

	// 부모/자식 포인터들을 연결시킨다.
	DoLinkParentChild( unit );

  return nullptr;
}

void XPropTech::DoLinkParentChild( XGAME::xtUnit unit )
{
	// 부모/자식 포인터들을 연결시킨다.
//	XARRAYLINEARN_LOOP( aryTemp, xNodeAbil*, pNodeAbil )
	const auto& aryAbils = m_aryUnitsAbil[unit];
	for( auto pNodeAbil : m_aryUnitsAbil[unit] ) {
		// 부모포인터를 모두 찾아서 연결시킨다.
		for( auto idParent : pNodeAbil->listParentID ) {
			xNodeAbil *pNodeParent = GetNodeAbilByAry( aryAbils, idParent );
			if( pNodeParent )
				pNodeAbil->listParent.Add( pNodeParent );
		}
		// 자식포인터를 모두 찾아서 연결시킨다.
		for( auto idChild : pNodeAbil->listChildID ) {
			xNodeAbil *pNodeChild = GetNodeAbilByAry( aryAbils, idChild );
			if( pNodeChild )
				pNodeAbil->listChild.Add( pNodeChild );
		}
	}
	// 트리구조를 위해 최상위 루트를 연결한다.
	{
		auto& listNodes = m_aryUnitsAbil[unit];
		auto pRoot = new xNodeAbil;
		pRoot->idNode = 0;		// 더미
		m_aryRoots[unit] = pRoot;
		// unit이 가진 모든 특성을 돌며
		for( auto pNode : listNodes ) {
			// 부모가 없는 노드는 최상단 노드이다. 최상단노드가 반드시 tier==1인것은아니다.
			if( pNode->listParent.size() == 0 ) {
				pRoot->AddChild( pNode );
			}
		}
	}
}

XPropTech::xNodeAbil* XPropTech::GetNodeAbilByAry( XArrayLinearN<xNodeAbil*, 256>& ary, 
													ID idNode )
{
	XARRAYLINEARN_LOOP( ary, xNodeAbil*, pNodeAbil )
	{
		if( pNodeAbil->idNode == idNode )
			return pNodeAbil;
	} END_LOOP;
	return nullptr;
}

XPropTech::xNodeAbil* XPropTech::GetNodeAbilByAry( const XVector<xNodeAbil*>& ary, ID idNode )
{
	for( auto pNodeAbil : ary ) {
		if( pNodeAbil->idNode == idNode )
			return pNodeAbil;
	}
	return nullptr;
}

XPropTech::xNodeAbil* XPropTech::GetNodeAbilByAry( const XList4<xNodeAbil*>& ary, ID idNode )
{
	for( auto pNodeAbil : ary ) {
		if( pNodeAbil->idNode == idNode )
			return pNodeAbil;
	}
	return nullptr;
}

void XPropTech::Add( XGAME::xtUnit unit, xNodeAbil *pNewNode )
{
	XBREAK( XGAME::IsInvalidUnit(unit) );
	m_listNode.Add( pNewNode );
	m_aryUnitsAbil[ unit ].Add( pNewNode );
}

/**
 @brief 유닛명 없이 스킬 식별자로만 찾는버전. 느림
*/
XPropTech::xNodeAbil* XPropTech::GetpNode( LPCTSTR szSkill )
{
	for( auto pNodeAbil : m_listNode ) {
		if( pNodeAbil->strSkill == szSkill )
			return pNodeAbil;
	}
	return nullptr;
}

/**
 @brief 유닛명 없이 찾는버전. 느림
*/
XPropTech::xNodeAbil* XPropTech::GetpNode( ID idNode )
{
	if( idNode == 0 )
		return nullptr;
	for( auto pNodeAbil : m_listNode ) {
		if( pNodeAbil->idNode == idNode )
			return pNodeAbil;
	}
	return nullptr; 
}

XPropTech::xNodeAbil* XPropTech::GetpNode( XGAME::xtUnit unit, ID idNode )
{
	if( !unit )
		return nullptr;
	if( idNode == 0 )
		return nullptr;
	auto& listNodeAbil = m_aryUnitsAbil[ unit ];
	if( listNodeAbil.size() == 0 )
		return nullptr;
	for( auto pNodeAbil : listNodeAbil ) {
		if( pNodeAbil->idNode == idNode )
			return pNodeAbil;
	}
	return nullptr;
}

/**
 @brief 특성 이름으로 검색
*/
XPropTech::xNodeAbil* XPropTech::GetpNodeByName( XGAME::xtUnit unit, LPCTSTR szName )
{
	XBREAK( XE::IsEmpty( szName ) );
	XList4<xNodeAbil*>& listNodeAbil = m_aryUnitsAbil[ unit ];
	if( listNodeAbil.size() == 0 )
		return nullptr;
	for( auto pNodeAbil : listNodeAbil )
	{
		if( XE::IsSame( XTEXT( pNodeAbil->idName ), szName ) )
			return pNodeAbil;
	}
	return nullptr;
}

/**
 @brief 패시브 스킬 식별자로 검색
*/
XPropTech::xNodeAbil* XPropTech::GetpNodeBySkill( XGAME::xtUnit unit, LPCTSTR idsSkill )
{
	XBREAK( XE::IsEmpty( idsSkill ) );
	XList4<xNodeAbil*>& listNodeAbil = m_aryUnitsAbil[ unit ];
	if( listNodeAbil.size() == 0 )
		return nullptr;
	for( auto pNodeAbil : listNodeAbil )
	{
		if( pNodeAbil->strSkill == idsSkill )
			return pNodeAbil;
	}
	return nullptr;
}

/**
 @brief vPos가 들어가는 노드가 있는지 검사.
*/
XPropTech::xNodeAbil* XPropTech::GetNodeByPos( XGAME::xtUnit unit, const XE::VEC2& vPos )
{
	XList4<xNodeAbil*>& listNodeAbil = m_aryUnitsAbil[ unit ];
	if( listNodeAbil.size() == 0 )
		return nullptr;
	for( auto pNodeAbil : listNodeAbil )
	{
		if( XE::IsArea( pNodeAbil->vPos, ICON_SIZE, vPos) )
			return pNodeAbil;
	}
	return nullptr;
}

BOOL XPropTech::Save(  LPCTSTR szXml )
{
#ifdef _xIN_TOOL
	// 자동 백업.
	_tstring strSrc = XE::MakePackageFullPath( DIR_PROP, szXml );
	TCHAR szSystemPath[4096];
	XE::MakeHighPath( szSystemPath, XE::MakePackageFullPath( _T(""), _T("") ) );
	_tstring strDstPath = szSystemPath;
	strDstPath += _T("backup/");
	_tstring strDst = strDstPath;
		//XE::MakePackageFullPath( _T("../../backup/"), XE::GetFileTitle(szXml) );
	int h, m, s;
	XSYSTEM::GetHourMinSec( &h, &m, &s );
	strDst += XE::Format(_T("propTech_%02d%02d%02d.xml"), h, m, s );
	XSYSTEM::MakeDir( strDstPath.c_str() );
	XSYSTEM::CopyFileX( strSrc.c_str(), strDst.c_str() );

	_tstring strPath = XE::MakePackageFullPath( DIR_PROP, szXml );
	XE::xtERROR err = XSYSTEM::IsReadOnly( strPath.c_str() );
	if( err == XE::ERR_READONLY || err == XE::ERR_PERMISSION_DENIED )
	{
		XALERT( "파일을 check out하십시오.\n%s", XE::GetFileName( strPath.c_str() ) );
		return FALSE;
	}
	XXMLDoc xml;
	xml.SetDeclaration();
	XEXmlNode nodeRoot = xml.AddNode( "root" );

	int size = m_aryUnitsAbil.size();
	XBREAK( size == 0 );
	for( int i = 1; i < size; ++i )
	{
		XList4<xNodeAbil*>& listNodeAbil = m_aryUnitsAbil[i];
		if( listNodeAbil.size() == 0 )
			continue;;
		XGAME::xtUnit unit = (XGAME::xtUnit)i;
		XEXmlNode nodeUnit = nodeRoot.AddNode( XGAME::GetUnitIdentifier(unit) );
		for( auto pNodeAbil : listNodeAbil )
		{
			// 
			XEXmlNode nodeAbil = nodeUnit.AddNode( "abil" );
			nodeAbil.AddAttribute( "id", pNodeAbil->idNode );
			nodeAbil.AddAttribute( "skill", pNodeAbil->strSkill.c_str() );
			if( pNodeAbil->strIcon != _T("icon_dummy.png"))
				nodeAbil.AddAttribute( "icon", pNodeAbil->strIcon.c_str() );
			nodeAbil.AddAttribute( "id_name", pNodeAbil->idName );
			nodeAbil.AddAttribute( "id_desc", pNodeAbil->idDesc );
//			nodeAbil.AddAttribute( "level", pNodeAbil->lvOpenable );
			XBREAK( pNodeAbil->tier == 0 );
			nodeAbil.AddAttribute( "tier", pNodeAbil->tier );
			nodeAbil.AddAttribute( "max_point", pNodeAbil->maxPoint );
			nodeAbil.AddAttribute( "posx", (int)pNodeAbil->vPos.x/* - ADJ_X*/);
			nodeAbil.AddAttribute( "posy", (int)pNodeAbil->vPos.y );
			for( auto pNodeParent : pNodeAbil->listParent )
			{
				XEXmlNode nodeParent = nodeAbil.AddNode( "parent" );
				nodeParent.AddAttribute( "node", pNodeParent->idNode );
			}
			for( auto pNodeChild : pNodeAbil->listChild )
			{
				XEXmlNode nodeChild = nodeAbil.AddNode( "child" );
				nodeChild.AddAttribute( "node", pNodeChild->idNode );
			}
		}
	} // for

	CONSOLE( "save propTech" );
	//
	return xml.Save( XE::MakePackageFullPath( DIR_PROP, szXml ) );
	return TRUE;
#else
	return TRUE;
#endif // _xIN_TOOL
}

void XPropTech::DelNode( ID idNode )
{
	m_listNode.DelByID( idNode );
	int size = m_aryUnitsAbil.size();
	for( int i = 1; i < size; ++i )	{
		XList4<xNodeAbil*>& listNodeAbil = m_aryUnitsAbil[ i ];
		if( listNodeAbil.size() == 0 )
			continue;;

		xNodeAbil **ppNode = listNodeAbil.FindpByID( idNode );
		listNodeAbil.DelByID( idNode );
		if( XASSERT(ppNode) )	{
			// 삭제된 노드를 자식으로 두고있던 부모에게서 삭제노드를 뺀다.
			for( auto pParent : (*ppNode)->listParent )	{
				pParent->listChild.DelByID( idNode );
			}
			// 삭제된 노드를 부모로 두고 있던 노드들에게서 삭제노드를 뺀다.
			for( auto pChild : (*ppNode)->listChild )	{
				pChild->listParent.DelByID( idNode );
			}
//			SAFE_DELETE( pNode );		// 언두를 위해서 아직 삭제안하고 남겨둠.
		}
	}
}

void XPropTech::OnDidBeforeReadProp( CToken& token )
{
}

/**
 @brief 엑셀파일로부터 연구시 필요한 데이타 테이블 읽음.
*/
BOOL XPropTech::ReadProp( CToken& token, DWORD dwParam )
{
	int num = token.GetNumber();		// 포인트개수
	xtResearch& cost = m_aryCost[ num ];
	cost.sec = token.GetNumber() * 2;		// 훈련시간
	for( int i = 0; i < XGAME::xRES_MAX; ++i ) {
		int numRes = (int)(token.GetNumber() * 0.5f);
		if( numRes > 0 ) {
			cost.aryResourceNeed.Add( (XGAME::xtResource)i );	// 자원종류
			cost.aryNumNeed.Add( numRes );										// 자원개수
		}
	}
	if( num >= (int)m_aryCost.size() - 1)
		return FALSE;
	return TRUE;	// 한줄씩 읽지않고 위에서 한번에 다 읽어버리고 블럭을 빠져나옴.
}

void XPropTech::OnDidFinishReadProp( CToken& token ) 
{
	// 잠금해제 비용 읽음
	for( int i = 1; i < (int)m_aryGoldUnlock.size(); ++i ) {
		int numUnlock = token.GetNumber();
		XBREAK( numUnlock == TOKEN_EOF );
		m_aryGoldUnlock[ numUnlock ] = token.GetNumber() / 2;
	}
}

/**
 @brief 
*/
void XPropTech::SetResource( int tier, int point, XGAME::xtResource resType, int num )
{
}

/**
 @brief unit의 특성중 iter단계의 특성들을 모두 꺼낸다.
*/
int XPropTech::GetNodesByTierToAry( XGAME::xtUnit unit, int tier, XArrayLinearN<xNodeAbil*, 128> *pOut ) 
{
	int max = m_aryUnitsAbil[ unit ].size();
	for( auto pNode : m_aryUnitsAbil[ unit ] ) 
	{
		if( pNode->tier == tier )
			pOut->Add( pNode );
	}
	return pOut->size();
}
/**
 @brief idNode에서 나가는 연결(자식으로 가는)을 모두 끊는다.
*/
void XPropTech::DelOutLinkByNode( XGAME::xtUnit unit, ID idNode )
{
	auto pParent = GetpNode( unit, idNode );
	if( XASSERT(pParent) ) {
		// 자식들이 가리키는 부모부터 클리어
		for( auto pChild : pParent->listChild ) {
			pChild->listParent.DelByID( pParent->idNode );
			pChild->listParentID.Del( pParent->idNode );
		}
		pParent->listChild.clear();
		pParent->listChildID.clear();
	}
}

/**
 @brief pParent의 자식중에서 idChild의 연결을 끊는다. 동시에 child쪽에서도 pParent를 부모리스트에서 뺀다.
*/
void XPropTech::ClearChild( xNodeAbil* pParent, xNodeAbil* pChild )
{
	pChild->listParent.DelByID( pParent->idNode );
	pChild->listParentID.Del( pParent->idNode );
	pParent->listChild.DelByID( pChild->idNode );
	pParent->listChildID.Del( pChild->idNode );
}

void XPropTech::ClearChild( xNodeAbil* pParent, ID idChild )
{
	auto pChild = GetpNode( idChild );
	if( pChild )
		ClearChild( pParent, pChild );
}

void XPropTech::ClearChild( ID idParent, xNodeAbil* pChild )
{
	auto pParent = GetpNode( idParent );
	if( pParent )
		ClearChild( pParent, pChild );
}

void XPropTech::ClearChild( ID idParent, ID idChild )
{
	auto pParent = GetpNode( idParent );
	if( pParent ) {
		auto pChild = GetpNode( idChild );
		ClearChild( pParent, pChild );
	}
}

void XPropTech::Serialize( XArchive& ar ) const
{
	ar << VER_PROP_TECH;
	ar << s_idGlobal;
	ar << (int)m_listNode.size();
	int idx = 0;
	for( auto pNode : m_listNode ) {
		pNode->Serialize( ar );
	}
	ar << m_aryGoldUnlock;
	ar << m_aryCost;
}
void XPropTech::DeSerialize( XArchive& ar, int )
{
	Destroy();
	int ver, num;
	ar >> ver;
	ar >> s_idGlobal;
	ar >> num;
	for( int i = 0; i < num; ++i ) {
		auto pNode = new xNodeAbil();
		pNode->DeSerialize( ar, ver );
		Add( pNode->unit, pNode );
	}
	ar >> m_aryGoldUnlock;
	ar >> m_aryCost;
	//
	m_aryRoots.resize( xUNIT_MAX );
	for( int i = 1; i < xUNIT_MAX; ++i ) {
		const auto unit = (xtUnit)(i);
		DoLinkParentChild( unit );;
	}
}

void XPropTech::xtResearch::Serialize( XArchive& ar ) const {
	ar << sec;
	ar << (int)aryResourceNeed.size();
	XARRAYLINEARN_LOOP_AUTO( aryResourceNeed, resType ) {
		ar << (int)resType;
	} END_LOOP;
	ar << aryNumNeed;
}
void XPropTech::xtResearch::DeSerialize( XArchive& ar, int ) {
	aryResourceNeed.Clear();
	aryNumNeed.Clear();
	int num;
	ar >> sec >> num;
	for( int i = 0; i < num; ++i ) {
		int i0;
		ar >> i0;		aryResourceNeed.Add( (xtResource)i0 );
	}
	ar >> aryNumNeed;
}

void XPropTech::xNodeAbil::Serialize( XArchive& ar ) const {
	ar << listParentID;
	ar << listChildID;
	ar << idNode;
	ar << strSkill << strIcon;
	ar << (int)unit;
	ar << idName << idDesc;
	ar << point << maxPoint << tier;
	ar << vPos;

}
void XPropTech::xNodeAbil::DeSerialize( XArchive& ar, int ) {
	int i0;
	listParentID.clear();
	listParent.clear();
	listChildID.clear();
	listChild.clear();
	ar >> listParentID;
	ar >> listChildID;
	ar >> idNode;
	ar >> strSkill >> strIcon;
	ar >> i0;		unit = (xtUnit)i0;
	ar >> idName >> idDesc;
	ar >> point >> maxPoint >> tier;
	ar >> vPos;
}

#ifdef _CLIENT
void XPropTech::xNodeAbil::DrawIcon( const XE::VEC2& vAdj )
{
	psfcIcon->Draw( vPos + vAdj );
}
#endif // _CLIENT

#endif // #if defined(_CLIENT) || defined(_GAME_SERVER)
