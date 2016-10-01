#include "stdafx.h"
#include "XPropHelp.h"
#include "XPropSeq.h"
#include "XPropOrder.h"
#include "XPropWorld.h"
#include "XResObj.h"
#include "XPropHelp.h"
#include "XSystem.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


XE_NAMESPACE_START( xHelp )

std::shared_ptr<XPropHelp> XPropHelp::s_spInstance;
bool XPropHelp::s_bTextAdded = false;
std::shared_ptr<XPropHelp>& XPropHelp::sGet() 
{
	if( s_spInstance == nullptr )
		s_spInstance = std::shared_ptr<XPropHelp>( new XPropHelp );
	return s_spInstance;
}
////////////////////////////////////////////////////////////////
XPropHelp::XPropHelp()
{
	Init();
}
void XPropHelp::Destroy()
{
	DestroyAll();
}

void XPropHelp::DestroyAll()
{
	m_mapSeq.clear();
//	XMAP_DESTROY( m_mapSeq );
	XXMLDoc::Clear();
}

bool XPropHelp::Load( LPCTSTR szXml )
{
	return XXMLDoc::Load( XE::MakePath( DIR_PROP, szXml ) );
}

// bool XPropHelp::Load( LPCTSTR szXml )
// {
// 	auto bRet = XXMLDoc::Load( XE::MakePath( DIR_PROP, szXml ) );
// 	//
// 	XEXmlNode nodeRoot = FindNode( "root" );
// 	if( nodeRoot.IsEmpty() )
// 		return bRet;
// 	///< 
// 	XEXmlNode nodeSeq = nodeRoot.GetFirst();
// 	if( XBREAK( nodeSeq.IsEmpty() ) )
// 		return bRet;
// 	while( !nodeSeq.IsEmpty() )
// 	{
// 		auto spNewProp = LoadSeq( nodeSeq );
// 		m_mapSeq[ spNewProp->m_strIdentifier ] = spNewProp;
// #ifdef _xIN_TOOL
// 		m_listSeq.Add( spNewProp );
// #endif // _xIN_TOOL
// 		// 다음 캠페인 노드로
// 		nodeSeq = nodeSeq.GetNext();
// 	}
// 	if( m_bError ) {
// 		XALERT( "%s:헬프 프로퍼티 읽던 중 에러. 콘솔창을 참고하시오.", GetstrFilename().c_str() );
// 	}
// 	return bRet;
// }
bool XPropHelp::OnDidFinishLoad()
{
	XEXmlNode nodeRoot = FindNode( "root" );
	if( nodeRoot.IsEmpty() )
		return false;
	///< 
	XEXmlNode nodeSeq = nodeRoot.GetFirst();
	if( XBREAK( nodeSeq.IsEmpty() ) )
		return false;
	while( !nodeSeq.IsEmpty() )
	{
		auto spNewProp = LoadSeq( nodeSeq );
		m_mapSeq[ spNewProp->m_strIdentifier ] = spNewProp;
#ifdef _xIN_TOOL
		m_listSeq.Add( spNewProp );
#endif // _xIN_TOOL
		// 다음 캠페인 노드로
		nodeSeq = nodeSeq.GetNext();
	}
	if( m_bError ) {
		XALERT( "%s:헬프 프로퍼티 읽던 중 에러. 콘솔창을 참고하시오.", GetstrFilename().c_str() );
		return false;
	}
	return true;
}
PropSeqPtr XPropHelp::GetspProp( const std::string& strIdentifier )
{
	auto itor = m_mapSeq.find( strIdentifier );
	if( itor == m_mapSeq.end() ) {
		// 없으면 해당 시퀀스만 lazy로딩한다.
		auto spPropSeq = LoadSeq2( strIdentifier );
		if( spPropSeq ) {
			return spPropSeq;
		}
		return nullptr;
	}
	PropSeqPtr spProp = itor->second;
	XBREAK( spProp == nullptr );
	return spProp;
}

/**
 @brief idsSeq만 읽어서 메모리에 올린다.
*/
PropSeqPtr XPropHelp::LoadSeq2( const std::string& idsSeq )
{
	if( !IsLoaded() ) {
		if( XXMLDoc::Load( XE::MakePath( DIR_PROP, _T( "propHelp.xml" ) ) ) == FALSE )
			return nullptr;
	}
	XEXmlNode nodeRoot = FindNode( "root" );
	if( nodeRoot.IsEmpty() )
		return nullptr;
	///< 
	auto nodeSeq = nodeRoot.FindNode( idsSeq.c_str() );
	if( !nodeSeq.IsEmpty() ) {
		auto spNewProp = LoadSeq( nodeSeq );
		if( spNewProp ) {
			m_mapSeq[ spNewProp->m_strIdentifier ] = spNewProp;
#ifdef _xIN_TOOL
			m_listSeq.Add( spNewProp );
#endif // _xIN_TOOL
			return spNewProp;
		}
		if( m_bError ) {
			XALERT( "%s:헬프 프로퍼티 읽던 중 에러. 콘솔창을 참고하시오.", GetstrFilename().c_str() );
		}
	}
	return nullptr;
}

PropSeqPtr XPropHelp::LoadSeq( XEXmlNode& node )
{
	PropSeqPtr spProp = PropSeqPtr( new XPropSeq );
	spProp->m_strIdentifier = node.GetcstrName();
	_tstring tstrIdentifier = C2SZ(spProp->m_strIdentifier);
	m_idsSeqLast = C2SZ(spProp->m_strIdentifier.c_str());
//	spProp->m_strCondition = node.GetString( "condition" );		// 현재 하드코딩용으로 사용
	spProp->m_idsPrev = node.GetString( "prev_seq" );
	spProp->m_idsCompletedSeq = node.GetString( "complete_seq" );
	spProp->m_idsFinishededQuest = node.GetString( "finish_quest" );
	spProp->m_vStopPos.x = node.GetFloat("stop_pos_x");
	spProp->m_vStopPos.y = node.GetFloat( "stop_pos_y" );
	if( node.GetInt("debug") ) {
		int a= 0;
	}
	_tstring strToken = node.GetTString( "event" );
	if( !strToken.empty() ) {
		auto seqEvent = ( XGAME::xtActionEvent )CONSTANT->GetValue( strToken );
		if( CONSTANT->IsFound() )
			spProp->m_SeqEvent = seqEvent;
	}
	if( !spProp->m_SeqEvent ) {
		CONSOLE("%s:event가 지정되지 않음.", tstrIdentifier.c_str() );
		m_bError = true;
	}
	strToken = node.GetTString( "event_param" );
	if( !strToken.empty() ) {
#ifdef _xIN_TOOL
		spProp->m_strOrigEventParam = strToken;	// 저장을 위해서 문자열도 받아둠.
#endif // _xIN_TOOL
		DWORD dwParam = CONSTANT->GetValue( strToken );
		if( CONSTANT->IsFound() )
			spProp->m_dwEventParam = dwParam;
		else {
			// 숫자로 변환해보고
			DWORD dwNum = _tcstoul( strToken.c_str(), nullptr, 10 );
			if( dwNum )
				spProp->m_dwEventParam = dwNum;
			else
				// 숫자로 변환이 안되면 문자열파라메터로 저장
				spProp->m_strEventParam = SZ2C(strToken);
		}
	}
	strToken = node.GetTString( "param_spot" );
	if( !strToken.empty() ) {
		CONSOLE( "%s:아직 지원하지 않음[param_spot]", tstrIdentifier.c_str() );
		m_bError = true;
	}
	strToken = node.GetTString( "event_param2" );
	if( !strToken.empty() ) {
#ifdef _xIN_TOOL
		spProp->m_strOrigEventParam2 = strToken;	// 저장을 위해서 문자열도 받아둠.
#endif // _xIN_TOOL
		DWORD dwParam = CONSTANT->GetValue( strToken );
		if( CONSTANT->IsFound() )
			spProp->m_dwEventParam2 = dwParam;
		else {
			// 숫자로 변환해보고
			DWORD dwNum = _tcstoul( strToken.c_str(), nullptr, 10 );
			if( dwNum )
				spProp->m_dwEventParam2 = dwNum;
			else
				// 숫자로 변환이 안되면 문자열파라메터로 저장
				spProp->m_strEventParam2 = SZ2C(strToken);
		}
	} else {
		strToken = node.GetTString( "param_spot2" );
		if( !strToken.empty() ) {
	#ifdef _xIN_TOOL
			spProp->m_strOrigEventParam2 = strToken;	// 저장을 위해서 문자열도 받아둠.
	#endif // _xIN_TOOL
			auto pPropSpot = PROP_WORLD->GetpProp( strToken );
			if( XASSERT(pPropSpot) )
				spProp->m_dwEventParam2 = pPropSpot->idSpot;
		}
	}
	if( spProp->IsNotHaveParam() ) {
		CONSOLE( "%s:이벤트 파라메터가 없음.", tstrIdentifier.c_str() );
		m_bError = true;
	}
	auto nodeOrder = node.GetFirst();
	while( !nodeOrder.IsEmpty() ) {
		std::string strOrder = nodeOrder.GetcstrName();
		PropOrderPtr spBaseOrder;
		if( strOrder == "dialog" ) {
			spBaseOrder = LoadOrderDialog( nodeOrder );
		} else
		if( strOrder == "indicate" ) {
			spBaseOrder = LoadOrderIndicate( nodeOrder );
		} else
		if( strOrder == "camera" ) {
			spBaseOrder = LoadOrderCamera( nodeOrder );
		} else
		if( strOrder == "touch" ) {
			spBaseOrder = LoadOrderTouch( nodeOrder );
		} else
		if( strOrder == "ready_touch" ) {
			XBREAK(1);	// 사용안함.
// 			spBaseOrder = LoadOrderReadyTouch( nodeOrder );
		} else
		if( strOrder == "delay" ) {
			spBaseOrder = LoadOrderDelay( nodeOrder );
		} else
		if( strOrder == "mode" ) {
			spBaseOrder = LoadOrderMode( nodeOrder );
		} else
			XBREAK(1);
		if( spBaseOrder ) {
#ifdef _xIN_TOOL
			spBaseOrder->m_strXmlIds = strOrder;
#endif // _xIN_TOOL
			spProp->AddOrder( spBaseOrder );
			if( spBaseOrder->m_typeEnd == xFIN_END_ORDER ) {
				if( spBaseOrder->m_idsEnd.empty() ) {
					CONSOLE( "%s:ids_end가 지정되지 않았다.", m_idsSeqLast.c_str() );
					m_bError = true;
				}
			}
			if( spBaseOrder->m_typeEnd == xFIN_PUSH_TARGET ) {
//				if( spBaseOrder->m_idsTarget.empty() ) {
				if( spBaseOrder->m_aryIdsTarget.size() == 0 ) {
					CONSOLE( "%s:ids_target이 지정되지 않았다.", m_idsSeqLast.c_str() );
					m_bError = true;
				}
			}
		}
		//
		nodeOrder = nodeOrder.GetNext();
	}
	return spProp;
}

/**
 @brief order노드의 공통속성부 읽음.
*/
void XPropHelp::LoadOrderCommon( XEXmlNode& node, XPropOrder *pOut )
{
	pOut->m_strIdentifer = node.GetString( "ids" );
	pOut->m_idsEnd = node.GetString( "ids_end" );
//	pOut->m_idsTarget = node.GetString( "ids_target" );
	node.GetStringAry( "ids_target", &pOut->m_aryIdsTarget );
	pOut->m_typeEnd = xFIN_NONE;
	std::string strAttr;
	// end_order를 직접 입력하는 버전
	strAttr = node.GetString( "end_order" );
	std::string strEnd = node.GetString( "end" );
	if( strAttr.empty() ) {
		if( strEnd == "touch" )
			pOut->m_typeEnd = xFIN_TOUCH;
		else 
	// 	if( strEnd == "push_butt" ) {
	// 		pOut->m_typeEnd = xFIN_PUSH_BUTT;
	// 	} else 
		if( strEnd == "push_target" ) {
			pOut->m_typeEnd = xFIN_PUSH_TARGET;
		} else
		if( strEnd == "end_order" ) {
			pOut->m_typeEnd = xFIN_END_ORDER;
		} else
		if( strEnd == "move_camera" ) {
			pOut->m_typeEnd = xFIN_MOVE_CAMERA;
		} else
		if( strEnd == "draged_slot" ) {
			pOut->m_typeEnd = xFIN_ORDER_COMPLETE;
			pOut->m_idsEnd = strEnd;		// custom 조건
		} else
		if( strEnd.empty() ) {
			pOut->m_typeEnd = xFIN_NONE;
		} else {
			m_bError = true;
			CONSOLE("%s:알수없는 end명령=%s", m_idsSeqLast.c_str(), C2SZ(strEnd.c_str()) );
		}
#ifdef _xIN_TOOL
		pOut->m_strXmlTypeEnd = strEnd;
#endif // _xIN_TOOL
	} else {
		pOut->m_typeEnd = xFIN_END_ORDER;
		pOut->m_idsEnd = strAttr;
	}
	// ids_end가 지정되어 있을때
	if( !pOut->m_idsEnd.empty() ) {
		if( pOut->m_typeEnd != xFIN_END_ORDER 
			&& pOut->m_typeEnd != xFIN_ORDER_COMPLETE ) {
			_tstring idsEnd = C2SZ(pOut->m_idsEnd.c_str());
			_tstring cstrEnd = C2SZ(strEnd.c_str());
			CONSOLE( "%s:ids_end(%s)가 지정되어있는데 end조건(%s)이 잘못됨", 
					m_idsSeqLast.c_str(), idsEnd.c_str(), cstrEnd.c_str() );
			m_bError = true;

		}
	}


}
/**
 @brief order노드를 dialog타입으로 읽는다.
*/
PropOrderPtr XPropHelp::LoadOrderDialog( XEXmlNode& node )
{
	auto pProp = new XPropDialog;
	LoadOrderCommon( node, pProp );
	if( pProp->m_typeEnd == xFIN_NONE )
		pProp->m_typeEnd = xFIN_TOUCH;		// 디폴트값
	pProp->m_idText = (ID)node.GetInt( "id_text" );
	if( pProp->m_idText == 0 )
		pProp->SetstrText( node.GetTString( "text" ) );
	if( pProp->m_typeEnd == xFIN_NONE )
		pProp->m_typeEnd = xFIN_TOUCH;	// 지정이 되지 않을경우 대화는 디폴트가 터치다.
	pProp->m_strTalker = node.GetTString( "talker" );
	float x = (float)node.GetInt( "x" );
	if( !node.IsNotFound() )
		pProp->m_vPos.x = x;
	float y = (float)node.GetInt( "y" );
	if( !node.IsNotFound() )
		pProp->m_vPos.y = y;
	pProp->m_bSavePos = node.GetBool( "save_pos" );
	// dialog는 인디게이터를 직접 가질수 있다.
	auto nodeChild = node.GetFirst();
	while( !nodeChild.IsEmpty() ) {
		if( nodeChild.GetcstrName() == "indicate" ) {
			XBREAK(1);	// 아직 지원안함.
			auto spOrder = LoadOrderIndicate( node );
			if( spOrder ) {
				auto spOrderIndicate = std::static_pointer_cast<XPropIndicate>( spOrder );
				pProp->m_aryIndicate.push_back( spOrderIndicate );
			}
		}
		//
		nodeChild = nodeChild.GetNext();
	}
	return PropOrderPtr(pProp);
}
PropOrderPtr XPropHelp::LoadOrderIndicate( XEXmlNode& node )
{
	auto pProp = new XPropIndicate;
	LoadOrderCommon( node, pProp );
	pProp->m_vPos.x = (float)node.GetInt( "x" );
	pProp->m_vPos.y = (float)node.GetInt( "y" );
	std::string strDir = node.GetString( "dir" );
	// 방향을 매크로값으로 먼저 읽어보고 맞는게 없으면 숫자로 다시 읽어 입력
	if( strDir == "none" )
		pProp->m_dAng = -2.f;
	else if( strDir == "center" )
		pProp->m_dAng = -1.f;
	else if( strDir == "right" )
		pProp->m_dAng = 0;
	else if( strDir == "left" )
		pProp->m_dAng = 180.f;
	else if( strDir == "up" )
		pProp->m_dAng = 270.f;
	else if( strDir == "down" )
		pProp->m_dAng = 90.f;
	else
		pProp->m_dAng = (float)node.GetInt( "dir" );
	pProp->m_idsRoot = node.GetString( "root" );
	pProp->m_strSpr = node.GetTString( "spr" );
	pProp->m_idAct = node.GetInt( "id_act" );
	pProp->m_secLife = node.GetFloat( "delay" );
	if( pProp->m_secLife > 0 )
		pProp->m_typeEnd = xFIN_DELAY;
// 	if( pProp->m_idAct == 0 )	// id_act가 생략된걸 저장하기 위해서 지정하지 않으면 0값을 갖고 있도록함.
// 		pProp->m_idAct = 1;
	node.GetStringAry( "ids_allow", &pProp->m_aryIdsAllow );
	return PropOrderPtr( pProp );
}

PropOrderPtr XPropHelp::LoadOrderCamera( XEXmlNode& node )
{
	auto pProp = new XPropCamera;
	LoadOrderCommon( node, pProp );
	if( pProp->m_typeEnd == xFIN_NONE )
		pProp->m_typeEnd = xFIN_MOVE_CAMERA;
	pProp->m_vwDst.x = (float)node.GetInt( "x" );
	pProp->m_vwDst.y = (float)node.GetInt( "y" );
	pProp->m_bPushPos = node.GetBool( "push_pos" );
	pProp->m_bPopPos = node.GetBool( "pop_pos" );
//	pProp->m_idsTarget = node.GetString( "ids_target" );
	return PropOrderPtr( pProp );
}

PropOrderPtr XPropHelp::LoadOrderTouch( XEXmlNode& node )
{
	auto pProp = new XPropTouch;
	LoadOrderCommon( node, pProp );
	if( pProp->m_typeEnd == xFIN_NONE )
		pProp->m_typeEnd = xFIN_ORDER_COMPLETE;
	pProp->m_bFlag = node.GetBool( "flag" );
//	pProp->m_idsAllow = node.GetString( "ids_allow" );
	return PropOrderPtr( pProp );
}
// PropOrderPtr XPropHelp::LoadOrderReadyTouch( XEXmlNode& node )
// {
// 	auto pProp = new XPropReadyTouch;
// 	LoadOrderCommon( node, pProp );
// //	pProp->m_idsTarget = node.GetString( "ids_target" );
// 	return PropOrderPtr( pProp );
// }
PropOrderPtr XPropHelp::LoadOrderDelay( XEXmlNode& node )
{
	auto pProp = new XPropDelay;
	LoadOrderCommon( node, pProp );
	if( pProp->m_typeEnd == xFIN_NONE )
		pProp->m_typeEnd = xFIN_DELAY;
	pProp->m_secDelay = node.GetFloat( "sec" );
	return PropOrderPtr(pProp);
}
PropOrderPtr XPropHelp::LoadOrderMode( XEXmlNode& node )
{
	auto pProp = new XPropMode;
	LoadOrderCommon( node, pProp );
	if( pProp->m_typeEnd == xFIN_NONE )
		pProp->m_typeEnd = xFIN_ORDER_COMPLETE;	
	pProp->m_strMode = node.GetString( "mode" );
	pProp->m_bFlag = node.GetBool( "flag" );
	return PropOrderPtr( pProp );
}
/**
 @brief 모든 시퀀스를 검사해서 조건에 맞는 시퀀스를 얻는다.
 idsSeqLast로부터 발생하는가
 typeScene에 발생하는가
 seqEvent도 일치하는가.
*/
// PropSeqPtr XPropHelp::FindSeqByCond( const std::string& idsSeqLast, 
// 									XGAME::xtActionEvent actEvent,
// 									DWORD dwParam )
// {
// 	if( idsSeqLast.empty() )
// 		return PropSeqPtr();
// 	for( auto itor : m_mapSeq ) {
// 		PropSeqPtr& spPropSeq = itor.second;
// 		if( spPropSeq && !idsSeqLast.empty() ) {
// 			if( spPropSeq->m_SeqEvent == actEvent )
// 				if( spPropSeq->m_idsPrev == idsSeqLast || spPropSeq->m_idsPrev.empty() )
// 					if( spPropSeq->m_dwEventParam == dwParam || spPropSeq->m_dwEventParam == 0 )
// 						return spPropSeq;
// 		}
// 	}
// 	return PropSeqPtr();
// }
// PropSeqPtr XPropHelp::FindSeqByCond( const std::string& idsSeqLast, 
// 									XGAME::xtActionEvent actEvent,
// 									const std::string& strParam )
// {
// 	if( idsSeqLast.empty() )
// 		return PropSeqPtr();
// 	for( auto itor : m_mapSeq ) {
// 		PropSeqPtr& spPropSeq = itor.second;
// 		if( spPropSeq && !idsSeqLast.empty() ) {
// 			if( spPropSeq->m_SeqEvent == actEvent )
// 				if( spPropSeq->m_idsPrev == idsSeqLast || spPropSeq->m_idsPrev.empty() )
// 					if( spPropSeq->m_strEventParam == strParam || spPropSeq->m_strEventParam.empty() )
// 						return spPropSeq;
// 		}
// 	}
// 	return PropSeqPtr();
// }
PropSeqPtr XPropHelp::FindSeqByCond( const std::string& idsSeqLast, 
									XGAME::xtActionEvent actEvent,
									DWORD dwParam,
									const std::string& strParam,
									DWORD dwParam2 )
{
//   if( idsSeqLast.empty() )
// 	return PropSeqPtr();
  for( auto itor : m_mapSeq ) {
	PropSeqPtr& _sp = itor.second;
	XPropSeq *sp = _sp.get();
	if( sp /*&& !idsSeqLast.empty()*/ ) {
	  if( sp->m_SeqEvent == actEvent )
		if( ( !idsSeqLast.empty() && sp->m_idsPrev == idsSeqLast) || sp->m_idsPrev.empty() )
		  if( ( dwParam != DEFAULT && sp->m_dwEventParam == dwParam ) || sp->m_dwEventParam == 0 )
    		if( ( dwParam2 != DEFAULT && sp->m_dwEventParam2 == dwParam2 ) || sp->m_dwEventParam2 == 0 )
			  if( ( !sp->m_strEventParam.empty() && sp->m_strEventParam == strParam) || sp->m_strEventParam.empty() )
				return _sp;
					
    }
  }
  return PropSeqPtr();
}

bool XPropHelp::Save( LPCTSTR szXml )
{
#ifdef _xIN_TOOL
	// 자동 백업.
	_tstring strSrc = XE::MakePackageFullPath( DIR_PROP, szXml );
	TCHAR szSystemPath[ 4096 ];
	XE::MakeHighPath( szSystemPath, XE::MakePackageFullPath( _T( "" ), _T( "" ) ) );
	_tstring strDstPath = szSystemPath;
	strDstPath += _T( "backup/" );
	_tstring strDst = strDstPath;
	int h, m, s;
	XSYSTEM::GetHourMinSec( &h, &m, &s );
	strDst += XE::Format( _T( "propHelp_%02d%02d%02d.xml" ), h, m, s );
	XSYSTEM::MakeDir( strDstPath.c_str() );
	XSYSTEM::CopyFileX( strSrc.c_str(), strDst.c_str() );

	_tstring strPath = XE::MakePackageFullPath( DIR_PROP, szXml );
	XE::xtERROR err = XSYSTEM::IsReadOnly( strPath.c_str() );
	if( err == XE::ERR_READONLY || err == XE::ERR_PERMISSION_DENIED ) {
		XALERT( "파일을 check out하십시오.\n%s", XE::GetFileName( strPath.c_str() ) );
		return false;
	}
	XXMLDoc xml;
	xml.SetDeclaration();
	XEXmlNode nodeRoot = xml.AddNode( "root" );
	for( auto& spProp : m_listSeq ) {
		spProp->Save( nodeRoot );
	}
	BOOL bRet = xml.Save( XE::MakePackageFullPath( DIR_PROP, szXml ) );
	if( s_bTextAdded ) {
		TEXT_TBL->Save( _T("text_ko2.txt") );
		AfxMessageBox( _T("text_ko2.txt saved....") );
		s_bTextAdded = false;
	}
	return ( bRet ) ? true : false;
#else
	return true;
#endif
}

void XPropHelp::Serialize( XArchive& ar ) const
{
	ar << VER_PROP_HELP;
	ar << (int)m_mapSeq.size();
	for( auto itor : m_mapSeq ) {
		PropSeqPtr spSeq = itor.second;
		if( XASSERT(spSeq) ) {
			spSeq->Serialize( ar );
		}
	}
}
void XPropHelp::DeSerialize( XArchive& ar, int )
{
	m_mapSeq.clear();
#ifdef _xIN_TOOL
	m_listSeq.clear();
#endif // _xIN_TOOL
	int ver, num;
	ar >> ver >> num;
	for( int i = 0; i < num; ++i ) {
		auto spSeq = std::make_shared<XPropSeq>();
		spSeq->DeSerialize( ar, ver );
		m_mapSeq[ spSeq->m_strIdentifier] = spSeq;
#ifdef _xIN_TOOL
		m_listSeq.Add( spSeq );
#endif // _xIN_TOOL
	}
}


XE_NAMESPACE_END;


