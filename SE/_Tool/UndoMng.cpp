#include "stdafx.h"
#include "UndoMng.h"
/*#ifdef _DEBUG
#ifdef _LE
#include "XDlgconsole.h"
#endif
#endif
*/
XUndoMng *XUndoMng::s_pUndoMng = NULL;

void XUndoMng::Destroy()
{
	DestroyStack( m_stackUndoPack );
	DestroyStack( m_stackRedoPack );
}

// �μ��� �Ѿ�� stl stack����Ÿ�� ��� ������Ŵ
void XUndoMng::DestroyStack( UndoPack_Stack &stackData )
{
	while( !stackData.empty() )
	{
		UNDO_PACK *pPack = stackData.top();
		while( !pPack->stackUndo.empty() )
		{
			XBaseUndo *pUndoAction = pPack->stackUndo.top();
			SAFE_DELETE( pUndoAction );
			pPack->stackUndo.pop();
		}
		SAFE_DELETE( pPack );
		stackData.pop();
	}
}

// bAppend: ������ Ǫ������ �ʰ� Top���ÿ� �ൿ�� �߰���Ŵ
void XUndoMng::PushUndo( XBaseUndo* pUndoAction, BOOL bAppend, BOOL bRedoStackClear )
{
	UNDO_PACK *pUndoPack = NULL;
	if( bAppend == FALSE )		
	{
		if( bRedoStackClear && m_stackUndoPack.empty() )		// ��ν��ÿ� ���ʷ� Ǫ���ɶ��� ���ν����� ����ش�
			DestroyStack( m_stackRedoPack );
		// ����� Ǫ������ ���ο� ������� ����� �װ���Ǫ���Ѵ�.
		pUndoPack = new UNDO_PACK;
		m_stackUndoPack.push( pUndoPack );
		while( !m_qLast.empty() )		// ������� Ǫ���ÿ��� ��Ʈť�� ��� ����� 
			m_qLast.pop();
	}
	// ������ ž���߰��Ѵ�
	pUndoPack = m_stackUndoPack.top();
	pUndoPack->stackUndo.push( pUndoAction );
	m_qLast.push( pUndoAction );
}	
void XUndoMng::PushRedo( XBaseUndo* pRedoAction, BOOL bAppend )
{
	UNDO_PACK *pUndoPack = NULL;
	if( bAppend == FALSE )		
	{
		// ����� Ǫ������ ���ο� ������� ����� �װ���Ǫ���Ѵ�.
		pUndoPack = new UNDO_PACK;
		m_stackRedoPack.push( pUndoPack );
	}
	// ������ ž���߰��Ѵ�
	pUndoPack = m_stackRedoPack.top();
	pUndoPack->stackUndo.push( pRedoAction );
}

BOOL XUndoMng::Undo( void )
{
	BOOL bAppend = FALSE;
	if( m_stackUndoPack.empty() )		// ��ν����� �ϳ��� �Ƚ׿������� �� ����
		return FALSE;
	UNDO_PACK *pUndoPack = m_stackUndoPack.top();		// ��ν��ÿ��� ����ൿ�� ������
	if( XBREAK(pUndoPack->stackUndo.empty()) )
	{
		Destroy();
		return FALSE;
	}

/*#if defined(_DEBUG) && defined(_XTOOL)
	CONSOLE( "Undo: " );
#endif */
	while( !pUndoPack->stackUndo.empty() )
	{
		XBaseUndo *pUndoAction = pUndoPack->stackUndo.top();
		if( pUndoAction->DoUndo() )		// ��ξ׼��� ������
		{
/*	#if defined(_DEBUG) && defined(_XTOOL)
			CONSOLE( "%s ", pUndoAction->GetText() );
	#endif */
			PushRedo( pUndoAction, bAppend );		// ����� �׼��� ���ν��ÿ� Ǫ��
			bAppend = TRUE;		// �����ѹ��� Ǫ���ϰ� ����� ž�� ��� �߰��Ѵ�
		}
		pUndoPack->stackUndo.pop();		// ��ν��ÿ��� ����
	}
	SAFE_DELETE( pUndoPack );			// ����� �ȿ��ִ� ��ε��� ��� ���η� �Ű������� �����⸸ ���� ���� ������Ŵ
	m_stackUndoPack.pop();
	return TRUE;
}

BOOL XUndoMng::Redo( void )
{
	BOOL bAppend = FALSE;
	if( m_stackRedoPack.empty() )		// ���ν����� �ϳ��� �Ƚ׿������� �� ����
		return FALSE;
	UNDO_PACK *pUndoPack = m_stackRedoPack.top();		// ���ν��ÿ��� ����ൿ�� ������
	if( XBREAK(pUndoPack->stackUndo.empty()) )
	{
		Destroy();
		return FALSE;
	}

/*#if defined(_DEBUG) && defined(_XTOOL)
	CONSOLE( "Redo: " );
#endif */
	while( !pUndoPack->stackUndo.empty() )
	{
		XBaseUndo *pUndoAction = pUndoPack->stackUndo.top();
		if( pUndoAction->DoRedo() )		// ���ξ׼��� ������
		{
//			DBG_CONSOLE( "%s", pUndoAction->GetText() );
			PushUndo( pUndoAction, bAppend, FALSE );		// ������ �׼��� ��ν��ÿ� Ǫ��(�̶� Ǫ����ο��� ���ν���Ŭ��� �����ʴ´�)
			bAppend = TRUE;		// �����ѹ��� Ǫ���ϰ� ����� ž�� ��� �߰��Ѵ�
		}
		pUndoPack->stackUndo.pop();		// ���εν��ÿ��� ����
	}
	SAFE_DELETE( pUndoPack );			// ����� �ȿ��ִ� ��ε��� ��� ���η� �Ű������� �����⸸ ���� ���� ������Ŵ
	m_stackRedoPack.pop();
	return TRUE;
}
