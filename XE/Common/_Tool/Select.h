#pragma once

#include <list>
using namespace std;

class XBaseElem
{
	ID m_idObj;		// 고유아이디
	XE::VEC2 m_vDrawPos;
protected:
	SET_ACCESSOR( ID, idObj );
public:
	XBaseElem() {
		m_idObj = 0;
	}
	virtual ~XBaseElem() {}
	//
	GET_ACCESSOR( ID, idObj );
	void MakeNewID( void ) { m_idObj = XE::GenerateID(); }
	ID MoveidKey( void ) {
		ID idObj = m_idObj;
		m_idObj = 0;
		return idObj;
	}

	//
//	virtual BOOL IsInside( const XE::VEC2& v1, const XE::VEC2& v2 ) {}
	virtual XE::VEC2 GetPos( void ) { return XE::VEC2( 0, 0 ); }		// 뷰에서 오브젝트가 위치하는 가상좌표를 얻는다. XKeyPos의 경우 XKeyPos::GetPos()가 된다.
	virtual void SetPos( const XE::VEC2& vPos ) {}		
	virtual GET_SET_ACCESSOR(const XE::VEC2&, vDrawPos );
};

typedef list<XBaseElem*>				XBaseElem_List;
typedef list<XBaseElem*>::iterator	XBaseElem_Itor;

class XSelect
{
	XBaseElem_List m_listElem;
	XBaseElem_Itor m_itorElem;
	void Init( void ) {
	}
public:
	XSelect() { Init(); }
	virtual ~XSelect() {}

	int GetNum( void ) { return m_listElem.size(); }
	void Add( XBaseElem *pElem ) {
		m_listElem.push_back( pElem );
	}
	void Del( XBaseElem *pElem, BOOL bDestroyElem=FALSE );
	BOOL Find( XBaseElem *pElem );
	void GetNextClear() { m_itorElem = m_listElem.begin(); }
	XBaseElem *GetNext( void ) { 
		if( m_itorElem == m_listElem.end() )
			return NULL;
		return (*m_itorElem++);
	}
	void Clear( void );			// 알맹이는 놔두고 노드만 파괴시킴
	void Destroy( void );		// 노드안에 알맹이까지 파괴시키는 버전
	XBaseElem* GetFirst( void ) {
		XBaseElem_Itor itor = m_listElem.begin();
		return (*itor);
	}
	XBaseElem* GetLast( void ) {				// 
		XBaseElem_Itor itor = m_listElem.end();
		return (*--itor);
	}
	void Sort( bool (*compFunc)(XBaseElem*, XBaseElem*) ) {
		m_listElem.sort( compFunc );
	}
};

template<typename T>
class XSelect2
{
	list<T*>				m_listElem;
	typename list<T*>::iterator		m_itorElem;
	void Init( void ) {
	}
public:
	XSelect2() { Init(); }
	virtual ~XSelect2() {}

	int GetNum( void ) { return m_listElem.size(); }
	void Add( T *pElem ) {
		m_listElem.push_back( pElem );
	}
	void Del( T *pElem, BOOL bDestroyElem=FALSE );
	BOOL Find( T *pElem );
	void GetNextClear() { m_itorElem = m_listElem.begin(); }
	T *GetNext( void ) { 
		if( m_itorElem == m_listElem.end() )
			return NULL;
		return (*m_itorElem++);
	}
	void Clear( void );			// 알맹이는 놔두고 노드만 파괴시킴
	void Destroy( void );		// 노드안에 알맹이까지 파괴시키는 버전
	T* GetFirst( void ) {
		list<T*>::iterator itor = m_listElem.begin();
		return (*itor);
	}
	XBaseElem* GetLast( void ) {				// 
		list<T*>::iterator itor = m_listElem.end();
		return (*--itor);
	}
	void Sort( bool (*compFunc)(T*, T*) ) {
		m_listElem.sort( compFunc );
	}
};

#define ELEM_LOOP( I )				\
								XBaseElem_Itor I; \
								for( I = m_listElem.begin(); I != m_listElem.end(); I ++ ) \
		
#define ELEM_MANUAL_LOOP( I )				\
								XBaseElem_Itor I; \
								for( I = m_listElem.begin(); I != m_listElem.end(); ) 

#define SELECT_LOOP( SEL_LIST, NODE_TYPE, E )	{ \
		NODE_TYPE E;				\
		SEL_LIST.GetNextClear();	\
		while( E = (NODE_TYPE)SEL_LIST.GetNext() )		\
		{	


template<typename T>
class XSelect3
{
	XList<T*>		m_listElem;
	void Init( void ) {
	}
public:
	XSelect3() { Init(); }
	virtual ~XSelect3() {}
	//
	int GetNum( void ) { return m_listElem.size(); }
	void Select( T *pElem ) {
		m_listElem.Add( pElem );
	}
	void Clear( void ) { m_listElem.Clear(); }
	void GetNextClear() { m_listElem.GetNextClear(); }
	T *GetNext( void ) { 
		return m_listElem.GetNext();
	}
};
