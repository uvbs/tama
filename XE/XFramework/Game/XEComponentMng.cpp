#include "stdafx.h"
#include "XEComponentMng.h"
#include "XEComponents.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
void XEComponentMng::Destroy()
{
	AUTO_DESTROY( m_aryComponent );
}

int XEComponentMng::Process( float dt )
{
	if( m_bActive ) {
		// 시범적으로 콤포넌트 방식을 써봄.
		for( auto pComp : m_aryComponent ) {
// 			if( pComp->IsbActive() )
				pComp->Process( dt );
		}
	}
	return 1;
}

XEComponent* XEComponentMng::AddComponent( XEComponent *pComp )
{
	auto pExist = FindComponentByIds( pComp->GetstrIdentifier() );
	if( pExist )
		return pExist;
	pExist = FindComponentByFunc( pComp->GetstrFunc() );
	if( pExist )
		return pExist;
	m_aryComponent.Add( pComp );
	return pComp;
}

XEComponent* XEComponentMng::FindComponentByFunc( const std::string& strFunc )
{
	XBREAK( strFunc.empty() );
	for( auto pComp : m_aryComponent ) {
		if( pComp->GetstrFunc() == strFunc )
			return pComp;
	}
	return nullptr;
}

XEComponent* XEComponentMng::FindComponentByIds( const std::string& strIdentifier )
{
	if( strIdentifier.empty() )
		return nullptr;
	for( auto pComp : m_aryComponent ) {
		if( pComp->GetstrIdentifier() == strIdentifier )
			return pComp;
	}
	return nullptr;
}

XEComponent* XEComponentMng::AddComponentWave( const std::string& strIdentifier
																							, XDelegateCompWave *pDelegate /*= nullptr*/
																							, float secDelay /*= 1.f*/
																							, float secFade /*= 1.f*/
																							, float minValue
																							, float maxValue )
{
	auto pComp = FindComponentByFunc( strIdentifier );
	if( pComp == nullptr ) {
		auto pCompWave = new XECompWave( strIdentifier, pDelegate, secDelay,secFade );
		pComp = pCompWave;
		pCompWave->SetminValue( minValue );
		pCompWave->SetmaxValue( maxValue );
		AddComponent( pComp );
	}
	return pComp;
}