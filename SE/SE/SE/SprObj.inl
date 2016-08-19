#include "SprObj.h"
#include "XActObj.h"

template<typename T>
std::shared_ptr<T> XSprObj::GetspLayerByidLayer( ID idLayer ) const 
{
	if( m_spActObjCurr == nullptr )
		return nullptr;
	return m_spActObjCurr->GetspLayerByidLayer<T>( idLayer );
}
