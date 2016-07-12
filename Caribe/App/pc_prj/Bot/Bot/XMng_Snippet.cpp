#include "stdafx.h"
#include "XBotMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

std::shared_ptr<X$selected$Mng> X$selected$Mng::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<X$selected$Mng>& X$selected$Mng::sGet() {
	if( s_spInstance == nullptr )
		s_spInstance = std::shared_ptr<X$selected$Mng>( new X$selected$Mng );
	return s_spInstance;
}
void X$selected$Mng::sDestroyInstance() {
	s_spInstance.reset();
}
////////////////////////////////////////////////////////////////
X$selected$Mng::X$selected$Mng()
{
	Init();
}

void X$selected$Mng::Destroy()
{
}

X$selected$Obj* X$selected$Mng::Add$selected$Obj( X$selected$Obj* pObj )
{
	m_list$selected$s.Add( pObj );
	return pObj;
}
void X$selected$Mng::Process( float dt )
{
	for( auto pObj : m_list$selected$s ) {
		pObj->Process( dt );
	}
	//
	
	for( auto itor = m_list$selected$s.begin(); itor != m_list$selected$s.end(); ) {
		X$selected$Obj* pObj = (*itor);
		if( pObj->IsDestroy() ) {
			SAFE_DELETE( pObj );
			m_list$selected$s.erase( itor++ );
		} else
			++itor;
	}
	

}
void X$selected$Mng::Destroy$selected$Obj( X$selected$Obj* pObj )
{
	
}

