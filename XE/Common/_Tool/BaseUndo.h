#pragma once
//#include "Select.h"

class XBaseUndo //: public XBaseElem
{
private:
	CString m_strText;
	void Init() {}
	void Destroy() {}
protected:
public:
	XBaseUndo( LPCTSTR szText ) { Init(); m_strText = szText; }
	virtual ~XBaseUndo() { Destroy(); }

	virtual void SetValueAfter( void ) { XBREAKF( 1, "구현되지 않았습니다" ); }
	virtual BOOL DoUndo( void ) = 0;
	virtual BOOL DoRedo( void ) = 0;
	virtual const CString& GetText( void ) { return m_strText; }
	void SetText( LPCTSTR szText ) { m_strText = szText; }
};

namespace UNDO {
	enum xtUNDO_ACTION
	{ 
		xUA_NONE=0, 
		xUA_CREATE, 
		xUA_DELETE
	};
}
template <typename OBJ>	
class XUndoCreateDelObj : public XBaseUndo
{
private:
	UNDO::xtUNDO_ACTION m_UndoAction;	// "생성" 언두인지 "삭제"언두인지
	OBJ	*m_pObjRef;		// 원본 포인터
	OBJ	*m_pObj;			// 소유권이 넘어온 포인터. this것이기때문에 삭제해야함 

	void Init() {
		m_UndoAction = xUA_NONE;
		m_pObj = m_pObjRef = NULL;
	}
	void Destroy() {	SAFE_DELETE( m_pObj ); }		// 소유권이 넘어온 포인터는 여기서 삭제함
public:
	XUndoCreateDelObj( OBJ *pObj, UNDO::xtUNDO_ACTION actType, LPCTSTR szText=_T("") ) : XBaseUndo( szText ) { 
		Init(); 
		m_pObjRef = pObj;
		if( actType == xUA_DELETE )
			m_pObj = pObj;				// 삭제언두일경우는 삭제한 오브젝트의 소유권을 가져온다
		m_UndoAction = actType;
		if( XE::IsEmpty( szText ) )
		{
			if( actType == xUA_CREATE )
				SetText( _T("생성") );
			else
			if( actType == xUA_DELETE )
				SetText( _T("삭제") );
		}
	}
	virtual ~XUndoCreateDelObj() { Destroy(); }
	// get/set
	OBJ* GetUndoObj( void ) { return m_pAttrRef; }
	// virtual
	// undo
	virtual BOOL DoUndo( void ) { 
		XBREAK( m_UndoAction == xUA_NONE );
		if( m_UndoAction == xUA_CREATE ) {
			m_pObj = DeleteAction( m_pObjRef );		// 언두오브젝트를 삭제하라고 알려주고 내가 소유권을 가짐
			XBREAK( m_pObj == NULL );
		}
		else if( m_UndoAction == xUA_DELETE )
		{
			XBREAK( m_pObj == NULL );
			AddAction( m_pObj );	// m_pObj를 애드시키도록 던져주고 난 소유권을 버림
			m_pObj = NULL;
		}
		return TRUE;
	}
	// redo
	virtual BOOL DoRedo( void ) {
		XBREAK( m_UndoAction == xUA_NONE );
		if( m_UndoAction == xUA_CREATE )
		{
			XBREAK( m_pObj == NULL );
			AddAction( m_pObj );		// 언두오브젝트를 다시 애드시키라고 알려줌
			m_pObj = NULL;
		}
		else if( m_UndoAction == xUA_DELETE )
			m_pObj = DeleteAction( m_pObjRef );	// m_pObj를 다시 삭제시키도록 넘겨주고 그 소유권을 받아옴

		return TRUE;
	}
	virtual void AddAction( OBJ *pUndoObj ) = 0;	// 생성에 대한 언두행동
	virtual OBJ* DeleteAction( OBJ *pUndoObj ) = 0;	// 삭제에 대한 언두행동
}; // XUndoCreateDelObj

// 값이 바뀌는것에 대한 언두
template<typename T>
class XUndoValue : public XBaseUndo
{
	T m_ValueBefore;		// 바뀌기전 값
	T m_ValueAfter;			// 바뀐후 값
	T *m_prefValue;			// 값이 원래 있던 어드레스. 이 어드레스를 사용하는 객체는 자신이 삭제될때 실제로 삭제시키지 말고 노드만 삭제되어 알맹이포인터는 살아있어야 한다
	BOOL m_bOwnerBefore;		// before값의 파괴책임이 this에 있다
	BOOL m_bOwnerAfter;	
	//ID m_idUndoObj;			// 언두시킬 객체의 아이디
	void Init() {
		//m_idUndoObj = 0;
//		m_ValueAfter = 0;		// list같은 타입도 T로 올수 있어서 뺌
//		m_ValueBefore = 0;
		m_prefValue = NULL;
		m_bOwnerBefore = m_bOwnerAfter = FALSE;
	}
	void Destroy() {	
//		if( m_bOwnerBefore == FALSE )		// 소유권이 this에게 없는건 내부가 파괴되지 않도록 그냥 클리어만 시켜준다
//			m_ValueBefore = 0;
//		if( m_bOwnerAfter == FALSE )
//			m_ValueAfter = 0;
		// 일단 LEAK가 생기더라도 없애놓음 T형에 XBG_LAYER형이면 내부 서피스를 날려야 되는데 T형이 list같은거면 할수없기 때문

	}	// 여기서 T객체는 모두 소멸자를 제대로 갖고 있어야 한다. 그래야 T객체 안에있는 또다른포인터들이 제대로 삭제된다
public:
	XUndoValue( T *pValue, LPCTSTR szText=_T("") ) : XBaseUndo( szText ) {
		Init();
		if( XE::IsEmpty( szText ) )	// 별도로 텍스트로 넘겨준게없으면 디폴트로 ...
			SetText( _T("값 변경") );
		m_ValueBefore = *pValue;
		m_bOwnerBefore = TRUE;
		m_prefValue = pValue;
	}
	virtual ~XUndoValue() { Destroy(); }
	virtual void SetValueAfter( void ) { m_ValueAfter = *m_prefValue;  }
	//
	virtual BOOL DoUndo( void ) {
		//if( m_idUndoObj )
			//m_prefValue = m_pContainer->GetPointer( m_idUndoObj )	<<- 이런식으로 만들기 시작하면 사용하기가 넘 복잡해진다
		*m_prefValue = m_ValueBefore;		// 원래 값을 복구함
		m_bOwnerBefore = FALSE;			// Before값의 소유권을 포기한다
		m_bOwnerAfter = TRUE;				// 대신 After값의 파괴책임은 this에게 생긴다
		return TRUE;
	}
	virtual BOOL DoRedo( void ) {
		*m_prefValue = m_ValueAfter;		// 바뀐후값으로 재입력
		m_bOwnerAfter = FALSE;
		m_bOwnerBefore = TRUE;			// Before값의 소유권을 가져온다
		return TRUE;
	}
};

// 메모리 블럭 전체의 바뀌는값을 언두
class XUndoValueBlock : public XBaseUndo 
{
	int m_nNum;
	float *m_pBlockBefore;
	float *m_pBlockAfter;
	float **m_pprefBlock;
	void Init() {
		m_pBlockBefore = m_pBlockAfter = NULL;
		m_pprefBlock = NULL;
		m_nNum = 0;
	}
public:
	XUndoValueBlock( float **ppBlock, int num ) : XBaseUndo( _T("블럭값 변경") ){
		m_nNum = num;
		m_pprefBlock = ppBlock;
		if( *ppBlock ) 	{
			m_pBlockBefore = new float[ num ];
			memcpy( m_pBlockBefore, *ppBlock, num * sizeof(float) );
		}
	}
	virtual void SetValueAfter( void ) { 
		if( *m_pprefBlock )	{
			m_pBlockAfter = new float[ m_nNum ];
			memcpy( m_pBlockAfter, *m_pprefBlock, m_nNum * sizeof(float) );
		}
	}
	virtual BOOL DoUndo( void ) {
		if( *m_pprefBlock )
			memcpy( *m_pprefBlock, m_pBlockBefore, m_nNum * sizeof(float) );
		return TRUE;
	}
	virtual BOOL DoRedo( void ) {
		if( *m_pprefBlock )
			memcpy( m_pprefBlock, m_pBlockAfter, m_nNum * sizeof(float) );
		return TRUE;
	}

};
// 메모리 블럭의 언두 템플릿 버전
template<typename T>
class XUndoBlock : public XBaseUndo
{
	int m_nLen;
	T *m_pBefore;	// 값바뀜 작업전의 언두용 데이타
	T *m_pAfter;		// 값바뀜 작업후의 언두용 데이타
	T *m_prefBlock;		// 언두작업이 일어날 소스메모리. (이방식 위험하다. 만약 언두객체생성후 이 포인터가 파괴되고 SetValueAfter가 불린다면 바로 크래쉬)
	void Init() {
		m_pBefore = NULL;
		m_pAfter = NULL;
		m_prefBlock = NULL;
		m_nLen = 0;
	}
	void Destroy() {
		// 만약 객체배열내에 다른 객체의 포인터를 소유하고 있어서 delete []로 인해 그 포인터까지 삭제시킬수 있다.
		// 그러므로 대상이 소유하고 있는 다른 객체의 포인터는 참조가 아니고 카피본이어야 한다.
		// = 연산자로 언두객체에 백업받으므로 모든 데이타는 카피본으로 들어와야 맘놓고 삭제할수 있다.
		SAFE_DELETE_ARRAY( m_pBefore );
		SAFE_DELETE_ARRAY( m_pAfter );
	}
public:
	// 언두 객체가 생성될때 원래 값을 백업받음.
	XUndoBlock( T *pBlock, int len ) : XBaseUndo( _T("블럭값 변경") ) { 
		Init();
		XBREAK( pBlock == NULL );
		m_nLen = len;					// 메모리 블럭의 T단위 길이
		m_pBefore = new T[ len ];		// 언두를 위해 작업전 값을 받아둠.
		m_prefBlock = pBlock;			// SetValueAfter()때 따로 파라메터를 받지 않기 위해 미리 받아둠.
		for( int i = 0; i < len; ++i )
			m_pBefore[i] = pBlock[i];	// 객체카피도 되게 하기 위해서 memcpy를 쓰지않고 =를 썼음.
	}
	virtual ~XUndoBlock() { Destroy(); }
	// 값바꿈작업을 한 후의 값을 받음. 리두에 사용.
	virtual void SetValueAfter( void ) {
		if( XBREAK( m_prefBlock == NULL ) )
			return;
		m_pAfter = new T[ m_nLen ];
		for( int i = 0; i < m_nLen; ++i )
			m_pAfter[i] = m_prefBlock[i];
	}
	virtual BOOL DoUndo( void ) {
		if( XBREAK( m_prefBlock == NULL ) )
			return FALSE;
		for( int i = 0; i < m_nLen; ++i )
			m_prefBlock[i] = m_pBefore[i];
		return TRUE;
	}
	virtual BOOL DoRedo( void ) {
		if( XBREAK( m_prefBlock == NULL ) )
			return FALSE;
		for( int i = 0; i < m_nLen; ++i )
			m_prefBlock[i] = m_pAfter[i];
		return TRUE;
	}
};
