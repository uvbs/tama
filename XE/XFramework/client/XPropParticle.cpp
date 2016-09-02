#include "stdafx.h"
#include "XPropParticle.h"
#include "etc/XSurfaceDef.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XE;
using namespace xParticle;
std::shared_ptr<XPropParticle> XPropParticle::s_spInstance;
ID XPropParticle::s_idGlobal = 0;

XPropParticle::xEmitter::xEmitter() 
	: aryFunc( xParticle::xIC_MAX )
	, blendFunc(xBF_MULTIPLY) {
	aryFunc[xParticle::xIC_SPEED].range.Set( 1.f );
	aryFunc[xParticle::xIC_ALPHA].range.Set( 1.f );
	aryFunc[xParticle::xIC_SCALE].range.Set( 1.f );
}

//////////////////////////////////////////////////////////////////////////
std::shared_ptr<XPropParticle>& XPropParticle::sGet() 
{
	if( s_spInstance == nullptr )
		s_spInstance = std::shared_ptr<XPropParticle>( new XPropParticle );
	return s_spInstance;
}


XPropParticle::xSfx::~xSfx()
{
	for( auto pEmitter : aryEmitter ) {
		SAFE_DELETE( pEmitter );
	}
}

////////////////////////////////////////////////////////////////
XPropParticle::XPropParticle()
{
	Init();
}

void XPropParticle::Destroy()
{
	for( auto node : m_mapSfx ) {
		xSfx *pSfx = node.second;
		SAFE_DELETE( pSfx );
	}
	m_mapSfx.clear();
}

bool XPropParticle::Load( LPCTSTR szXml )
{
	Destroy();
	return XXMLDoc::Load( XE::MakePath( DIR_PROP, szXml ) );
}

bool XPropParticle::OnDidFinishLoad()
{
	XEXmlNode nodeRoot = FindNode( "root" );
	if( nodeRoot.IsEmpty() )
		return false;;
	///< 
	XEXmlNode nodeObj = nodeRoot.GetFirst();
	if( XBREAK( nodeObj.IsEmpty() ) ) {
		CONSOLE("particle emitter가 없음.");
		m_bError = true;
	}
	while( !nodeObj.IsEmpty() ) {
		LoadObj( nodeObj );
		// 다음 노드로
		nodeObj = nodeObj.GetNext();
	}
	if( m_bError ) {
		XALERT( "%s:프로퍼티 읽던 중 에러. 콘솔창을 참고하시오.", GetstrFilename().c_str() );
	}
	return !m_bError;
}
/**
 @brief 이미터들의 묶음 객체. 각자 식별 이름을 갖고 있다.
*/
void XPropParticle::LoadObj( XEXmlNode& node )
{
	XEXmlNode nodeChild = node.GetFirst();
	if( nodeChild.IsEmpty() )
		return;
	auto pSfx = new xSfx;
	pSfx->strIdentifier = node.GetstrName();
	while( !nodeChild.IsEmpty() ) {
		auto pEmitter = LoadEmitter( nodeChild );
		if( pEmitter ) {
			pSfx->aryEmitter.push_back( pEmitter );
		}
		nodeChild = nodeChild.GetNext();
	}
	XBREAK( pSfx->aryEmitter.size() == 0 );
	std::string strIdentifer = node.GetcstrName();
	m_mapSfx[ strIdentifer ] = pSfx;
}

/**
 @brief emitter 블럭을 읽어 메모리를 할당해 리턴
*/
XPropParticle::xEmitter* XPropParticle::LoadEmitter( XEXmlNode& node )
{
	auto pEmitter = new xEmitter;
	pEmitter->idProp = sGenerateID();
	pEmitter->vwPos.x = (float)node.GetInt( "x" );
	pEmitter->vwPos.y = (float)node.GetInt( "y" );
	pEmitter->strSpr = node.GetTString( "spr" );
//	pEmitter->idxSpr = node.GetInt( "idx_spr" );
	node.GetAry( "idx_spr", &pEmitter->m_aryIdxSpr );
//	pEmitter->secInterval = node.GetFloat( "interval" );
	pEmitter->blendFunc = GetBlendFunc( node, "blend" );
	if( pEmitter->blendFunc == xBF_NONE )
		pEmitter->blendFunc = xBF_MULTIPLY;
	for( int i = 0; i < xIC_MAX; ++i ) {
		auto idxComp = (xtIdxComp)i;
		auto idsComp = xParticle::GetIdsByIdxComp( idxComp );
		LoadFunc( node, idsComp, &pEmitter->aryFunc[ idxComp ] );
	}
	auto nodeParticle = node.FindNode( "particle" );
	if( !nodeParticle.IsEmpty() )
		LoadParticle( nodeParticle, pEmitter );
	return pEmitter;
}

void XPropParticle::LoadParticle( XEXmlNode& node, xEmitter* pOut )
{
//	pOut->secLife = node.GetFloat( "life" );
// 	LoadFunc( node, "life", &pOut->particle.aryFunc[ xIC_LIFE ] );
// 	LoadFunc( node, "alpha", &pOut->particle.aryFunc[ xIC_ALPHA ] );
// 	LoadFunc( node, "scale", &pOut->particle.aryFunc[ xIC_SCALE ] );
	for( int i = 0; i < xIC_MAX; ++i ) {
		auto idxComp = (xtIdxComp)i;
		auto idsComp = xParticle::GetIdsByIdxComp( idxComp );
		LoadFunc( node, idsComp, &pOut->particle.aryFunc[ idxComp ] );
	}
}

/**
 @brief angle,speed,alpha등 각종 파라메터의 함수와 범위값을 얻는다.
 @param nodeRoot particle노드
*/
int XPropParticle::LoadFunc( XEXmlNode& nodeRoot, const char *cKey, xParticle::xFunc *pOut )
{
	auto node = nodeRoot.FindNode( cKey );
	int ret = 0;
	if( node.IsEmpty() )
		return ret;
	pOut->funcType = xFN_FIXED;		// 디폴트 함수.
	std::string strParam = node.GetString( "func" );
	if( strParam == "random" )
		pOut->funcType = xFN_RANDOM;
	else if( strParam == "linear" )
		pOut->funcType = xFN_LINEAR;
	else if( strParam == "sin" )
		pOut->funcType = xFN_SIN;
	float secCycle = node.GetFloat( "cycle" );
	if( secCycle )
		pOut->secCycle = secCycle;
	// val의 범위값을 얻는다.
	_tstring strVal = node.GetTString( "val" );
	float v1 = 0, v2 = 0;
	CToken token;
	if( token.LoadStr( strVal.c_str() ) ) {
		v1 = token.GetNumberF();
		++ret;
		if( !token.IsEof() ) {
			token.GetToken();	// ,
			if( !token.IsEof() ) {
				v2 = token.GetNumberF();
				++ret;
			}
		}
	}
	if( pOut ) {
		pOut->range.Set( v1, v2 );
	}
	return ret;
}

xtBlendFunc XPropParticle::GetBlendFunc( XEXmlNode& node, const char *cKey )
{
	xtBlendFunc func = xBF_NONE;
	std::string str = node.GetString( cKey );
	if( str == "normal" || str == "multiply" )
		func = xBF_MULTIPLY;
	else if( str == "add" )
		func = xBF_ADD;
	return func;
}