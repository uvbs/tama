#pragma once

#include "UndoAction.h"
#include <stack>
#include <queue>
using namespace std;

typedef stack<XBaseUndo *>				XBaseUndo_Stack;
typedef queue<XBaseUndo *>			XBaseUndo_Q;

typedef struct 
{
	XBaseUndo_Stack stackUndo;			// 하나의 언두스택안에는 여러개의 언두행동들이 들어갈수 있다
} UNDO_PACK;

typedef stack<UNDO_PACK *>			UndoPack_Stack;
class XUndoMng;
class XUndoMng
{
public:
	static XUndoMng *s_pUndoMng;
private:
	UndoPack_Stack	m_stackUndoPack;
	UndoPack_Stack m_stackRedoPack;
	XBaseUndo_Q m_qLast;				// 마지막으로 푸쉬된 노드들을 기억하고 있는 큐
	void Init() {
		s_pUndoMng = this;
	}
	void Destroy();
public:
	XUndoMng() { Init(); }
	virtual ~XUndoMng() { Destroy(); }

	void PushUndo( XBaseUndo* pUndoAction, BOOL bAppend=FALSE, BOOL bRedoStackClear=TRUE);	// bAppend: 스택을 푸쉬하지 않고 Top스택에 행동을 추가시킴
	void PushRedo( XBaseUndo* pRedoActioin, BOOL bAppend=FALSE );
	UNDO_PACK* UndoTop() { 
		if( m_stackUndoPack.empty() )		return NULL;
		return m_stackUndoPack.top(); 
	}
	UNDO_PACK* RedoTop() { 
		if( m_stackRedoPack.empty() )		return NULL;
		return m_stackRedoPack.top(); 
	}
	void DestroyStack( UndoPack_Stack &stack );
	void Clear() {
		DestroyStack( m_stackUndoPack );
		DestroyStack( m_stackRedoPack );
	}
	XUndoKeyChange* Find( XBaseKey *pSrc );			// 현재 언두스택 탑에서 XUndoKeyChange타입의 언두중에서 pSrc와 idKey와 같은 키를 찾는다
	BOOL Undo();
	BOOL Redo();
	XBaseUndo* GetLastQ() {
		if( m_qLast.empty() )		return NULL;
		XBaseUndo *pUndo = m_qLast.front();
		m_qLast.pop();
		return pUndo;
	}
};

#define UNDOMNG		XUndoMng::s_pUndoMng