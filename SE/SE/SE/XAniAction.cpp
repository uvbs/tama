#include "stdafx.h"
#include "XAniAction.h"
#include "XLayerBase.h"
#include "XKeyAll.h"
#include "XLayerAll.h"
#include "SprDat.h"
#include "Sprite.h"
#include "MainFrm.h"
#include <algorithm>					// find함수를 사용하기 위해
//#include "FrameView.h"
//#include "AnimationView.h"
#include "XViews.h"
#include "Tool.h"
#include "ResMng.h"
#include "FrameView.h"
#include "xLayerInfo.h"

/**
 의문점:
 현재 구조는 같은 레이어의 POS채널내 키라도 패스에 속하는게 있고 아닌게 있다.
 어차피 시간순서로 진행되면 모든 키를 순차적으로 돌아야 하니 채널내 모든 키는 다 패스
 라고도 볼수 있다. 여기에 패스속성을 라인으로 하느냐 스플라인으로 하느냐의 차이일뿐.
*/
#define KEY_LOOP( I )				\
 								for( auto I = m_listKey.begin(); I != m_listKey.end(); I ++ ) 
		
#define KEY_MANUAL_LOOP( I )				\
								for( auto I = m_listKey.begin(); I != m_listKey.end(); ) 

#define KEY_LOOP2( I, E )		LIST_LOOP( m_listKey, XBaseKey*, I, E )

bool xLayerInfo::Save( XResFile& res ) const
{
	res << m_idLayer;
	XBREAK( m_bitType > 0xffff );	// 
	res << (WORD)m_bitType;
	res << (BYTE)m_nLayer;
	res << m_fLastFrame;
	res << m_vAdjAxis;
//	res << m_vAdjAxis.x;
//	res << m_vAdjAxis.y;
	return true;
}

// 구버전 레이어타입을 바뀐포맷으로 변경해준다.
namespace xSpr {
xtLayer ConvertOldType( BYTE b0 ) {
	switch( b0 ) {
	case 1: return xLT_IMAGE;
	case 2: return xLT_OBJECT;
	case 3: return xLT_SOUND;
	case 4: return xLT_EVENT;
	case 5: return xLT_DUMMY;
	default:
		XBREAKF( 1, "변환할 수 없는 레이어 타입:b0=%d", b0 );
		break;
	}
	return xLT_NONE;
}
}

bool xLayerInfo::Load( XResFile& res, int verSprDat )
{
	BYTE b0;
	WORD w0;
	res >> m_idLayer;
	XBREAK( m_idLayer == 0 );
	if( verSprDat >= 28 ) {
		res >> w0;	m_bitType = (xSpr::xtLayer)w0;
	} else {
		res >> b0;	
		m_bitType = xSpr::ConvertOldType( b0 );
		XBREAKF( m_bitType == xSpr::xLT_NONE, "파일읽던중 레이어 타입 변환 실패" );
	}
	res >> b0;	m_nLayer = b0;
	res >> m_fLastFrame;
	res >> m_vAdjAxis;
//	res >> m_vAdjAxis.x;
//	res >> m_vAdjAxis.y;
	if( verSprDat < 15 ) {
		m_vAdjAxis *= 0.5f;
	}
	return true;
}

//bool CompLayer( XSPBaseLayer spLayer1, XSPBaseLayer spLayer2 )
//{
//	if( spLayer1->GetfPriority() < spLayer2->GetfPriority() )
//		return TRUE;
//	return FALSE;
//}



/////////////////////////////////////////////////////////
//
// class XAniAction
//
/////////////////////////////////////////////////////////
void XAniAction::Destroy()
{
	// 키 리스트 삭제
	LIST_DESTROY_AUTO( m_listKey );
	// 	XBaseKey_Itor itor;
	// 	for( itor = m_listKey.begin(); itor != m_listKey.end(); )
	// 	{
	// 		XBaseKey *pKey = (*itor);
	// //			m_pKeyMng->Clear( pKey );
	// 		SAFE_DELETE( pKey );
	// 		m_listKey.erase( itor++ );
	// 	}
	SAFE_DELETE( m_pKeyMng );		// 액션이 사라지면 키도 모두 사라지는것이므로 통째로 날려버리면 된다
}
void XAniAction::Create()
{
	m_pKeyMng = new XKeyMng;
	//	CreateLayer();
}
/**
 @brief idLayer내에서 고유한 지역 키 아이디를 생성한다.
 @param idLocalInLayer 0이면 새로운 값을 생성하고 값이 있으면 id최대값만 조정한다.
*/
// ID XAniAction::GenerateidKeyLocal( ID idLayer, ID idLocalInLayer )
// {
// 	auto pLayerInfo = FindpLayerInfoMutable( idLayer );
// 	if( pLayerInfo ) {
// 		if( idLocalInLayer ) {
// 			pLayerInfo->m_idLocalByNewKey = std::max( idLocalInLayer, pLayerInfo->m_idLocalByNewKey );
// 			return idLocalInLayer;				
// 		} else {
// 			return pLayerInfo->GenerateidKeyLocal();
// 		}
// 	}
// 	return 0;
// }
/**
 @brief src액션으로부터 카피본을 만든다.
*/
// XAniAction::XAniAction( const XAniAction& src, XSprDat* pSprDat )
// {
// 	// 일단 전체 복사
// 	*this = src;
// 	// 소스측으로부터 레이어정보 복사.
// 	m_listLayerInfo = src.m_listLayerInfo;
// 	// keymng
// 	m_pKeyMng = new XKeyMng();
// 	// keys
// 	m_listKey.clear();
// 	for( auto pKeySrc : src.m_listKey ) {
// 		auto pKeyCopy = pKeySrc->CreateCopy( )
// 		auto typeLayer = pKeySrc->GetspLayer()->GetType();
// 		auto nLayer = pKeySrc->GetspLayer()->GetnLayer();
// 		//		pKeyCopy->MakeNewID(); 키 새로할당하면 안됨 키끼리 id로 참조하고 있음.
// 		pKeyCopy->SetspAction( XSPAction() );	// 지금은 GetThis()를 쓸수 없으므로 
// 																					// 		auto spLayer = GetspLayer( typeLayer, nLayer );
// 																					// 		XBREAK( spLayer == nullptr );
// 																					//		pKeyCopy->SetspLayer( spLayer );
// 		pKeyCopy->SetidLayer( pKeySrc->GetidLayer() );
// 		m_listKey.push_back( pKeyCopy );
// 		m_pKeyMng->Add( pKeyCopy );	// fuck AddKey()를 사용하지 않고 바로 넣도록 바꿈.
// 	}
// 	/*
// 	auto spNewAct = XSPAction( new XAniAction(*(spSrcAct)) );
// 	auto spNewAct = spSrcAct->CreateCopy( pSprDat );
// 	*/
// }
/**
 @brief 액션이 카피가 된 후 새로 부여받을 값들 설정.
*/
void XAniAction::SetNewForAfterCopy()
{
	// 새 액션아이디 부여
	m_ActID = m_pSprDat->GenerateActID();
	// 새 액션 이름 부여
	SetszActName( GetstrActName() + _T( " copy" ) );
	// 
// 	m_itorKey = m_listKey.begin();
// 	m_itorRangeKey = m_listKey.begin();
	// m_mapMaxLayerNumber도 클리어 하면 안됨. 레이어까지 카피해왔기때문.
}
/**
 @brief this액션으로부터 액션카피본을 만든다.
 리턴되는 복사본은 시리얼번호까지 똑같은 액션을 돌려줘야한다. 이것을 새 액션으로 쓸지 백업(언두)용으로 쓸지는 밖에서 결정해야한다.
*/
XSPAction XAniAction::CreateCopy( XSprDat* pSprDat ) const
{
	auto spNewAct = XSPAction( new XAniAction( *this ) );	// 기본복사
	auto pNewAct = spNewAct.get();
	// 소스측으로부터 레이어정보 복사.
//	pNewAct->m_listLayerInfo = m_listLayerInfo;
	// keymng
	pNewAct->m_pKeyMng = new XKeyMng();
	// keys
	pNewAct->m_listKey.clear();
	for( auto pKeySrc : m_listKey ) {
		// 액션전체가 복사되는것이므로 레이어내의 고유키아이디를 바꾸면 안됨.
		const ID idLayer = pKeySrc->GetidLayer();
		const ID idKeyLocalInLayer = pKeySrc->GetidLocalInLayer();
// 		auto pKeyCopy = pKeySrc->CreateCopy( spNewAct, idLayer, idKeyLocalInLayer  );
		auto pKeyCopy = pKeySrc->CreateCopy();
		pKeyCopy->SetNewInstance( spNewAct, idLayer, idKeyLocalInLayer );
		pKeyCopy->m_pShadowParent = nullptr;
		pNewAct->m_listKey.push_back( pKeyCopy );
 		pNewAct->m_pKeyMng->Add( pKeyCopy );
		pKeyCopy->OnAdded();
	}
// 	pNewAct->m_ActID = pSprDat->GenerateActID();
// 	pNewAct->SetszActName( GetstrActName() + _T( " copy" ) );
// 	pNewAct->m_idLayerGlobal = 1;
	// 기본값으로 초기화
	pNewAct->m_itorKey = pNewAct->m_listKey.end();
	pNewAct->m_itorRangeKey = pNewAct->m_listKey.end();
	// 원본에 키 이터레이터를 가지고 있었다면 같은키를 가리키는 이터레이터로 세팅함.
	if( m_listKey.end() != m_itorKey ) {
		pNewAct->m_itorKey = pNewAct->FindKeyItor( (*m_itorKey) );
// 		if( pNewAct->m_itorKey == pNewAct->GetlistKey().end() )
// 			pNewAct->m_itorKey = pNewAct->m_listKey.begin();
	}
	if( m_listKey.end() != m_itorRangeKey ) {
		pNewAct->m_itorRangeKey = pNewAct->FindKeyItor( (*m_itorRangeKey) );
// 		if( pNewAct->m_itorRangeKey == pNewAct->GetlistKey().end() )
// 			pNewAct->m_itorRangeKey = pNewAct->m_listKey.begin();
	}
//  	pNewAct->m_itorKey = pNewAct->m_listKey.begin();
//  	pNewAct->m_itorRangeKey = pNewAct->m_listKey.begin();
// 	pNewAct->m_mapMaxLayerNumber.clear();
	return spNewAct;
}

/**
 @brief pKeySrc(다른액션에 있는)와 같은키의 이터레이터를 찾아서 돌려준다.
*/
XBaseKey_Itor XAniAction::FindKeyItor( XBaseKey* pKeySrc )
{
	for( auto itor = m_listKey.begin(); itor != m_listKey.end(); ++itor ) {
		const auto pKey = (*itor);
		if( pKey->GetidLocalInLayer() == pKeySrc->GetidLocalInLayer() )
			return itor;
	}
	return m_listKey.end();
}
/**
 @brief 레이어카피용. idLayerSrc에 속하는 모든 키들을 카피해서 this액션의 키에 추가한다.
 새로 생성된 키들은 idLocal을 새로 부여받는다.
 @param idLayerNew 생성된 키가 소속될 레이어
*/
void XAniAction::CopyKeyByLayer( ID idLayerSrc, ID idLayerNew )
{
	XBREAK( idLayerSrc == 0 );
	// 모든키들을 순회하며 idLayerSrc를 쓰는 키들을 찾아서 복사한후 어레이에 담는다.
	std::vector<XBaseKey*> aryNewKey;
	for( auto pKey : m_listKey ) {
		if( pKey->GetidLayer() == idLayerSrc ) {
			// 레이어가 통째로 카피되는것이므로 지역아이디가 바뀌면 안됨.
			ID idKeyLocalInLayer = pKey->GetidLocalInLayer();
// 			auto pNewKey = pKey->CreateCopy( GetThis(), idLayerNew, idKeyLocalInLayer );
			auto pNewKey = pKey->CreateCopy();
			pNewKey->SetNewInstance( GetThis(), idLayerNew, idKeyLocalInLayer );
			aryNewKey.push_back( pNewKey );
		}
	}
	// 어레이에 담긴 새 키들을 추가한다.
	for( auto pKeyNew : aryNewKey ) {
//		AddKey( pKeyNew );
 		m_listKey.push_back( pKeyNew );
 		m_pKeyMng->Add( pKeyNew );
		pKeyNew->OnAdded();
	}
}
// /**
//  @brief 같은 레이어내에서 키 카피
//  @param idLocalInLayer 카피할 소스키의 레이어내 지역아이디
//  @param idLayer 카피될 레이어
// */
// XBaseKey* XAniAction::CreateNewInstanceKeyBySameLayer( ID idLocalinLayerSrc, ID idLayerSrc )
// {
// 	XBREAK( idLayerSrc == 0 );
// 	XBREAK( idLocalinLayerSrc == 0 );
// 	// 소스키를 찾음.
// 	auto pKeySrc = GetpKeyByidLocal( idLocalinLayerSrc, idLayerSrc );
// 	if( XASSERT(pKeySrc) )
// 		return CreateNewInstanceKeyBySameLayer( pKeySrc );
// 	return nullptr;
// }
// 
// XBaseKey* XAniAction::CreateNewInstanceKeyBySameLayer( XBaseKey* pKeySrc )
// {
// 	XBREAK( pKeySrc == nullptr );
// 	const ID idLayer = pKeySrc->GetidLayer();
// 	if( XASSERT(idLayer) ) {
// 		ID idLocalNew = GenerateidKeyLocal( idLayer, 0 );
// 		if( XASSERT(idLocalNew) ) {
// // 			const auto pNewKey = pKeySrc->CreateCopy( GetThis(), idLayer, idLocalNew );
// 			const auto pNewKey = pKeySrc->CreateCopy();
// 			pNewKey->SetNewInstance( GetThis(), idLayer, idLocalNew );
// 			AddKey( pNewKey, pKeySrc->GetfFrame() );
// 			return pNewKey;
// 		}
// 	}
// 	return nullptr;
// }

/**
 @brief idLayer에 속한 키들중 idLocalInLayer를 가진 키를 찾음.
*/
XBaseKey* XAniAction::GetpKeyByidLocal( ID idLocalInLayer, ID idLayer )
{
	for( auto pKey : m_listKey ) {
		if( pKey->GetidLayer() == idLayer ) {
			if( pKey->GetidLocalInLayer() == idLocalInLayer )
				return pKey;
		}
	}
	return nullptr;
}

// pSrc의  this로 카피
/**
 @brief spSrc의 XAniAction멤버값들을 복구한다. 하위 키들은 건드리지 않는다.
*/
void XAniAction::RestoreValue( const XSPAction& spSrc ) 
{ 		
	if( XASSERT(this != spSrc.get()) ) {
		// 일단 this의 리스트 내용을 백업받음
		auto listKeyThis = m_listKey;
		auto pKeyMng = m_pKeyMng;
		// 한꺼번에 옮김
		*this = *spSrc;
		// 복구
		m_pKeyMng = pKeyMng;
		m_listKey = listKeyThis;
		m_itorKey = m_listKey.end();
		m_itorRangeKey = m_listKey.end();
		if( spSrc->m_listKey.end() != spSrc->m_itorKey )
			m_itorKey = FindKeyItor( (*spSrc->m_itorKey) );
		if( spSrc->m_listKey.end() != spSrc->m_itorRangeKey )
			m_itorRangeKey = FindKeyItor( ( *spSrc->m_itorRangeKey ) );
// 		{
// 			XBaseKey_Itor srcItor;
// 			XBaseKey_Itor dstItor;
//  			for( auto srcItor = spSrc->m_listKey.begin(), dstItor= m_listKey.begin(); 
// 				srcItor != spSrc->m_listKey.end(); 
// 				srcItor++, dstItor++ ) {
// 				(*dstItor)->ValueAssign( (*srcItor) );		// 액션포인터는 빼고 값만 복사
// 			}
// 		}
	}
}

void XAniAction::Save( XResFile *pRes, const std::vector<xLayerInfo>& aryLayerInfo )
{
	WORD wData = (WORD)(_tcslen( m_szActName ) * sizeof(TCHAR) + sizeof(TCHAR));		// strlen + nullptr
	pRes->Write( &wData, 2 );							// action name length + nullptr
	pRes->Write( m_szActName, (int)wData );				// action name string
	wData = (WORD)m_listKey.size();
	pRes->Write( &wData, 2 );							// key개수
	pRes->Write( &m_fMaxFrame, 4 );					// maxframe
	pRes->Write( &m_fSpeed, 4 );						// speed
	pRes->Write( &m_PlayMode, 4 );
	pRes->Write( &m_RepeatMark, 4 );				// 도돌이표 위치
	pRes->Write( &m_vBoundBox[0], sizeof(XE::VEC2) );
	pRes->Write( &m_vBoundBox[1], sizeof(XE::VEC2) );
	pRes->Write( &m_vScale, sizeof(m_vScale) );
	pRes->Write( &m_vRotate, sizeof(m_vRotate) );
	BYTE b1;
	pRes->Write( &m_idLayerGlobal, 4 );
	// 레이어 정보 저장
	DWORD reserved[ 9 ] = {0,};
	pRes->Write( reserved, XNUM_ARRAY(reserved), sizeof(DWORD) );	
	XBREAK(aryLayerInfo.size() > 0xff);
 	b1 = (BYTE)aryLayerInfo.size();
	pRes->Write( &b1, 1 );
	for( const auto& layerInfo : aryLayerInfo ) {
		layerInfo.Save( (*pRes) );
	}
// 	b1 = m_listLayer.size();
// 	pRes->Write( &b1, 1 );								// layer 개수
// 	for( auto spLayer : m_listLayer ) {
// 		XBaseLayer::sSave( pRes, spLayer );
// 	}
// 	b1 = m_listLayerInfo.size();
// 	pRes->Write( &b1, 1 );								// layer 개수
// 	{
// 		LAYERINFO_LOOP( itor ) {
// 			LAYER_INFO *layer = (*itor);
// 			pRes->Write( &layer->idLayer, 4 );
// 			b1 = (BYTE)layer->type;
// 			pRes->Write( &b1, 1 );		// layer type
// 			b1 = (BYTE)layer->nLayer;		// 레이어 번호
// 			pRes->Write( &b1, 1 );
// 			pRes->Write( &layer->fLastFrame, 4 );
// 			pRes->Write( &layer->fAdjustAxisX, 4 );		// ver 8
// 			pRes->Write( &layer->fAdjustAxisY, 4 );
// 		}
// 	}
	// key정보 저장
	// 각키들의 인덱스를 만든다
	int i = 0;
	GetNextKeyClear();
	while( XBaseKey *pKey = GetNextKey() ) {
		pKey->SetnIdx( i++ );		// 한 액션 내의 모든 키에 순서대로 인덱스를 붙인다
	}
	// 키들을 저장한다
	GetNextKeyClear();
	while( XBaseKey *pKey = GetNextKey() ) {
		const auto pLayerInfo = FindpLayerInfo( aryLayerInfo, pKey->GetidLayer() );
		pKey->Save( pLayerInfo->m_bitType, pLayerInfo->m_nLayer, pRes );
	}
//	m_aryLayerInfoForSave.clear();		// 세이브후엔 바로 제거
}
// LAYER_INFO *XAniAction::AddLayerInfo( xSpr::xtLayer type, int nLayer, int idLayer )
// {
// 	LAYER_INFO *spLayer = new LAYER_INFO( m_pSprDat, idLayer );
// 	if( idLayer )				// 파라메터로 idLayer가 0이 날아오면 new LAYER_INFO에서 자동생성한 아이디를 쓰고 값이 있는채로(파일에서 읽은)오면 그 값을 쓴다
// 		spLayer->idLayer = idLayer;
// 	spLayer->type = type;
// 	spLayer->nLayer = nLayer;
// 	spLayer->fLastFrame = 0;
// 	spLayer->fAdjustAxisX = spLayer->fAdjustAxisY = 0;
// 	m_listLayerInfo.push_back( spLayer );
// 	return spLayer;
// }
// XKeyPos *XAniAction::CreatePosKey( xSpr::xtKey type )
// {
// 	return new XKeyPos( GetThis() );
// }
// XKeyRot *XAniAction::CreateRotKey( xSpr::xtKey type )
// {
// 	return new XKeyRot( GetThis() );
// }
// XKeyScale *XAniAction::CreateScaleKey( xSpr::xtKey type )
// {
// 	return new XKeyScale( GetThis() );
// }
// XKeyEffect *XAniAction::CreateEffectKey( xSpr::xtKey type )
// {
// 	return new XKeyEffect( GetThis() );
// }
void XAniAction::Load( XSprDat *pSprDat, XResFile *pRes, std::vector<xLayerInfo> *pOutAryLayerInfo )
{
	XBREAK( pOutAryLayerInfo == nullptr );
	pOutAryLayerInfo->clear();
	WORD wData;
	BYTE b1;
	DWORD dw0;
	int i;
	int ver = pSprDat->GetnVersion();
	pRes->Read( &wData, 2 );					// action name string length
	pRes->Read( m_szActName, (int)wData );		// action name + nullptr
	pRes->Read( &wData, 2 );					// key 개수
	int nNumKeyList = wData;
	pRes->Read( &m_fMaxFrame, 4 );
	pRes->Read( &m_fSpeed, 4 );
	pRes->Read( &m_PlayMode, 4 );
	pRes->Read( &m_RepeatMark, 4 );
	if( pSprDat->IsUpperVersioin(22) ) {
		pRes->Read( &m_vBoundBox[0], sizeof(XE::VEC2) );
		pRes->Read( &m_vBoundBox[1], sizeof(XE::VEC2) );
		if( pSprDat->IsUpperVersioin(24) ) {
			pRes->Read( &m_vScale, sizeof(m_vScale) );
			pRes->Read( &m_vRotate, sizeof(m_vRotate) );
		}
		if( pSprDat->IsUpperVersioin(26) )
			pRes->Read( &m_idLayerGlobal, 4 );
		else
			pRes->Read( &dw0, 4 );
		DWORD reserved[ 9 ] = {0,};
		pRes->Read( reserved, XNUM_ARRAY(reserved), sizeof(DWORD) );
	}
	pRes->Read( &b1, 1 );					// 레이어 개수
	// 레이어 리스트 로드
	int nNumLayerList =(int)b1;
	for( i = 0; i < nNumLayerList; i++ ) {
		xLayerInfo layerInfo;
		layerInfo.Load( (*pRes), ver );
		pOutAryLayerInfo->push_back( layerInfo );
//		AddLayerInfo( layerInfo );

	}
// 	for( i = 0; i < nNumLayerList; i ++ ) {
// 		auto spLayer = XBaseLayer::sCreateRead( pRes, ver );
// 		AddLayer( spLayer );
// 	}
	// 26이전버전에선 액션별로 idLayer의 seed값이 없었기때문에 직접 계산해준다.
	if( pSprDat->IsLowerVersioin(26) )
		SetMaxidLayer( *pOutAryLayerInfo );
	XBREAK( m_pSprDat == nullptr );
	//
	for( i = 0; i < nNumKeyList; i++ ) {
		auto pKey = XBaseKey::sCreateRead( pRes, GetThis(), ver, *pOutAryLayerInfo );
		if( ver < 29 ) {
			// 29버전 이하 파일은 이 값이 없다. XSprObj::Load에서 차후 넣어준다.
			XASSERT( pKey->GetidLocalInLayer() == 0 );
		}
		XBREAK( pKey->GetSubType() == xSpr::xKTS_NONE );
		m_listKey.push_back( pKey );
		m_pKeyMng->Add( pKey );
	}

	// 20버전 미만 포맷을 위해 각 보간키들에게 다음키를 지정해준다
	if( pSprDat->IsLowerVersioin(20) ) {
// 		LIST_LOOP( m_listKey, XBaseKey*, itor, pKey ) {
// 			if( pKey->GetSubType() == xSpr::xKTS_POS ) {
// 				XKeyPos *pPosKey = SafeCast<XKeyPos*, XBaseKey*>( pKey );
// 				// 보간키인데 다음키가 지정안되어 있을때
// 				if( pPosKey && pPosKey->GetInterpolation() &&	
// 					/*(pPosKey->GetidNextPathKey() == 0 ||*/ pPosKey->GetidInterpolationEndKey() == 0 ) {
// 					XBaseKey *pNextKey = pPosKey->GetNextKey();
// 					if( pNextKey ) {
// 						XKeyPos *pNextPosKey = SafeCast<XKeyPos*>( pNextKey );
// 						if( pNextPosKey ) {
// // 							pPosKey->SetidNextPathKey( pNextPosKey->GetidKey() );
// // 							pPosKey->SetidPrevPathKey( 0 );
// 							pPosKey->SetidInterpolationEndKey( pNextPosKey->GetidKey() );
// 						} else {
// 							XBREAK( pNextPosKey == nullptr );
// 						}
// 					}
// 				} else {
// 					XBREAK( pPosKey == nullptr );
// 				}
// 			}
// 		} END_LOOP;
	}
	// 각 레이어타입들의 최대 인덱스값을 얻어서 테이블에 넣는다.
	SetMaxIndexByLayerType( *pOutAryLayerInfo );
	
} // Load

// LAYER_INFO* XAniAction::GetLayerInfo( xSpr::xtLayer type, int nLayer )
// {
// 	LAYERINFO_LOOP( itor )
// 	{
// 		LAYER_INFO *spLayer = (*itor);
// 		if( spLayer->type == type && spLayer->nLayer == nLayer )
// 			return spLayer;
// 	}
// 	XERROR( "%d, %dLayerInfo가 %s액션에 없다", (int)type, nLayer, GetszActName() );
// 	return nullptr;
// }
XBaseKey_Itor XAniAction::AddKey( XBaseKey *pNewKey, float fFrame, BOOL bNewGenerateID )
{
	if( fFrame == -1.0f )
		fFrame = pNewKey->GetfFrame();
	XBaseKey_Itor itor;
	XBaseKey *pKey;
	if( m_listKey.size() == 0 )	{
		m_listKey.push_back( pNewKey );
		itor = m_listKey.begin();
	} else {
		// frame번호순서에 따라 끼워넣는다.
		for( itor = m_listKey.begin(); itor != m_listKey.end(); ) {
			pKey = (*itor);
			if( pKey->GetfFrame() > fFrame )
				break;
			++itor;
		}
		if( itor == m_listKey.end() )
			m_listKey.push_back( pNewKey );
		else {
			m_listKey.insert( itor, pNewKey );
			itor --;			// --를 해줘야 방금 삽입된 노드를 가르킨다
		}
	}
	pNewKey->SetfFrame( fFrame );
	if( bNewGenerateID ) {				// 언두를 위한 카피본만드는중에 AddKey()등은 이게 false다.
		pNewKey->MakeNewID();		// 새 아이디를 할당받는다
	}
	m_pKeyMng->Add( pNewKey );
	// 키가 액션에 애드될때 핸들러를 호출
	pNewKey->OnAdded();

	return itor;
}

XKeyImage *XAniAction::AddKeyImage( float fFrame, xSPLayerImage spLayer, XSprite *pSprite )
{
//	XKeyImage *pNewKey = new XKeyImage( GetThis(), fFrame, spLayer, pSprite );
	auto pNewKey = new XKeyImage( GetThis(), fFrame, spLayer, pSprite->GetnIdx() );
	AddKey( pNewKey, fFrame );
	if( fFrame >= GetfMaxFrame() )
		m_fMaxFrame += 1.0f;
	return pNewKey;
}

XKeyDummy *XAniAction::AddKeyDummy( float fFrame, XSPLayerDummy spLayer, BOOL bActive )
{
	auto pNewKey = new XKeyDummy( GetThis(), fFrame, spLayer, bActive );
	AddKey( pNewKey, fFrame );
	if( fFrame >= GetfMaxFrame() )
		m_fMaxFrame += 1.0f;
	return pNewKey;
}


// XKeyPos *XAniAction::AddKeyPos( float fFrame, xSpr::xtLayer type, int nLayer, float x, float y )
XKeyPos *XAniAction::AddKeyPos( float fFrame, XSPLayerMove spLayer, float x, float y )
{
	auto typeKey = XBaseKey::GetTypeFromLayer( spLayer->GetbitType() );
	auto pNewKey = new XKeyPos( GetThis(), typeKey, fFrame, spLayer, x, y );
	XBaseKey_Itor itor = AddKey( pNewKey, fFrame );
	if( fFrame >= GetfMaxFrame() )
		m_fMaxFrame += 1.0f;
	return pNewKey;
}
XKeyRot *XAniAction::AddKeyRot( float fFrame, XSPLayerMove spLayer, float fAngleZ )
{
	auto typeKey = XBaseKey::GetTypeFromLayer( spLayer->GetbitType() );
	auto pNewKey = new XKeyRot( GetThis(), typeKey, fFrame, spLayer, fAngleZ );
	XBaseKey_Itor itor = AddKey( pNewKey, fFrame );
	if( fFrame >= GetfMaxFrame() )
		m_fMaxFrame += 1.0f;
	return pNewKey;
}
XKeyScale *XAniAction::AddKeyScale( float fFrame, XSPLayerMove spLayer, float sx, float sy )
{
	auto typeKey = XBaseKey::GetTypeFromLayer( spLayer->GetbitType() );
	auto pNewKey = new XKeyScale( GetThis(), typeKey, fFrame, spLayer, sx, sy );
	XBaseKey_Itor itor = AddKey( pNewKey, fFrame );
	if( fFrame >= GetfMaxFrame() )
		m_fMaxFrame += 1.0f;
	return pNewKey;
}
XKeyEffect *XAniAction::AddKeyEffect( float fFrame, XSPLayerMove spLayer, BOOL bInterpolation, DWORD dwDrawFlag, xDM_TYPE drawMode, float fOpacity )
{
	auto typeKey = XBaseKey::GetTypeFromLayer( spLayer->GetbitType() );
	auto pNewKey = new XKeyEffect( GetThis(), typeKey, fFrame, spLayer );
	pNewKey->Set( (bInterpolation)? xSpr::xLINEAR : xSpr::xNONE, dwDrawFlag, drawMode, fOpacity );
	AddKey( pNewKey, fFrame );
	if( fFrame >= GetfMaxFrame() )
		m_fMaxFrame += 1.0f;
	return pNewKey;
}
XKeyEvent *XAniAction::AddKeyEventCreateObj( float fFrame, XSPLayerEvent spLayer, WORD id, float x, float y )
{
	auto pNewKey = new XKeyEvent( GetThis(), fFrame, spLayer, id, x, y );
	AddKey( pNewKey, fFrame );
	if( fFrame >= GetfMaxFrame() )
		m_fMaxFrame += 1.0f;
	return pNewKey;
}
XKeyEvent *XAniAction::AddKeyEvent( float fFrame, XSPLayerEvent spLayer, xSpr::xtEventKey Event, float x, float y )
{
	auto pNewKey = new XKeyEvent( GetThis(), fFrame, spLayer, Event, x, y );
	AddKey( pNewKey, fFrame );
	if( fFrame >= GetfMaxFrame() )
		m_fMaxFrame += 1.0f;
	return pNewKey;
}
// XKeyCreateObj *XAniAction::AddKeyCreateObj( XBaseKey& keyBase, XKeyCreateObj& key )
// {
// 	XKeyCreateObj *pNewKey = new XKeyCreateObj( GetThis(), keyBase, key );
// 	pNewKey->CopyLua( &keyBase );
// 	AddKey( pNewKey );
// 	if( pNewKey->GetfFrame() >= GetfMaxFrame() )
// 		m_fMaxFrame += 1.0f;
// 	return pNewKey;
// }
// XKeyCreateObj *XAniAction::AddKeyCreateObj( DWORD id, LPCTSTR szSprObj, ID idAct, xRPT_TYPE playType, xSpr::xtLayer type, int nLayer, float fFrame, float x, float y )
XKeyCreateObj *XAniAction::AddKeyCreateObj( DWORD id, LPCTSTR szSprObj, ID idAct, xRPT_TYPE playType, XSPLayerObject spLayer, float fFrame, float x, float y )
{
	auto pNewKey = new XKeyCreateObj( GetThis(), id, szSprObj, spLayer, idAct, playType, fFrame, x, y );
	AddKey( pNewKey, fFrame );
	if( fFrame >= GetfMaxFrame() )
		m_fMaxFrame += 1.0f;
	return pNewKey;
}
XKeySound *XAniAction::AddKeySound( float fFrame, XSPLayerSound spLayer, ID idSound )
{
	XKeySound *pNewKey = new XKeySound( GetThis(), fFrame, spLayer, idSound );
	AddKey( pNewKey, fFrame );
	return pNewKey;
}
// spLayer에서 fFrame이전의 type,subtype과 일치하는 키를 찾아낸다
XBaseKey *XAniAction::FindPrevKey( xSpr::xtKey type, xSpr::xtKeySub subType, float fFrame, XSPBaseLayer spLayer )
{
	XBaseKey *pFindKey = nullptr;
	for( auto pKey : m_listKey ) {
		if( pKey->GetfFrame() > fFrame )
			break;
		// 같은 채널의 키를 찾는다.
		if( pKey->GetidLayer() == spLayer->GetidLayer() && pKey->GetSubType() == subType ) {
			XASSERT( pKey->GetType() == type );
			if( pFindKey ) {
				if( pFindKey->GetfFrame() < pKey->GetfFrame() )
					pFindKey = pKey;
			} else
				pFindKey = pKey;
		}
	}
	return pFindKey;
}

/**
 @brief pKeyBase의 앞에있는 같은채널의 키를 찾는다.
*/
XBaseKey *XAniAction::FindPrevKey( XBaseKey* pKeyBase )
{
	XBaseKey *pKeyMax = nullptr;
	for( auto pKey : m_listKey ) {
		if( pKey->GetidKey() == pKeyBase->GetidKey() )
			break;
		// 같은 채널의 키를 찾는다.
		if( pKey->GetidLayer() == pKeyBase->GetidLayer() && pKey->GetSubType() == pKeyBase->GetSubType() ) {
			XASSERT( pKey->GetType() == pKeyBase->GetType() );
			if( pKeyMax ) {
				if( pKey->GetfFrame() > pKeyMax->GetfFrame() )
					pKeyMax = pKey;
			} else
				pKeyMax = pKey;
		}
	}
	return pKeyMax;
}

XBaseKey *XAniAction::FindNextKey( XBaseKey* pKeyBase )
{
	auto itor = std::find( m_listKey.begin(), m_listKey.end(), pKeyBase );
	while( itor != m_listKey.end() ) {
		++itor;
		if( itor != m_listKey.end() ) {
			auto pKey = (*itor);
			if( pKey ) {
				if( pKey->IsSameLayerAndChannel(pKeyBase) )
					return pKey;
			}
		}
	}
	return nullptr;
// 
// 	XBaseKey *pFindKey = nullptr;
// 	for( auto pKey : m_listKey ) {
// 		// 기준프레임값보다 크고
// 		if( pKey->GetfFrame() > fFrame ) {
// 			// 같은레이어 같은 채널에 속한 키를 찾는다.,
// 			if( pKey->GetidLayer() == idLayer && pKey->GetSubType() == subType ) {
// 				XASSERT( pKey->GetType() == type );
// 				pFindKey = pKey;
// 				break;
// 			}
// 		}
// 	}
// 	return pFindKey;
}

void XAniAction::JumpKey( XSprObj *pSprObj, XBaseKey_Itor &itorKeyCurr, float fFrame ) const
{
	if( m_listKey.empty() )
		return;
	if( itorKeyCurr == m_listKey.end() )
		return;
	while(1)
	{
		XBaseKey *pKeyCurr = (*itorKeyCurr);
		if( fFrame > pKeyCurr->GetfFrame() )
		{
			++ itorKeyCurr;
			if( itorKeyCurr == m_listKey.end() )
				break;
		}  else
			break;
	}
}

// void XAniAction::ExecuteKey( XSprObj *pSprObj, XBaseKey_Itor &itorKeyCurr, float fFrame )
// {
// 	if( m_listKey.empty() )
// 		return;
// 	if( itorKeyCurr == m_listKey.end() )
// 		return;
// 	while(1)
// 	{
// 		XBaseKey *pKeyCurr = (*itorKeyCurr);
// 		if( fFrame >= pKeyCurr->GetfFrame() )
// 		{
// 			pKeyCurr->Execute( pSprObj );
// 			++ itorKeyCurr;
// 			if( itorKeyCurr == m_listKey.end() )
// 				break;
// 		}  else
// 			break;
// 	}
// }

// 
// fuck map버전으로 바꿀것.
/**
 @brief type, subtype, layer가 같은 키가 있는지 찾는다
*/
XBaseKey *XAniAction::FindKey( XSPBaseLayer spLayer, 
															xSpr::xtKey typeFilter, 
															xSpr::xtKeySub subType, 
															float fFrame ) const
{
	KEY_LOOP( itor ) {
		auto pKey = (*itor);
		// type인자가 NONE으로 들어오면 TYPE검사는 무시되어 항상 TRUE가 된다
		if( typeFilter == xSpr::xKT_NONE || pKey->GetType() == typeFilter )
			if( pKey->GetSubType() == subType )
				if( pKey->IsSameLayerOnly( spLayer ) )
					if( fFrame == -1.0f || pKey->GetfFrame() == fFrame )
						return pKey;	
	}
	return nullptr;
}
/**
 @brief 템플릿 버전
 fuck return 방식으로 바꾸면 안됨?
*/
// template<class T>
// void XAniAction::FindKey( SPBaseLayer spLayer, float fFrame, T **ppKey ) 
// {
// 	xSpr::xtKeySub subType = T::s_subType;
// 	XBaseKey_Itor itor = m_listKey.begin();
// 	XBaseKey *pKey;
// 	*ppKey = nullptr;
// 	if( m_listKey.size() == 0 ) {
// 		return;
// 	} else {
// 		for( itor = m_listKey.begin(); itor != m_listKey.end(); ) {
// 			pKey = ( *itor );
// 			// 레이어가 같고 프레임도 같으면
// 			if( pKey->IsSameLayer( spLayer ) && pKey->GetfFrame() == fFrame ) {
// 				// 게다가 키 서브타입도 같으면
// 				if( pKey->GetSubType() == subType ) {
// 					*ppKey = dynamic_cast<T*>( pKey );
// 					return;
// 				}
// 			}
// 			++itor;
// 		}
// 	}
// }
/**
 @brief 레이어 내에서 POS키를 찾는다.
 @param fFrame 해당 프레임위치의 키를 찾는다. -1이면 가장처음 키를 찾는다.
*/
// XKeyPos* XAniAction::FindKeyPos( XSPLayerMove spLayer, float fFrame )
// {
// 	for( auto pKey : m_listKey ) {
// 		if( XASSERT( pKey->GetspLayer() ) ) {
// 			if( pKey->GetSubType() == xSpr::xKTS_POS )
// 				if( pKey->GetidLayer() == spLayer->GetidLayer() )
// 					if( fFrame == -1.0f || pKey->GetfFrame() == fFrame )
// 						return static_cast<XKeyPos*>( pKey );
// 		}
// 	}
// 	return nullptr;
// }

// XKeyRot* XAniAction::FindKeyRot( XSPLayerMove spLayer, float fFrame )
// {
// 	for( auto pKey : m_listKey ) {
// 		if( XASSERT( pKey->GetspLayer() ) ) {
// 			if( pKey->GetSubType() == xSpr::xKTS_ROT )
// 				if( pKey->GetidLayer() == spLayer->GetidLayer() )
// 					if( fFrame == -1.0f || pKey->GetfFrame() == fFrame )
// 						return static_cast<XKeyRot*>( pKey );
// 		}
// 	}
// 	return nullptr;
// }
// XKeyScale* XAniAction::FindKeyScale( XSPLayerMove spLayer, float fFrame )
// {
// 	for( auto pKey : m_listKey ) {
// 		if( XASSERT( pKey->GetspLayer() ) ) {
// 			if( pKey->GetSubType() == xSpr::xKTS_SCALE )
// 				if( pKey->GetidLayer() == spLayer->GetidLayer() )
// 					if( fFrame == -1.0f || pKey->GetfFrame() == fFrame )
// 						return static_cast<XKeyScale*>( pKey );
// 		}
// 	}
// 	return nullptr;
// }
// 
// XKeyEffect* XAniAction::FindKeyEffect( XSPLayerMove spLayer, float fFrame )
// {
// 	for( auto pKey : m_listKey ) {
// 		if( XASSERT( pKey->GetspLayer() ) ) {
// 			if( pKey->GetSubType() == xSpr::xKTS_EFFECT )
// 				if( pKey->GetidLayer() == spLayer->GetidLayer() )
// 					if( fFrame == -1.0f || pKey->GetfFrame() == fFrame )
// 						return static_cast<XKeyEffect*>( pKey );
// 		}
// 	}
// 	return nullptr;
// }
// XKeyDummy* XAniAction::FindKeyDummy( XSPLayerMove spLayer, float fFrame )
// {
// 	for( auto pKey : m_listKey ) {
// 		if( XASSERT( pKey->GetspLayer() ) ) {
// 			if( pKey->GetSubType() == xSpr::xKTS_MAIN )
// 				if( pKey->GetidLayer() == spLayer->GetidLayer() )
// 					if( fFrame == -1.0f || pKey->GetfFrame() == fFrame )
// 						return static_cast<XKeyDummy*>( pKey );
// 		}
// 	}
// 	return nullptr;
// }
// XKeyEvent* XAniAction::FindKeyEvent( XSPLayerMove spLayer, float fFrame )
// {
// 	for( auto pKey : m_listKey ) {
// 		if( XASSERT( pKey->GetidLayer() ) ) {
// 			if( pKey->GetSubType() == xSpr::xKTS_MAIN )
// 				if( pKey->GetidLayer() == spLayer->GetidLayer() )
// 					if( fFrame == -1.0f || pKey->GetfFrame() == fFrame )
// 						return static_cast<XKeyEvent*>( pKey );
// 		}
// 	}
// 	return nullptr;
// }
// XKeySound* XAniAction::FindKeySound( XSPLayerMove spLayer, float fFrame )
// {
// 	for( auto pKey : m_listKey ) {
// 		if( XASSERT( pKey->GetidLayer() ) ) {
// 			if( pKey->GetSubType() == xSpr::xKTS_MAIN )
// 				if( pKey->GetidLayer() == spLayer->GetidLayer() )
// 					if( fFrame == -1.0f || pKey->GetfFrame() == fFrame )
// 						return static_cast<XKeySound*>( pKey );
// 		}
// 	}
// 	return nullptr;
// }

// idKey의 아이디를 가진 키를 찾는다
XBaseKey *XAniAction::FindKey( ID idKey ) const
{
	for( auto pKey : m_listKey ) {
		if( pKey->GetidKey() == idKey )
			return pKey;
	}
	return nullptr;
}

bool Comp( XBaseKey *key1, XBaseKey *key2 )
{
	return key1->GetfFrame() < key2->GetfFrame();
}
/**
 이 액션이 가지고 있는 키중에서 파라메터 조건과 일치하는 시작키를 찾는다.
*/
// XBaseKey* XAniAction::GetKeyFirst( xSpr::xtKeySub typeSub, xSpr::xtLayer typeLayer, int nLayer )
// XBaseKey* XAniAction::GetKeyFirst( xSpr::xtKeySub typeSub, ID idLayer )
// {
// 	for( auto pKey : m_listKey ) {
// 		// 같은레이어내에 패스의 일원인 시작키를 찾는다
// 		if( pKey->GetSubType() == typeSub && pKey->GetidLayer() == idLayer ) {
// // 			pKey->GetLayerType() == typeLayer &&
// // 			pKey->GetnLayer() == nLayer ) {
// 			XKeyPos* pPosKey = SafeCast<XKeyPos*, XBaseKey*>( pKey );
// 			if( pPosKey )
// 				return pPosKey;
// 		}
// 	}
// 	return nullptr;
// }

/**
 같은 채널에 속하는 키들을 찾아서 리스트에 담는다.
 m_listKey는 소트된상태여야 한다.
*/
// int XAniAction::GetKeysToAry( xSpr::xtKeySub typeSub,
// 							xSpr::xtLayer typeLayer,
// 							int nLayer,
// 							XList4<XBaseKey*> *pOut )
int XAniAction::GetKeysToAryByChannel( xSpr::xtKeySub typeSub,
																			XSPBaseLayer spLayer,
																			XList4<XBaseKey*> *pOut )
{
	XASSERT( pOut->size() == 0 );
	float secFramePrev = -1.f;
	for( auto pKey : m_listKey ) {
		if( pKey->GetSubType() == typeSub && pKey->IsSameLayerOnly( spLayer ) ) {
			// 키들은 순서대로 소트된 상태여야 한다.
			XBREAK( pKey->GetfFrame() < secFramePrev );
			secFramePrev = pKey->GetfFrame();
			pOut->Add( pKey );
		}
	}
	return pOut->size();
}
/**
 @brief 같은 레이어안에 있는 키들을 어레이에 담는다.
*/
int XAniAction::GetKeysToAryByLayer( XSPBaseLayer spLayer,
																		std::vector<XBaseKey*> *pOut ) const
{
	XASSERT( pOut->size() == 0 );
	float secFramePrev = -1.f;
	for( auto pKey : m_listKey ) {
		if( pKey->IsSameLayerOnly( spLayer ) ) {
			// 키들은 순서대로 소트된 상태여야 한다.
			XBREAK( pKey->GetfFrame() < secFramePrev );
			secFramePrev = pKey->GetfFrame();
			pOut->push_back( pKey );
		}
	}
	return pOut->size();
}

/**
 @brief 오직 XActObj::SortKey()에서만 호출해야함.
*/
void XAniAction::SortKeyForXActObj( const std::vector<xLayerInfo>& aryLayerInfo )
{
	m_listKey.sort( Comp );
	// 패스로 연결된 키들의 앞뒤 순서를 다시 맞춘다
	// 레이어별로 차례대로...
	// 레이어내의xSpr::xKTS_POS키만을 대상으로 한다.
	// 패스의 일원인 키를 만나면 그 아이디가 끝날때까지 차례로 훑어가며 앞뒤 연결을 시킨다.
	// 키가 더이상 나오지 않을때까지 위과정을 반복한다.
// 	for( const auto& layerInfo : aryLayerInfo ) {
// 		// 현재 레이어의 POS채널의 키만 뽑는다.
// 		XList4<XKeyPos*> listKeyPos;	// 현 레이어에 속하는 키들의 리스트(소팅된 상태임)
// 		GetKeysToAryByChannel<XKeyPos*>( xSpr::xKTS_POS, layerInfo.m_idLayer, &listKeyPos );
// 		if( listKeyPos.size() > 0 ) {
// 			// 일단 소트한 레이어내 키들은 앞뒤 연결관계를 모두 끊어준다.
// 			for( auto pKey : listKeyPos ) {
// // 				pKey->SetidPrevPathKey( 0 );
// // 				pKey->SetidNextPathKey( 0 );
// 				pKey->SetidInterpolationEndKey( 0 );
// 			}
// 			XKeyPos *pKeyPrev = nullptr;
// 			for( auto pKey : listKeyPos ) {
// 				if( pKeyPrev ) {
// 					// 이전키와 같은 패스상일경우만 연결
// 					// fuck 일단 기존 호환성을 유지하도록 수정해두고 패스아이디를 굳이 가질필요 없게 바꿔야 할듯.
// // 					if( pKey->GetidPath() == pKeyPrev->GetidPath() ) {
// // 						// 이전 키와 연결
// // 						pKeyPrev->SetidNextPathKey( pKey->GetidKey() );
// // 						pKey->SetidPrevPathKey( pKeyPrev->GetidKey() );
// 						// 이전키가 보간속성이 있는데 보간끝키가 지정되어있지 않으면 지금키를 끝키로 지정
// 						if( pKeyPrev->GetInterpolation()
// 							&& pKeyPrev->GetidInterpolationEndKey() == 0 )
// 							pKeyPrev->SetidInterpolationEndKey( pKey->GetidKey() );
// // 					}
// 				}
// 				pKeyPrev = pKey;
// 			}
// 		}
// 	}
}
// void XAniAction::SortKeyForXActObj()
// {
// 	m_listKey.sort( Comp );
// 	// 패스로 연결된 키들의 앞뒤 순서를 다시 맞춘다
// 	// 레이어별로 차례대로...
// 	// 레이어내의xSpr::xKTS_POS키만을 대상으로 한다.
// 	// 패스의 일원인 키를 만나면 그 아이디가 끝날때까지 차례로 훑어가며 앞뒤 연결을 시킨다.
// 	// 키가 더이상 나오지 않을때까지 위과정을 반복한다.
// 	for( const auto& layerInfo : m_listLayerInfo ) {
// 		// 현재 레이어의 POS채널의 키만 뽑는다.
// 		XList4<XKeyPos*> listKeyPos;	// 현 레이어에 속하는 키들의 리스트(소팅된 상태임)
// 		GetKeysToAryByChannel<XKeyPos*>( xSpr::xKTS_POS, layerInfo.m_idLayer, &listKeyPos );
// 		if( listKeyPos.size() > 0 ) {
// 			// 일단 소트한 레이어내 키들은 앞뒤 연결관계를 모두 끊어준다.
// 			for( auto pKey : listKeyPos ) {
// 				pKey->SetidPrevPathKey( 0 );
// 				pKey->SetidNextPathKey( 0 );
// 				pKey->SetidInterpolationEndKey( 0 );
// 			}
// 			XKeyPos *pKeyPrev = nullptr;
// 			for( auto pKey : listKeyPos ) {
// 				if( pKeyPrev ) {
// 					// 이전키와 같은 패스상일경우만 연결
// 					// fuck 일단 기존 호환성을 유지하도록 수정해두고 패스아이디를 굳이 가질필요 없게 바꿔야 할듯.
// 					if( pKey->GetidPath() == pKeyPrev->GetidPath() ) {
// 						// 이전 키와 연결
// 						pKeyPrev->SetidNextPathKey( pKey->GetidKey() );
// 						pKey->SetidPrevPathKey( pKeyPrev->GetidKey() );
// 						// 이전키가 보간속성이 있는데 보간끝키가 지정되어있지 않으면 지금키를 끝키로 지정
// 						if( pKeyPrev->GetInterpolation() 
// 							&& pKeyPrev->GetidInterpolationEndKey() == 0 )
// 							pKeyPrev->SetidInterpolationEndKey( pKey->GetidKey() );
// 					}
// 				}
// 				pKeyPrev = pKey;
// 			}
// 		}
// 	}
// }
// void XAniAction::SortKey()
// {
// 	m_listKey.sort( Comp );
// 	// 패스로 연결된 키들의 앞뒤 순서를 다시 맞춘다
// 	// 레이어별로 차례대로...
// 	// 레이어내의xSpr::xKTS_POS키만을 대상으로 한다.
// 	// 패스의 일원인 키를 만나면 그 아이디가 끝날때까지 차례로 훑어가며 앞뒤 연결을 시킨다.
// 	// 키가 더이상 나오지 않을때까지 위과정을 반복한다.
// 	for( auto spLayer : m_listLayer ) {
// 		// 현재 레이어의 POS채널의 키만 뽑는다.
// 		XList4<XKeyPos*> listKeyPos;	// 현 레이어에 속하는 키들의 리스트(소팅된 상태임)
// 		GetKeysToAryByChannel<XKeyPos*>( xSpr::xKTS_POS, spLayer, &listKeyPos );
// 		if( listKeyPos.size() > 0 ) {
// 			// 일단 소트한 레이어내 키들은 앞뒤 연결관계를 모두 끊어준다.
// 			for( auto pKey : listKeyPos ) {
// 				pKey->SetidPrevPathKey( 0 );
// 				pKey->SetidNextPathKey( 0 );
// 				pKey->SetidInterpolationEndKey( 0 );
// 			}
// 			XKeyPos *pKeyPrev = nullptr;
// 			for( auto pKey : listKeyPos ) {
// 				if( pKeyPrev ) {
// 					// 이전키와 같은 패스상일경우만 연결
// 					// fuck 일단 기존 호환성을 유지하도록 수정해두고 패스아이디를 굳이 가질필요 없게 바꿔야 할듯.
// 					if( pKey->GetidPath() == pKeyPrev->GetidPath() ) {
// 						// 이전 키와 연결
// 						pKeyPrev->SetidNextPathKey( pKey->GetidKey() );
// 						pKey->SetidPrevPathKey( pKeyPrev->GetidKey() );
// 						// 이전키가 보간속성이 있는데 보간끝키가 지정되어있지 않으면 지금키를 끝키로 지정
// 						if( pKeyPrev->GetInterpolation() 
// 							&& pKeyPrev->GetidInterpolationEndKey() == 0 )
// 							pKeyPrev->SetidInterpolationEndKey( pKey->GetidKey() );
// 					}
// 				}
// 				pKeyPrev = pKey;
// 			}
// 		}
// 	}
// /*
// meA---------A--------A          0
// A---------A--------A----------B---------B
// */
// 
// }
// pSprObj사용하지 않음
void XAniAction::DelKey( XSprObj *pSprObj, XBaseKey *pDelKey )
{
	if( pDelKey == nullptr ) {
		XLOG( "pDelKey==nullptr" );
		return;
	}
	XBaseKey_Itor itor;
	itor = find( m_listKey.begin(), m_listKey.end(), pDelKey );
	if( XBREAK( itor == m_listKey.end() ) )
		return;
	// 패스로 연결된 키였다면 끊어진부분을 다른키와 연결시킨다
// 	if( pDelKey->GetSubType() == xSpr::xKTS_POS )	{
// 		XKeyPos *pPosKey = XKeyPos::Cast( pDelKey );
// 		if( pPosKey ) {
// 			XKeyPos *pNextKey = SafeCast<XKeyPos*>(pPosKey->GetNextKey());
// 			XKeyPos *pPrevKey = SafeCast<XKeyPos*>(pPosKey->GetPrevKey());
// 			if( pNextKey ) {		// 내 뒤로 연결된키가 있었다면...
// 				if( pPrevKey ) {
// 					auto pNextKeyByDelKey = SafeCast<XKeyPos*>( pPosKey->GetNextKey() );
// 					pPrevKey->SetpInterpolationEndKey( pNextKey );	// 내 앞키의 보간키를 내 뒤키로 연결시킴
// 				} else {
// 					// 내뒤로키가 있는데 내 앞으로는 키가 없다면 내가 패스의 선두인데 삭제됐다
// 					pNextKey->SetInterpolation( pPosKey->GetInterpolation() );
// 					if( pPosKey->GetpInterpolationEndKey() != pNextKey )
// 						pNextKey->SetpInterpolationEndKey( pPosKey->GetpInterpolationEndKey() );
// 				}
// 			} else {
// 				// 내뒤로 키가 없고 내 앞으로는 키가 있다면 패스의 끝이다
// 			}
// 		} else {
// 			XLOG( "SafeCast실패" );
// 		}
// 	}
	m_pKeyMng->Clear( pDelKey );
	SAFE_DELETE( pDelKey );
	m_listKey.erase( itor );
}

// 전체키의 프레임를 fScale만큼 조절한다
void XAniAction::ScaleKey( float fScale )
{
	for( auto pKey : m_listKey ) {
		pKey->SetfFrame( pKey->GetfFrame() * fScale );
	}
}
void XAniAction::GetRangeKeyClear()
{
	m_itorRangeKey = m_listKey.begin();
}
// pKey1에서부터 pKey2까지 사이에 있는 키들을 하나씩 되돌려준다.
// 단, 같은종류의 레이어에 있어야 한다.
XBaseKey *XAniAction::GetRangeKey( XSprObj *pSprObj, XBaseKey *pKey1, XBaseKey *pKey2 )
{
	if( m_itorRangeKey == m_listKey.begin() ) {			// GetRangeKeyClear를 하고 처음 들어왔다
		for( ; m_itorRangeKey != m_listKey.end(); m_itorRangeKey++ ) {
			if( (*m_itorRangeKey) == pKey1 ) {			// 시작 키를 찾아서 리턴시키고 이터레이터를 위치시킨다
				m_itorRangeKey ++;
				break;
			}
		}
	} 
	for( ; m_itorRangeKey != m_listKey.end(); m_itorRangeKey ++ ) {
		auto pKey = (*m_itorRangeKey);
		if( pKey1->IsSameLayerOnly( pKey ) )
			if( (*m_itorRangeKey)->GetfFrame() >= pKey1->GetfFrame() && 
				(*m_itorRangeKey)->GetfFrame() <= pKey2->GetfFrame() )					// pKey1과 pKey2프레임사이에 있는지 검사
				return (*m_itorRangeKey++);
	}
	return nullptr;
}

/**
 @brief spLayer에 속한 키들을 모두 삭제한다.
*/
BOOL XAniAction::DelLayerLinkKey( XSPBaseLayer spLayer )
{
	BOOL bErase = FALSE;
	KEY_MANUAL_LOOP( kitor ) {
		if( (*kitor)->IsSameLayerOnly( spLayer ) ) {
			XBaseKey *pKey = (*kitor);
			m_pKeyMng->Clear( pKey );
			SAFE_DELETE( pKey );					// 실제 키를 삭제
			m_listKey.erase( kitor++ );		// 키 노드도 삭제
			bErase = TRUE;
		} else
			++kitor;
	}
	return bErase;
}

// LAYER_INFO* XAniAction::DelLayerInfo( SPBaseLayer spLayer, BOOL bNodeOnly )
// {
// 	LAYER_INFO *pDelLayerInfo = nullptr;
// 	LIST_MANUAL_LOOP_AUTO( m_listLayer, itor, spLayer ) {
// 		if( pLayerInfo->IsSame( spLayer ) ) {
// 			if( bNodeOnly == FALSE ) {
// 				SAFE_DELETE( pLayerInfo );
// 			}
// 			else
// 				pDelLayerInfo = pLayerInfo;		// 삭제한 노드의 포인터
// 			m_listLayerInfo.erase( itor++ );
// 		}
// 		else
// 			itor++;
// 	} END_LOOP;
// 	return pDelLayerInfo;
// }
// pSrc를 pDst의 앞에 삽입 
// void XAniAction::InsertLayerInfo( SPBaseLayer spDst, LAYER_INFO *pSrc )
// {
// 	LIST_LOOP( m_listLayerInfo, LAYER_INFO*, itor, pLayerInfo )
// 	{
// 		if( pLayerInfo->IsSame( pDst ) )
// 		{
// 			m_listLayerInfo.insert( itor, pSrc );
// 			break;
// 		}
// 	} END_LOOP
// }

const xLayerInfo* XAniAction::FindpLayerInfo( const std::vector<xLayerInfo>& aryLayerInfo, ID idLayer )
{
	for( auto& layerInfo : aryLayerInfo ) {
		if( idLayer == layerInfo.m_idLayer )
			return &layerInfo;
	}
	return nullptr;
}

// 구 코드의 잔재
const xLayerInfo* XAniAction::FindpLayerInfoByType( const std::vector<xLayerInfo>& aryLayerInfo
																								, xSpr::xtLayer bitLayerType
																								, int nLayer )
{
	for( auto& layerInfo : aryLayerInfo ) {
		if( bitLayerType == layerInfo.m_bitType && nLayer == layerInfo.m_nLayer )
			return &layerInfo;
	}
	return nullptr;
}

// const xLayerInfo* XAniAction::FindpLayerInfo( ID idLayer ) 
// {
// 	return m_listLayerInfo.FindByIDNonPtr( idLayer );
// }
// xLayerInfo* XAniAction::FindpLayerInfoMutable( ID idLayer )
// {
// 	return m_listLayerInfo.FindByIDNonPtr( idLayer );
// }
// XSPBaseLayer XAniAction::FindLayer( XSPBaseLayer spLayer )
// {
// 	return m_listLayer.FindspByID( spLayer->getid() );
// }

// BOOL XAniAction::IsEmptyLayer( XSPBaseLayer spLayer )
// {
// 	KEY_LOOP2( itor, pKey )
// 	{
// 		if( pKey->IsSameLayer( spLayer ) )		// 비교대상 레이어와 이 키의 레이어가 같은거냐
// 			return FALSE;			// spLayer에 속한 키가 있으면 empty가 아님
// 	}
// 	END_LOOP;
// 	return TRUE;
// }
// BOOL XAniAction::IsEmptyLayer( ID idLayer )
// {
// 	KEY_LOOP2( itor, pKey )
// 	{
// 		if( pKey->IsSameLayer( spLayer ) )		// 비교대상 레이어와 이 키의 레이어가 같은거냐
// 			return FALSE;			// spLayer에 속한 키가 있으면 empty가 아님
// 	}
// 	END_LOOP;
// 	return TRUE;
// }
XBaseKey *XAniAction::FindUseSprite( XSprite *pSpr )
{
	KEY_LOOP( itor )
	{
		XBaseKey *pKey = (*itor);
		if( pKey->IsUseSprite( pSpr ) )
			return pKey;
	}
	return nullptr;
}
/**
pSrc액션의 내용을 this에 deep copy한다
다시말하면 액션이 가진 레이어나 키도 포인터만 가지고 오는게 아니라 새로 메모리할당을 해서 카피된다.
@param bKeepIDKey 키를 카피할때 원본아이디를 보존할때
*/
// void XAniAction::CopyActionDeep( SPAction spSrc, BOOL bKeepIDKey )
// {
// //	_tcscpy_s( m_szActName, pSrc->GetszActName() );
// 	m_fMaxFrame = spSrc->GetfMaxFrame();
// 	m_fSpeed = spSrc->GetSpeed();
// // 	LAYER_INFO *pLayerInfo;
// // 	pSrc->GetNextLayerInfoClear();
// // 	while( pLayerInfo = pSrc->GetNextLayerInfo() ) {
// // 		LAYER_INFO *pNewLayer = new LAYER_INFO( m_pSprDat, 0 );
// // 		* pNewLayer = *pLayerInfo;
// // 		m_listLayerInfo.push_back( pNewLayer );
// // 	}
// 	for( auto spLayer : m_listLayer ) {
// 		auto spLayerCopy = SPBaseLayer( spLayer->CopyDeep() );
// 		m_listLayer.Add( spLayerCopy );
// 	}
// 	// keys
// 	XBREAK( m_pKeyMng != nullptr );
// 	m_pKeyMng = new XKeyMng();
// 	XBREAK( m_listKey.size() > 0 );
// 	m_listKey.clear();
// 	spSrc->GetNextKeyClear();
// 	while( XBaseKey *pSrcKey = spSrc->GetNextKey() ) {
// 		XBaseKey *pKeyCopy = pSrcKey->CopyDeep();
// 		m_listKey.push_back( pKeyCopy );
// 		m_pKeyMng->Add( pKeyCopy );	// fuck AddKey()를 사용하지 않고 바로 넣도록 바꿈.
// //		AddKey( Key, -1, !bKeepIDKey );
// 	}
// }
// XSPAction XAniAction::CopyDeep()
// {
// 	return XSPAction( new XAniAction( *this ) );
// // 	fuck SPAction spNewAction = new XAniAction( m_pSprDat, GetActID(), GetszActName() );
// // 	pNewAction->CopyActionDeep( GetThis(), bKeepIDKey );
// // 	return pNewAction;
// }
/**
 @brief 딥카피를 하지만 액션아이디는 새로 만든다.
*/
XSPAction XAniAction::CreateNewInstance() const
{
// 	auto SpCopy = XSPAction( new XAniAction( *this ) );
// 	SpCopy->SetidAct( m_pSprDat->GenerateActID() );
	auto spCopy = CreateCopy( m_pSprDat );
	if( XASSERT(spCopy) ) {
		spCopy->SetNewForAfterCopy();
	}
	return spCopy;
}
/**
 @brief 모든키들의 액션을 다시 세팅해준다.
*/
void XAniAction::ReassignspActionByKey( XSPAction spAction )
{
	for( auto pKey : m_listKey ) {
		pKey->SetspAction( spAction );
		XBREAK( pKey->GetidLayer() == 0 );
	}
}
void XAniAction::CompositLuaCodes( char *cDst, int bufflen )
{
	LIST_LOOP( m_listKey, XBaseKey*, itor, pKey )
	{
		if( pKey->GetpcLua() && XE::IsHave( pKey->GetpcLua() ) )		// 루아 있는 키만
		{
			CString strLua( Convert_char_To_TCHAR( pKey->GetpcLua() ) );		// 일단 char형 루아코드를 CString으로 변환
			pKey->CompositLuaCodes( &strLua );									// 함수명들을 모두 테이블:함수 형태로 바꿈
			CString strTable;
			strTable.Format( _T("%s={}\n"), pKey->MakeTableName() );			// key_xxxxx={} 코드생성
			strcat_s( cDst, bufflen, Convert_TCHAR_To_char( strTable ) );
			strcat_s( cDst, bufflen, Convert_TCHAR_To_char( strLua ) );			// 큰 버퍼에다 바꾼걸 붙임
			strcat_s( cDst, bufflen, "\n" );

		}
	} END_LOOP;
}

// XSPBaseLayer XAniAction::GetspLayer( xSpr::xtLayer type, int nLayer )
// {
// 	for( auto spLayer : m_listLayer ) {
// 		if( spLayer->IsSameType( type ) && spLayer->GetnLayer() == nLayer )
// 			return spLayer;
// 	}
// 	return nullptr;
// }
// SPBaseLayer XAniAction::GetspLayer( ID idSerial )
// {
// 	for( auto spLayer : m_listLayer ) {
// 		if( spLayer->GetidSerial() == idSerial )
// 			return spLayer;
// 	}
// 	return nullptr;
// }
// XSPBaseLayer XAniAction::GetspLayer( ID idLayer )
// {
// 	for( auto spLayer : m_listLayer ) {
// 		if( spLayer->GetidLayer() == idLayer )
// 			return spLayer;
// 	}
// 	return nullptr;
// }
// XSPLayerMove XAniAction::GetspLayerMove( xSpr::xtLayer type, int nLayer ) 
// {
// 	return std::static_pointer_cast<XLayerMove>( GetspLayer( type, nLayer ) );
// }
// 
// xSPLayerImage XAniAction::GetspLayerImage( int nLayer )
// {
// 	return std::static_pointer_cast<XLayerImage>( 
// 		GetspLayer( xSpr::xLT_IMAGE, nLayer ) );
// }
// 
// XSPLayerSound XAniAction::GetspLayerSound( int nLayer )
// {
// 	return std::static_pointer_cast<XLayerSound>( 
// 		GetspLayer( xSpr::xLT_SOUND, nLayer ) );
// }
// 
// XSPLayerEvent XAniAction::GetspLayerEvent( int nLayer )
// {
// 	return std::static_pointer_cast<XLayerEvent>(
// 		GetspLayer( xSpr::xLT_EVENT, nLayer ) );
// }

// void XAniAction::AddLayer( XSPBaseLayer spNewLayer, BOOL bCalcPriority/* = TRUE */ )
// {
// 	if( XBREAK( spNewLayer == nullptr ) )
// 		return;
// 	if( m_listLayer.size() ) {
// // 		XBaseLayer_Itor itorEnd = m_listLayer.end();
// // 		SPBaseLayer spLast = ( *--itorEnd );
// 		auto sppLast = m_listLayer.GetLast();
// 		if( XASSERT(sppLast) ) {
// 			if( bCalcPriority )		// 프라이오리티를 건드리지 않는 옵션
// 				spNewLayer->SetfPriority( (*sppLast)->GetfPriority() + 1.0f );
// 		}
// 	} else {
// 		if( bCalcPriority )
// 			spNewLayer->SetfPriority( 1.0f );
// 	}
// 	m_listLayer.push_back( spNewLayer );
// }
// void XAniAction::AddLayerInfo( const xLayerInfo& layerInfo )
// {
// 	m_listLayerInfo.Add( layerInfo );
// }
/**
 @brief spNewLayer를 리스트에 추가한다.
 @param idLayer 지정한 id로 세팅한다. 0이면 자동으로 생성한다.
*/
// void XAniAction::AddLayer( ID idLayer, XSPBaseLayer spNewLayer )
// {
// 	if( idLayer == 0 )
// 		idLayer = GenerateIDLayer();
// 	int nLayer = GenerateMaxLayer( spNewLayer->GetType() );
// 	spNewLayer->SetidLayer( idLayer );
// 	spNewLayer->SetnLayer( nLayer );
// 	AddLayer( spNewLayer, TRUE );
// }

// xSPLayerImage XAniAction::AddImgLayer()
// {
// 	auto spLayer = CreateAddLayer( xSpr::xLT_IMAGE, -1 );
// 	return std::static_pointer_cast<XLayerImage>( spLayer );
// // 	SPBaseLayer spLayer = CreateLayer( 0, XBaseLayer::xIMAGE_LAYER );
// // 	spLayer->SetidLayer( m_pAction->AddLayerInfo( spLayer->GetType(), spLayer->GetnLayer(), 0 )->idLayer );
// // 	return spLayer;
// }
// XSPLayerObject XAniAction::AddObjLayer()
// {
// 	auto spLayer = CreateAddLayer( xSpr::xLT_OBJECT, -1 );
// 	return std::static_pointer_cast<XLayerObject>( spLayer );
// // 	SPBaseLayer spLayer = CreateLayer( 0, XBaseLayer::xOBJ_LAYER );
// // 	spLayer->SetidLayer( m_pAction->AddLayerInfo( spLayer->GetType(), spLayer->GetnLayer(), 0 )->idLayer );
// //	return spLayer;
// }
// XSPLayerSound XAniAction::AddSndLayer()
// {
// 	auto spLayer = CreateAddLayer( xSpr::xLT_SOUND, -1 );
// 	return std::static_pointer_cast<XLayerSound>( spLayer );
// //	SPBaseLayer spLayer = CreateLayer( 0, XBaseLayer::xSOUND_LAYER );
// // 	spLayer->SetidLayer( m_pAction->AddLayerInfo( spLayer->GetType(), spLayer->GetnLayer(), 0 )->idLayer );
// }
// XSPLayerEvent XAniAction::AddEventLayer()
// {
// 	auto spLayer = CreateAddLayer( xSpr::xLT_EVENT, -1 );
// 	return std::static_pointer_cast<XLayerEvent>( spLayer );
// // 	SPBaseLayer spLayer = CreateLayer( 0, XBaseLayer::xEVENT_LAYER );
// // 	spLayer->SetidLayer( m_pAction->AddLayerInfo( spLayer->GetType(), spLayer->GetnLayer(), 0 )->idLayer );
// // 	return spLayer;
// }
// /**
//  @brief 
// */
// XSPLayerDummy XAniAction::AddDummyLayer() 
// {
// 	// idLayer를 0으로 넣으면 XSprDat::GenerateIDLayer()로 순차적으로 자동생성했었음.
// 	// 더미레이어를 추가
// 	auto spLayer = CreateAddLayer( xSpr::xLT_DUMMY, -1 ); // fuck nLayer를 꼭 수동으로 넣어줘야 하나?
// 	return std::static_pointer_cast<XLayerDummy>( spLayer );
// 	// 더미레이어의 앵커를 애니메이션뷰에서 쓸수있도록 등록
// // 	XLayerDummy *spLayer = SafeCast<XLayerDummy*>( CreateLayer( 0, XBaseLayer::xDUMMY_LAYER ) );
// // 	spLayer->SetidLayer( m_pAction->AddLayerInfo( spLayer->GetType(), spLayer->GetnLayer(), 0 )->idLayer );
// //	GetAnimationView()->RegisterMI( spLayer->GetpMICrossDir() ); // 이코드 밖으로 못뺌?
// }

// void XAniAction::DelLayer( XSPBaseLayer spLayerDel, BOOL bDelLayerOnly /*= FALSE*/ )
// {
// 	BOOL bErase = FALSE;
// 	for( auto itor = m_listLayer.begin(); itor != m_listLayer.end(); ) {
// 		auto spLayer = (*itor);
// 		if( spLayer->getid() == spLayerDel->getid() ) {
// 			// 알맹이까지 삭제해야한다면
// 			if( !bDelLayerOnly ) {		
// 				bErase = DelLayerLinkKey( spLayer );		// 이 레이어에 속해있는 모든키를 삭제해야 한다
// 				//delete ( *itor );			// 알맹이 레이어도 삭제
// 				// 레이어 순서바꾸기에서는 레이어노드만 삭제하고 다시 삽입하기때문에 레이어를 삭제하면 안된다. bDelLayerOnly안에서 삭제하는게 맞음
// 			}
// 			// 레이어순서바꾸기에선 기존엔 리스트에서만 빼고 다시 새위치에 삽입했지만
// 			// 지금은 shared_ptr이므로 erase하면 바로 삭제가 되어버리므로 삭제/삽업 순서를 바꿔야 한다.
// 			// 삽입을 먼저하고 삭제를 시켜야함.
// 			XBREAK( bDelLayerOnly );	// 이경우 기존코드에선 레이어가 파괴되지 않았는데 지금은 shared_ptr이라 자동파괴되므로 파괴안되도록 수정할것.
// 			m_listLayer.erase( itor++ );			// 레이어 노드 삭제
// 			break;
// 		} else
// 			++itor;
// 	}
// 	if( bErase ) {			// 키를 삭제했는가
// 		// 레이어 내용 초기화
// 		ClearLayer();
// 	}
// }
// 
// void XAniAction::ClearLayer()
// {
// 	for( auto spLayer : m_listLayer )
// 		spLayer->Clear();
// }

// pSrc를 pDst가 있는 노드앞에 삽입한다.
// pDst가 널이면 맨뒤에 추가한다
// void XAniAction::InsertLayer( XSPBaseLayer spDst, XSPBaseLayer spSrc )
// {
// 	if( !spSrc )	
// 		return;
// 	XSPBaseLayer spPrevLayer;
// 	// 끼워넣을 타겟레이어의 이터레이터를 찾음.
// // 	auto itorDst = m_listLayer.GetItorByID( spDst->getid() );
// // 	// 못찾았으면 그냥 맨뒤에다 넣는다.
// // 	if( itorDst == m_listLayer.end() ) {
// // 		AddLayer( spSrc );
// // 	} else
// // 	// 타겟레이어가 첫번째 레이어면 Priority값의 절반을 새레이어의 priority값으로 쓴다
// // 	// fuck Priority쓰이는데 있는지 찾아보고 필요없으면 가급적 빼자
// // 	if( itorDst == m_listLayer.begin() ) {
// // 		pSrc->SetfPriority( spDst->GetfPriority() / 2.0f );
// // 	} else {
// // 
// // 	}
// 
// 	LIST_LOOP_AUTO( m_listLayer, itor, spLayer ) {
// 		if( spLayer->getid() == spDst->getid() ) {
// 			// 맨앞에 레이어면 Priority값의 절반을 새레이어의 priority값으로 쓴다
// 			// fuck Priority쓰이는데 있는지 찾아보고 필요없으면 가급적 빼자
// 			if( m_listLayer.begin() == itor ) {
// 				spSrc->SetfPriority( spLayer->GetfPriority() / 2.0f );	
// 			} else {
// 				auto prev = spPrevLayer->GetfPriority();
// 				auto curr = spLayer->GetfPriority();
// 				// 다음키와 pDst키 사이의 중간값을 계산해서 삽입될 노드의 우선순위로 쓴다
// 				spSrc->SetfPriority( prev + (curr - prev) / 2.0f );		
// 			}
// 			m_listLayer.insert( itor, spSrc );
// 			return;
// 		}
// 		spPrevLayer = spLayer;
// 	} END_LOOP;
// 	// pDst를 찾지못했으면 맨뒤에다가 추가시킨다
// 	if( spPrevLayer == nullptr )		
// 		AddLayer( spSrc );
// 	//
// 	//UpdateLayersPos();	// 이제 레이어 스왑할때 삽입먼저하고 삭제하는걸로 바껴서 삭제후에 Update를 하기위해 빠짐
// }

/**
 @brief 두 레이어의 위치를 바꾼다.
 @return 실패하면 false를 리턴한다.
*/
// bool XAniAction::SwapLayer( XSPBaseLayer spLayer1, XSPBaseLayer spLayer2 )
// {
// 	bool bRet = true;
// 	auto& itor1 =std::find( m_listLayer.begin(), m_listLayer.end(), spLayer1 );
// 	auto& itor2 = std::find( m_listLayer.begin(), m_listLayer.end(), spLayer2 );
// 	std::swap( *itor1, *itor2 );
// 	
// 	return bRet;
// }

/**
 각 레이어의 y좌표를 미리 계산한다.
*/
// void XAniAction::UpdateLayersPos( float hKey )
// {
// 	float y = 0;
// 	LIST_LOOP_AUTO( m_listLayer, itor, spLayer ) {
// 		spLayer->Setscry( y );
// 		y += hKey;
// 		if( spLayer->GetbOpen() ) {
// 			y += hKey;				// 기본적으로 pos채널은 열린다
// 			// 트랜스폼 가능한 레이어일경우
// 			if( spLayer->IsTypeLayerMove() )	{
// 				// 회전 채널을 쓰는 키가 있으면 한줄 더 내린다
// 				if( FindKey( spLayer, xSpr::xKT_NONE, xSpr::xKTS_ROT ) )	{
// 					y += hKey;
// 					spLayer->SetBitDrawChannel( CHA_ROT );		
// 				} else
// 					spLayer->ClearBitDrawChannel( CHA_ROT );		
// 				// 확축 채널을 쓰는 키가 있으면 한줄 더 내린다
// 				if( FindKey( spLayer, xSpr::xKT_NONE, xSpr::xKTS_SCALE ) ) {
// 					y += hKey;
// 					spLayer->SetBitDrawChannel( CHA_SCALE );		
// 				} else
// 					spLayer->ClearBitDrawChannel( CHA_SCALE );		
// 				// 이펙트 채널을 쓰는 키가 있으면 한줄 더 내린다
// 				if( FindKey( spLayer, xSpr::xKT_NONE, xSpr::xKTS_EFFECT ) ) {
// 					y += hKey;
// 					spLayer->SetBitDrawChannel( CHA_EFFECT );		// 이펙트 채널을 그려야 함을 알림
// 				} else
// 					spLayer->ClearBitDrawChannel( CHA_EFFECT );		// 이펙트 채널을 그려야 함을 알림
// 			}
// 		}
// 	} END_LOOP;
// }
// 
/**
 @brief 레이어를 생성한다.
 @param nLayer -1이면 레이어번호를 자동생성한다
*/
// XSPBaseLayer XAniAction::CreateAddLayer( xSpr::xtLayer type, 
// 									int nLayer, 
// 									const XE::VEC2& vAdjAxis )
// {
// 	XBREAK( !type );
// 	XBREAK( nLayer < -1 );
// 	XSPBaseLayer spBaseLayer;
// 	// 이거 나중에 XBaseLayer의스태틱 함수로 집어넣자. 안그러면 레이어종류가 추가될때마다 일일히 이런곳 찾아서 수정해줘야 한다
// 	switch( type ) {
// 	case xSpr::xLT_IMAGE: {
// 		auto spLayer = xSPLayerImage( new XLayerImage );
// 		spLayer->SetAdjustAxis( vAdjAxis );
// 		spBaseLayer = spLayer;
// 	} break;
// 	case xSpr::xLT_OBJECT: {
// 		auto spLayer = XSPLayerObject( new XLayerObject );
// 		spLayer->SetAdjustAxis( vAdjAxis );
// 		spBaseLayer = spLayer;
// 	} break;
// 	case xSpr::xLT_SOUND: {
// 		auto spLayer = XSPLayerSound( new XLayerSound );
// 		spBaseLayer = spLayer;
// 	} break;
// 	case xSpr::xLT_EVENT: {
// 		auto spLayer = XSPLayerEvent( new XLayerEvent );
// 		spBaseLayer = spLayer;
// 	} break;
// 	case xSpr::xLT_DUMMY: {
// 		auto spLayer = XSPLayerDummy( new XLayerDummy );
// 		spLayer->SetAdjustAxis( vAdjAxis );
// 		spBaseLayer = spLayer;
// 	} break;
// 	default:
// 		spBaseLayer = nullptr;
// 		XBREAK(1);
// 		XALERT( "XAniAction::CreateLayer 생성타입이 이상함. %d", (int)type );
// 		break;
// 	}
// 	//
// 	//
// 	if( nLayer == -1 )
// 		spBaseLayer->SetnLayer( GenerateMaxLayer( type ) );		// 레이어번호 자동생성
// 	else
// 		spBaseLayer->SetnLayer( nLayer );	 // 지정한 레이어번호로 생성
// 	AddLayer( spBaseLayer );
// 	ID idLayer = GenerateIDLayer();
// 	spBaseLayer->SetidLayer( idLayer );
// 	return spBaseLayer;
// }

// void XAniAction::SortLayer()
// {
// 	m_listLayer.sort( CompLayer );
// 	UpdateLayersPos();
// }

/**
 각 레이어의 y좌표를 미리 계산한다.
*/
// void XAniAction::UpdateLayersPos()
// {
// 	float y = 0;
// 	for( auto spLayer : m_listLayer ) {
// 		spLayer->Setscry( y );
// 		y += KEY_HEIGHT;
// 		if( spLayer->GetbOpen() ) {
// 			y += KEY_HEIGHT;				// 기본적으로 pos채널은 열린다
// //			if( spLayer->GetType() == XBaseLayer::xIMAGE_LAYER )		// 이미지 레이어일경우
// 			// 트랜스폼 가능한 레이어일경우
// 			if( spLayer->IsTypeLayerMove() )	{
// 				// 회전 채널을 쓰는 키가 있으면 한줄 더 내린다
// 				if( FindKey( spLayer, xSpr::xKT_NONE, xSpr::xKTS_ROT ) )	{
// 					y += KEY_HEIGHT;
// 					spLayer->SetBitDrawChannel( CHA_ROT );		
// 				} else
// 					spLayer->ClearBitDrawChannel( CHA_ROT );		
// 				// 확축 채널을 쓰는 키가 있으면 한줄 더 내린다
// 				if( FindKey( spLayer, xSpr::xKT_NONE, xSpr::xKTS_SCALE ) ) {
// 					y += KEY_HEIGHT;
// 					spLayer->SetBitDrawChannel( CHA_SCALE );		
// 				} else
// 					spLayer->ClearBitDrawChannel( CHA_SCALE );		
// 				// 이펙트 채널을 쓰는 키가 있으면 한줄 더 내린다
// 				if( FindKey( spLayer, xSpr::xKT_NONE, xSpr::xKTS_EFFECT ) ) {
// 					y += KEY_HEIGHT;
// 					spLayer->SetBitDrawChannel( CHA_EFFECT );		// 이펙트 채널을 그려야 함을 알림
// 				} else
// 					spLayer->ClearBitDrawChannel( CHA_EFFECT );		// 이펙트 채널을 그려야 함을 알림
// 			}
// 		}
// 	}
// }

// 액션을 클릭해서 선택하면 각 레이어에 진입/나옴 이벤트를 보낸다
// void XAniAction::SendLayerEvent_OnSelected( int nEvent )
// {
// 	for( auto& spLayer : m_listLayer ) {
// 		spLayer->OnSelected( nEvent );
// 	}
// }
// 
// XSPBaseLayer XAniAction::GetLastLayer()
// {
// 	if( m_listLayer.size() == 0 )
// 		return XSPBaseLayer();
// 	auto itor = m_listLayer.end();
// 	return ( *--itor );
// }
// 
// XSPBaseLayer XAniAction::GetFirstLayer()
// {
// 	if( m_listLayer.size() == 0 )
// 		return XSPBaseLayer();
// 	auto itor = m_listLayer.begin();
// 	return ( *itor );
// }
// void XAniAction::FrameMove( float dt, float fFrmCurr, XSprObj *pSprObj )
// {
// 	for( auto& spLayer : m_listLayer )
// 		spLayer->FrameMove( dt, fFrmCurr, pSprObj );
// }
// void XAniAction::MoveFrame( float fFrmCurr )
// {
// 	for( auto& spLayer : m_listLayer )
// 		spLayer->MoveFrame( fFrmCurr );
// }
// 
// void XAniAction::Draw( const D3DXMATRIX &m )
// {
// 	for( auto& spLayer : m_listLayer ) {
// 		if( spLayer->GetbShow() )	// show상태에서만 레이어를 그린다
// 			spLayer->Draw( 0, 0, m );
// 	}
// }

// lx, ly는 0,0센터 기준 로컬좌표
// DWORD XAniAction::GetPixel( float cx, float cy, float mx, float my, const D3DXMATRIX &m, BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb )
// {
// 	DWORD pixel;
// 	auto itor = m_listLayer.end();
// 	--itor;
// 	while( 1 ) {
// 		auto& spLayer = (*itor);
// 		pixel = spLayer->GetPixel( cx, cy, mx, my, m, pa, pr, pg, pb );
// 		if( *pa > 0 )
// 			return pixel;			// 투명색이 아니면 픽셀이 있는것으로 간주하고 그 픽셀을 리턴한다
// 		if( itor == m_listLayer.begin() )
// 			break;
// 		--itor;
// 	}
// 	return 0;
// }

// lx, ly는 0,0센터 기준 로컬좌표
// 해당좌표에 색깔이 있는 레이어가 있는지 찾아낸다
// XSPBaseLayer XAniAction::GetLayerInPixel( float cx, float cy, float mx, float my, const D3DXMATRIX &m )
// {
// 	auto itor = m_listLayer.end();
// 	--itor;
// 	while( 1 ) {
// 		auto& spLayer = ( *itor );
// 		BYTE a = 0;
// 		spLayer->GetPixel( cx, cy, mx, my, m, &a );
// 		if( a > 0 )
// 			return ( *itor );
// 		if( itor == m_listLayer.begin() )
// 			break;
// 		--itor;
// 	}
// 	return XSPBaseLayer();
// }
// 
// XSPBaseLayer XAniAction::GetLayerInPixel( const D3DXMATRIX& mCamera, const D3DXVECTOR2& mv )
// {
// 	auto itor = m_listLayer.end();
// 	--itor;
// 	while( 1 ) {
// 		auto& spLayer = ( *itor );
// 		BYTE a = 0;
// 		if( spLayer->GetbShow() ) {
// 			spLayer->GetPixel( mCamera, mv, &a );
// 			if( a > 0 )
// 				return ( *itor );
// 		}
// 		if( itor == m_listLayer.begin() )
// 			break;
// 		--itor;
// 	}
// 	return XSPBaseLayer();
// }
// 
// // 키가 하나도 없는 레이어를 찾아 삭제하라
// void XAniAction::DoDelEmptyLayer()
// {
// 	//LAYER_MANUAL_LOOP( itor )
// 	LIST_MANUAL_LOOP_AUTO( m_listLayer, itor, spLayer ) {
// 		if( IsEmptyLayer( spLayer ) ) {		// 빈레이어면 
// //			DelLayerInfo( spLayer );		// LayerInfo먼저 삭제
// 			DelLayer( spLayer, FALSE );
// 			// 이터레이터 꼬일지 모르겠군.
// //			SAFE_DELETE( spLayer );					// Layer삭제
// 			m_listLayer.erase( itor++ );		// 레이어 리스트 삭제
// 		} else
// 			++itor;
// 	} END_LOOP;
// }
/**
 @brief 각 레이어타입들의 최대 인덱스값을 얻어서 테이블에 넣는다.
*/
void XAniAction::SetMaxIndexByLayerType( const std::vector<xLayerInfo>& aryLayerInfo )
{
	for( auto& spLayerInfo : aryLayerInfo ) {
		auto maxLayer = GetMaxLayer( spLayerInfo.m_bitType  );
		if( spLayerInfo.m_nLayer >= maxLayer )
			SetMaxLayerNumber( spLayerInfo.m_bitType, spLayerInfo.m_nLayer + 1 );
//			m_nMaxLayer[spLayerInfo.m_bitType] = spLayerInfo.m_nLayer + 1;
	}
}
/**
@brief 각 레이어를 돌며 가장큰 idLayer값을 얻어 그것의 +1로 seed값을 쓴다.
*/
void XAniAction::SetMaxidLayer( const std::vector<xLayerInfo>& aryLayerInfo )
{
	ID idMax = 0;
	for( auto& spLayerInfo : aryLayerInfo ) {
		if( spLayerInfo.m_idLayer > idMax )
			idMax = spLayerInfo.m_idLayer;
	}
	if( idMax )
		m_idLayerGlobal = idMax + 1;
}
// 
// void XAniAction::CalcBoundBox( float dt, float fFrmCurr, XSprObj *pSprObj )
// {
// 	for( auto& spLayer : m_listLayer ) {
// 		spLayer->CalcBoundBox( dt, fFrmCurr, pSprObj );
// 	}
// }
// /**
//  @brief 
// */
// void XAniAction::DrawLayerName( float left, float top, XSPBaseLayer spPushLayer )
// {
// 	D3DCOLOR colWhite = XCOLOR_WHITE;
// 	auto spSelected = TOOL->GetspSelLayer();
// 	for( auto& spLayer : m_listLayer ) {
// 		bool bHide = spLayer->GetbShow() == FALSE;
// 		XCOLOR col = XCOLOR_WHITE;
// 		// 레이어가 눌린상태면 바탕색 다르게
// 		if( spPushLayer &&spPushLayer->getid() == spLayer->getid() ) {
// 			XE::FillRectangle( 0, top + spLayer->Getscry() + 1, TRACK_X, top + spLayer->Getscry() + 1 + KEY_HEIGHT, XCOLOR_WHITE );	
// 		}
// 		// + 표시
// 		if( spLayer->GetbAbleOpen() ) {
// 			float top2 = top + spLayer->Getscry() + 5;
// 			XE::DrawRectangle( 0, top2, 8, top2+8, XCOLOR_WHITE );							// 박스
// 			XE::DrawLine( 0, top2+4, 8, top2+4, XCOLOR_WHITE );		// 가로선
// 			if( spLayer->GetbOpen() == FALSE ) {	// 닫혀있는상태 +표시
// 				XE::DrawLine( 0+4, top2, 0+4, top2+8, XCOLOR_WHITE );		// 세로선
// 			}
// 		}
// 		// label 표시
// 		if( spPushLayer && spPushLayer->getid() == spLayer->getid() )
// 			col = XCOLOR_BLACK;
// 		else {
// 			if( bHide )
// 				col = XCOLOR_LIGHTGRAY;
// 			else
// 			if( spSelected && spLayer->getid() == spSelected->getid() )
// 				col = XCOLOR_YELLOW;
// 		}
// 		spLayer->DrawLabel( left, top, col );
// 	}
// }

/**
 @brief 주어진 좌표로 어느레이어를 눌렀는지 얻는다.
 @param ly TOOLBAR_HEIGHT+KEY_HEIGHT를 뷰좌표에서 뺀 로컬좌표다
 @param h TOOLBAR_HEIGHT+KEY_HEIGHT를 뷰좌표에서 뺀 로컬좌표다
 보너스로 어느 서브레이어를 선택한건지도 pOutSubType에 담아준다
*/
// XSPBaseLayer XAniAction::CheckPushLayer( float lx, float ly
// 																			, float h
// 																			, xSpr::xtLayerSub *pOutSubType )
// {
// 	if( pOutSubType )
// 		*pOutSubType = xSpr::xLTS_NONE;
// 	float ori_h = h;
// 	for( auto& spLayer : m_listLayer ) {
// 		h = ori_h;
// 		if( h == 0 ) {
// 			// 레이어가 열려있으면
// 			if( spLayer->GetbOpen() ) {
// 				h = KEY_HEIGHT * 2;			// 기본으로 두줄을 검사하고
// 				if( spLayer->GetBitDrawChannel( CHA_ROT ) )		
// 					h += KEY_HEIGHT;			
// 				if( spLayer->GetBitDrawChannel( CHA_SCALE ) )		
// 					h += KEY_HEIGHT;			
// 				if( spLayer->GetBitDrawChannel( CHA_EFFECT ) )		// 이펙트 채널을 그리고 있으면
// 					h += KEY_HEIGHT;			// 한줄 더 늘인다
// 			} else 									
// 				h = KEY_HEIGHT;			// 레이어가 닫혀있으면 한줄만 검사하면 된다
// 		}
// 		if( ly >= spLayer->Getscry() && ly < spLayer->Getscry()+h ) {
// 			if( pOutSubType && ori_h == 0 ) {
// 				if( spLayer->GetbOpen() ) {
// 					int nArea = (int)(( ly - spLayer->Getscry() ) / KEY_HEIGHT);
// 					switch( nArea ) {
// 					case 0:	*pOutSubType = xSpr::xLTS_MAIN;	break;		// 레이어의 첫번째줄
// 					case 1:	*pOutSubType = xSpr::xLTS_POS;	break;		
// 					case 2:	*pOutSubType = xSpr::xLTS_ROT;	break;		
// 					case 3:	*pOutSubType = xSpr::xLTS_SCALE;	break;		
// 					case 4:	*pOutSubType = xSpr::xLTS_EFFECT;	break;		
// 					}
// 				}
// 			}
// 			return spLayer;
// 		}
// 	}
// 	return nullptr;
// }
/**
 @brief spLayerSrc를 카피해서 새로운 레이어를 생성한다.
*/
// XSPBaseLayer XAniAction::CreateLayerFromOtherLayer( XSPBaseLayer spLayerSrc )
// {
// 	auto spLayerCopy = spLayerSrc->CopyDeep();
// 	spLayerCopy->SetidLayer( GenerateIDLayer() );
// 	return spLayerCopy;
// }
// /**
//  @brief 인자로 주어진 레이어를 제외한 나머지 레이어를 감춘다.
// */
// void XAniAction::SetShowLayerExcept( XSPBaseLayer spExceptLayer )
// {
// 	for( auto& spLayer : m_listLayer ) {
// 		if( !spLayer->IsSameLayer( spExceptLayer ) )
// 			spLayer->SetbShow( false );
// 	}
// }
// /**
//  @brief 모든 레이어를 보이게 한다.
// */
// void XAniAction::SetShowLayerAll()
// {
// 	for( auto& spLayer : m_listLayer ) {
// 			spLayer->SetbShow( TRUE );
// 	}
// }

// void XAniAction::SetLayerInfo( const XList4<xLayerInfo>& listLayerInfo )
// {
// 	m_listLayerInfo = listLayerInfo;
// }

void XAniAction::UpdateBoundBox( const XE::VEC2& v ) 
{
	if( v.x < m_vBoundBox[ 0 ].x )		// vLT
		m_vBoundBox[ 0 ].x = v.x;
	if( v.y < m_vBoundBox[ 0 ].y )
		m_vBoundBox[ 0 ].y = v.y;
	if( v.x > m_vBoundBox[ 1 ].x )		// vRB
		m_vBoundBox[ 1 ].x = v.x;
	if( v.y > m_vBoundBox[ 1 ].y )
		m_vBoundBox[ 1 ].y = v.y;
}

D3DXMATRIX XAniAction::GetLocalMatrix() const 
{
	D3DXMATRIX mTrans, mScale, mRot;
	D3DXMatrixTranslation( &mTrans, 0, 0, 0 );		// 현재는 사용안함.
	D3DXMatrixRotationYawPitchRoll( &mRot, D3DXToRadian( m_vRotate.y ), D3DXToRadian( m_vRotate.x ), D3DXToRadian( m_vRotate.z ) );
	D3DXMatrixScaling( &mScale, m_vScale.x, m_vScale.y, 1.0f );
	// 좌표축 옮겨놓고 * 축소하고 * 회전하고 * 다시 좌표축 돌려놓고 * 원하는 스크린위치에다 옮김
	const auto mLocal = mScale * mRot * mTrans;
	return mLocal;
}

// /**
//  @brief 타입과 레이어번호로 idLayer를 얻는다.(구버전 코드의 잔재)
// */
// ID XAniAction::GetidLayer( xSpr::xtLayer typeLayer, int nLayer ) const
// {
// 	for( auto& layerInfo : m_listLayerInfo ) {
// 		if( layerInfo.IsSame( typeLayer, nLayer ) )
// 			return layerInfo.m_idLayer;
// 	}
// 	return 0;
// }
// 
// void XAniAction::DelLayerInfo( ID idLayer )
// {
// 	m_listLayerInfo.DelByIDNonPtr( idLayer );
// }
// 

#include "XActObj.h"
/**
 @brief 29미만 버전을 위해 idLocalKeyInLayer값을 모든 키에 넣어준다.
*/
void XAniAction::SetidLocalKeyInLayerToKeys( XSPActObj spActObj )
{
	for( auto pKey : m_listKey ) {
		XASSERT( pKey->GetidLocalInLayer() == 0 );
		const ID idLayer = pKey->GetidLayer();
		auto spLayer = spActObj->GetspLayerByidLayer( idLayer );
		if( XASSERT(spLayer) ) {
			const auto idLocalKeyInLayer = spLayer->GenerateidKeyLocal();
			pKey->SetidLocalInLayer( idLocalKeyInLayer );
		}
	}
}

/**
 @brief 세이브용 레이어정보 버퍼를 옮겨온다.
 @warning 오직 세이브를 위해서만 쓰여야 한다.
*/
// void XAniAction::SetLayerInfoForSave( const std::vector<xLayerInfo>& aryLayerInfo )
// {
// 	m_aryLayerInfoForSave = aryLayerInfo;
// }

/**
 @brief 프레임값을 시간단위로 변환한다.
*/
float XAniAction::GetFrameToSec( float fFrame ) const
{
// 	speed1.0 => 60fps => 1.0sec
// 	speed0.2 => 60 / 5 = 60 * 0.2 = 12 = 12/60
//   frame3.7 => 37f/60 => 0.616sec
//	 0.616sec => 0.616*60 => frame3.7
// 	const auto speed = m_fSpeed
	return 0;
}

void XAniAction::InitKeysRandom()
{
	for( auto pKey : m_listKey ) {
		pKey->InitRandom();
	}
}
