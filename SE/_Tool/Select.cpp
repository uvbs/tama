#include "stdafx.h"
#include "Select.h"


void XSelect::Del( XBaseElem *pElem, BOOL bDestroyElem )
{
	ELEM_MANUAL_LOOP( itor )
	{
		if( (*itor) == pElem ) {
			if( bDestroyElem )
				delete (*itor);
			m_listElem.erase( itor++ );
		}
		else
			itor++;
	}
}
BOOL XSelect::Find( XBaseElem *pElem )
{
	ELEM_LOOP( itor )
	{
		if( (*itor) == pElem )
			return TRUE;
	}
	return FALSE;
}

void XSelect::Clear()
{
	ELEM_MANUAL_LOOP( itor )
	{
		m_listElem.erase( itor++ );
	}
}

void XSelect::Destroy()
{
	ELEM_MANUAL_LOOP( itor )
	{
		delete (*itor);
		m_listElem.erase( itor++ );
	}
}
