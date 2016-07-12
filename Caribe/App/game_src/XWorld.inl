#include "XSpots.h"
#include "XWorld.h"

template<typename T>
T XWorld::GetSpot( XGAME::xtSpot typeSpot, LPCTSTR szIdentifier ) 
{
	if( XE::IsEmpty( szIdentifier ) )
		return nullptr;
	for( XSpot *pSpot : m_listSpots ) {
		if( pSpot->GettypeSpot() == typeSpot &&
			pSpot->GetpBaseProp()->strIdentifier == szIdentifier )
			return static_cast<T>( pSpot );
	};
	return nullptr;
}
/**
 @brief 
*/
template<typename T>
T XWorld::GetSpot( XGAME::xtSpot typeSpot, ID idSpot ) 
{
	if( idSpot == 0 )
		return nullptr;
	for( XSpot *pSpot : m_listSpots ) {
		if( pSpot->GettypeSpot() == typeSpot &&
			pSpot->GetpBaseProp()->idSpot == idSpot )
			return static_cast<T>( pSpot );
	};
	return nullptr;
}
/**
 @brief 
*/
template<typename T, int N>
int XWorld::GetSpotsToAry( XArrayLinearN<T, N> *pOut, XGAME::xtSpot typeSpot ) 
{
	for( XSpot *pSpot : m_listSpots ) {
		if( pSpot->GettypeSpot() == typeSpot || typeSpot == XGAME::xSPOT_NONE )
			pOut->Add( static_cast<T>( pSpot ) );
	};
	return pOut->size();
}

template<typename T>
int XWorld::GetSpotsToAry( XVector<T> *pOut, XGAME::xtSpot typeSpot )
{
	for( XSpot *pSpot : m_listSpots ) {
		if( pSpot->GettypeSpot() == typeSpot || typeSpot == XGAME::xSPOT_NONE )
			pOut->Add( static_cast<T>( pSpot ) );
	};
	return pOut->size();
}
