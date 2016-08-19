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

// 인수로 넘어온 stl stack데이타를 모두 삭제시킴
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

// bAppend: 스택을 푸쉬하지 않고 Top스택에 행동을 추가시킴
void XUndoMng::PushUndo( XBaseUndo* pUndoAction, BOOL bAppend, BOOL bRedoStackClear )
{
	UNDO_PACK *pUndoPack = NULL;
	if( bAppend == FALSE )		
	{
		if( bRedoStackClear && m_stackUndoPack.empty() )		// 언두스택에 최초로 푸쉬될때는 리두스택을 비워준다
			DestroyStack( m_stackRedoPack );
		// 새노드 푸쉬모드는 새로운 언두팩을 만들고 그것을푸쉬한다.
		pUndoPack = new UNDO_PACK;
		m_stackUndoPack.push( pUndoPack );
		while( !m_qLast.empty() )		// 언두최초 푸쉬시에는 라스트큐을 모두 비워줌 
			m_qLast.pop();
	}
	// 스택의 탑에추가한다
	pUndoPack = m_stackUndoPack.top();
	pUndoPack->stackUndo.push( pUndoAction );
	m_qLast.push( pUndoAction );
}	
void XUndoMng::PushRedo( XBaseUndo* pRedoAction, BOOL bAppend )
{
	UNDO_PACK *pUndoPack = NULL;
	if( bAppend == FALSE )		
	{
		// 새노드 푸쉬모드는 새로운 언두팩을 만들고 그것을푸쉬한다.
		pUndoPack = new UNDO_PACK;
		m_stackRedoPack.push( pUndoPack );
	}
	// 스택의 탑에추가한다
	pUndoPack = m_stackRedoPack.top();
	pUndoPack->stackUndo.push( pRedoAction );
}

BOOL XUndoMng::Undo( void )
{
	BOOL bAppend = FALSE;
	if( m_stackUndoPack.empty() )		// 언두스택이 하나도 안쌓여있으면 걍 리턴
		return FALSE;
	UNDO_PACK *pUndoPack = m_stackUndoPack.top();		// 언두스택에서 언두행동을 꺼낸다
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
		if( pUndoAction->DoUndo() )		// 언두액션을 실행함
		{
/*	#if defined(_DEBUG) && defined(_XTOOL)
			CONSOLE( "%s ", pUndoAction->GetText() );
	#endif */
			PushRedo( pUndoAction, bAppend );		// 언두한 액션을 리두스택에 푸쉬
			bAppend = TRUE;		// 최초한번만 푸쉬하고 담부턴 탑에 계속 추가한다
		}
		pUndoPack->stackUndo.pop();		// 언두스택에선 삭제
	}
	SAFE_DELETE( pUndoPack );			// 언두팩 안에있던 언두들은 모두 리두로 옮겨졌으니 껍데기만 남은 팩은 삭제시킴
	m_stackUndoPack.pop();
	return TRUE;
}

BOOL XUndoMng::Redo( void )
{
	BOOL bAppend = FALSE;
	if( m_stackRedoPack.empty() )		// 리두스택이 하나도 안쌓여있으면 걍 리턴
		return FALSE;
	UNDO_PACK *pUndoPack = m_stackRedoPack.top();		// 리두스택에서 언두행동을 꺼낸다
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
		if( pUndoAction->DoRedo() )		// 리두액션을 실행함
		{
//			DBG_CONSOLE( "%s", pUndoAction->GetText() );
			PushUndo( pUndoAction, bAppend, FALSE );		// 리두한 액션을 언두스택에 푸쉬(이때 푸쉬언두에는 리두스택클리어를 하지않는다)
			bAppend = TRUE;		// 최초한번만 푸쉬하고 담부턴 탑에 계속 추가한다
		}
		pUndoPack->stackUndo.pop();		// 리두두스택에선 삭제
	}
	SAFE_DELETE( pUndoPack );			// 언두팩 안에있던 언두들은 모두 리두로 옮겨졌으니 껍데기만 남은 팩은 삭제시킴
	m_stackRedoPack.pop();
	return TRUE;
}
