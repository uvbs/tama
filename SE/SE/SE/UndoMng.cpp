#include "stdafx.h"
#include "UndoMng.h"
#include "ConsoleView.h"

XUndoMng *XUndoMng::s_pUndoMng = NULL;
//static XUndoMng s_UndoMng;

void XUndoMng::Destroy()
{
	DestroyStack( m_stackUndoPack );
	DestroyStack( m_stackRedoPack );
	s_pUndoMng = NULL;
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
// 현재 언두스택 탑에서 XUndoKeyChange타입의 언두중에서 pSrc와 idKey와 같은 키를 찾는다
XUndoKeyChange* XUndoMng::Find( XBaseKey *pSrc )
{
/*	if( m_stackUndoPack.empty() )
		return NULL;
	UNDO_PACK *pPack = m_stackUndoPack.top();
	while( pPack->stackUndo.em */
	return NULL;
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
	if( m_stackUndoPack.size() > 0 )
	{
		pUndoPack = m_stackUndoPack.top();
		pUndoPack->stackUndo.push( pUndoAction );
		m_qLast.push( pUndoAction );
	}
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

BOOL XUndoMng::Undo()
{
	BOOL bAppend = FALSE;
	if( m_stackUndoPack.empty() )		// 언두스택이 하나도 안쌓여있으면 걍 리턴
		return FALSE;
	UNDO_PACK *pUndoPack = m_stackUndoPack.top();		// 언두스택에서 언두행동을 꺼낸다
#ifdef _XDEBUG
	if( pUndoPack->stackUndo.empty() )
		XLOG( "UndoPack에 Undo행동이 없다" );
#endif
#ifdef _DEBUG
	GetConsoleView()->Message( _T("Undo: ") );
#endif
	while( !pUndoPack->stackUndo.empty() )
	{
		XBaseUndo *pUndoAction = pUndoPack->stackUndo.top();
		if( pUndoAction->DoUndo() )		// 언두액션을 실행함
		{
	#ifdef _DEBUG
			GetConsoleView()->Message( _T("%s "), pUndoAction->GetText() );
	#endif
			PushRedo( pUndoAction, bAppend );		// 언두한 액션을 리두스택에 푸쉬
			bAppend = TRUE;		// 최초한번만 푸쉬하고 담부턴 탑에 계속 추가한다
		}
		pUndoPack->stackUndo.pop();		// 언두스택에선 삭제
	}
	SAFE_DELETE( pUndoPack );			// 언두팩 안에있던 언두들은 모두 리두로 옮겨졌으니 껍데기만 남은 팩은 삭제시킴
	m_stackUndoPack.pop();
	return TRUE;
}

BOOL XUndoMng::Redo()
{
	BOOL bAppend = FALSE;
	if( m_stackRedoPack.empty() )		// 리두스택이 하나도 안쌓여있으면 걍 리턴
		return FALSE;
	UNDO_PACK *pUndoPack = m_stackRedoPack.top();		// 리두스택에서 언두행동을 꺼낸다
#ifdef _XDEBUG
	if( pUndoPack->stackUndo.empty() )
		XLOG( "redoPack에 Redo행동이 없다" );
#endif
#ifdef _DEBUG
	GetConsoleView()->Message( _T("Redo: ") );
#endif
	while( !pUndoPack->stackUndo.empty() )
	{
		XBaseUndo *pUndoAction = pUndoPack->stackUndo.top();
		if( pUndoAction->DoRedo() )		// 리두액션을 실행함
		{
			DBG_CONSOLE( "%s", pUndoAction->GetText() );
//	#ifdef _DEBUG
//			GetConsoleView()->Message( _T("%s "), pUndoAction->GetText() );
//	#endif
			PushUndo( pUndoAction, bAppend, FALSE );		// 리두한 액션을 언두스택에 푸쉬(이때 푸쉬언두에는 리두스택클리어를 하지않는다)
			bAppend = TRUE;		// 최초한번만 푸쉬하고 담부턴 탑에 계속 추가한다
		}
		pUndoPack->stackUndo.pop();		// 리두두스택에선 삭제
	}
	SAFE_DELETE( pUndoPack );			// 언두팩 안에있던 언두들은 모두 리두로 옮겨졌으니 껍데기만 남은 팩은 삭제시킴
	m_stackRedoPack.pop();
	return TRUE;
}
