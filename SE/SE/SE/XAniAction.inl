#include "XAniAction.h"

template<typename T>
int XAniAction::GetKeysToAryByChannel( xSpr::xtKeySub typeSub, ID idLayer, XList4<T> *pOut ) 
{
	XASSERT( pOut->size() == 0 );
	float secFramePrev = -1.f;
	for( auto pKey : m_listKey ) {
		if( pKey->GetSubType() == typeSub && pKey->GetidLayer() == idLayer ) {
			// 키들은 순서대로 소트된 상태여야 한다.
			XBREAK( pKey->GetfFrame() < secFramePrev );
			secFramePrev = pKey->GetfFrame();
			T pKeyType = SafeCast<T>( pKey );
			if( XASSERT(pKeyType) )
				pOut->Add( pKeyType );
		}
	}
	return pOut->size();
}

template<typename T>
T* XAniAction::FindKeyByidLayer( ID idLayer, float frame ) const {
	for( auto pKey : m_listKey ) {
		if( XASSERT( pKey->GetidLayer() ) ) {
			if( pKey->GetSubType() == T::sGetChannelType() )
				if( pKey->GetidLayer() == idLayer )
					if( frame < 0 || pKey->GetfFrame() == frame )
						return static_cast<T*>(pKey);
		}
	}
	return nullptr;
}

/**
 @brief idLayer에서 frame과 가장 가까이 있는 키를 찾는다. 채널은 같지 않아도 된다.

*/
template<typename T>
T* XAniAction::FindKeyByidLayerNear( ID idLayer, float frame ) const {
	XBaseKey* pKeyNear = nullptr;
	for( auto pKey : m_listKey ) {
		if( XASSERT( pKey->GetidLayer() ) ) {
			if( pKey->GetidLayer() == idLayer ) {
				if( pKey->GetfFrame() == frame ) {
					pKeyNear = pKey;
				}
			}
		}
	}
	return static_cast<T*>( pKeyNear );
}
