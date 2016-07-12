#pragma once

#include <stack>
#include <queue>
#include <vector>
#include <map>
#include <list>

// STL을 내부로 숨기기위해 만들었다.
template <typename T>
class XStack
{
	std::stack<T> m_stack;
public:
	XStack() {}
	~XStack() {}

	void	Push( T data ) {
		m_stack.push( data );
	}
	T	Pop() {
		T data = m_stack.top();
		m_stack.pop();
		return data;
	}
	T	Top() {
		if( m_stack.size() <= 0 )
			return 0;
		return m_stack.top();
	}
};

// template <typename T>
// class XQueue
// {
// protected:
// 	std::queue<T> m_queue;
// public:
// 	XQueue() {}
// 	~XQueue() {}
// 
// 	BOOL IsEmpty() { return m_queue.empty(); }
// 	int Size() { return m_queue.size(); }
// 	int size() { return m_queue.size(); }
// 	void	Push( T data ) {
// 		m_queue.push( data );
// 	}
// 	void Pop() {
// 		if( m_queue.empty() )	return;
// 		m_queue.pop();
// 	}
// 	T Front() {
// 		return m_queue.front();
// 	}
// 	T Back() {
// 		return m_queue.back();
// 	}
// 	// 
// 	void Create( int size ) {}
// 	// 큐가 가득찼는가....stl버젼에서는 항상 false
// 	BOOL IsFull() {
// 		return FALSE;
// 	}
// 	// 큐의 남은 공간개수를 리턴....stl버전에서는 항상 1이 리턴된다
// //	int GetLeftSpace() {
// //		return 1;
// //	}
// 	// T로 이루어진 배열(버퍼)을 차례로 푸쉬한다
// 	void PushBlock( T* pBuffer, int len ) {
// 		for( int i = 0; i < len; ++i )
// 			m_queue.push( *pBuffer++ );
// 	}
// 	// lenBuff:pBuffer의 배열크기(size아님), 
// 	void PopBlock( T* pBuffer, int lenBuff, int lenBlock ) {
// 		XBREAK( lenBlock > lenBuff );
// 		XBREAK( lenBlock > (int)m_queue.size() );	// (int)캐스팅 빼면 안됨. 결과 이상함
// 		T *pEnd = pBuffer+(lenBuff-1);
// 		int size = xmin( (int)m_queue.size(), lenBlock );
// 		while( size-- )
// 		{
// 			if( XBREAK( pBuffer > pEnd ) )
// 				break;
// 			*pBuffer++ = m_queue.front();
// 			m_queue.pop();
// 		}
// 	}
// 	// 큐에서 4바이트를 꺼내 DWORD값으로 돌려준다
// 	DWORD PopDWORD() {
// 		char buff[4];
// 		for( int i=0; i < 4; ++i )
// 		{
// 			buff[i] = m_queue.front();
// 			m_queue.pop();
// 		}
// 		return *((DWORD*)buff);
// 	}
// };

// 큐를 정적인 배열로 구현한것
template <typename T, int N>
class XQueueArrayN
{
protected:
	T m_Array[ N ];
	int m_nMax;			// 배열메모리의 전체 크기
	int m_nNum;			// 큐의 현재 크기
	int m_idxFront;		// 큐front의 인덱스
	int m_idxBack;		// 큐back의 인덱스
public:
	XQueueArrayN() {
//		m_Array = NULL;
//		m_nMax = 0;
		m_nMax = N;
		m_nNum = 0;
		m_idxBack = m_idxFront = 0;
	}
	~XQueueArrayN() {
//		SAFE_DELETE( m_Array );
	}

	void Clear() {
		memset( (void *)m_Array, 0, sizeof(T) * m_nMax );
		m_nNum = 0;
		m_idxFront = m_idxBack = 0;
	}
	BOOL IsEmpty() { return m_idxFront == m_idxBack; }
	int Size() { return m_nNum; }
	int size() { return m_nNum; }
	int GetMax() { return m_nMax; }
	BOOL Push( T data ) {
//		XBREAK( m_Array == NULL );
		if( m_idxBack == -1 ) {
			XBREAK_POINT();
		}
		if( XBREAK( m_idxBack == -1 ) )		// 큐가 꽉차서 더이상 못넣음
			return FALSE;
		m_Array[ m_idxBack++ ] = data;
		++ m_nNum;
		if( m_idxBack >= m_nMax )		// 인덱스가 끝까지 가면 0으로 되돌아감
			m_idxBack = 0;
		if( m_idxBack == m_idxFront )	// 큐가 꽉참
			m_idxBack = -1;
		return TRUE;
	}
	void Pop() {
//		XBREAK( m_Array == NULL );
		if( IsEmpty() )		return;
		if( m_idxBack == -1 )	// 큐가 꽉찬상태이면
			m_idxBack = m_idxFront;	// 큐꼬리를 현재 front위치로 지정
		++m_idxFront;
		-- m_nNum;
		if( m_idxFront >= m_nMax )		// 인덱스 오버플로우
			m_idxFront = 0;
	}
	T Front() {
//		XBREAK( m_Array == NULL );
		XBREAK( IsEmpty() == TRUE );
		return m_Array[ m_idxFront ];
	}
	T Back() {
//		XBREAK( m_Array == NULL );
		XBREAK( IsEmpty() == TRUE );		// 큐가 비어있는 상태에서 이걸 쓰면 안된다. 밖에서 IsEmpty를 해보고 할것.
		int idxBack = m_idxBack - 1;
		if( m_idxBack == -1 )				// 큐가 꽉차 back인덱스가 없는 상태면
			idxBack = m_idxFront - 1;		// front의 바로 앞이 큐의 마지막 위치다
		if( idxBack < 0 )						// 인덱스가 언더플로우 되지 않도록
			idxBack = m_nMax - 1;
		return m_Array[ idxBack ];
	}
	// 큐가 가득찼는가
	BOOL IsFull() {
		return m_idxBack == -1;
	}
	// 큐전체메모리의 남은 공간개수를 리턴
//	int GetLeftSpace() {
//	}
	// T로 이루어진 배열(버퍼)을 차례로 푸쉬한다. 푸쉬에 성공한만큼의 개수를 리턴한다.
	int PushBlock( T* pBuffer, int len ) {
		for( int i = 0; i < len; ++i ) {
			if( Push( *pBuffer++ ) == FALSE )
				return i;
		}
		return len;
	}
	// lenBuff:pBuffer의 최대배열갯수(sizeof아님), 
	int PopBlock( T* pBuffer, int lenBuff, int lenBlock ) {
		XBREAK( lenBlock > lenBuff );
		if( lenBlock > lenBuff )	// pop시키려는 블럭크기가 pBuffer의 최대크기보다 클수 없다.
			return 0;				// lenBlock이 잘못왔거나 pBuffer의 크기가 너무 작다.
		XBREAK( lenBlock > size() );		// pop시키려는 블럭크기가 큐전체 크기보다 클 순 없다.
		if( lenBlock > size() )
			return 0;
		T *pEnd = pBuffer+(lenBuff-1);
		int popSize = xmin( (int)size(), lenBlock );
		int s = popSize;
		while( s-- )
		{
			if( XBREAK( pBuffer > pEnd ) )
				break;
			*pBuffer++ = Front();
			Pop();
		}
		return popSize;
	}
	// 큐에서 4바이트를 꺼내 DWORD값으로 돌려준다
	DWORD PopDWORD() {
		XBREAK( sizeof(T) != 1 );		
		T buff[4];
		for( int i=0; i < 4; ++i )
		{
			buff[i] = Front();
			Pop();
		}
		return *((DWORD*)buff);
	}
	void PushDWORD( DWORD data ) {
		T *p = (T*)(&data);
		for( int i = 0; i < 4; ++i )
			Push( *p++ );
	}
}; // QArrayN

template <typename T>
class XQueue : public std::queue<T>
{
protected:
public:
	XQueue() {	}
	~XQueue() {}
	// 
	inline void Clear() {
		while( std::queue<T>::size() > 0 ) {
			std::queue<T>::pop();
		}
	}
	inline bool IsEmpty() const { 
		return std::queue<T>::empty();
	}
	inline int Size() const { 
		return std::queue<T>::size();
	}
	inline void Push( const T& data ) {
		std::queue<T>::push( data );
	}
	inline void Push( const T&& data ) {
		std::queue<T>::push( data );
	}
	inline void Pop() {
		std::queue<T>::pop();
	}
	inline T& Front() {
		XBREAK( IsEmpty() );
		return std::queue<T>::front();
	}
	inline T& Back() {
		return std::queue<T>::back();
	}
	// T로 이루어진 배열(버퍼)을 차례로 푸쉬한다
	inline void PushBlock( T* pBuffer, int len ) {
		for( int i = 0; i < len; ++i )
			std::queue<T>::push( *pBuffer++ );
	}
	// lenBuff:pBuffer의 최대배열갯수(sizeof아님), 
	int PopBlock( T* pBuffer, int lenBuff, int lenBlock ) {
		if( XBREAK( lenBlock > lenBuff ) )	// pop시키려는 블럭크기가 pBuffer의 최대크기보다 클수 없다.
			return 0;				// lenBlock이 잘못왔거나 pBuffer의 크기가 너무 작다.
		if( XBREAK( lenBlock > Size() ) )		// pop시키려는 블럭크기가 큐전체 크기보다 클 순 없다.
			return 0;
		int s = lenBlock;
		while( s-- ) {
			*pBuffer++ = std::queue<T>::front();
			std::queue<T>::pop();
		}
		return lenBlock;
	}
	// 큐에서 4바이트를 꺼내 DWORD값으로 돌려준다
	DWORD PopDWORD() {
		XBREAK( sizeof( T ) != 1 );
		T buff[ 4 ];
		for( int i = 0; i < 4; ++i ) {
			buff[ i ] = std::queue<T>::front();
			std::queue<T>::pop();
		}
		return *((DWORD*)buff);
	}
	void PushDWORD( DWORD data ) {
		T *p = (T*)( &data );
		for( int i = 0; i < 4; ++i )
			std::queue<T>::push( *p++ );
	}
};

// 큐를 동적인 배열로 구현한것
template <typename T>
class XQueueArray
{
protected:
	T *m_Array;
	int m_nMax;			// 배열메모리의 전체 크기
	int m_nNum;			// 큐의 현재 크기
	int m_idxFront;		// 큐front의 인덱스
	int m_idxBack;		// 큐back의 인덱스
public:
	XQueueArray() {
		m_Array = NULL;
		m_nMax = 0;
		m_nNum = 0;
		m_idxBack = m_idxFront = 0;
	}
	~XQueueArray() {
		SAFE_DELETE_ARRAY( m_Array );
	}
	// 
	void Create( int size );
	void clear() {
//		memset( (void *)m_Array, 0, sizeof(T) * m_nMax );
		m_nNum = 0;
		m_idxFront = m_idxBack = 0;
	} 
	BOOL IsEmpty() const { return m_idxFront == m_idxBack; }
	int Size() const { return m_nNum; }
	int size() const { return m_nNum; }
	int GetMax() const { return m_nMax; }
	bool push( const T& data ) {
//		XBREAK( m_Array == NULL );
		XBREAK(m_idxBack == -1);
		if( XBREAKF( m_idxBack == -1, "queue is full: max=%d", m_nMax ) )		// 큐가 꽉차서 더이상 못넣음
			return false;
		m_Array[ m_idxBack++ ] = data;
		++ m_nNum;
		if( m_idxBack >= m_nMax )		// 인덱스가 끝까지 가면 0으로 되돌아감
			m_idxBack = 0;
		if( m_idxBack == m_idxFront )	// 큐가 꽉참
			m_idxBack = -1;
		return true;
	}
	void pop() {
//		XBREAK( m_Array == NULL );
		if( IsEmpty() )		return;
		if( m_idxBack == -1 )	// 큐가 꽉찬상태이면
			m_idxBack = m_idxFront;	// 큐꼬리를 현재 front위치로 지정
		++m_idxFront;
		-- m_nNum;
		if( m_idxFront >= m_nMax )		// 인덱스 오버플로우
			m_idxFront = 0;
	}
	T Front() {
//		XBREAK( m_Array == NULL );
		XBREAK( IsEmpty() == TRUE );
		return m_Array[ m_idxFront ];
	}
	T Back() {
//		XBREAK( m_Array == NULL );
		XBREAK( IsEmpty() == TRUE );		// 큐가 비어있는 상태에서 이걸 쓰면 안된다. 밖에서 IsEmpty를 해보고 할것.
		int idxBack = m_idxBack - 1;
		if( m_idxBack == -1 )				// 큐가 꽉차 back인덱스가 없는 상태면
			idxBack = m_idxFront - 1;		// front의 바로 앞이 큐의 마지막 위치다
		if( idxBack < 0 )						// 인덱스가 언더플로우 되지 않도록
			idxBack = m_nMax - 1;
		return m_Array[ idxBack ];
	}
	// 큐가 가득찼는가
	BOOL IsFull() const {
		return m_idxBack == -1;
	}
	// 큐전체메모리의 남은 공간개수를 리턴
//	int GetLeftSpace() {
//	}
	// T로 이루어진 배열(버퍼)을 차례로 푸쉬한다
	int PushBlock( T* pBuffer, int len ) {
		for( int i = 0; i < len; ++i )
			if( push( *pBuffer++ ) == FALSE )
				return i;
		return len;
	}
	// lenBuff:pBuffer의 최대배열갯수(sizeof아님), 
	int PopBlock( T* pBuffer, int lenBuff, int lenBlock ) {
		if( XBREAK( lenBlock > lenBuff ) )	// pop시키려는 블럭크기가 pBuffer의 최대크기보다 클수 없다.
			return 0;				// lenBlock이 잘못왔거나 pBuffer의 크기가 너무 작다.
		if( XBREAK( lenBlock > size() ) )		// pop시키려는 블럭크기가 큐전체 크기보다 클 순 없다.
			return 0;
		T *pEnd = pBuffer+(lenBuff-1);
		int popSize = xmin( (int)size(), lenBlock );
		int s = popSize;
		while( s-- )
		{
			if( XBREAK( pBuffer > pEnd ) )		// 받을버퍼가 모자르면 그것도 문제로 처리해야 할듯.
				return 0;
			*pBuffer++ = Front();
			pop();
		}
		return popSize;
	}
	// 큐에서 4바이트를 꺼내 DWORD값으로 돌려준다
	DWORD PopDWORD() {
		XBREAK( sizeof(T) != 1 );		
		T buff[4];
		for( int i=0; i < 4; ++i )
		{
			buff[i] = Front();
			pop();
		}
		return *((DWORD*)buff);
	}
	void PushDWORD( DWORD data ) {
		T *p = (T*)(&data);
		for( int i = 0; i < 4; ++i )
			push( *p++ );
	}
};

////////////////////////////////////////////////////////////////////////////////
// 큐의 back에서도 임의로 뺄수 있는 큐를 위해 리스트로 구현함.
template <typename T>
class XQueueList
{
protected:
	std::list<T> m_queue;
	typename std::list<T>::iterator m_itor;
public:
	XQueueList() {}
	~XQueueList() {}

	std::list<T>& _GetList() { return m_queue; }	// 소트검증용 임시
	BOOL IsEmpty() { return m_queue.empty(); }
	int Size() { return m_queue.size(); }
	int size() { return m_queue.size(); }
	void	Push( T data ) {
		m_queue.push_back( data );
	}
	void Pop() {
		if( m_queue.empty() )	return;
        typename std::list<T>::iterator itor = m_queue.begin();		// 리스트의 맨 앞을 
		m_queue.erase( itor );		// 삭제한다
	}
	T Front() {
		typename std::list<T>::iterator itor = m_queue.begin();		// 리스트의 맨 앞을 
		T temp = (*itor);			// 읽어낸다
		return temp;
	}
	T Back() {
		typename std::list<T>::iterator itor = m_queue.end();		// 리스트의 맨 뒤를
		--itor;
		T temp = (*itor);		// 읽어낸다
		return temp;
	}
	void PopBack() {
		typename std::list<T>::iterator itor = m_queue.end();		// 리스트의 맨 뒤를
		--itor;
		m_queue.erase( itor );		// 삭제한다
	}
	// 
	void Create( int size ) {}
	// 큐의 남은 공간개수를 리턴....stl버전에서는 항상 1이 리턴된다
	int GetLeftSpace() {
		return 1;
	}
	//
	void Sort( bool (*ptFuncComp)( const T& obj1, const T& obj2 ) ) {
		m_queue.sort( ptFuncComp );
	}
	//
	void GetNextClear() {
		m_itor = m_queue.begin();
	}
	T GetNext() {
		return (*m_itor++);
	}
};


// struct전용 vector클래스
// T는 반드시 구조체여야 한다.
// 배열사이즈를 조절할수 있는 장점이 있다.
template <typename T>
class XVectorStruct
{
	int m_nLastIdx;
	int m_nMax;
	int m_nUnit;
	int m_nIdx;
protected:
	std::vector<int> m_vecActive;
	std::vector<T> m_vector;
	void Init() {
		m_nMax = m_nLastIdx = 0; 
		m_nUnit = 8;  
		m_nIdx = 0;
	}
public:
	XVectorStruct() {		
		Init(); 
//		m_vector.clear();
//		m_vecActive.clear();
		m_vector.resize(m_nUnit );  
		m_vecActive.resize(m_nUnit ); 
	}			
	XVectorStruct( int nUnit ) 
	{ 
		Init(); 
		m_nUnit = nUnit;	
		m_vector.resize(nUnit); 
		m_vecActive.resize(m_nUnit); 
		m_vector.clear();
		m_vecActive.clear();
	}
	~XVectorStruct() {}

	// 모든 요소를 삭제한다.
	void Clear() {
		int nSize = Size();
		int i;
		for( i = 0; i < nSize; i ++ )	
			m_vecActive[i] = 0;
		m_nLastIdx = 0;
		m_nMax = 0;
	}

	// 비어있는 슬롯을 찾아 data를 넣는다.
	T* Add( const T &data ) 
	{
		T* add = &m_vector[ m_nLastIdx ];
		m_vecActive[ m_nLastIdx ] = 1;
		m_vector[ m_nLastIdx++ ] = data;									// 비어있는 슬롯에 데이타를 넣는다.
		
		int nSize = Size();
		if( m_nLastIdx < nSize )
			if( m_vecActive[ m_nLastIdx ] == 0 )		return add;		// 때마침 다음 인덱스가 비어있으면 그걸쓰고 리턴한다.
		
		BOOL exist = 0;
		int i;
		for( i = 0; i < nSize; i ++ )											// 비어있는 슬롯을 찾아둔다.
		{
			if( m_vecActive[i] )	continue;
			exist = 1;
			break;
		}
		if( exist == 0 )						// 빈슬롯을 못찾았다.
			AddSize( m_nUnit );		// 지정된 단위만큼 배열을 늘인다.
		m_nLastIdx = i;					// for를 빠져나왔으므로 i는 size늘린곳의 최초 인덱스가 되어 있을것이다.
		return add;
	}

	void Delete( int idx ) {
		m_vecActive[ idx ] = 0;
		m_nLastIdx = idx;
		m_nMax --;
	}

	void Delete( const T &data ) { Delete( Find(data) );	return; }

	void DeleteCurrent() {
		Delete( m_nIdx-1 );
	}

	// data와 같은 데이타를 찾아 그 인덱스를 리턴한다.
	// struct의 operator== 로 검사하는 버전.
	int	Find( const T &data )	{
		int nSize = Size();
		int i;
		for( i = 0; i < nSize; i ++ )	{
			if( m_vecActive[i] )
				if( m_vector[i] == data )	
					return i;
		}
		return -1;
	}

	// pData가 m_vector[]중의 하나의 포인터라면 이 함수가 낫다.
	int	Find( const T *pData )	{
		int nSize = Size();
		int i;
		for( i = 0; i < nSize; i ++ )	{
			if( m_vecActive[i] )
				if( &m_vector[i] == pData )	
					return i;
		}
		return -1;
	}
	
	T* FindPtr( const T &data ) {
		return &m_vector[ Find(data) ];
	}
	
	T* Get( int idx ) {	return &m_vector[idx];	}
	void Set( int idx, const T &data ) { 
		if( m_vecActive[idx] == 0 )								// 기존값이 빈슬롯이었다면 전체 크기 증가.
			m_nMax ++;
		m_vector[idx] = data;
	}
	
	void GetNextClear() { m_nIdx = 0; }			// GetNext()를 첨부터 할수있도록 초기화.

	T* GetNext() {											// 값이 들어있는 슬롯을 찾아 차례대로 리턴해준다
		int nSize = Size();
		int i;
		for( i = m_nIdx; i < nSize; i ++ ) {
			if( m_vecActive[i] )	{
				m_nIdx = ++i;										// 다음턴엔 m_nIdx다음부터 해야하므로.
				return &m_vector[m_nIdx-1];
			}
		}
		m_nIdx = 0;													// 끝까지 다찾았으면 초기화시킨다.
		return NULL;												// NULL이라는것은 빈슬롯이 더이상 없이 배열끝까지 탐색했다.
	}
	
	void AddSize( int add ) {
		m_nUnit += add;
		m_vector.resize( m_nUnit );
		m_vecActive.resize( m_nUnit );
	}
	
	void SubSize( int sub ) {
		m_nUnit += sub;
		m_vector.resize( m_nUnit );
		m_vecActive.resize( m_nUnit );
	}
	
	void Resize( int size ) {
		m_nUnit = size;
		m_vector.resize( m_nUnit );
		m_vecActive.resize( m_nUnit );
	}
	
	int Size() { return m_vector.size(); }

};

#include "ContainerSTL.inl"
