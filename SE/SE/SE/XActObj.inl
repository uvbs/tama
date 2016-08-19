#include "XActObj.h"
#include "XLayerAll.h"

template<typename T>
std::shared_ptr<T> XActObj::AddLayerAuto() {
	auto spLayer = CreateAddLayer(T::sGetType());
	if( XASSERT( spLayer ) ) {
		return std::static_pointer_cast<T>(spLayer);
		// 	SPBaseLayer spLayer = CreateLayer( 0, XBaseLayer::xIMAGE_LAYER );
		// 	spLayer->SetidLayer( m_pAction->AddLayerInfo( spLayer->GetType(), spLayer->GetnLayer(), 0 )->idLayer );
		// 	return spLayer;
	}
		return nullptr;
}

template<typename T>
std::shared_ptr<T> XActObj::GetspLayerByidLayer( ID idLayer ) const {
	auto spBaseLayer = GetspLayerByidLayer( idLayer );
	if( XASSERT( spBaseLayer ) ) {
		if( spBaseLayer->IsSameType( T::sGetType() ) )
			return std::static_pointer_cast<T>(spBaseLayer);
	}
	return nullptr;
}
// 레이어 번호로 찾는 버전.
// template<typename T>
// std::shared_ptr<T> XActObj::GetspLayerByNumber( int nLayer ) const {
// 	auto spBaseLayer = GetspLayerBynLayer( nLayer );
// 	if( XASSERT( spBaseLayer ) ) {
// 		if( spBaseLayer->IsSameType( T::sGetType() ) )
// 			return std::static_pointer_cast<T>(spBaseLayer);
// 	}
// 	return nullptr;
// }

template<typename T>
std::shared_ptr<T> XActObj::GetspLayerByTypeMutable( ID idLayer ) const {
	auto spBaseLayer = GetspLayerMutable( idLayer );
	if( XASSERT( spBaseLayer ) ) {
		if( spBaseLayer->IsSameType( T::sGetType() ) )
			return std::static_pointer_cast<T>(spBaseLayer);
	}
	return nullptr;
}

/**
 @brief idx번째 typeLayer타입의 레이어를 얻는다.
 @param typeLayer xnone일경우 모든 레이어를 상대로 idx번째 레이어를 얻는다.
*/
template<typename T>
std::shared_ptr<T> XActObj::GetspLayerByIndex( /*xSpr::xtLayer typeLayer, */int idx ) const {
	int cnt = 0;
	const auto typeLayer = T::sGetType();
	for( auto& spLayer : m_listLayer ) {
		if( spLayer->GetbitType() == typeLayer ) {
			if( cnt == idx ) {
				return std::static_pointer_cast<T>( spLayer );
			}
		}
// 		if( typeLayer == xSpr::xLT_NONE 
// 			&& (T::sGetType() != xSpr::xLT_NONE && spLayer->GetbitType() == typeLayer) ) {
// 			if( cnt == idx ) {
// 				return std::static_pointer_cast<T>( spLayer );
// 			}
// 		}
		++cnt;
	}
	return nullptr;
}

