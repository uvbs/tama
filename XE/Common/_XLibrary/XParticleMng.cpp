#include "stdafx.h"
#include "XParticleMng.h"
#include "etc/xGraphics.h"
#include "XFramework/client/XPropParticle.h"
#include "Sprite/SprObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xParticle;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//XPointSpriteParticleMng *PARTICLE_MNG = NULL;

XPointSpriteParticleMng::XPointSpriteParticleMng( XParticleDelegate *pDelegate, int maxParticle ) 
	: XBaseParticleMng( pDelegate, maxParticle ) 
{ 
	Init(); 
	m_pBuffer = new XPointSpriteParticle::xOUT[ maxParticle ];		// 포인트 스프라이트용 draw버퍼
}
void XBaseParticleMng::DestroyAll( void ) 
{
	XLIST2_LOOP( m_listParticle, XBaseParticle*, pObj )
//	LIST_LOOP( m_listParticle, XBaseParticle*, itor, pObj )
	{
		SAFE_DELETE( pObj );
	} END_LOOP;
//	XLIST2_DESTROY( m_listParticle, XBaseParticle* );
	m_listParticle.Clear();
//	m_listParticle.clear();
	XLIST4_DESTROY( m_listEmitter );
}

void XBaseParticleMng::FrameMove( float dt )
{
	XLIST2_MANUAL_LOOP( m_listParticle, XBaseParticle*, itor, pNode ) {
		// 파괴된 노드는 삭제시키고 FrameMove를 처리하지 않는다.
		if( pNode->IsDestroy() ) {
			SAFE_DELETE( pNode );
			m_listParticle.Delete( itor++ );
			continue;
		}
		pNode->FrameMove( dt );
		++itor;
	} END_LOOP;
	//
// 	for( auto pEmitter : m_listEmitter ) {
// 		pEmitter->FrameMove( this, dt );
// 	}
}

void XBaseParticleMng::AddEmitter( XEmitter* pEmitter )
{
	m_listEmitter.Add( pEmitter );
}
//////////////////////////////////////////////////////////////////////////
//
int XPointSpriteParticleMng::Draw( void )
{
	// 텍스쳐 아이디별로소트를 한다.
	// 같은 텍스쳐 아이디끼리 루프를 돌면서 버텍스 버퍼를 만든다.
	// 텍스쳐를 로딩하고
	// 버텍스 버퍼를 밀어넣고
	// 그린다.
	float x, y;
	auto pBuffer = m_pBuffer;
	int num = 0;
	// 파티클들을 디바이스정보만 중간버퍼에 옮긴다.
	XLIST2_LOOP( m_listParticle, XBaseParticle*, pNode )
	{
		auto pPoint = static_cast<XPointSpriteParticle*>( pNode );
		pPoint->Projection( pPoint->GetvPos(), &x, &y );			// 화면좌표를 얻음.
		if( pPoint->IsInScreen( x, y ) )		// 화면 안에 들어온것들만 draw콜을 함.
		{
			if( pPoint->Draw( x, y, pBuffer ) )	// 드로우에 성공해야
			{
				++pBuffer;							// 포인터 증가
				++num;
				if( num >= m_MaxParticle )
					break;
			}
		}
	} END_LOOP;
	// D3D, GL에 따라서 버퍼의 내용을 한꺼번에 그린다.
#ifdef WIN32
	int itor = 0;
	auto pParticle = m_listParticle.GetNext( &itor );
	if( pParticle ) {
		auto pPoint = static_cast<XPointSpriteParticle*>( pParticle );
		XSurface *psfcTexture = pPoint->GetpsfcTexture();
		XBREAK( psfcTexture == NULL );
		XBREAK( num >= m_MaxParticle );
		for( int i = 0; i < num; ++i )
		{
			XPointSpriteParticle::xOUT *p = &m_pBuffer[i];
			psfcTexture->SetColor( p->r, p->g, p->b );
//			psfcTexture->SetfAlpha( p->a );
			psfcTexture->SetfAlpha( 1.0f );
			psfcTexture->SetDrawMode( m_DrawMode );
//			psfcTexture->SetDrawMode( xDM_SCREEN );
			float scale = p->size / (float)psfcTexture->GetTextureWidth();		// 파티클 사이즈가 원래 텍스쳐에 비해 얼마나 큰가,작은가
			psfcTexture->SetScale( scale );
			psfcTexture->Draw( p->x, p->y );
			x = p->x;
			y = p->y;
			XCOLOR col = XCOLOR_RGBA( (BYTE)(p->r * 255), (BYTE)(p->g * 255), (BYTE)(p->b * 255), (BYTE)(p->a * 255) );
		}
	}
#else
#endif
    return num;
}

// std::shared_ptr<XParticleMng> XParticleMng::s_spInstance;

//////////////////////////////////////////////////////////////////////////
// std::shared_ptr<XParticleMng>& XParticleMng::sGet() 
// {
// 	if( s_spInstance == nullptr )
// #ifdef _VER_OPENGL
// 		s_spInstance = std::shared_ptr<XParticleMng>( new XParticleMngGL );
// #else
// 		s_spInstance = std::shared_ptr<XParticleMng>( new XParticleMngDX );
// #endif // _VER_OPENGL
// 	return s_spInstance;
// }

////////////////////////////////////////////////////////////////
XParticleMng* XParticleMng::sCreate()
{
#ifdef _VER_OPENGL
	return new XParticleMngGL;
#else
	return new XParticleMngDX;
#endif // _VER_OPENGL
}
/**
 @brief 
*/
XParticleMng::XParticleMng()
{
	Init();
	// 기본 1000로 시작해서 포인트파티클 개수가 1000개를 넘어가면 실시간으로 다시 재할당 받는다.
	m_maxBuffer = 1000;
	m_pBuffer = new XPointSpriteParticle::xOUT[ m_maxBuffer ];		// 포인트 스프라이트용 draw버퍼
}

void XParticleMng::Destroy()
{
	DestroyAll();
	SAFE_DELETE_ARRAY( m_pBuffer );
}
void XParticleMng::DestroyAll( void )
{
	XLIST4_DESTROY( m_listPoints );
	XLIST4_DESTROY( m_listSprs );
	XLIST4_DESTROY( m_listEmitter );
}


void XParticleMng::Add( XPointSpriteParticle* pParticle ) 
{
	m_listPoints.Add( pParticle );
	// 버퍼가 부족하면 재할당 받음.
	if( (int)m_listPoints.size() >= m_maxBuffer ) {
		SAFE_DELETE_ARRAY( m_pBuffer );
		m_maxBuffer *= 2;
		m_pBuffer = new XPointSpriteParticle::xOUT[ m_maxBuffer ];		// 포인트 스프라이트용 draw버퍼
	}

}
void XParticleMng::Add( XParticleSprite* pParticle ) 
{
	m_listSprs.Add( pParticle );
}

void XParticleMng::FrameMove( float dt )
{
	for( auto itor = m_listPoints.begin(); itor != m_listPoints.end(); ) {
		auto pNode = (*itor);
		if( pNode->IsDestroy() ) {
			SAFE_DELETE( pNode );
			m_listPoints.erase( itor++ );
		} else {
			pNode->FrameMove( dt );
			++itor;
		}
	}
	for( auto itor = m_listSprs.begin(); itor != m_listSprs.end(); ) {
		auto pNode = ( *itor );
		if( pNode->IsDestroy() ) {
			SAFE_DELETE( pNode );
			m_listSprs.erase( itor++ );
		} else {
			pNode->FrameMove( dt );
			++itor;
		}
	}
	//
	for( auto itor = m_listEmitter.begin(); itor != m_listEmitter.end(); ) {
		XEmitter* pEmitter = (*itor);
		pEmitter->FrameMove( this, dt );
		if( pEmitter->GetbDestroy() )
			m_listEmitter.erase( itor++ );
		else
			++itor;
	}
}

void XParticleMng::AddEmitter( XEmitter* pEmitter )
{
	m_listEmitter.Add( pEmitter );
}

void XParticleMng::Draw( void )
{

	float scaleViewport = 1.f;
	if( m_pDelegate ) 
		scaleViewport = m_pDelegate->OnDelegateGetScaleViewport();
	// 포인트 파티클 draw
	if( m_listPoints.size() > 0 ) {
		// 포인트파티클리스트를 각 파티클의 텍스쳐 아이디로 소트
		m_listPoints.sort( 
			[](XPointSpriteParticle* pNode1, XPointSpriteParticle* pNode2)->bool {
			if( pNode1 && pNode2 )
				return pNode1->GetidTexture() > pNode2->GetidTexture();
			return false;
		});
		auto pBuffer = m_pBuffer;
		int size = m_listPoints.size();
		int num = 0;
		ID idTexturePrev = 0;
		XSurface *pTexture = nullptr;
		XSurface *pTexturePrev = nullptr;
		XE::xtBlendFunc blendFuncPrev = XE::xBF_MULTIPLY;
		for( auto pNode : m_listPoints ) {
			if( pNode->IsDestroy() )
				continue;
			// 3d좌표를 2d화면좌표로 변환
			const auto vPos = pNode->GetvPos();
			XE::VEC2 vScr = vPos.ToVec2();
			if( m_pDelegate ) 
				vScr = m_pDelegate->OnDelegateProjection2( pNode->GetvPos() );
			else
				vScr = XE::VEC2( vPos.x, vPos.y + vPos.z );		// 기본 프로젝션(2d)
			XE::VEC2 vSize( pNode->GetSize() );
			float scale = pNode->GetScale();
			bool bInScreen = true;
			if( m_pDelegate )
				bInScreen = m_pDelegate->OnDelegateIsInScreen( vScr );
			if( bInScreen ) {
				if( idTexturePrev != pNode->GetidTexture() ) {
					// 이전 노드와 텍스쳐 아이디가 달라지면 
					if( idTexturePrev != 0 ) {
						// 이제까지 쌓인 버퍼를 draw한다.
						DrawPointSprites( num, pTexturePrev, blendFuncPrev );
						// 버퍼 초기화
						pBuffer = m_pBuffer;
						num = 0;
					}
				}
				pBuffer->x = vScr.x;
				pBuffer->y = vScr.y;
				pBuffer->size = vSize.w * pNode->GetScale() * scaleViewport;
				pBuffer->r = pNode->Getr();
				pBuffer->g = pNode->Getg();
				pBuffer->b = pNode->Getb();
				pBuffer->a = pNode->Geta();
				idTexturePrev = pNode->GetidTexture();
				pTexture = pNode->GetpsfcTexture();
				pTexturePrev = pTexture;
				blendFuncPrev = pNode->GetBlendFunc();
				++pBuffer;
				++num;
				XBREAK( num >= m_maxBuffer );
			}
		}
		// 나머지 파티클 draw
		if( pTexture )
			DrawPointSprites( num, pTexture, blendFuncPrev );
	} // point particle draw
	// sprObj파티클 draw
	for( auto pNode : m_listSprs ) {
		if( !pNode->IsDestroy() ) {
			// 3d좌표를 2d화면좌표로 변환
			auto vScr = m_pDelegate->OnDelegateProjection2( pNode->GetvPos() );
			XE::VEC2 vSize = pNode->GetSize();
			float scale = pNode->GetScale();
			if( m_pDelegate->OnDelegateIsInScreen( vScr ) ) {
				pNode->Draw( vScr, scaleViewport );
			}
		}
	}
} // XParticleMng::Draw

/**
 @brief 프로퍼티로부터 파티클 sfx를 읽어들인다.
*/
void XParticleMng::CreateSfx( const char* cIdentifier, const XE::VEC2& vPos )
{
	auto pSfx = XPropParticle::sGet()->GetpProp( cIdentifier );
	if( XBREAK(pSfx == nullptr) ) 
		return;
	for( auto pEmitterProp : pSfx->aryEmitter ) {
		// 만약 이미터프로퍼티에 idxSpr이 여러개라면 최적화때문에 그 개수만큼 이미터를 만들어야함.
		for( auto idxSpr : pEmitterProp->m_aryIdxSpr ) {
			auto pEmitter = new XEmitter( vPos, pEmitterProp, idxSpr );
			if( XASSERT( pEmitter ) ) {
				AddEmitter( pEmitter );
			}
		}
	}
}

xParticle::XCompFunc* 
XParticleMng::sCreateFuncComponent( xParticle::xFunc& funcProp )
{
	switch( funcProp.funcType ) 
	{
	case xParticle::xFN_NONE:
		return nullptr;
	case xParticle::xFN_FIXED:
		return new xParticle::XCompFuncFixed( funcProp.range.x );
	case xParticle::xFN_RANDOM:
		return new xParticle::XCompFuncRandom( funcProp.range.x, 
												funcProp.range.y );
	case xParticle::xFN_LINEAR: {
		XBREAK( funcProp.secCycle == 0 );
		return new xParticle::XCompFuncLinear( funcProp.range.x,
												funcProp.range.y,
												funcProp.secCycle );
	}
	case xParticle::xFN_SIN: {
		XBREAK( funcProp.secCycle == 0 );
		return new xParticle::XCompFuncSin( funcProp.range.x,
											funcProp.range.y,
											funcProp.secCycle );
	}
	default:
		XBREAK(1);
		break;
	}
	return nullptr;
}
////////////////////////////////////////////////////////////////
#ifdef _VER_DX
/**
 @brief 플랫폼별로 포인트 스프라이트 draw를 구현한다.
*/
void XParticleMngDX::DrawPointSprites( int numDraw, XSurface* pTexture, XE::xtBlendFunc blendFunc )
{
	XBREAK( pTexture == nullptr );
	int num = numDraw;
	if( XBREAK(num >= m_maxBuffer) )
		num = m_maxBuffer;
	float scaleTexture = (float)pTexture->GetTextureWidth();
	for( int i = 0; i < num; ++i ) {
		auto p = &m_pBuffer[ i ];
		pTexture->SetColor( p->r, p->g, p->b );
		pTexture->SetfAlpha( p->a );
// 		pTexture->SetfAlpha( 1.0f );
		pTexture->SetBlendFunc( blendFunc );
		pTexture->SetScale( p->size / scaleTexture );
		pTexture->Draw( p->x, p->y );
// 		x = p->x;
// 		y = p->y;
//		XCOLOR col = XCOLOR_RGBA( (BYTE)( p->r * 255 ), (BYTE)( p->g * 255 ), (BYTE)( p->b * 255 ), (BYTE)( p->a * 255 ) );
	}
}
#endif // _VER_DX

