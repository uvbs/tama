#pragma once

//#include "UndoAction.h"		// �������� ������ �ϹǷ� �̷��� ��Ŭ��� ��Ű������
#include "BaseUndo.h"
#include <stack>
#include <queue>
using namespace std;

typedef stack<XBaseUndo *>				XBaseUndo_Stack;
typedef queue<XBaseUndo *>			XBaseUndo_Q;

typedef struct 
{
	XBaseUndo_Stack stackUndo;			// �ϳ��� ��ν��þȿ��� �������� ����ൿ���� ���� �ִ�
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
	XBaseUndo_Q m_qLast;				// ���������� Ǫ���� ������ ����ϰ� �ִ� ť
	void Init() {	}
	void Destroy();
public:
	XUndoMng() { Init(); }
	virtual ~XUndoMng() { Destroy(); }

	void PushUndo( XBaseUndo* pUndoAction, BOOL bAppend=FALSE, BOOL bRedoStackClear=TRUE);	// bAppend: ������ Ǫ������ �ʰ� Top���ÿ� �ൿ�� �߰���Ŵ
	void PushRedo( XBaseUndo* pRedoActioin, BOOL bAppend=FALSE );
	UNDO_PACK* GetUndoTop( void ) { 
		if( m_stackUndoPack.empty() )		return NULL;
		return m_stackUndoPack.top(); 
	}
	UNDO_PACK* GetRedoTop( void ) { 
		if( m_stackRedoPack.empty() )		return NULL;
		return m_stackRedoPack.top(); 
	}
	void DestroyStack( UndoPack_Stack &stack );
	void Clear( void ) {
		DestroyStack( m_stackUndoPack );
		DestroyStack( m_stackRedoPack );
	}
	BOOL Undo( void );
	BOOL Redo( void );
	XBaseUndo* GetLastQ( void ) {
		if( m_qLast.empty() )		return NULL;
		XBaseUndo *pUndo = m_qLast.front();
		m_qLast.pop();
		return pUndo;
	}
};

#define UNDOMNG		XUndoMng::s_pUndoMng