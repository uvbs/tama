#pragma once

#include <list>
#include "Global.h"
#include "xe.h"
using namespace std;

class XBaseElem
{
	ID m_idObj = 0;		// 고유아이디(전 영역에서 유일한 키)
	XE::VEC2 m_vDrawPos;
private:
protected:
	SET_ACCESSOR( ID, idObj );
// 	XBaseElem( const XBaseElem& src ) {
// 		*this = src;
// 	}
	void SetNewInstance() {
		m_idObj = GenerateNewID();
	}
public:
	XBaseElem() {}
	virtual ~XBaseElem() {}
	//
	GET_ACCESSOR_CONST( ID, idObj );
	ID getid() {
		return m_idObj;
	}
	void MakeNewID() { 
		m_idObj = GenerateNewID(); 
	}
	ID GenerateNewID() {
		return XE::GenerateID();
	}
	ID MoveidKey() {
		ID idObj = m_idObj;
		m_idObj = 0;
		return idObj;
	}
	//
//	virtual BOOL IsInside( const XE::VEC2& v1, const XE::VEC2& v2 ) {}
	virtual XE::VEC2 GetPos() { return XE::VEC2( 0, 0 ); }		// 뷰에서 오브젝트가 위치하는 가상좌표를 얻는다. XKeyPos의 경우 XKeyPos::GetPos()가 된다.
	virtual void SetPos( const XE::VEC2& vPos ) {}		
//	virtual GET_SET_ACCESSOR(const XE::VEC2&, vDrawPos );
	virtual const XE::VEC2& GetvDrawPos() const {
		return m_vDrawPos;
	}
	void SetvDrawPos( const XE::VEC2& vPos ) {
		m_vDrawPos = vPos;
	}
};

typedef list<XBaseElem*>				XBaseElem_List;
typedef list<XBaseElem*>::iterator	XBaseElem_Itor;

class XSelect
{
	XBaseElem_List m_listElem;
	XBaseElem_Itor m_itorElem;
	void Init() {
	}
public:
	XSelect() { Init(); }
	virtual ~XSelect() {}
	GET_ACCESSOR( std::list<XBaseElem*>&, listElem );
	int GetNum() { return m_listElem.size(); }
	void Add( XBaseElem *pElem ) {
		m_listElem.push_back( pElem );
	}
	void Del( XBaseElem *pElem, BOOL bDestroyElem=FALSE );
	BOOL Find( XBaseElem *pElem );
	void GetNextClear() { m_itorElem = m_listElem.begin(); }
	XBaseElem *GetNext() { 
		if( m_itorElem == m_listElem.end() )
			return NULL;
		return (*m_itorElem++);
	}
	void Clear();			// 알맹이는 놔두고 노드만 파괴시킴
	void Destroy();		// 노드안에 알맹이까지 파괴시키는 버전
	XBaseElem* GetFirst() {
		XBaseElem_Itor itor = m_listElem.begin();
		return (*itor);
	}
	XBaseElem* GetLast() {				// 
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
	void Init() {
	}
public:
	XSelect2() { Init(); }
	virtual ~XSelect2() {}

	int GetNum() { return m_listElem.size(); }
	void Add( T *pElem ) {
		m_listElem.push_back( pElem );
	}
	void Del( T *pElem, BOOL bDestroyElem=FALSE );
	BOOL Find( T *pElem );
	void GetNextClear() { m_itorElem = m_listElem.begin(); }
	T *GetNext() { 
		if( m_itorElem == m_listElem.end() )
			return NULL;
		return (*m_itorElem++);
	}
	void Clear();			// 알맹이는 놔두고 노드만 파괴시킴
	void Destroy();		// 노드안에 알맹이까지 파괴시키는 버전
	T* GetFirst() {
		list<T*>::iterator itor = m_listElem.begin();
		return (*itor);
	}
	XBaseElem* GetLast() {				// 
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
