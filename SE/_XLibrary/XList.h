#pragma once

#include "global.h"
#include <list>
using namespace std;

#ifdef __cplusplus
// global.h에 있는 LIST_LOOP등을 쓰면 이터레이터앞에 typename이 없다고 에러난다.
// 그렇다고 그쪽에 typename을 붙여버리면 이번엔 템플릿아닌데서 쓰는건 다 에러난다
// 그래서 여기에 전용코드를 만듬
#define __LIST_DESTROY( LIST, NODE_TYPE )		{			\
										typename list<NODE_TYPE>::iterator itor; \
										for( itor = LIST.begin(); itor != LIST.end(); ) \
										{	\
											NODE_TYPE pNode = (*itor);		\
											SAFE_DELETE( pNode );		\
											LIST.erase( itor++ );		\
										}	\
									}	\

#define __LIST_CLEAR( LIST, NODE_TYPE )		{			\
								typename list<NODE_TYPE>::iterator itor; \
								for( itor = LIST.begin(); itor != LIST.end(); ) \
								{	\
								LIST.erase( itor++ );		\
								}	\
								}	\

#define __LIST_LOOP( LIST, NODE_TYPE, I, E )	{			\
								typename list<NODE_TYPE>::iterator I; \
								int li;	\
								for( li=0, I = LIST.begin(); I != LIST.end(); li++, I ++ ) \
								{	\
								NODE_TYPE E = (*I);		\

#define __LIST_LOOP_REVERSE( LIST, NODE_TYPE, I, E )	{			\
								typename list<NODE_TYPE>::reverse_iterator I; \
								for( I = LIST.rbegin(); I != LIST.rend(); I ++ ) \
								{	\
								NODE_TYPE E = (*I);		\


#define __LIST_MANUAL_LOOP( LIST, NODE_TYPE, I, E )	{			\
								typename list<NODE_TYPE>::iterator I; \
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
		typename list<T>::iterator	m_Itor;
		Itor() {}
		Itor( typename list<T>::iterator itor ) { m_Itor = itor; }
		virtual ~Itor() {}

		Itor& operator ++ () {	// ++A
			++m_Itor;
			return *this;
		}
		const Itor operator ++( int ) {	// A++
			Itor itor = *this;
			++*this;
			return itor;
		}
	};
private:
//	BOOL m_bNodeDestroy;		// 노드 알맹이까지 여기서 파괴하는지 여부
	list<T>			m_List;
	typename list<T>::iterator	m_Itor;
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
	int size() { return m_List.size(); }		// 호환성을 위해
	int Size() { return m_List.size(); }
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

	BOOL Find( T pElem ) {
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
	Itor GetNextClear() { 
		Itor itor( m_Itor = m_List.begin() );
		return itor; 
	}
	T GetNext( typename XList::Itor *pItor ) { 
		if( pItor->m_Itor == m_List.end() )
			return nullptr;
		return (*(pItor->m_Itor)++);
	}
	T GetPrev( typename XList::Itor *pItor ) {		// 이거 구현좀 이상하군. prev의 동작정의를 확실히 해야할듯 itor의 end()가 cdcd인 특성상 prev동작은 이전으로 포인터부터 이동하고 그 포인터를 리턴해줘야 할듯
		if( pItor->m_Itor == m_List.begin() )
			return nullptr;
		if( pItor->m_Itor == m_List.end() )	// 포인터가 맨 뒤노드를 가리키고 있으면
		{
			--pItor->m_Itor;							// 일단 마지막 노드로 포인터를 옮기고
			if( pItor->m_Itor == m_List.begin() )		// 마지막 노드가 시작노드면
				return *pItor->m_Itor;					// 시작노드를 리턴
		} else
			--m_Itor;							// 일단 마지막 노드로 포인터를 옮기고
		return (*m_Itor--);
	}
	T GetCurrent() {
		if( m_Itor == m_List.end() )
			return nullptr;
		return (*m_Itor);
	}
	T GetNext() { 
		if( m_Itor == m_List.end() )
			return nullptr;
		return (*m_Itor++);
	}
	T GetPrev() {		// 이거 구현좀 이상하군. prev의 동작정의를 확실히 해야할듯 itor의 end()가 cdcd인 특성상 prev동작은 이전으로 포인터부터 이동하고 그 포인터를 리턴해줘야 할듯
		if( m_Itor == m_List.begin() )
			return nullptr;
		if( m_Itor == m_List.end() )	// 포인터가 맨 뒤노드를 가리키고 있으면
		{
			--m_Itor;							// 일단 마지막 노드로 포인터를 옮기고
			if( m_Itor == m_List.begin() )		// 마지막 노드가 시작노드면
				return *m_Itor;					// 시작노드를 리턴
		} else
			--m_Itor;							// 일단 마지막 노드로 포인터를 옮기고
		if( m_Itor == m_List.begin() )
			return nullptr;
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
		__LIST_MANUAL_LOOP( m_List, T, itor, pNode )
		{
			m_List.erase( itor++ );
		} END_LOOP;
	}

	void Destroy() {		// 노드안에 알맹이까지 파괴시키는 버전. 리스트 삭제할때 XLIST_LOOP로 삭제하기가 귀찮아서 부활시킴. ~XGRoll에 사용
		__LIST_MANUAL_LOOP( m_List, T, itor, pNode )
		{
			delete pNode;
			m_List.erase( itor++ );
		} END_LOOP;
	} 

	T GetFirst() {
		typename list<T>::iterator itor = m_List.begin();
		return (*itor);
	}
	T GetLast() {				// 
		typename list<T>::iterator itor = m_List.end();
		return (*--itor);
	}

	T GetFromIndex( int idx ) {
		int i = 0;
		__LIST_LOOP( m_List, T, itor, pNode ) {
			if( i++ == idx )
				return pNode;
		} END_LOOP;
		return nullptr;
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
	T LuaGetNextClear() {
		GetNextClear();
		return GetNext();
	}
	T LuaGetNext() {
		return GetNext();
	}
};
/*
#define XLIST_LOOP( S, NODE_TYPE, E )	  { \
		NODE_TYPE E;				\
		S.GetNextClear();	\
		while( E = S.GetNext() )		\
		{
*/
#define XLIST_LOOP( S, NODE_TYPE, E )	  { \
		NODE_TYPE E;				\
		XList<NODE_TYPE>::Itor _itor = S.GetNextClear();	\
		while( E = S.GetNext( &_itor ) )		\
		{
#define XLIST_DESTROY( LIST, TYPE )	\
		XLIST_LOOP( LIST, TYPE, pObj ) \
			SAFE_DELETE( pObj );			\
		END_LOOP;	

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
	void Init() {
		m_nNum = m_idxLastEmpty = 0; 
		m_nMax = 0; 
		m_ppList = nullptr;
		m_pIdxNextEmpty = nullptr;
		m_bSorted = TRUE;		// Delete()가 들어오기전까진 첨엔 마치 소트된것처럼 앞에서부터 채워지기땜에 이렇게 해도 된다
	}
	void Destroy() {
		if( m_ppList )
		{
			for( int i = 0; i < m_nMax; ++i )
				m_ppList[i] = nullptr;
		}
		SAFE_DELETE_ARRAY( m_ppList );
		SAFE_DELETE_ARRAY( m_pIdxNextEmpty );
	}
public:
	XList2() {	Init(); 	}			
	XList2( int nMax ) 
	{ 
		Init(); 
		Create( nMax );
	}
	virtual ~XList2() { Destroy(); }
	void Create( int nMax ) {
		m_nMax = nMax;
		XBREAK( m_ppList != nullptr );
		XBREAK( m_pIdxNextEmpty != nullptr );
		m_ppList = new T*[ nMax ];
		m_pIdxNextEmpty = new int[ nMax ];
		memset( m_ppList, 0, sizeof( T* ) * nMax );
		// 비어있는 다음 노드를 세1팅
		int i;
		for( i = 0; i < nMax-1; i ++ )
			m_pIdxNextEmpty[i] = i+1;
		m_pIdxNextEmpty[ i ] = -1;		// 가장 끝은 '뒤가'없다
	}
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
	GET_ACCESSOR( int, nMax );
	GET_ACCESSOR( int, nNum );
	// 
	void Add( T* pObj ) {
		int idx = m_idxLastEmpty;
		if( XBREAK( idx < 0 || idx >= m_nMax ) )	
		{
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
		m_ppList[ idx ] = nullptr;
		m_pIdxNextEmpty[ idx ] = m_idxLastEmpty;		// 원래 '빈곳'을 가리켰던 포인터를 다음포인터로 연결
//		XBREAK( m_idxLastEmpty < 0 );
		m_idxLastEmpty = idx;								// 삭제된곳을 '빈곳'으로 설정
		--m_nNum;
	}
	// 리스트에서 pNode의 노드지운다.
	void Delete( const T* pNode ) { 
		int idx = FindIdx( pNode );
		if( idx < 0 )	return;
		Delete( idx ); 
	}
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

//	T* GetNode( int idx ) { 
//		XBREAK( idx >= 0 && idx < m_nMax ); 
//		return m_ppList[ idx ]; 
//	}
	// pItorIdx로부터 탐색하여 채워진 노드의 포인터를 리턴한다. 
	// 이터레이터는 자동으로 다음 노드를 가리킨다. 다음노드는 빈곳일수도 있으므로 그대로 사용해선 안된다.
	// pItorIdx가 대부분 쓸모없기때문에 가급적 XLIST2_LOOP매크로를 사용하길 권한다
	T* GetNext( int *pItorIdx ) {	
		int nMax = m_nMax;
		for( int i = *pItorIdx; i < nMax; ++i ) {		// 채워진곳을 찾는 루프
			if( m_ppList[i] )	{
				*pItorIdx = ++i;							// 다음턴엔 m_nIdx다음부터 해야하므로.
				return m_ppList[ i - 1 ];
			}
		}
		*pItorIdx = 0;													// 끝까지 다찾았으면 초기화시킨다.
		return nullptr;												// nullptr이라는것은 빈슬롯이 더이상 없이 배열끝까지 탐색했다.
	}
	// pItorIdx로부터 탐색하여 채워진 노드의 포인터를 리턴한다. 
	// *pItorIdx는 return T*의 인덱스를 가리킨다. 이터레이터가 자동으로 증가하지 않으므로 사용후 사용자가 직접 itor++를 해줘야 한다. STL처럼
	T* GetManualNext( int *pItorIdx ) {	
		int nMax = m_nMax;
		for( int i = *pItorIdx; i < nMax; ++i ) {		// 채워진곳을 찾는 루프
			if( m_ppList[i] )	{
				*pItorIdx = i;	
				return m_ppList[ i ];
			}
		}
		*pItorIdx = 0;													// 끝까지 다찾았으면 초기화시킨다.
		return nullptr;												// nullptr이라는것은 빈슬롯이 더이상 없이 배열끝까지 탐색했다.
	}
	// sort
	// void*로 받지말고 T*로 받아야 할듯
	void Sort( int (*ptFuncCompare)( const void *p1, const void *p2 ) ) {
		qsort( m_ppList, m_nMax, sizeof(T*), ptFuncCompare );
		int nMax = m_nMax;
		int i;
		for( i = 0; i < nMax-1; ++ i )
		{
			if( m_ppList[i] == nullptr )
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
		NODE_TYPE ELEM;				\
		int ELEM##_idxPool = 0;				\
		while( ELEM = LIST.GetNext( &ELEM##_idxPool ) )		\
		{

#define XLIST2_MANUAL_LOOP( LIST, NODE_TYPE, ITOR, ELEM )	  { \
		NODE_TYPE ELEM;				\
		int ITOR = 0;				\
		while( ELEM = LIST.GetManualNext( &ITOR ) )		\
		{

#define XLIST2_DESTROY( LIST, TYPE )	\
		XLIST2_LOOP( LIST, TYPE, pObj ) \
			SAFE_DELETE( pObj );			\
		END_LOOP;	


//////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
class XQueue2
{
	T **m_ppList;
	int m_nSize;
	int m_nMax;
	int m_itor;
	void Init() {
		m_ppList = nullptr;
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
	void Create( int nMax ) {
		m_nMax = nMax;
		m_ppList = new T*[ nMax ];
		memset( m_ppList, 0, sizeof(T*) * nMax );
	}

	void Push( T* pObj ) {
//		if( XASSERT( m_nSize < m_nMax ) )
		if( m_nSize < m_nMax )
			m_ppList[ m_nSize++ ] = pObj;
	}
	T* Pop() {
		T* front = m_ppList[0];		// 젤 앞에걸 하나 빼고
		int size = m_nSize--;		// 큐 전체 크기는 하나 줄인다
		int i;
		for( i = 1; i < size; ++i )
			m_ppList[i-1] = m_ppList[i];	// 앞으로 하나씩 당긴다
		m_ppList[size-1] = nullptr;
		return front;
	}
	T* Front() {
		return m_ppList[0];	
	}
	void GetNextClear() { m_itor = 0; }
	T* GetNext() {
		if( m_itor < m_nSize )
			return m_ppList[ m_itor++ ];
		return nullptr;
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

	


//////////////////////////////////////////////////////////////////////////////////////////////////////
// 100개 이하의 아이템을 배열리스트로 관리해주는 클래스. 삭제기능없음
// 초기 배열크기 지정이 필요없다. local용 임시배열로 쓰기 좋다.
template<typename T>
class XArray100
{
	T m_ppList[ 100 ];
	int m_nNum;
	int m_itor;
public:
	XArray100() {
		// 가볍고 빠른게 목적이므로 속도를 위해서 list초기화 하지 않았음
		m_nNum = 0;
	}
	virtual ~XArray100() {}
	// idx번째의 배열요소를 꺼낸다.
	T& operator [] ( int idx ) {
		XBREAK( idx >= m_nNum );
		return m_ppList[idx];
	}
	GET_ACCESSOR( int, nNum );
	int size() { return m_nNum; }
	void push_back( T pObj ) { Add( pObj ); }
	void Add( T pObj ) {
		if( XASSERT( m_nNum < 100 ) )
			m_ppList[ m_nNum++ ] = pObj;
	}
	void GetNextClear() { m_itor = 0; }
	T GetNext() { 
		if( m_itor >= m_nNum )
			return nullptr;
		return m_ppList[ m_itor++ ];
	}
	void sort( int (*compFunc)( const void *p1, const void *p2 ) ) {
		qsort( m_ppList, 100, sizeof(T), compFunc );
	}
};

//#define XARRAY100_LOOP( ARRAY, ELEM_TYPE, ELEM )	{	\
//	ELEM_TYPE ELEM;	\
//	ARRAY.GetNextClear();	\
//	while( ELEM = ARRAY.GetNext() ) {	
#define XARRAY100_LOOP( ARRAY, ELEM_TYPE, ELEM )	{	\
	int _num = ARRAY.GetnNum();	\
	for( int i = 0; i < _num; ++i ) {	\
		ELEM_TYPE ELEM = ARRAY[ i ];


	
// c의 배열이나 std::vector와 거의 흡사한 기능의 배열 템플릿
// 리니어 하지 않은 배열데이타에 사용한다.
#define	DEBUG_ARRAY
template<typename T>
class XArray
{
	void Init() {
		m_nMax = 0;
		m_pArray = nullptr;
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
	virtual ~XArray() { Destroy(); }
	// 배열을 최초 생성한다.
	void Create( int max ) {
		XBREAK( m_pArray );
		XBREAK( max <= 0 );
		m_pArray = new T[ max ];		// 배열 최대크기만큼 할당.
		m_nMax = max;
	}
	void Clear() {
		Destroy();		// 어레이 껍데기만 날림.
		Init();
	}
	int GetMax() { return m_nMax; }
	T* operator&() const {
		return m_pArray;
	}
	void operator=( XArray& ary ) {
		Destroy();
		Init();
		Create( ary.GetMax() );
		int num = ary.GetMax();
		for( int i = 0; i < num; ++i ) {
			Add( ary[i] );
		}
	}
	// idx번째의 배열요소를 꺼낸다.
	T& operator [] ( int idx ) {
		XBREAK( m_pArray == nullptr );
		XBREAK( idx >= m_nMax );
#ifdef DEBUG_ARRAY
		m_ppDebugArray[ idx ] = &m_pArray[ idx ];
#endif
		return m_pArray[idx];
	}
	T& Get( int idx ) {
		XBREAK( m_pArray == nullptr );
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
		XBREAK( m_pArray == nullptr );
		XBREAK( idx >= m_nMax );
		m_pArray[ idx ] = elem;
		DebugArrayCopy();
	}
	// 배열의 크기를 바꾼다.
	// 리사이즈가 되면 m_pArray의 어드레스를 다른곳에서 참조하는경우 크래쉬 되는 위험이 있다.
	void Resize( int newSize ) {
		XBREAK( m_pArray == nullptr );
		XBREAK( newSize <= 0 );
		XBREAK( newSize <= m_nMax  );
		T *pNewArray = new T[ newSize ];
		// 새 버퍼로 옮김.
		int Min = xmin( newSize, m_nMax );
		for( int i = 0; i < Min; ++i )
			pNewArray[i] = m_pArray[i];
		SAFE_DELETE_ARRAY( m_pArray );
		m_pArray = pNewArray;
		m_nMax = newSize;
	}
	// e와 같은걸 찾아서 그 인덱스를 리턴한다.
	int Find( T e ) {
		XBREAK( m_pArray == nullptr );
		T *p = m_pArray;
		int max = m_nMax;
		for( int i = 0; i < max; ++i ) {
			if( *p++ == e )
				return i;
		}
		return -1;
	}
};	// XArray<>
// 리니어하게 데이타가 들어있는 배열의 템플릿.
template<typename T>
class XArrayLinear
{
	void Init() {
		m_nNum = m_nMax = 0;
		m_pArray = nullptr;
#ifdef _DEBUG
//		XCLEAR_ARRAY( m_debugArray );	// T가 클래스일수도 있으므로 이런식으로 쓰지 말것.
#endif
	}
	void Destroy() {
		SAFE_DELETE_ARRAY( m_pArray );	
		/*
		음.... delete[]로 지우는게 맞다. 내부가 안지워지길 원한다면 애초에 <클래스*>형으로 넣었어야 했다.
		delete로 지운다고 해도 여전히 파괴자 호출하더라.
		*/
	}
protected:
	T *m_pArray;			// 배열
#ifdef DEBUG_ARRAY
	T *m_ppDebugArray[ 1024 ];		// 디버깅용.
#endif
	int m_nNum;			// 배열 크기
	int m_nMax;			// 배열 최대크기
public:	
	XArrayLinear() { Init(); }
	virtual ~XArrayLinear() { Destroy(); }
	// 배열을 최초 생성한다.
	void Create( int max ) {
		XBREAK( m_pArray );
		XBREAK( max <= 0 );
		m_pArray = new T[ max ];		// 배열 최대크기만큼 할당.
		m_nNum = 0;
		m_nMax = max;
	}
//	void DestroyAll() {
//		Destroy();
//		Init();
//	}
	void Clear() {
		Destroy();		// 어레이 껍데기만 날림.
		Init();
	}
	int GetNum() { return m_nNum; }
	int GetMaxSize() { return m_nMax; }
	T* operator&() const {
		return m_pArray;
	}
	void operator=( XArrayLinear& ary ) {
		Destroy();
		Init();
		Create( ary.GetMaxSize() );
		int num = ary.GetNum();
		for( int i = 0; i < num; ++i ) {
			Add( ary[i] );
		}
	}
	// idx번째의 배열요소를 꺼낸다.
	T& operator [] ( int idx ) const {
		XBREAK( m_pArray == nullptr );
		XBREAK( idx >= m_nMax );
		return m_pArray[idx];
//		return Get( idx );
	}
	const T& Get( int idx ) {
		XBREAK( m_pArray == nullptr );
		XBREAK( idx >= m_nNum );
		XBREAK( idx >= m_nMax );
		return m_pArray[idx];
	}
	// 배열의 가장 뒤에 있는 요소를 꺼낸다.
	const T& GetTail() {
		XBREAK( m_pArray == nullptr );
		XBREAK( m_nNum <= 0 );
		return m_pArray[ m_nNum-1 ];
	}
	void DebugArrayCopy()	{
#ifdef DEBUG_ARRAY
		int num = (m_nMax>1024)? 1024:m_nMax;
		for( int i = 0; i < num; ++i )
			m_ppDebugArray[i] = &m_pArray[i];	// T가 클래스일수도 있으므로 memcpy쓰면 안됨.
#endif
	}
	// idx번째의 배열에 값을 덮어씌운다.
	// 리니어한 배열전용이므로 이것은 사용하지 말것. 모두 Add로 추가시킬것
/*	void Set( int idx, const T& elem ) {
		XBREAK( m_pArray == nullptr );
		XBREAK( idx >= m_nNum );
		XBREAK( idx >= m_nMax );
		m_pArray[ idx ] = elem;
		DebugArrayCopy();
	} */
	void Clear( int idx, const T& elem ) {
		XBREAK( m_pArray == nullptr );
		XBREAK( idx >= m_nMax );
		m_pArray[ idx ] = elem;
		DebugArrayCopy();
	}
	// elem을 배열에 추가시킨다.
	int Add( const T& elem ) {
		XBREAK( m_pArray == nullptr );
		XBREAK( m_nNum >= m_nMax );
		m_pArray[ m_nNum ] = elem;
		DebugArrayCopy();
		return m_nNum++;
	}
	// 배열 맨 뒤를 삭제
	void DelTail() {
		XBREAK( m_pArray == nullptr );
		XBREAK( m_nNum <= 0 );
		--m_nNum;
	}
	// 배열의 크기를 바꾼다.
	// 리사이즈가 되면 m_pArray의 어드레스를 다른곳에서 참조하는경우 크래쉬 되는 위험이 있다.
	void Resize( int newSize ) {
		XBREAK( m_pArray == nullptr );
		XBREAK( m_nNum > m_nMax );
		XBREAK( newSize <= 0 );
		XBREAK( newSize <= m_nMax && newSize <= m_nNum );
		T *pNewArray = new T[ newSize ];
		// 새 버퍼로 옮김.
		int Min = xmin( newSize, m_nMax );
		for( int i = 0; i < Min; ++i )
			pNewArray[i] = m_pArray[i];
		SAFE_DELETE_ARRAY( m_pArray );
		m_pArray = pNewArray;
		m_nMax = newSize;
	}
	// e와 같은걸 찾아서 그 인덱스를 리턴한다.
	int Find( T e ) {
		XBREAK( m_pArray == nullptr );
		XBREAK( m_nNum <= 0 );
		T *p = m_pArray;
		for( int i = 0; i < m_nNum; ++i ) {
			if( *p++ == e )
				return i;
		}
		return -1;
	}
};
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
        if( m_pArray == nullptr )
            return;
		XBREAK( m_pArray == nullptr );
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
		XBREAK( m_pArray == nullptr );
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
		XBREAK( m_pArray == nullptr );
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
	int max = ARY.GetMax();		\
	for( int _i = 0; _i < max; ++_i )		\
	{	\
		TYPE ELEM = ARY[ _i ];

#define XARRAY_DESTROY( ARY )	\
{	\
	int max = ARY.GetMax();		\
	for( int _i = 0; _i < max; ++_i )		\
	{	\
		if( ARY[ _i ] ) \
		{ \
			delete ARY[ _i ];	\
			ARY[ _i ] = nullptr; \
		} \
	} \
}

#define XARRAYLINEAR_LOOP( ARY, TYPE, ELEM )			\
{	\
	int num = ARY.GetNum();		\
	for( int _i = 0; _i < num; ++_i )		\
	{	\
		TYPE ELEM = ARY[ _i ];

#define XARRAYLINEAR_CLEAR( ARY )			\
{	\
	int max = ARY.GetMaxSize();		\
	for( int _i = 0; _i < max; ++_i )		\
		ARY.Clear( _i, 0 ); \
}

#define XARRAYLINEAR_DESTROY( ARY )	\
{	\
	int num = ARY.GetNum();		\
	for( int _i = 0; _i < num; ++_i )		\
		delete ARY[ _i ];	\
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
