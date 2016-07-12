#include "stdafx.h"
#include "XEBaseFSM.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//////////////////////////////////////////////////////////////////////////
XEBaseFSM::XEBaseFSM( ID idFSM, XEControllerFSM *pController )
{
	_Init();
	m_idFSM = idFSM;
	m_pController = pController;
}

void XEBaseFSM::Destroy()
{
}

XEBaseFSM* XEBaseFSM::ChangeFSM( ID idFSM ) 
{
	m_idNextFSM = 0;
	return m_pController->ChangeFSM( idFSM );
}


////////////////////////////////////////////////////////////////
XEControllerFSM::XEControllerFSM()
{
	Init();
}

void XEControllerFSM::Destroy()
{
	XARRAYLINEARN_DESTROY( m_aryStates );
}

void XEControllerFSM::Release( void )
{
	XARRAYLINEARN_LOOP( m_aryStates, XEBaseFSM*, pFSM )
	{
//		pFSM->Uninit();
		pFSM->Release();
	} END_LOOP;
}

/**
 @brief 등록된 상태객체들 중에서 idFSM을 가진 객체를 찾아준다.
*/
XEBaseFSM* XEControllerFSM::FindFSM( ID idFSM )
{
	XARRAYLINEARN_LOOP( m_aryStates, XEBaseFSM*, pFSM )
	{
		if( pFSM->GetidFSM() == idFSM )
			return pFSM;
	} END_LOOP;
	XBREAKF(1, "not found fsm:id=%d", idFSM );;
	return nullptr;
}

