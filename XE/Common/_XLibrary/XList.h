#pragma once

#include "etc/global.h"
#include "etc/xUtil.h"
#include <list>
#include <algorithm>

#pragma warning( disable : 4700 )		// warning C4700: 초기화되지 않은 't' 지역 변수를 사용했습니다.

//using namespace std;
class XArchive;
template<typename T, int N>
class XArrayLinearN;

template<typename T>
class XList4;

#ifdef __cplusplus
// global.h에 있는 LIST_LOOP등을 쓰면 이터레이터앞에 typename이 없다고 에러난다.
// 그렇다고 그쪽에 typename을 붙여버리면 이번엔 템플릿아닌데서 쓰는건 다 에러난다
// 그래서 여기에 전용코드를 만듬
#define __LIST_DESTROY( LIST, NODE_TYPE )		{			\
										typename std::list<NODE_TYPE>::iterator itor; \
										for( itor = LIST.begin(); itor != LIST.end(); ) \
										{	\
										NODE_TYPE pNode = (*itor);		\
											SAFE_DELETE( pNode );		\
											LIST.erase( itor++ );		\
										}	\
									}	\

#define __LIST_CLEAR( LIST, NODE_TYPE )		{			\
								typename std::list<NODE_TYPE>::iterator itor; \
								for( itor = LIST.begin(); itor != LIST.end(); ) \
								{	\
								LIST.erase( itor++ );		\
								}	\
								}	\

#define __LIST_LOOP( LIST, NODE_TYPE, I, E )	{			\
								typename std::list<NODE_TYPE>::iterator I; \
								int li;	\
								for( li=0, I = LIST.begin(); I != LIST.end(); li++, I ++ ) \
								{	\
								NODE_TYPE E = (*I);		\

#define __LIST_LOOP_REVERSE( LIST, NODE_TYPE, I, E )	{			\
								typename std::list<NODE_TYPE>::reverse_iterator I; \
								for( I = LIST.rbegin(); I != LIST.rend(); I ++ ) \
								{	\
								NODE_TYPE E = (*I);		\


#define __LIST_MANUAL_LOOP( LIST, NODE_TYPE, I, E )	{			\
								typename std::list<NODE_TYPE>::iterator I; \
								for( I = LIST.begin(); I != LIST.end(); ) \
								{	\
								NODE_TYPE E = (*I);		\

// stl::list의 wrap
template<typename T>
class XList 
{
public:
	class Itor
	{
	public:
//		typename std::list<T>::const_iterator	m_Itor;
		typename std::list<T>::iterator	m_Itor;
		Itor() {}
		Itor( typename std::list<T>::iterator itor ) { m_Itor = itor; }
//		Itor( typename std::list<T>::const_iterator itor ) { m_Itor = itor; }
		virtual ~Itor() {}

		Itor& operator ++ () {	// ++A
			++m_Itor;
			return *this;
		}
		Itor operator ++( int ) {	// A++
			Itor itor = *this;
			++*this;
			return itor;
		}
		BOOL operator == ( Itor& rh ) {
			return m_Itor == rh.m_Itor;
		}
		BOOL operator != ( Itor& rh ) {
			return m_Itor != rh.m_Itor;
		}
	};
	class ConstItor
	{
	public:
		typename std::list<T>::const_iterator	m_Itor;
		ConstItor() {}
		ConstItor( typename std::list<T>::iterator itor ) { m_Itor = itor; }
		ConstItor( typename std::list<T>::const_iterator itor ) { m_Itor = itor; }
		virtual ~ConstItor() {}

		ConstItor& operator ++ ( ) {	// ++A
			++m_Itor;
			return *this;
		}
		ConstItor operator ++( int ) {	// A++
			Itor itor = *this;
			++*this;
			return itor;
		}
	};
private:
//	BOOL m_bNodeDestroy;		// 노드 알맹이까지 여기서 파괴하는지 여부
	std::list<T>			m_List;
	typename std::list<T>::iterator	m_Itor;
public:
//	XList( BOOL bNodeDestroy=FALSE ) { m_bNodeDestroy = bNodeDestroy; }
	XList() {}
	virtual ~XList() {
//		if( m_bNodeDestroy )
//			Destroy();	
		Clear();		// 이거 여기다 넣으면 안되나? 넣는게 맞는건다.
	}
	// 
//	GET_SET_ACCESSOR( BOOL, bNodeDestroy );
	//
	int size() const { return m_List.size(); }		// 호환성을 위해
	int Size() const { return m_List.size(); }
	void pop_back(){ m_List.pop_back(); }
	void pop_front(){ m_List.pop_front(); }
	void push_front( T pElem ) { m_List.push_front( pElem ); }
	T push_back( T pElem ) { return Add( pElem ); }
	T Add( T pElem ) {
		m_List.push_back( pElem );
		return pElem;
	}
/*	void Del( T pElem, BOOL bDestroyElem=FALSE )	{
		__LIST_MANUAL_LOOP( m_List, T, itor, pNode )
		{
			if( pNode == pElem ) {
				if( bDestroyElem )
					delete pNode;
				m_List.erase( itor++ );
			}
			else
				itor++;
		} END_LOOP;
	} */
	void Del( T pElem )	{
		__LIST_MANUAL_LOOP( m_List, T, itor, pNode )
		{
			if( pNode == pElem ) {
				m_List.erase( itor++ );
				break;
			}
			else
				++itor;
		} END_LOOP;
	}
	void DelFromIndex( int idx )	{
		int i = 0;
		__LIST_MANUAL_LOOP( m_List, T, itor, pNode )
		{
			if( i++ == idx ) {
				m_List.erase( itor++ );
				break;
			}
			else
				++itor;
		} END_LOOP;
	}
	/**
	 @brief id로 찾는버전
	 현재는 클래스 설계가 완전하지 않아서 노드의 객체가 getid()를 가지고 있다고 가정하고 하는것인데
	 장차 getid()를 가진 최상위 슈퍼클래스를 만들어 리스트의 노드는 슈퍼클래스 타입을 가지도록 할 예정
	*/
	T DelByID( ID idNode ) {
		int i = 0;
		__LIST_MANUAL_LOOP( m_List, T, itor, pNode )
		{
			if( pNode->getid() == idNode ) {
				m_List.erase( itor++ );
				return pNode;
			}
			else
				++itor;
		} END_LOOP;
		return nullptr;
	}
	void DelCurrent( typename XList::Itor *pItor ) {
		m_List.erase( pItor->m_Itor++ );
	}
	T* FindByID( ID idNode ) {
		int i = 0;
		__LIST_MANUAL_LOOP( m_List, T, itor, pNode )
		{
			if( pNode->getid() == idNode ) {
				return &(*itor);
			}
			else
				++itor;
		} END_LOOP;
		return nullptr;
	}
	BOOL Find( T pElem ) /*const*/ {	// const가 안된다 ㅠㅠ;
		__LIST_LOOP( m_List, T, itor, pNode )
		{
			if( pNode == pElem )
				return TRUE;
		} END_LOOP;
		return FALSE;
	}
	int FindIndex( T pElem ) {
		int i = 0;
		__LIST_LOOP( m_List, T, itor, pNode )
		{
			if( pNode == pElem )
				return i;
			++i;
		} END_LOOP;
		return -1;
	}
	/// 사용하지 말것.
	Itor __GetNextClear() {
		Itor itor( m_Itor = m_List.begin() );
		return itor;
	}
	/// 내부에서 m_Itor를 안쓰는버전
	Itor GetNextClear2() { 
		return Itor( m_List.begin() ); 
	}
	Itor begin() {
		return Itor( m_List.begin() ); 
	}
	Itor end() {
		return Itor( m_List.end() );
	}
// 	void GetNextClear2( typename XList<T>::Itor *pItorOut ) const {
// 		typename std::list<T>::const_iterator stdItor = m_List.begin();
// 		Itor itor( stdItor );
// 		*pItorOut = itor;
// 	}
	Itor GetPrevClear() {
		Itor itor( m_Itor = m_List.end() );
		return itor; 
	}
	T GetNext( typename XList::Itor *pItor ) { 
		// 사용금지(return NULL땜에)
		if( pItor->m_Itor == m_List.end() )
			return NULL;
		return (*(pItor->m_Itor)++);
	}
	T GetPrev( typename XList::Itor *pItor ) {		// 이거 구현좀 이상하군. prev의 동작정의를 확실히 해야할듯 itor의 end()가 cdcd인 특성상 prev동작은 이전으로 포인터부터 이동하고 그 포인터를 리턴해줘야 할듯
		// 사용금지(return NULL땜에)
		if( pItor->m_Itor == m_List.begin() )
			return NULL;
		if( pItor->m_Itor == m_List.end() )	// 포인터가 맨 뒤노드를 가리키고 있으면
		{
			--pItor->m_Itor;							// 일단 마지막 노드로 포인터를 옮기고
			if( pItor->m_Itor == m_List.begin() )		// 마지막 노드가 시작노드면
				return *pItor->m_Itor;					// 시작노드를 리턴
		} else
			--m_Itor;							// 일단 마지막 노드로 포인터를 옮기고
		return (*m_Itor--);
	}
	T GetNext2( typename XList::Itor *pItor ) const { 
		return (*(pItor->m_Itor)++);
	}
	T GetNext2( typename XList::Itor& itor ) const {
		return *itor.m_Itor++;
	}
	T& GetNext2_ref( typename XList::Itor& itor ) const {
		return *itor.m_Itor++;
	}
	T GetNext2Const( typename XList::ConstItor *pItor ) const {
		return ( *( pItor->m_Itor )++ );
	}
	T* __GetNext3( typename XList::Itor *pItor ) const {
		return &(*(pItor->m_Itor)++);
	}
	T* GetNext3Const( typename XList::ConstItor *pItor ) const {
		return &( *pItor->m_Itor++ );
	}
	T* GetNext3( typename XList::Itor& itor ) {
		return &( *itor.m_Itor++ );
	}
	T GetPrev2( typename XList::Itor *pItor ) {
		return *(--pItor->m_Itor);
	}
	T* GetCurrent( typename XList::Itor *pItor ) {
		if( (*pItor).m_Itor == m_List.end() )
			return nullptr;
		return &(*(pItor->m_Itor));
	}
	T GetNext() { 
		// 사용금지(return NULL땜에)
		if( m_Itor == m_List.end() )
			return NULL;
		return (*m_Itor++);
	}
	T GetPrev() {		// 이거 구현좀 이상하군. prev의 동작정의를 확실히 해야할듯 itor의 end()가 cdcd인 특성상 prev동작은 이전으로 포인터부터 이동하고 그 포인터를 리턴해줘야 할듯
		// 사용금지(return NULL땜에)
		if( m_Itor == m_List.begin() )
			return NULL;
		if( m_Itor == m_List.end() )	// 포인터가 맨 뒤노드를 가리키고 있으면
		{
			--m_Itor;							// 일단 마지막 노드로 포인터를 옮기고
			if( m_Itor == m_List.begin() )		// 마지막 노드가 시작노드면
				return *m_Itor;					// 시작노드를 리턴
		} else
			--m_Itor;							// 일단 마지막 노드로 포인터를 옮기고
		if( m_Itor == m_List.begin() )
			return NULL;
		return (*m_Itor--);
	}
	// 단순히 이터레이터를 앞으로 감는다
	void MovePrev() {
		if( m_Itor == m_List.begin() )
			return;
		--m_Itor;
	}
	// 이터레이터를 다음으로 이동한다. GetCurrent()와 짝으로 쓰인다.
	void MoveNext() {
		if( m_Itor == m_List.end() )
			return;
		++m_Itor;
	}
	void Clear() {			// 알맹이는 놔두고 노드만 파괴시킴
		m_List.clear();
/*		__LIST_MANUAL_LOOP( m_List, T, itor, pNode )
		{
			m_List.erase( itor++ );
		} END_LOOP; */
	}

	void Destroy() {		// 노드안에 알맹이까지 파괴시키는 버전. 리스트 삭제할때 XLIST_LOOP로 삭제하기가 귀찮아서 부활시킴. ~XGRoll에 사용
		__LIST_MANUAL_LOOP( m_List, T, itor, pNode )
		{
			delete pNode;
			m_List.erase( itor++ );
		} END_LOOP;
	} 

	T GetFirst() {
		typename std::list<T>::iterator itor = m_List.begin();
		return (*itor);
	}
	T GetLast() {				// 
		typename std::list<T>::iterator itor = m_List.end();
		return (*--itor);
	}

	T GetFromIndex( int idx ) {
		int i = 0;
		__LIST_LOOP( m_List, T, itor, pNode ) {
			if( i++ == idx )
				return pNode;
		} END_LOOP;
		XBREAKF( 1, "idx not found" );
		T t;
		return t;
	}
	T GetFromRandom() {
		int max = size();
		return GetFromIndex( xRandom(max) );
	}
	int GetIndex( T pElem ) {
		int i = 0;
		__LIST_LOOP( m_List, T, itor, pNode ) {
			if( pNode == pElem )
				return i;
			++i;
		} END_LOOP;
		return -1;
	}
	/* 소트용 비교함수 예제
	bool compFrame( XBaseElem *pKey1, XBaseElem *pKey2 )
	{
		XBaseKey *pBaseKey1 = SafeCast<XBaseKey*, XBaseElem*>( pKey1 );
		XBaseKey *pBaseKey2 = SafeCast<XBaseKey*, XBaseElem*>( pKey2 );
		return pBaseKey1->GetfFrame() < pBaseKey2->GetfFrame();
	}
	*/
	void sort( bool (*compFunc)(T, T) ) { Sort( compFunc ); }
	void Sort( bool (*compFunc)(T, T) ) {
		m_List.sort( compFunc );
	}
	template<int N>
	void operator=( XArrayLinearN<T, N>& ary );

	//
	T LuaGetNextClear() {
		this->GetNextClear();
		return GetNext();
	}
	T LuaGetNext() {
		return GetNext();
	}
}; // XList 
/*
#define XLIST_LOOP( S, NODE_TYPE, E )	  { \
		NODE_TYPE E;				\
		S.GetNextClear();	\
		while( E = S.GetNext() )		\
		{
*/
// 기존의 while( E = .... )방식이 비 포인터형일때 맹점이 있어서 XLIST_LOOP2 방식으로 바꿨다.
/**
 @brief 이제 이건 쓰지말것. XLIST_LOOP를 사용할것.
*/
// #define XLIST_LOOP( S, NODE_TYPE, E )	  { \
//         int _max = (S).size();		\
//         XList<NODE_TYPE>::Itor _itor = (S).GetNextClear2();	\
//         for( int _i = 0; _i < _max; ++_i )		\
//         {										\
//             NODE_TYPE E = (S).GetNext2( _itor );

// #define XLIST_LOOP_H( S, NODE_TYPE, E )	  { \
//         int _max = (S).size();		\
//         NODE_TYPE E;				\
//         typename XList<NODE_TYPE>::Itor _itor = (S).GetNextClear2();	\
//         for( int _i = 0; _i < _max; ++_i )		\
// 		        {										\
//             E = (S).GetNext2( _itor );
// 내부의 m_Itor를 전혀 안쓰는 버전(이버전이 문제가 없다면 앞으론 이것만 쓸것)
#define XLIST_LOOP( S, NODE_TYPE, E )	  { \
	int _max = S.size();		\
	XList<NODE_TYPE>::Itor _itor = S.GetNextClear2(); \
	for( int _i = 0; _i < _max; ++_i ) { \
		NODE_TYPE E = S.GetNext2( _itor );

#define XLIST_LOOP_H( S, NODE_TYPE, E )	  { \
	int _max = S.size();		\
	typename XList<NODE_TYPE>::Itor _itor = S.GetNextClear2(); \
	for( int _i = 0; _i < _max; ++_i ) { \
		NODE_TYPE E = S.GetNext2( _itor );

#define XLIST_DEL( S, ITOR ) \
	S.DelCurrent( &ITOR )

/**
 @brief 루프중 del을 써야 할때 주로 쓴다.
 @param NODE_TYPE 리스트의 T형
 @param pE는 T의 포인터형이어야 한다.
 @code
 XLIST_LOOP_MANUAL( listSample, int*, itor, pNode ) {
	 printf("%d", (*pNode));
	if( (*pNode) == 99 )
		XLIST_DEL( listSample, itor )
	else
		++itor;
 } END_LOOP;
*/
#define XLIST_LOOP_MANUAL( S, NODE_TYPE, ITOR, pE )	  { \
	int _max = S.size();		\
	XList<NODE_TYPE>::Itor ITOR = S.begin(); \
	for( int _i = 0; _i < _max; ++_i ) { \
		NODE_TYPE *pE = S.GetCurrent( &ITOR );

#define XLIST_LOOP_IDX( S, NODE_TYPE, I, E )	  { \
	int _max = S.size();		\
	XList<NODE_TYPE>::Itor _itor = S.GetNextClear2(); \
	for( int I = 0; I < _max; ++I ) { \
		NODE_TYPE E = S.GetNext2( _itor );

#define XLIST_LOOP_REF( S, NODE_TYPE, E )	  { \
	int _max = S.size();		\
	XList<NODE_TYPE>::Itor _itor = S.GetNextClear2(); \
	for( int _i = 0; _i < _max; ++_i ) { \
		NODE_TYPE& E = S.GetNext2_ref( _itor );

#define XLIST_LOOP_REV( S, NODE_TYPE, E )	  { \
	int _max = (S).size();		\
	NODE_TYPE E;				\
	XList<NODE_TYPE>::Itor _itor = (S).GetPrevClear();	\
	for( int _i = 0; _i < _max; ++_i )		\
		{										\
		E = (S).GetPrev2( &_itor );
																		
#define XLIST_LOOP2( S, NODE_TYPE, pE )	  { \
		int _max = S.size();		\
		XList<NODE_TYPE>::Itor _itor = S.GetNextClear2();	\
		for( int _i = 0; _i < _max; ++_i )		\
		{										\
			NODE_TYPE *pE = S.GetNext3( _itor );
// 헤더용
#define XLIST_LOOP2_H( S, NODE_TYPE, pE )	  { \
				int _max = (S).size();		\
				typename XList<NODE_TYPE>::Itor _itor = (S).GetNextClear2();	\
				for( int _i = 0; _i < _max; ++_i )		\
		{										\
						NODE_TYPE *pE = (S).GetNext3( _itor );

#define XLIST_LOOP2_CONST( S, NODE_TYPE, pE )	  { \
		int _max = S.size();		\
		const XList<NODE_TYPE>::ConstItor _itor = S.GetNextClear();	\
		for( int _i = 0; _i < _max; ++_i )		\
		{										\
			NODE_TYPE *pE = S.GetNext3Const( &_itor );



#define XLIST_DESTROY( LIST, TYPE )	\
		XLIST_LOOP2( LIST, TYPE, pObj ) \
			delete (*pObj);			\
		END_LOOP;	\
		LIST.Clear();

// #define XLIST_LOOP( S, NODE_TYPE, E )	  { \
// 	int _max = (S).size();		\
// 	NODE_TYPE E;				\
// 	typename XList<NODE_TYPE>::Itor _itor = (S).GetNextClear();	\
// 	for( int _i = 0; _i < _max; ++_i )		\
// 		{										\
// 		E = (S).GetNext2( &_itor );

////////////////////////////////////////////////////////////////////////////////////////////
// 노드 추가/삭제시 malloc이 일어나지 않는 배열로 구현한 리스트
// 내부에서 모든 데이타를 포인터형으로 다루기 때문에 <T>는 비포인터형으로 넣어준다.
// XList2<XObj>: O       XList2<XObj*>: X
template <typename T>
class XList2
{
	int m_idxLastEmpty;		// 비어있는 풀의 인덱스
	int m_nNum;				// 오브젝트 개수
	int m_nMax;				// 리스트의 최대 크기
	BOOL m_bSorted;		// 소트되어 있는가
	T **m_ppList;				// 오브젝트 포인터의 리스트
	int *m_pIdxNextEmpty;	// 비어있는 다음풀의 인덱스 
	_tstring m_strIdentifier;
	void Init() {
		m_nNum = m_idxLastEmpty = 0; 
		m_nMax = 0; 
		m_ppList = NULL;
		m_pIdxNextEmpty = NULL;
		m_bSorted = TRUE;		// Delete()가 들어오기전까진 첨엔 마치 소트된것처럼 앞에서부터 채워지기땜에 이렇게 해도 된다
	}
	void Destroy() {
		if( m_ppList )
		{
			for( int i = 0; i < m_nMax; ++i )
				m_ppList[i] = NULL;
		}
		SAFE_DELETE_ARRAY( m_ppList );
		SAFE_DELETE_ARRAY( m_pIdxNextEmpty );
	}
public:
#ifdef _CLIENT
	XList2( LPCTSTR szIdentifier ) {	
		Init(); 	
		m_strIdentifier = szIdentifier;
	}			
#else
	XList2() {Init();}
#endif
	XList2( int nMax ) 
	{ 
		Init(); 
		Create( nMax );
	}
	virtual ~XList2() { Destroy(); }
	void Create( int nMax );
	//
	// 모든 요소를 삭제한다.
	void Clear() {
		// 비어있는 다음 노드를 세1팅
		int nMax = m_nMax;
		int i;
		if( nMax )
		{
			for( i = 0; i < nMax-1; i ++ )
				m_pIdxNextEmpty[i] = i+1;
			m_pIdxNextEmpty[ i ] = -1;		// 가장 끝은 '뒤가'없다
		}
		m_idxLastEmpty = 0;
		m_nNum = 0;
		memset( m_ppList, 0, sizeof( T* ) * nMax );

	}
	GET_ACCESSOR_CONST( int, nMax );
	GET_ACCESSOR_CONST( int, nNum );
	int size() const {
		return m_nNum;
	}
	BOOL IsFull() {
		return ( m_nNum == m_nMax );
	}
	// 
	void Add( T* pObj ) {
		int idx = m_idxLastEmpty;
		if( XBREAK( idx < 0 || idx >= m_nMax ) )	
		{
			XBREAKF( 1, "list2(%s) full: idx=%d max=%d", m_strIdentifier.c_str(), idx, m_nMax );
			// idx < 0 : list full
#ifdef _DEBUG
			T *list[ 1000 ];
			memcpy( list, m_ppList, sizeof(T*) * 1000 );
#endif
			return;
		}
		m_idxLastEmpty = m_pIdxNextEmpty[ idx ];		// 빈곳을 가리키는 포인터를 '다음'에 지정된 포인터로 대치
/*		if( XBREAK( m_idxLastEmpty < 0 ) )
		{
			int list[1000];
			memcpy( list, m_pIdxNextEmpty, 4 * m_nMax );
			list[0] = 0;
		} */
		m_ppList[ idx ] = pObj;
		m_pIdxNextEmpty[ idx ] = -2;
		++m_nNum;		// 노드 개수 증가
	}
	void Delete( int idx ) {		
		XBREAK( idx < 0 || idx >= m_nMax );
		m_ppList[ idx ] = NULL;
		m_pIdxNextEmpty[ idx ] = m_idxLastEmpty;		// 원래 '빈곳'을 가리켰던 포인터를 다음포인터로 연결
//		XBREAK( m_idxLastEmpty < 0 );
		m_idxLastEmpty = idx;								// 삭제된곳을 '빈곳'으로 설정
		--m_nNum;
	}
	// 리스트에서 pNode의 노드지운다.
	int Delete( const T* pNode ) { 
		int idx = FindIdx( pNode );
		if( idx < 0 )	return -1;
		Delete( idx ); 
		return idx;
	}
	int DeleteByID( ID idNode ) {
		int idx = FindIdxByID( idNode );
		if( idx < 0 )
			return -1;
		Delete( idx );
		return idx;
	}
	int Del( const T* pNode ) {	return Delete( pNode ); 	}
	//
	void DeleteCurrent( const int& itor ) {
		Delete( itor - 1 );
	}
	// pNode와 같은 데이타를 찾아 그 인덱스를 리턴한다.
	int	FindIdx( const T* pNode )	{
		T** ppList = m_ppList;
		int nMax = m_nMax;
		for( int i = 0; i < nMax; ++i )
		{
			if( *ppList++ == pNode )
				return i;
		}
		return -1;
	}
	BOOL Find( const T* pNode ) {
		return (FindIdx( pNode ) < 0)? FALSE : TRUE;
	}
	/**
	 @brief id로 찾는버전
	 현재는 클래스 설계가 완전하지 않아서 노드의 객체가 getid()를 가지고 있다고 가정하고 하는것인데
	 장차 getid()를 가진 최상위 슈퍼클래스를 만들어 리스트의 노드는 슈퍼클래스 타입을 가지도록 할 예정
	*/
	int	FindIdxByID( ID idNode )	{
		T** ppList = m_ppList;
		int nMax = m_nMax;
		for( int i = 0; i < nMax; ++i )
		{
			if( (*ppList++)->getid() == idNode )
				return i;
		}
		return -1;
	}
	T* GetRandom() {
		int itor = 0;
		return GetNext( &itor );
	}
//	T* GetFirst() {
//		int itor = 0;
//		return GetNext( &itor );
//	}
//	T* GetNode( int idx ) { 
//		XBREAK( idx >= 0 && idx < m_nMax ); 
//		return m_ppList[ idx ]; 
//	}
	// pItorIdx로부터 탐색하여 채워진 노드의 포인터를 리턴한다. 
	// 이터레이터는 자동으로 다음 노드를 가리킨다. 다음노드는 빈곳일수도 있으므로 그대로 사용해선 안된다.
	// pItorIdx가 대부분 쓸모없기때문에 가급적 XLIST2_LOOP매크로를 사용하길 권한다
	T* GetNext( int *pItorIdx ) const {	
		if( m_nNum == 0 )		return NULL;
		int nMax = m_nMax;
		for( int i = *pItorIdx; i < nMax; ++i ) {		// 채워진곳을 찾는 루프
			if( m_ppList[i] )	{
				*pItorIdx = ++i;							// 다음턴엔 m_nIdx다음부터 해야하므로.
				return m_ppList[ i - 1 ];
			}
		}
		*pItorIdx = 0;													// 끝까지 다찾았으면 초기화시킨다.
		return NULL;												// NULL이라는것은 빈슬롯이 더이상 없이 배열끝까지 탐색했다.
	}
	// pItorIdx로부터 탐색하여 채워진 노드의 포인터를 리턴한다. 
	// *pItorIdx는 return T*의 인덱스를 가리킨다. 이터레이터가 자동으로 증가하지 않으므로 사용후 사용자가 직접 itor++를 해줘야 한다. STL처럼
	T* GetManualNext( int *pItorIdx ) const {	
		if( m_nNum == 0 )		return NULL;
		int nMax = m_nMax;
		for( int i = *pItorIdx; i < nMax; ++i ) {		// 채워진곳을 찾는 루프
			if( m_ppList[i] )	{
				*pItorIdx = i;	
				return m_ppList[ i ];
			}
		}
		*pItorIdx = 0;													// 끝까지 다찾았으면 초기화시킨다.
		return NULL;												// NULL이라는것은 빈슬롯이 더이상 없이 배열끝까지 탐색했다.
	}
	// sort
	void Sort( int (*ptFuncCompare)( const void *p1, const void *p2 ) ) {
		qsort( m_ppList, m_nMax, sizeof(T*), ptFuncCompare );
		int nMax = m_nMax;
		int i;
		for( i = 0; i < nMax-1; ++ i )
		{
			if( m_ppList[i] == NULL )
				break;
			m_pIdxNextEmpty[i] = -2;
		}
		m_idxLastEmpty = i;
		for( ; i < nMax-1; ++ i )
		{
			m_pIdxNextEmpty[i] = i+1;
		}
//		memcpy( list, m_ppList, sizeof(T*) * m_nMax );
//		memcpy( list2, m_pIdxNextEmpty, 4 * m_nMax );
		m_pIdxNextEmpty[i] = -1;

	}

}; // XList2
#define XLIST2_LOOP( LIST, NODE_TYPE, ELEM )	  { \
		int ELEM##_idxPool = 0;				\
		while( 1 ) {		\
			NODE_TYPE ELEM = (LIST).GetNext( &ELEM##_idxPool ); \
			if( !(ELEM) )	break; \

/*
#define XLIST2_LOOP( LIST, NODE_TYPE, ELEM )	  { \
		NODE_TYPE ELEM;				\
		int ELEM##_idxPool = 0;				\
		while( ELEM = (LIST).GetNext( &ELEM##_idxPool ) )		\
		{
*/

#define XLIST2_MANUAL_LOOP( LIST, NODE_TYPE, ITOR, ELEM )	  { \
		NODE_TYPE ELEM;				\
		int ITOR = 0;				\
		while(( ELEM = (LIST).GetManualNext( &ITOR ) ))		\
		{

#define XLIST2_DESTROY( LIST, TYPE )	{ \
		XLIST2_LOOP( LIST, TYPE, pObj ) \
			SAFE_DELETE( pObj );			\
		END_LOOP;		\
		LIST.Clear(); \
}
	



//////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
class XQueue2
{
	T **m_ppList;
	int m_nSize;
	int m_nMax;
	int m_itor;
	void Init() {
		m_ppList = NULL;
		m_nSize = m_nMax = 0;
		m_itor = 0;
	}
	void Destroy() {
		SAFE_DELETE_ARRAY( m_ppList );
	}
public:
	XQueue2() {	Init();	}
	XQueue2( int nMax ) {	
		Init();	
		Create( nMax );
	}
	virtual ~XQueue2() { Destroy(); }

	int size() { return m_nSize; }
	void Create( int nMax );
	void Push( T* pObj ) {
//		if( XASSERT( m_nSize < m_nMax ) )
		XBREAK( m_nMax == 0 );
		if( m_nSize < m_nMax )
			m_ppList[ m_nSize++ ] = pObj;
	}
	T* Pop() {
		T* front = m_ppList[0];		// 젤 앞에걸 하나 빼고
		int size = m_nSize--;		// 큐 전체 크기는 하나 줄인다
		int i;
		for( i = 1; i < size; ++i )
			m_ppList[i-1] = m_ppList[i];	// 앞으로 하나씩 당긴다
		m_ppList[size-1] = NULL;
		return front;
	}
	T* Front() {
		return m_ppList[0];	
	}
	void GetNextClear() { m_itor = 0; }
	T* GetNext() {
		if( m_itor < m_nSize )
			return m_ppList[ m_itor++ ];
		return NULL;
	}
};

#define XQ2_LOOP( Q, TYPE, ELEM )	{	\
	TYPE ELEM;	\
	Q.GetNextClear();	\
	while( ELEM = Q.GetNext() ) {	

#define XQ2_DESTROY( Q, TYPE ) \
	XQ2_LOOP( Q, TYPE, pObj )		\
		SAFE_DELETE( pObj );	\
	END_LOOP

	
// c의 배열이나 std::vector와 거의 흡사한 기능의 배열 템플릿
// 리니어 하지 않은 배열데이타에 사용한다.
#define	DEBUG_ARRAY
template<typename T>
class XArray
{
	void Init() {
		m_nMax = 0;
		m_pArray = NULL;
	}
	void Destroy() {
		SAFE_DELETE_ARRAY( m_pArray );	
	}
protected:
	T *m_pArray;			// 배열
#ifdef DEBUG_ARRAY
	T *m_ppDebugArray[ 1024 ];		// 디버깅용.
#endif
	int m_nMax;			// 배열 최대크기
public:	
	XArray() { Init(); }
	XArray( const XArray<T>& ary ) { 
		Init(); 
		if( ary.GetMax() <= 0 )
			return;
		Create( ary.GetMax() );
		*m_pArray = *(ary.m_pArray);
	}
	virtual ~XArray() { Destroy(); }
	// 배열을 최초 생성한다.
	void Create( int max );
	void Clear( T d ) {
//		Destroy();		// 어레이 껍데기만 날림. <- 왜 이따위 짓을 했었을까....
//		Init();
		for( int i = 0; i < m_nMax; ++i )
			m_pArray[ i ] = d;
	}
	void Fill( T& v ) {
		for( int i = 0; i < m_nMax; ++i )
			m_pArray[ i ] = v;
	}
	void Fill( const T&& v ) {
		for( int i = 0; i < m_nMax; ++i )
			m_pArray[ i ] = v;
	}
	int GetMax() const { 
		return m_nMax; 
	}
	T* operator&() const {
		return m_pArray;
	}
	void operator=( XArray& ary ) {
		Destroy();
		Init();
		Create( ary.GetMax() );
		int num = ary.GetMax();
		*m_pArray = *(ary.m_pArray);
// 		for( int i = 0; i < num; ++i ) {
// 			Add( ary[i] );
//		}
	}
	// idx번째의 배열요소를 꺼낸다.
	T& operator [] ( int idx ) {
		XBREAK( m_pArray == NULL );
		XBREAK( idx >= m_nMax );
#ifdef DEBUG_ARRAY
		m_ppDebugArray[ idx ] = &m_pArray[ idx ];
#endif
		return m_pArray[idx];
	}
	T& Get( int idx ) {
		XBREAK( m_pArray == NULL );
		XBREAK( idx >= m_nMax );
		return m_pArray[idx];
	}
	void DebugArrayCopy()	{
#ifdef DEBUG_ARRAY
		int num = (m_nMax>1024)? 1024:m_nMax;
		for( int i = 0; i < num; ++i )
			m_ppDebugArray[i] = &m_pArray[i];	// T가 클래스일수도 있으므로 memcpy쓰면 안됨.
#endif
	}
	// idx번째의 배열에 값을 덮어씌운다.
	void Set( int idx, const T& elem ) {
		XBREAK( m_pArray == NULL );
		XBREAK( idx >= m_nMax );
		m_pArray[ idx ] = elem;
		DebugArrayCopy();
	}
	// 배열의 크기를 바꾼다.
	// 리사이즈가 되면 m_pArray의 어드레스를 다른곳에서 참조하는경우 크래쉬 되는 위험이 있다.
	void Resize( int newSize );
	// e와 같은걸 찾아서 그 인덱스를 리턴한다.
	int Find( T e ) {
		XBREAK( m_pArray == NULL );
		T *p = m_pArray;
		int max = m_nMax;
		for( int i = 0; i < max; ++i ) {
			if( *p++ == e )
				return i;
		}
		return -1;
	}
};	// XArray<>

#include "ArrayLinear.h"
#include "ArrayLinearN.h"

// 데이타가 리니어하지 않은 배열
template<typename T, int N>
class XArrayN : public XArrayLinearN<T, N>
{
	int m_itor;
public:
		using XArrayLinearN<T,N>::m_ppList;
	XArrayN() {	}
	XArrayN( T& val ) {
		Clear( val );
	}
	XArrayN( T&& val ) {
		Clear( val );
	}
	virtual ~XArrayN() {}
	// idx번째의 배열요소를 꺼내거나 담는다.
	T& operator [] ( int idx ) {	// 아제길 복사로 리턴해야 하는게 맞는데 잘못했군...레퍼런스로 받아야 한다면 받는변수에서 레퍼런스로 선언하면 될것을.
		if( XBREAKF( idx >= N, "idx(%d) >= N(%d)", idx, N ) )
			return m_ppList[0];
		if( XBREAK( idx < 0 ) )
			return m_ppList[0];
		return m_ppList[idx];
	}
	int GetnNum() const {
		XBREAKF( 1, "do not use" );		// 사용금지
		return 0;
	}
	int size() const {  
		XBREAKF( 1, "do not use" );		// 사용금지
		return 0;
	}
	inline void push_back( T pObj ) { Add( pObj ); }
	void Add( T pObj ) {
		XBREAKF( 1, "do not use" );		// 사용금지
	}
	void Clear() {
		XBREAKF( 1, "do not use. use XArrayLinear::Clear( T val )" );		// 사용금지
	}
	void Clear( T val ) {
		int max = N;
		for( int i = 0; i < max; ++i ) {
			m_ppList[ i ] = val;
		}
	} 
	void GetNextClear() { m_itor = 0; }
	T GetNext() { 
		if( m_itor >= N )
			return NULL;
		for( int i = m_itor; i < N; ++i ) {
			if( m_ppList[ i ] != 0 ) {
				m_itor = i + 1;
				return m_ppList[ i ];
			}
		}
		return 0;
	}
	// 배열요소중 널이 아닌 요소들의 seq번째 요소를 리턴한다. pOutIdx는 실제 배열인덱스를 리턴한다.
	T GetFromSequence( int seq, int *pOutIdx ) {
		int s = 0;
		for( int i = 0; i < N; ++i ) {
			if( m_ppList[ i ] != 0 ) {
				if( s++ == seq )
				{
					*pOutIdx = i;
					return m_ppList[ i ];
				}
			}
		}
		*pOutIdx = -1;
		return NULL;
	}
	// 
	void DelFromIndex( int idx ) {
		m_ppList[ idx ] = 0;
	}
	// e와 같은걸 찾아서 그 인덱스를 리턴한다.
	int Find( T e ) {
		T *p = m_ppList;
		for( int i = 0; i < N; ++i ) {
			if( e == *p++)
				return i;
		}
		return -1;
	}
	int Find( T e, int max ) {		// max치를 알고 있다면 좀더 속도가 빠른 버전
		T *p = m_ppList;
		for( int i = 0; i < max; ++i ) {
			if( *p++ == e )
				return i;
		}
		return -1;
	}
	int FindByID( ID idElem ) {
		T *p = m_ppList;
		for( int i = 0; i < N; ++i ) {
			if( (*p) && (*p)->getid() == idElem )
				return i;
			++p;
		}
		return -1;
	}
	//
	template<int N2>
	void MemCopy( XArrayN<T, N2>& arySrc ) {
		XBREAK( N < N2 );
		memcpy_s( m_ppList, sizeof(T) * N, arySrc.GetStartPtr(), sizeof(T) * N2 );
	}
	template<int N2>
	void MemCopy( XArrayLinearN<T, N2>& arySrc ) {
		XBREAK( N < arySrc.GetnNum() );
		memcpy_s( m_ppList, sizeof(T) * N, arySrc.GetStartPtr(), sizeof(T) * arySrc.GetnNum() );
	}
	void MemCopy2( XArrayLinear<T>& arySrc ) {
		XBREAK( N < arySrc.GetNum() );
		memcpy_s( m_ppList, sizeof(T) * N, arySrc.GetStartPtr(), sizeof(T) * arySrc.GetNum() );
	}
}; // class XArrayN

// #define XARRAYLINEARN_LOOP( ARRAY, ELEM_TYPE, ELEM )	{	\
// 	int _num_ELEM = (ARRAY).GetnNum();	\
// 	for( int _i = 0; _i < _num_ELEM; ++_i ) {	\
// 		ELEM_TYPE ELEM = (ARRAY)[ _i ];

#define XARRAYLINEARN_LOOP( ARRAY, ELEM_TYPE, ELEM )	{	\
	int _num_ELEM = (ARRAY).GetnNum();	\
	for( int _i = 0; _i < _num_ELEM; ++_i ) {	\
		ELEM_TYPE ELEM = (ARRAY)[ _i ];

#define XARRAYLINEARN_LOOP_AUTO( ARRAY, ELEM )	{	\
	int _num_ELEM = (ARRAY).GetnNum();	\
	for( int _i = 0; _i < _num_ELEM; ++_i ) {	\
		auto ELEM = (ARRAY)[ _i ];

#define XARRAYLINEARN_LOOP_REF( ARRAY, ELEM_TYPE, ELEM )	{	\
	int _num_ELEM = (ARRAY).GetnNum();	\
	for( int _i = 0; _i < _num_ELEM; ++_i ) {	\
		const ELEM_TYPE& ELEM = (ARRAY)[ _i ];

// #define XARRAYLINEARN_LOOP_MAX( ARRAY, ELEM_TYPE, MAX, ELEM )	{	\
// 	int _num = MAX;	\
// 	for( int _i = 0; _i < _num; ++_i ) {	\
// 		ELEM_TYPE ELEM = (ARRAY)[ _i ];

#define XARRAYLINEARN_LOOP_MAX( ARRAY, ELEM_TYPE, MAX, ELEM )	{	\
	int _num = MAX;	\
	for( int _i = 0; _i < _num; ++_i ) {	\
	ELEM_TYPE ELEM = (ARRAY)[ _i ];

// #define XARRAYLINEARN_LOOP_IDX( ARRAY, ELEM_TYPE, IDX, ELEM )	{	\
// 	int _num_ELEM = (ARRAY).GetnNum();	\
// 	for( int IDX = 0; IDX < _num_ELEM; ++IDX ) {	\
// 	ELEM_TYPE ELEM = ARRAY[ IDX ];

#define XARRAYLINEARN_LOOP_IDX( ARRAY, ELEM_TYPE, IDX, ELEM )	{	\
	int _num_ELEM = (ARRAY).GetnNum();	\
	for( int IDX = 0; IDX < _num_ELEM; ++IDX ) {	\
	ELEM_TYPE ELEM = (ARRAY)[ IDX ];

// 리니어하지 않은 데이타의 배열
#define XARRAYN_LOOP( ARRAY, ELEM_TYPE, ELEM )	{	\
	int _max = ARRAY.GetMax();	\
	for( int _i = 0; _i < _max; ++_i ) {	\
		ELEM_TYPE ELEM = ARRAY[ _i ];
#define XARRAYN_LOOP_AUTO( ARRAY, ELEM )	{	\
	int _max = ARRAY.GetMax();	\
	for( int _i = 0; _i < _max; ++_i ) {	\
		auto ELEM = ARRAY[ _i ];
#define XARRAYN_REF_LOOP( ARRAY, ELEM_TYPE, pELEM )	{	\
	int _max = ARRAY.GetMax();	\
	for( int _i = 0; _i < _max; ++_i ) {	\
		ELEM_TYPE *pELEM = &ARRAY[ _i ];
#define XARRAYN_REF_LOOP_IDX( ARRAY, ELEM_TYPE, IDX, pELEM )	{	\
	int _max = ARRAY.GetMax();	\
	for( int IDX = 0; IDX < _max; ++IDX ) {	\
	ELEM_TYPE *pELEM = &ARRAY[ IDX ];
#define XARRAYN_LOOP_MAX( ARRAY, ELEM_TYPE, MAX, ELEM )	{	\
	int _max = MAX;	\
	for( int _i = 0; _i < _max; ++_i ) {	\
		ELEM_TYPE ELEM = ARRAY[ _i ];
#define XARRAYN_LOOP2( ARRAY, ELEM_TYPE, I, ELEM )	{	\
	int _max = ARRAY.GetMax();	\
	for( int I = 0; I < _max; ++I ) {	\
		ELEM_TYPE ELEM = ARRAY[ I ];
#define XARRAYN_LOOP_IDX( ARRAY, ELEM_TYPE, I, ELEM )	{	\
	int _max = ARRAY.GetMax();	\
	for( int I = 0; I < _max; ++I ) {	\
		ELEM_TYPE ELEM = ARRAY[ I ];
#define XARRAYN_LOOP_IDX_REV( ARRAY, ELEM_TYPE, I, ELEM )	{	\
	int _last = ARRAY.GetMax() - 1;	\
	for( int I = _last; I >= 0; --I ) {	\
		ELEM_TYPE ELEM = ARRAY[ I ];
#define XARRAYN_LOOP2_MAX( ARRAY, ELEM_TYPE, MAX, I, ELEM )	{	\
	int _max = MAX;	\
	for( int I = 0; I < _max; ++I ) {	\
		ELEM_TYPE ELEM = ARRAY[ I ];
#define XARRAYN_CLEAR( ARRAY, VALUE ) { \
	int _max = ARRAY.GetMax();		\
	for( int _i = 0; _i < _max; ++_i ) 	\
		ARRAY[ _i ] = VALUE;			\
}
#define XARRAYN_DESTROY( ARRAY ) { \
	int _max = ARRAY.GetMax();		\
	for( int _i = 0; _i < _max; ++_i ) 	\
		SAFE_DELETE( ARRAY[ _i ] );			\
}

#ifdef _XTOOL
// 배열을 기반으로 list의 기능을 갖는 컨테이너. 삽입/삭제등의 기능이 추가된다.
// 경고: 만약 T를 객체포인터가 아닌 객체자체로 쓸거라면 
// Delete()멤버등에서 객체 대입으로 인한 할당메모리 포인터가 중복생기므로 파괴또한 중복될수 있다.
// 할당메모리까지 완벽하게 복사본을 만들지 않는이상 T를 그냥 객체형으로 쓰지말것.
template<typename T>
class XArrayList : public XArrayLinear<T>
{
	void Init() {}
	void Destroy() {}
public:
	XArrayList() { Init(); }
	virtual ~XArrayList() { Destroy(); }
	//
	void operator=( XArrayList& ary ) {
		XArrayLinear::operator =( ary );
	}
	// idx인덱스에 elem을 삽입한다.
	void Insert( const T& elem, int idx ) {	
				if( m_pArray == NULL )
						return;
		XBREAK( m_pArray == NULL );
		XBREAK( idx < 0 );
		XBREAK( idx >= m_nNum );
		XBREAK( idx >= m_nMax );
		XBREAK( m_nNum == m_nMax );	// 더이상 삽입시키지 못함.
		if( idx == m_nNum )
		{
			Add( elem );
			return;
		}
		int dstSize = m_nMax - idx - 1;
		memmove_s( &m_pArray[ idx + 1], dstSize * sizeof(T), &m_pArray[ idx ], (m_nNum - idx) * sizeof(T) );
		m_pArray[ idx ] = elem;
		++m_nNum;
		DebugArrayCopy();
	}
	// idx인덱스의 배열요소를 삭제한다. 뒤에 있던 요소들은 한칸씩 당겨진다.
	void Delete( int idx ) {
		XBREAK( m_pArray == NULL );
		XBREAK( idx < 0 );
		XBREAK( idx >= m_nNum );
		XBREAK( idx >= m_nMax );
		if( idx == m_nNum-1 )		// 맨 끝의 요소는 그냥 전체 개수만 하나 줄이면 된다.
			--m_nNum;
		else
		{
			int dstSize = m_nNum - idx;
			for( int i = idx + 1; i < m_nNum; ++i )
				m_pArray[ idx-1 ] = m_pArray[ i ];
			--m_nNum;
			DebugArrayCopy();
		}
	}
	void Sort() {
		DebugArrayCopy();
	}
	void Swap( T e1, T e2 ) {
		XBREAK( m_pArray == NULL );
		// e1을 찾는다.
		int idx1 = Find( e1 );
		if( idx1 < 0 )		return;
		// e2를 찾는다.
		int idx2 = Find( e2 );
		if( idx2 < 0 )		return;
		// 바꾼다.
		T temp = m_pArray[ idx1 ];
		m_pArray[ idx1 ] = m_pArray[ idx2 ];
		m_pArray[ idx2 ] = temp;
		DebugArrayCopy();
	}
};
#endif // xtool
#endif // c++

#define XARRAY_LOOP( ARY, TYPE, ELEM )			\
{	\
	int _max = ARY.GetMax();		\
	for( int _i = 0; _i < _max; ++_i )		\
	{	\
		TYPE ELEM = ARY[ _i ];

// 이제 이거 사용하지 말고 XARRAY_LOOP_IDX를 사용할것.
#define XARRAY_LOOP2( ARY, TYPE, I, ELEM )			\
{	\
	int _max = ARY.GetMax();		\
	for( int I = 0; I < _max; ++I )		\
	{	\
		TYPE ELEM = ARY[ I ];

#define XARRAY_LOOP_IDX( ARY, TYPE, I, ELEM )			\
{	\
	int _max = ARY.GetMax();		\
	for( int I = 0; I < _max; ++I )		\
{	\
	TYPE ELEM = ARY[ I ];

#define XARRAY_DESTROY( ARY )	\
{	\
	int _max = ARY.GetMax();		\
	for( int _i = 0; _i < _max; ++_i )		\
	{	\
		if( ARY[ _i ] ) \
		{ \
			delete ARY[ _i ];	\
			ARY[ _i ] = NULL; \
		} \
	} \
}
#define XARRAY_CLEAR( ARY, VALUE )			\
{	\
	int _max = ARY.GetMax();		\
	for( int _i = 0; _i < _max; ++_i )		\
		ARY[ _i ] = VALUE;		\
}


#define XARRAYLINEAR_LOOP( ARY, TYPE, ELEM )			\
{	\
	int _num = ARY.GetNum();		\
	for( int _i = 0; _i < _num; ++_i )		\
	{	\
		TYPE ELEM = ARY[ _i ];	\

#define XARRAYLINEAR_LOOP_AUTO( ARY, ELEM )			\
{	\
	int _num = ARY.GetNum();		\
	for( int _i = 0; _i < _num; ++_i )		\
		{	\
		auto ELEM = ARY[ _i ];	\

#define XARRAYLINEAR_LOOP2( ARY, TYPE, I, ELEM )			\
{	\
	int _num = ARY.GetNum();		\
	for( int I = 0; I < _num; ++I )		\
	{	\
		TYPE ELEM = ARY[ I ];	\

#define XARRAYLINEAR_LOOP_IDX( ARY, TYPE, I, ELEM )			\
{	\
	int _num = ARY.GetNum();		\
	for( int I = 0; I < _num; ++I )		\
	{	\
	TYPE ELEM = ARY[ I ];	\

#define XARRAYLINEAR_LOOP_REV( ARY, TYPE, ELEM )			\
{	\
	int _num = ARY.GetNum();		\
	for( int _i = _num-1; _i >= 0; --_i )		\
	{	\
	TYPE ELEM = ARY[ _i ];	\

#define XARRAYLINEAR_CLEAR( ARY )			\
{	\
	int _max = ARY.GetMaxSize();		\
	for( int _i = 0; _i < _max; ++_i )		\
		ARY.Clear( _i, 0 ); \
}

#define XARRAYLINEAR_DESTROY( ARY )	\
{	\
	int _num = ARY.GetNum();		\
	for( int _i = 0; _i < _num; ++_i )		\
		delete ARY[ _i ];	\
	ARY.DestroyAll(); \
}

#define XARRAYLINEARN_DESTROY( ARY )	\
{	\
	int _num = ARY.size();		\
	for( int _i = 0; _i < _num; ++_i )		\
		delete ARY[ _i ];	\
	ARY.Clear(); \
}

#define XARRAYN_LINEAR_DESTROY( ARY )	XARRAYLINEARN_DESTROY( ARY )

/**
 @brief XList2는 각노드의 타입이 포인터형만 되는부분을 개선.
 XList3완성시키고 나서 부스트의 리스트 stl::list 메모리풀버전등 다양하게 속도테스트 해볼것.
*/
template <typename T>
class XList3
{
public:
	struct xNode;
	struct xNode {
		T value;			///< 실제 노드의 값
		xNode *pNext;	///< 다음노드의 포인터
		xNode *pPrev;	///< 앞노드의 인덱스
		bool bTail;		///< 꼬리노드인가
		xNode() {
			pNext = NULL;
			pPrev = NULL;
			bTail = false;
		}
	};
	class iterator {
	public:
		xNode *m_pCurr;
		iterator() : m_pCurr( NULL ) {}
		iterator( xNode *pNode ) : m_pCurr(NULL) {
			m_pCurr = pNode;
		}
		iterator& operator ++ () {	// ++A
			XBREAK( m_pCurr == NULL );
#ifdef _DEBUG
			xNode *pOld = m_pCurr;
#endif
			if( m_pCurr->bTail == false )
				m_pCurr = m_pCurr->pNext;
			else
				m_pCurr = nullptr;
//			XBREAK( m_pCurr == NULL );
			return *this;
		}
		iterator operator ++ ( int ) {	// A++
			XBREAK( m_pCurr == NULL );
			xNode *pVAlue = m_pCurr;
			// m_pCurr이 tail이면 이터레이터를 증가시키지 않는다.
			if( m_pCurr->bTail == false )
				m_pCurr = m_pCurr->pNext;
			else
				m_pCurr = nullptr;
//			XBREAK( m_pCurr == NULL );
			return iterator(pVAlue);
		}
		T& operator * () {
			return m_pCurr->value;
		}
		T* operator -> ( ) {
			return &m_pCurr->value;
		}
		BOOL operator != ( const iterator& itorComp ) {
			return m_pCurr != itorComp.m_pCurr;
		}
		BOOL operator == ( const iterator& itorComp ) {
			return m_pCurr == itorComp.m_pCurr;
		}
		
	};
private:
	xNode *m_pHead;			///< 리스트의 시작노드
	xNode *m_pTail;			///< 값이 있는 노드의 마지막노드
//	xNode *m_pEnd;			///< 값이 없는 노드의 마지막 노드
//	int m_idxLastEmpty;		// 비어있는 풀의 인덱스
	int m_nNum;				// 오브젝트 개수
	int m_nMax;				// 리스트의 최대 크기
	BOOL m_bSorted;		// 소트되어 있는가
//	T *m_pList;				// 오브젝트의 리스트
	xNode *m_aryNodes;		// 노드들의 정보를 갖는 어레이
//	int *m_pIdxNextEmpty;	// 비어있는 다음풀의 인덱스 
	void Init() {
		m_nNum = 0;
//		m_idxLastEmpty = 0; 
		m_nMax = 0; 
		m_pHead = NULL;
		m_pTail = NULL;
//		m_pEnd = NULL;
//		m_pList = NULL;
//		m_pIdxNextEmpty = NULL;
		m_aryNodes = NULL;
		m_bSorted = TRUE;		// Delete()가 들어오기전까진 첨엔 마치 소트된것처럼 앞에서부터 채워지기땜에 이렇게 해도 된다
	}
	void Destroy() {
//		SAFE_DELETE_ARRAY( m_ppList );
//		SAFE_DELETE_ARRAY( m_pIdxNextEmpty );
		SAFE_DELETE_ARRAY( m_aryNodes );
	}
public:
	XList3() { Init(); }
	XList3( int nMax ) { 
		Init(); 
		Create( nMax );
	}
	virtual ~XList3() { Destroy(); }
	void Create( int nMax );
	void clear() {
		InitLink();
		m_pTail = NULL;
		m_nNum = 0;
	}
	//
	// 어레이의 순서대로 링크를 다시 설정한다.
	void InitLink() {
		// 비어있는 다음 노드를 세1팅
		int nMax = m_nMax;
		int i;
		if( nMax )
		{
			for( i = 0; i < nMax-1; i ++ ) {
				xNode *p = &m_aryNodes[i];
				p->pNext = &m_aryNodes[i + 1];
				if( i == 0 )
					p->pPrev = NULL;
				else
					p->pPrev = &m_aryNodes[i - 1];
			}
			m_aryNodes[i].pNext = NULL;		// 가장 끝은 '뒤가'없다
			m_aryNodes[i].pPrev = &m_aryNodes[i - 1];		
//			m_aryNodes[i].bFill = 0;
			m_pHead = &m_aryNodes[0];
//			m_pTail = NULL;
		}
//		m_idxLastEmpty = 0;
//		m_nNum = 0;
//		memset( m_ppList, 0, sizeof( T* ) * nMax );

	}
	GET_ACCESSOR( int, nMax );
	GET_ACCESSOR( int, nNum );
	int size() {
		return m_nNum;
	}
	BOOL IsFull() {
		return ( m_nNum == m_nMax );
	}
	// 
	int push_back( const T& pObj ) {
		if( m_pTail == NULL ) {		// empty
			m_pTail = m_pHead;
			m_pTail->bTail = true;
			m_pTail->value = pObj;
		}
		else
		{
			if( m_pTail->pNext == NULL )	// 더이상 노드가 없음(full)
				return 0;
			m_pTail->bTail = false;
			m_pTail = m_pTail->pNext;
			m_pTail->bTail = true;
			m_pTail->value = pObj;
			if( m_pTail->pNext )
				m_pTail->pNext->pPrev = m_pTail;
//			m_pEnd = m_pTail->pNext;
		}
		++m_nNum;		// 노드 개수 증가
		return 1;
	}
	void erase( typename XList3<T>::iterator& itor ) {
		xNode *pCurr = itor.m_pCurr;
		xNode *pPrev = pCurr->pPrev;
		xNode *pNext = pCurr->pNext;
		if( pCurr == m_pHead ) {
			if( pCurr != m_pTail )
				m_pHead = pNext;
		}
		if( pCurr == m_pTail ) {
			if( m_pTail )
				m_pTail->bTail = false;
			m_pTail = pPrev;
			if( m_pTail )
				m_pTail->bTail = true;
		} else {
			// 앞뒤 노드를 연결시킴
			if( pPrev )
				pPrev->pNext = pNext;
			if( pNext )
				pNext->pPrev = pPrev;
			//현제삭제된 노드를 tail과 end의 사이에 껴넣는다.
			pCurr->pPrev = m_pTail;
			pCurr->pNext = m_pTail->pNext;
			if( m_pTail->pNext )
				m_pTail->pNext->pPrev = pCurr;
			m_pTail->pNext = pCurr;
		}
		--m_nNum;
	}
	typename XList3<T>::iterator begin() {
		return XList3<T>::iterator(m_pHead);
	}
	/// end는 무조건 tail의 다음노드다
	typename XList3<T>::iterator end() {
		if( m_pTail == nullptr )
			return XList3<T>::iterator( nullptr );
		if( m_pTail->bTail )
			return XList3<T>::iterator( nullptr );
		return XList3<T>::iterator(m_pTail->pNext);
	}
	typename XList3<T>::iterator tail() {
		if( m_pTail == nullptr )
			return XList3<T>::iterator( nullptr );
		return XList3<T>::iterator( m_pTail );
	}

// 	T* GetRandom() {
// 		int itor = 0;
// 		return GetNext( &itor );
// 	}
	struct compGreater { 
		bool operator()( const xNode &lhs, const xNode &rhs ){
			return lhs.value < rhs.value;
		}
	};
	struct compLesser {
		bool operator()( const xNode &lhs, const xNode &rhs ){
			return lhs.value > rhs.value;
		}
	};
	// sort
//	void Sort( int (*ptFuncCompare)( const void *p1, const void *p2 ) ) {
	template<typename C>
	void Sort( C& c ) {
//		std::sort( begin(), end() );
		std::sort( m_aryNodes, m_aryNodes+m_nMax, c );
//		qsort( m_aryNodes, m_nMax, sizeof(T), ptFuncCompare );
		InitLink();
		if( m_nNum > 0 ) {
			if( m_pTail )
				m_pTail->bTail = false;
			m_pTail = &m_aryNodes[m_nNum - 1];
			m_pTail->bTail = true;
		} else {
			if( m_pTail )
				m_pTail->bTail = false;
			m_pTail = NULL;
		}
	}

}; // XList3

template<typename T>
class XList4;
/**
 @brief std::list를 상속받은 버전.
*/
template<typename T>
class XList4 : public std::list<T>
{
	T m_Empty;
public:
	XList4() {}
	~XList4() {}
	//
	void operator = ( std::vector<T>& ary ) {
		this->clear();
		for( auto& elem : ary ) {
			push_back( elem );
		}
	}

	inline T Add( T&& elem ) {
		this->push_back( elem );
		return elem;
	}
	inline T Add( const T&& elem ) {
		this->push_back( elem );
		return elem;
	}
	inline T Add( const T& elem ) {
		this->push_back( elem );
		return elem;
	}
	void Del( T& elem ) {
		auto iter = std::find( this->begin(), this->end(), elem );
		if( iter != this->end() )
			this->erase(iter);
	}
	void Del( const T& elem ) {
		auto iter = std::find( this->begin(), this->end(), elem );
		if( iter != this->end() )
			this->erase(iter);
	}
	T* DelByIdx( int idxFind ) {
		int idx = 0;
		typename std::list<T>::iterator iter;
		for( iter = this->begin(); iter != this->end(); ++iter) {
			if( idx++ == idxFind )
				break;
		}
		if( iter != this->end() )
		{
			T* pTemp = &(*iter);
			this->erase( iter );
			return pTemp;
		}
		return nullptr;
	}
	bool DelByID( ID idElem ) {
		auto iter = std::find_if(this->begin(), this->end(), [idElem](T& elem)->bool {
			if( elem->getid() == idElem )
				return true;
			return false;
		} );
		if (iter != this->end()) {
			this->erase( iter );
			return true;
		}
		return false;
	}
	bool DelByIDNonPtr( ID idElem ) {
		auto iter = std::find_if( this->begin(), this->end(), [idElem]( T& elem )->bool {
			if( elem.getid() == idElem )
				return true;
			return false;
		} );
		if( iter != this->end() ) {
			this->erase( iter );
			return true;
		}
		return false;
	}
// 	T* DelByIDAndReturn( ID idElem ) {
// 		auto iter = std::find_if( this->begin(), this->end(), 
// 			[idElem]( T& elem )->bool {
// 			if( elem->getid() == idElem )
// 				return true;
// 			return false;
// 		} );
// 		if( iter != this->end() ) {
// 			T *pNode = &( *iter );
// 			this->erase( iter );
// 			return pNode;
// 		}
// 		return nullptr;
// 	}
	template<typename F>
	void DelIf( F func ) {
		auto iter = std::find_if(this->begin(), this->end(), func);
		if (iter != this->end())
			this->erase(iter);
	}
	template<typename F>
	T* FindpIf( F func ) {
		auto iter = std::find_if(this->begin(), this->end(), func);
		if (iter != this->end())
			return &(*iter);
		return nullptr;
	}
	T* Findp( const T& elem  ) {
		auto iter = std::find(this->begin(), this->end(), elem);
		if (iter != this->end())
			return &(*iter);
		return nullptr;
	}
	int FindToIdx( const T& findObj ) {
		int idx = 0;
		for( T& elem : *this ) {
			if( elem == findObj )
				return idx;
			++idx;
		}
		return -1;
	}
	T* FindpByID( ID idNode ) {
		auto iter = std::find_if(this->begin(), this->end(),
			[idNode](T pElem)->bool {  
			return pElem->getid() == idNode;
		} );
		if (iter != this->end())
			return &( *iter );
		return nullptr;
	}
	T* FindByIDNonPtr( ID idNode ) {
		auto iter = std::find_if(this->begin(), this->end(),
			[idNode](T pElem)->bool {  
			return pElem.getid() == idNode;
		} );
		if (iter != this->end())
			return &( *iter );
		return nullptr;
	}
	int GetIndex( T& elem ) {
		int idx = 0;
		for (auto iter = this->begin(); iter != this->end(); ++iter) {
			if( (*iter) == elem )
				break;
			++idx;
		}
		return idx;
	}
	// itorComp가 현재 리스트의 몇번째인지 얻는다.
	int GetIndex(const typename std::list<T>::iterator& itorComp) {
		int idx = 0;
		for( auto itor = this->begin(); itor != this->end(); ++itor ) {
			if( itor == itorComp )
				break;
			++idx;
		}
		return idx;
	}
	T* GetpFirst() {
		return &(*this->begin());
	}
	T* GetpLast() {
		return &(*--this->end());
	}
	T* GetpByIndex( int idx ) {
		int i = 0;
		for (auto iter = this->begin(); iter != this->end(); ++iter) {
			if( i++ == idx )
				return &(*iter);
		}
		return nullptr;
	}
	T& GetFirst() {
		return ( *this->begin() );
	}
	T& GetLast() {
		return ( *--this->end() );
	}
	T& GetByIndex( int idx ) {
		int i = 0;
		for( auto iter = this->begin(); iter != this->end(); ++iter ) {
			if( i++ == idx )
				return ( *iter );
		}
		return m_Empty;
	}
	T GetByIndexNonPtr( int idx ) {
		int i = 0;
		for( auto iter = this->begin(); iter != this->end(); ++iter ) {
			if( i++ == idx )
				return ( *iter );
		}
		return nullptr;
	}
	typename XList4<T>::iterator GetItorByIndex( int idx ) {
		int i = 0;
		for( auto iter = this->begin(); iter != this->end(); ++iter ) {
			if( i++ == idx )
				return iter;
		}
		return this->end();
	}
	typename XList4<T>::iterator GetItorByID( ID idElem ) {
		for( auto iter = this->begin(); iter != this->end(); ++iter ) {
			if( (*iter)->getid() == idElem )
				return iter;
		}
		return this->end();
	}
	typename XList4<T>::iterator GetItorByIDNonPtr( ID idElem ) {
		for( auto iter = this->begin(); iter != this->end(); ++iter ) {
			if( ( *iter ).getid() == idElem )
				return iter;
		}
		return this->end();
	}

	T* GetpFromRandom() {
		int max = this->size();
		return GetpByIndex( xRandom(max) );
	}
	T& GetFromRandom() {
		int max = this->size();
		return GetpByIndex( xRandom( max ) );
	}
	// 리스트내에서 랜덤으로 한 노드의 이터레이터를 꺼낸다.
	typename XList4<T>::iterator GetItorFromRandom() {
		int max = this->size();
		return GetItorByIndex( xRandom(max) );
	}
	// 리스트 요소중 numDel개만큼 삭제한다.
	bool DelByRandom( int numDel ) {
		int cnt = numDel;
		int numDeleted = 0;
		while( cnt-- > 0 && std::list<T>::size() > 0 ) {
			auto itor = GetItorFromRandom();
			std::list<T>::erase( itor );
			++numDeleted;
		}
		return numDel == numDeleted;
	}
	/**
		listNew.Sort( [](xTest* pNode1, xTest* pNode2)->bool{
			if( pNode1 && pNode2 )
				return pNode1->a > pNode2->a;
			return false;
		});
	*/
	template<typename F>
	void Sort( F func ) {
		std::list<T>::sort( func );
	}
	template<int N>
	void operator = ( XArrayLinearN<T, N>& ary ) {
		this->clear();
		int num = ary.size();
		for( int i = 0; i < num; ++i ) {
			this->push_back( ary[i] );
		}
	}
	void Clear() {
		std::list<T>::clear();
	}
}; // XList4

#define XLIST4_DESTROY( LIST )		{			\
	for( auto itor = LIST.begin(); itor != LIST.end(); ) \
		{	\
		auto pNode = (*itor);		\
		SAFE_DELETE( pNode );		\
		LIST.erase( itor++ );		\
		}	\
	LIST.clear(); \
}	
#define XVECTOR_DESTROY( VECTOR ) { \
	for( auto pElem : VECTOR ) \
		SAFE_DELETE( pElem ); \
	VECTOR.clear(); \
}
#define XMAP_DESTROY( MAP ) { \
	for( auto& itor : MAP ) { \
		SAFE_DELETE( itor.second ); \
	} \
	MAP.clear(); \
}
#define AUTO_DESTROY( CONTAINER ) { \
	for( auto pElem : CONTAINER ) { \
		SAFE_DELETE( pElem ); \
	} \
	CONTAINER.clear(); \
}


template<typename T, int N>
class XArrayN2 : public std::vector < T >
{
public:
	XArrayN2() : std::vector<T>( N ) {}
};

template<typename T, int N>
class XArrayLinearN2 : public std::vector < T >
{
public:
	XArrayLinearN2() : std::vector<T>( N ) {}
};

template<typename T>
class XVector : public std::vector < T >
{
public:
	XVector() : std::vector<T>() {}
	XVector( int size ) : std::vector<T>( size ) {}
	void operator=( const XList4<T>& listSrc ) {
		for( auto &elem : listSrc ) 
			Add( elem );
	}
	inline T* GetpFromRandom() {
		return &std::vector<T>::at( xRandom( std::vector<T>::size()) );
	}
	inline T& GetFromRandom() {
		return std::vector<T>::at( xRandom( std::vector<T>::size() ) );
	}
	inline void Add( T& elem ) {
		std::vector<T>::push_back( elem );
	}
	inline void Add( const T& elem ) {
		std::vector<T>::push_back( elem );
	}
	inline void Add( const T&& elem ) {
		std::vector<T>::push_back( elem );
	}
	inline void Clear() {
		std::vector<T>::clear();
	}
	inline int GetIdx( const T& val ) {
		int idx = 0;
		for( auto& v : this ) {
			if( v == val )
				return idx;
			++idx;
		}
	}
	inline int Size() const {
		return (int)std::vector<T>::size();
	}
	inline void Fill( const T& val ) {
		std::vector<T>::assign( Size(), val );
	}
// 	template<typename T>
// 	void DeSerializeSharedPtrElem( XArchive& ar, int ver ) {
// 		XBREAK( this->size() != 0 );
// 		int num;
// 		ar >> num;
// 		for( int i = 0; i < num; ++i ) {
// 			auto spElem = std::shared_ptr<T>( new T() );
// 			spElem->DeSerialize( ar, ver );
// 			this->push_back( spElem );
// 		}
// 	}
// 	template<typename T>
// 	void DeSerializePtrElem( XArchive& ar, int ver ) {
// 		XBREAK( this->size() != 0 );
// 		int num;
// 		ar >> num;
// 		for( int i = 0; i < num; ++i ) {
// 			auto pElem = new T();
// 			pElem->DeSerialize( ar, ver );
// 			this->push_back( pElem );
// 		}
// 	}
// 	template<typename T>
// 	void SerializePtrElem( XArchive& ar ) const {
// 		ar << (int)this->size();
// 		for( auto pElem : *this ) {
// 			pElem->Serialize( ar );
// 		}
// 	}
};

#include "XList.inl"

#pragma warning( default : 4700 )	
