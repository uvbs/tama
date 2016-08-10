#include "stdafx.h"
#include "XPropCloud.h"
#include "XArchive.h"
#include "XWorld.h"
#include "XGlobalConst.h"
#include "XSystem.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

#ifdef _xIN_TOOL
ID XPropCloud::s_idGlobal = 0;
#endif

XPropCloud *PROP_CLOUD = nullptr;

void XPropCloud::xCloud::AddSpot( ID idNew ) {
	LOOP_SPOTS( idSpot ) {
		if( idNew == idSpot )	// 이미 있는건 다시 추가 하지 않음.
			return;
	} END_LOOP;
	spots.Add( idNew );
}
void XPropCloud::xCloud::ClearSpots( void ) {
	spots.Clear();
}
///< 이 구름지역에 idSpot이 있는지 검사.
bool XPropCloud::xCloud::IsHaveSpot( ID _idSpot ) {
	LOOP_SPOTS( idSpot ) {
		if( idSpot == _idSpot )
			return true;
	} END_LOOP;
	return false;
}
void XPropCloud::xCloud::AddIdx( int idxNew ) {
	XBREAK( idxNew < 0 );
	LOOP_HEXA( hexa ) {
		if( idxNew == hexa.idx )	// 이미 있는건 다시 추가 하지 않음.
			return;
	} END_LOOP;
	xHexa hexa;
	hexa.idx = idxNew;
	idxs.Add( hexa );
}
bool XPropCloud::xCloud::SetIdxOpened( int idxHexa, bool bOpened )
{
	XBREAK( idxHexa <= 0 );
	for( auto& hexa : idxs ) {
		if( idxHexa == hexa.idx )	{
			hexa.bOpened = bOpened;
			return true;
		}
	}
	return false;
}

void XPropCloud::xCloud::ClearIdxs( void ) {
	idxs.Clear();
}
BOOL XPropCloud::xCloud::IsHaveIdx( int idxFind ) {
	LOOP_HEXA( hexa ) {
		if( hexa.idx == idxFind )
			return TRUE;
	} END_LOOP;
	return FALSE;
}
/**
 @brief idxFind헥사가 이 구름에 속해있고 뚫려있는가.
*/
bool XPropCloud::xCloud::IsHaveIdxOpened( int idxFind ) {
	for( auto& hexa : idxs ) {
		if( hexa.idx == idxFind && hexa.bOpened )
			return true;
	}
	return false;
}

void XPropCloud::xCloud::GetSpotsToAry( XVector<ID> *pOut ) {
	*pOut = spots;
}



////////////////////////////////////////////////////////////////
XPropCloud::XPropCloud( LPCTSTR szXml )
{
	Init();
	Load( XE::MakePath( DIR_PROP, szXml) );
}

void XPropCloud::Destroy()
{
	XLIST4_DESTROY( m_Clouds );
}

bool XPropCloud::OnDidFinishLoad()
{
	XEXmlNode rootNode = FindNode( "clouds" );
	XEXmlNode childNode = rootNode.GetFirst();
	if( childNode.IsEmpty() )
		return false;
	// 각 레벨지역이 몇개씩으로 쪼개져있는지정보.
	std::vector<int> aryNumArea( XGAME::MAX_AREA_LEVEL + 1 );
	while( !childNode.IsEmpty() ) {
		xCloud *pCloud = new xCloud;
//		m_Clouds.Add( pCloud );
		pCloud->idCloud = (ID)childNode.GetInt( "id" );
		XBREAK( pCloud->idCloud == 0 );
		// 식별자가 있는 경우 읽는다.
		const char *cIdentifier = childNode.GetString("identifier");
		if( XE::IsEmpty(cIdentifier) )
			cIdentifier = childNode.GetString( "ids" );
		if( XASSERT(XE::IsHave(cIdentifier)) ) {
			pCloud->strIdentifier = C2SZ(cIdentifier);
		} else {
			// 없으면 자동으로 만들어줌.
			pCloud->strIdentifier = XFORMAT("area.%d", pCloud->idCloud );
		}
// 		if( XASSERT(!pCloud->strIdentifier.empty()) ) {
// 			if( XASSERT(GetpProp( pCloud->strIdentifier ) == nullptr) )
// 				m_mapCloudsIds[ pCloud->strIdentifier ] = pCloud;
// 		}
// 		if( XASSERT(GetpProp(pCloud->idCloud) == nullptr) )
// 			m_mapCloudsID[ pCloud->idCloud ] = pCloud;
		AddCloud( pCloud );
#ifdef _xIN_TOOL
		if( pCloud->idCloud > XPropCloud::s_idGlobal )
			XPropCloud::s_idGlobal = pCloud->idCloud;	// id중에 가장큰수를 선택한다.
#endif
		pCloud->lvArea = childNode.GetInt( "level" );
		++aryNumArea[ pCloud->lvArea ];
		pCloud->lvOpenable2 = childNode.GetInt( "lv_open" );
		pCloud->cost = childNode.GetInt( "cost" );
		pCloud->vAdjust.x = (float)childNode.GetInt( "cost_pos_x" );
		pCloud->vAdjust.y = (float)childNode.GetInt( "cost_pos_y" );
		pCloud->idsPrecedeArea = childNode.GetTString( "ids_precede" );
		XBREAK( pCloud->idsPrecedeArea.empty() );		// 이제 반드시 이전지역이 연결되어 있어야 함.
		if( pCloud->idsPrecedeArea == _T("none") )	// 가장 첫번째 지역
			pCloud->idsPrecedeArea.clear();
		pCloud->idsQuest = childNode.GetTString( "ids_quest" );
		pCloud->idName = (DWORD)childNode.GetInt("name");
		pCloud->idsItem = childNode.GetTString( "key_item" );
		const _tstring strUnit = childNode.GetTString( "unlock" );
		if( !strUnit.empty() ) {
			int nUnit = CONSTANT->GetValue( strUnit );
			if( CONSTANT->IsFound() ) {
				pCloud->unitUnlock = (XGAME::xtUnit)nUnit;
			}
		}
		
		{
			XEXmlNode nodeSpots = childNode.GetFirst();
			XVector<ID> arySpots;
			while( !nodeSpots.IsEmpty() ) {
				if( nodeSpots.GetcstrName() == "spot" ) {
					ID idSpots = (ID)nodeSpots.GetInt( "id" );
					arySpots.Add( idSpots );
				}
				nodeSpots = nodeSpots.GetNext();
			}
			if( arySpots.size() > 0 )
				pCloud->spots = arySpots;
		}
		{
			// idx목록
//			XArrayLinearN<xHexa, 256> aryIdxs;
//			XVector<xHexa> aryIdxs;
			_tstring strIdxs = U82SZ(childNode.GetString("idx"));
			CToken token;
			token.LoadStr( strIdxs.c_str() );
			int idx = 0;
			while(1) {
				if( token.IsEof() )
					break;
				idx = token.GetNumber();
				xHexa hexa;
				hexa.idx = idx;
				pCloud->idxs.Add( hexa );
//				aryIdxs.Add( hexa );
			}
			if( pCloud->idxs.size() > 0 ) {
				// 지역내 스팟들의 정보를 꺼내서 opened속성이면 해당 헥사위치구름을 opened상태로 세팅한다.
				for( auto idSpot : pCloud->spots ) {
					XBREAK( PROP_WORLD == nullptr );
					auto pPropSpot = PROP_WORLD->GetpProp( idSpot );
					if( XASSERT(pPropSpot) ) {
						if( pPropSpot->IsOpened() ) {
							int idxHexa = GetIdxHexaFromWorld( pPropSpot->vWorld );
							bool bOk = pCloud->SetIdxOpened( idxHexa, true );
							// opened속성의 스팟이 계산오차로인해 idxHexa가 인접한 다른 헥사로 나와서 그럼. 스팟위치를 좀 조절해볼것.
							XBREAKF( bOk == false, "스팟좌표를 각 10pixel가량 조정바람: idSpot=%d", pPropSpot->idSpot );  
						}
					}
				}
			}
		}
		childNode = childNode.GetNext();
	}
	// 지역 비용 자동 계산
	for( auto pArea : m_Clouds ) {
		if( XASSERT(pArea) ) {
			int lvArea = pArea->lvArea;
			if( lvArea > 1 ) {
				int costArea = XGlobalConst::sGet()->m_aryAreaCost[ lvArea ];
				if( XBREAK( aryNumArea[ lvArea ] == 0 ) ) {
					SetbLoad( FALSE );
					return false;
				}
				pArea->cost = costArea / aryNumArea[ lvArea ];
				XBREAK( pArea->cost <= 0 );
			}
		}
	}
	CONSOLE("propCloud loaded....numArea=%d", m_Clouds.size() );
	return true;
}

/**
 PropCloud의 데이타를 xml로 쓴다.
*/
#ifdef _xIN_TOOL
BOOL XPropCloud::Save( LPCTSTR szXml )
{
#ifdef _XSINGLE
	return TRUE;
#endif
	// 자동백업
	_tstring strSrc = XE::MakePackageFullPath( DIR_PROP, szXml );
	TCHAR szSystemPath[ 4096 ];
	XE::MakeHighPath( szSystemPath, XE::MakePackageFullPath( _T( "" ), _T( "" ) ) );
	_tstring strDstPath = szSystemPath;
	strDstPath += _T( "backup/" );
	_tstring strDst = strDstPath;
	int h, m, s;
	XSYSTEM::GetHourMinSec( &h, &m, &s );
	strDst += XE::Format( _T( "propCloud_%02d%02d%02d.xml" ), h, m, s );
	XSYSTEM::MakeDir( strDstPath.c_str() );
	XSYSTEM::CopyFileX( strSrc.c_str(), strDst.c_str() );
	//
	_tstring strPath = XE::MakePackageFullPath( DIR_PROP, szXml);
	XE::xtERROR err = XSYSTEM::IsReadOnly( strPath.c_str() );
	if( err == XE::ERR_READONLY || err == XE::ERR_PERMISSION_DENIED ) {
		XALERT("파일을 check out하십시오.\n%s", XE::GetFileName(strPath.c_str()) );
		return FALSE;
	}
	XXMLDoc xml;
	xml.SetDeclaration();
	XEXmlNode nodeRoot = xml.AddNode("clouds");


	for( auto pCloud : m_Clouds ) {
		XEXmlNode nodeCloud = nodeRoot.AddNode( "cloud" );
		nodeCloud.AddAttribute( "id", (int)pCloud->idCloud );
		nodeCloud.AddAttribute( "ids", pCloud->strIdentifier.c_str() );
//		nodeCloud.AddAttribute( "idxImg", (int)pCloud->idxCloud );
// 		nodeCloud.AddAttribute( "x", (int)pCloud->vwPos.x );
// 		nodeCloud.AddAttribute( "y", (int)pCloud->vwPos.y );
		nodeCloud.AddAttribute( "level", (int)pCloud->lvArea );
		if( pCloud->lvOpenable2 > 0 )
			nodeCloud.AddAttribute( "lv_open", (int)pCloud->lvOpenable2 );
		nodeCloud.AddAttribute( "cost", (int)pCloud->cost );
		if( pCloud->vAdjust.x != 0 )
			nodeCloud.AddAttribute( "cost_pos_x", (int)pCloud->vAdjust.x );
		if( pCloud->vAdjust.y != 0 )
			nodeCloud.AddAttribute( "cost_pos_y", (int)pCloud->vAdjust.y );
		if( pCloud->idsPrecedeArea.empty() )
			nodeCloud.AddAttribute( "ids_precede", "none" );
		else
			nodeCloud.AddAttribute( "ids_precede", pCloud->idsPrecedeArea.c_str() );
		nodeCloud.AddAttribute( "ids_quest", pCloud->idsQuest.c_str() );
		if( pCloud->idName )
			nodeCloud.AddAttribute( "name", (int)pCloud->idName );
		nodeCloud.AddAttribute( "key_item", pCloud->idsItem.c_str() );
		if( pCloud->unitUnlock ) {
			const std::string strEnum = XGAME::GetstrEnumUnit( pCloud->unitUnlock );
			if( !strEnum.empty() )
				nodeCloud.AddAttribute( "unlock", strEnum );
		}
		//
		{
			XArrayLinearN<ID, 512> ary;
			pCloud->GetSpotsToAry( ary );
			XARRAYLINEARN_LOOP( ary, ID, idSpot ) {
				XEXmlNode nodeSpot = nodeCloud.AddNode( "spot" );
				nodeSpot.AddAttribute( "id", (int)idSpot );
			} END_LOOP;
		}
		// 헥사타일리스를 저장한다.
		{
			_tstring str;//, strOpened;
			XArrayLinearN<xHexa, 256> ary;
			pCloud->GetIdxsToAry( ary );
			XARRAYLINEARN_LOOP( ary, xHexa, hexa ) {
				str += XE::Format(_T("%d "), hexa.idx);
// 				if( hexa.bOpened )
// 					strOpened += XE::Format(_T("%d "), hexa.idx);
			} END_LOOP;
			if( str.empty() == false )
				nodeCloud.AddAttribute( "idx", str.c_str() );
// 			if( strOpened.empty() == false )
// 				nodeCloud.AddAttribute( "idx_opened", strOpened.c_str() );
		}

	}
	CONSOLE("save propCloud");
	//
	return xml.Save( XE::MakePackageFullPath( DIR_PROP, szXml ) );
}
#endif

void XPropCloud::AddCloud( XPropCloud::xCloud *pProp )
{
	m_Clouds.Add( pProp );
	if( XASSERT( !pProp->strIdentifier.empty() ) ) {
		if( XASSERT( GetpProp( pProp->strIdentifier ) == nullptr ) )
			m_mapCloudsIds[ pProp->strIdentifier ] = pProp;
	}
	if( XASSERT( GetpProp( pProp->idCloud ) == nullptr ) )
		m_mapCloudsID[ pProp->idCloud ] = pProp;
}

XPropCloud::xCloud* XPropCloud::GetpProp( ID idCloud )
{
	if( idCloud == 0 )
		return nullptr;
	auto itor = m_mapCloudsID.find( idCloud );
	if( itor == m_mapCloudsID.end() )
		return nullptr;
	auto pCloud = itor->second;
	XBREAK( pCloud == nullptr );
	return pCloud;
// 	for( auto pCloud : m_Clouds ) {
// 		if( pCloud->idCloud == idCloud )
// 			return pCloud;
// 	}
// 	return nullptr;
}

XPropCloud::xCloud* XPropCloud::GetpProp( const _tstring& strIdentifier )
{
	auto itor = m_mapCloudsIds.find( strIdentifier );
	if( itor == m_mapCloudsIds.end() )
		return nullptr;
	XPropCloud::xCloud *pCloud = itor->second;
	XBREAK( pCloud == nullptr );
	return pCloud;
}

/**
 @brief 월드좌표 x, y를 헥사타일 인덱스로 바꿔준다.
*/
int XPropCloud::GetIdxHexaFromWorld( float wx, float wy )
{
	const XE::VEC2 sizeHexa = XGAME::x_sizeHexa;
	XE::VEC2 sizeWorld = XWorld::sGetSize();
	// 월드를 타일단위로 쪼갯을때 가로세로 개수.
	int nw = (int)( sizeWorld.w / sizeHexa.w );		
	int nh = (int)( sizeWorld.h / sizeHexa.h ) + 1;
	int yofs = (int)(wy / sizeHexa.h);
	int xofs = (int)(wx / sizeHexa.w);
	if( yofs & 1 )
		xofs = (int)((wx + (sizeHexa.w / 2.f)) / sizeHexa.w);
	int idx = yofs * (nw+1) + xofs;
	return idx;
}

/**
 @brief 헥사인덱스 idx의 왼쪽첫번째 점 월드좌표를 구한다.
*/
XE::VEC2 XPropCloud::GetPosFromIdxHexa( int idx )
{
	const XE::VEC2 sizeHexa = XGAME::x_sizeHexa;
	XE::VEC2 sizeWorld = XWorld::sGetSize();
	// 월드를 타일단위로 쪼갯을때 가로세로 개수.
	int nw = (int)( sizeWorld.w / sizeHexa.w );
	int nh = (int)( sizeWorld.h / sizeHexa.h ) + 1;
	int idxx = idx % ( nw + 1 );
	int idxy = idx / ( nw + 1 );
	XE::VEC2 v;
	v.x = idxx * sizeHexa.w;
	v.y = idxy * sizeHexa.h;
	if( idxy & 1 )
		v.x -= sizeHexa.w / 2.f;
	return v;
}

/**
 @brief 헥사타일 인덱스를 받아서 그 타일이 속해있는 구름을 찾는다.
*/
ID XPropCloud::GetCloudByidxHexa( int idxHexa )
{
	for( auto pCloud : m_Clouds ) {
		if( pCloud->IsHaveIdx( idxHexa ) )
			return pCloud->idCloud;
	}
	return 0;
}

/**
 @brief  헥사타일이 포함된 구름그룹의 프로퍼티를 얻는다.
*/
XPropCloud::xCloud* XPropCloud::GetpPropByidxHexa( int idxHexa )
{
	for( auto pCloud : m_Clouds ) {
		if( pCloud->IsHaveIdx( idxHexa ) )
			return pCloud;
	}
	return nullptr;
}

void XPropCloud::DestroyCloud( ID idCloud )
{
#ifdef _xIN_TOOL
	xCloud *pProp = GetpProp( idCloud );
	if( pProp == nullptr )
		return;
	m_Clouds.DelByID( idCloud );
	SAFE_DELETE( pProp );
	//	
	if( !pProp->strIdentifier.empty() ) {
		auto itor = m_mapCloudsIds.find( pProp->strIdentifier );
		if( itor != m_mapCloudsIds.end() ) {
			m_mapCloudsIds.erase( itor++ );
		}
	}
	if( pProp->idCloud ) {
		auto itor = m_mapCloudsID.find( pProp->idCloud );
		if( itor != m_mapCloudsID.end() ) {
			m_mapCloudsID.erase( itor++ );
		}
	}
#else
	XBREAK(1);		// 툴모드에서만 쓸수 있는 기능임. 게임에서는 삭제할일이 없음.
#endif
}

void XPropCloud::DoReArrangeCloudRandom( void )
{
#ifdef _xIN_TOOL
	for( auto pCloud : m_Clouds ) {
		for( auto& hexa : pCloud->idxs ) {
			hexa.idxImg = random(MAX_IMG);
		}
	}
#endif
}

/**
 @brief idSpot을 품고있는 지역 아이디를 리턴한다.
*/
ID XPropCloud::GetidAreaHaveSpot( ID idSpot )
{
	for( auto pCloud : m_Clouds ) {
		if( pCloud->IsHaveSpot( idSpot ) )
			return pCloud->idCloud;
	}
	return 0;
}

/**
 @brief idSpot이 속한 지역의 프로퍼티를 꺼내준다.
*/
XPropCloud::xCloud* XPropCloud::GetpAreaHaveSpot( ID idSpot )
{
	for( auto pCloud : m_Clouds ) {
		if( pCloud->IsHaveSpot( idSpot ) )
			return pCloud;
	}
	return 0;
}

/**
 @brief idCloud지역의 중심좌표
*/
XE::VEC2 XPropCloud::GetPosByArea( ID idCloud )
{
	auto pPropArea = GetpProp( idCloud );
	if( XBREAK(pPropArea == nullptr) )
		return XE::VEC2(0);
	return GetPosByArea( pPropArea );
}
XE::VEC2 XPropCloud::GetPosByArea( xCloud *pProp )
{
	XArrayLinearN<XPropCloud::xHexa, 256> ary;
	pProp->GetIdxsToAry( ary );
	XE::VEC2 vSum;
	XARRAYLINEARN_LOOP_AUTO( ary, hexa )
	{
		XE::VEC2 vwc = GetCenterFromIdxHexa( hexa.idx );
		vSum += vwc;
	} END_LOOP;

	if( ary.size() == 0 )
		return XE::VEC2( 0 );
	XE::VEC2 vAvg = vSum / (float)ary.size();
	return vAvg + pProp->vAdjust;
}
/**
 @brief 모든 지역(구름) 프로퍼티를 어레이에 담아준다.
*/
int XPropCloud::GetPropToAry( XArrayLinearN<xCloud*, 512> *pOutAry ) 
{
	XBREAK( pOutAry == nullptr );
	for( auto pCloud : m_Clouds ) {
		pOutAry->Add( pCloud );

	}
	return pOutAry->size();
}
/**
 @brief 모든 지역(구름)의 지역ID를 어레이에 담아준다.
*/
int XPropCloud::GetPropToAry( XArrayLinearN<ID, 512> *pOutAry ) 
{
	XBREAK( pOutAry == nullptr );
	for( auto pCloud : m_Clouds ) {
		pOutAry->Add( pCloud->idCloud );

	}
	return pOutAry->size();
}
/**
 @brief 모든 지역(구름) 프로퍼티를 어레이에 담아준다.
*/
int XPropCloud::GetPropToAry( XVector<xCloud*> *pOutAry ) 
{
	XBREAK( pOutAry == nullptr );
	for( auto pCloud : m_Clouds ) {
		pOutAry->push_back( pCloud );

	}
	return pOutAry->size();
}
/**
 @brief 모든 지역(구름)의 지역ID를 어레이에 담아준다.
*/
int XPropCloud::GetPropToAry( XVector<ID> *pOutAry ) 
{
	XBREAK( pOutAry == nullptr );
	for( auto pCloud : m_Clouds ) {
		pOutAry->push_back( pCloud->idCloud );
	}
	return pOutAry->size();
}
/**
 @brief 사각영역내에 중심좌표가 속하는 모든 지역을 얻는다.
*/
int XPropCloud::GetAreaAryByRect( float wx, float wy, float ww, float wh, std::vector<ID> *pOutAry )
{
	XE::VEC2 vLT(wx, wy);
	XE::VEC2 vSize(ww, wh);
	for( auto pProp : m_Clouds ) {
		auto vCenter = GetPosByArea( pProp );
		vCenter += pProp->vAdjust;
		if( XE::IsArea( vLT, vSize, vCenter ) ) {
			pOutAry->push_back( pProp->idCloud );
		}
	}
	return pOutAry->size();
}
/**
 @brief pProp의 ids가 맵에 안등록되어있던거라면 새로 등록시킨다.
*/
void XPropCloud::UpdateAreaIds( xCloud *pProp )
{
	auto pExist = GetpProp( pProp->strIdentifier );
	if( pExist == nullptr ) {
		m_mapCloudsIds[ pProp->strIdentifier ] = pProp;
	}

}
/**
 @brief idsArea를 선행오픈지역으로 갖는 지역을 얻는다.
*/
XPropCloud::xCloud* XPropCloud::GetAreaByPrecedeArea( LPCTSTR idsArea )
{
	for( auto pArea : m_Clouds ) {
		if( pArea->idsPrecedeArea == idsArea )
			return pArea;
	}
	return nullptr;
}
/**
 @brief 뚫린영역의 스팟인가.
*/
bool XPropCloud::IsOpenedSpot( ID idSpot, ID idArea, const XE::VEC2& vwPos )
{
	if( XBREAK(idSpot == 0) )
		return false;
	int idxHexa = GetIdxHexaFromWorld( vwPos );
	xCloud *pPropArea = GetpProp( idArea );
	if( XASSERT(pPropArea) ) {
		return pPropArea->IsHaveIdxOpened(idxHexa);
	}
	return false;
}

void XPropCloud::xHexa::Serialize( XArchive& ar ) const {
	XBREAK( idxImg > 0x7f );
	ar << idx;
	ar << (BYTE)idxImg;
	ar << (BYTE)xboolToByte(bOpened);
	ar << (WORD)0;
}
void XPropCloud::xHexa::DeSerialize( XArchive& ar, int ) {
	BYTE b0;
	ar >> idx;
	ar >> b0;		idxImg = b0;
	ar >> b0;		bOpened = xbyteToBool(b0);
	ar >> b0 >> b0;
}

void XPropCloud::xCloud::Serialize( XArchive& ar ) const {
	ar << strIdentifier << idCloud ;
	ar << lvArea << lvOpenable2 << cost;
	ar << vAdjust << idName;
	ar << idsPrecedeArea << idsQuest << idsItem;
	ar << (int)unitUnlock;
	ar << spots << idxs;
}

void XPropCloud::xCloud::DeSerialize( XArchive& ar, int ) {
	int i0;
	ar >> strIdentifier >> idCloud;
	ar >> lvArea >> lvOpenable2 >> cost;
	ar >> vAdjust >> idName;
	ar >> idsPrecedeArea >> idsQuest >> idsItem;
	ar >> i0;		unitUnlock = (xtUnit)i0;
	ar >> spots >> idxs;
}


void XPropCloud::Serialize( XArchive& ar ) const
{
	ar << VER_PROP_CLOUD;
	ar << (int)m_Clouds.size();
	for( auto pProp : m_Clouds ) {
		pProp->Serialize( ar );
	}
}
void XPropCloud::DeSerialize( XArchive& ar, int )
{
	int ver, num;
	ar >> ver >> num;
	for( int i = 0; i < num; ++i ) {
		auto pProp = new xCloud();
		pProp->DeSerialize( ar, ver );
		AddCloud( pProp );
	}
}

