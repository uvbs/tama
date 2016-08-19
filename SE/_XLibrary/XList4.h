#pragma once

#include "etc/global.h"
#include "etc/xUtil.h"
#include <list>
#include <algorithm>

#pragma warning( disable : 4700 )		// warning C4700: 초기화되지 않은 't' 지역 변수를 사용했습니다.

template<typename T>
class XList4;
/**
 @brief std::list를 상속받은 버전.
*/
template<typename T>
class XList4 : public std::list<T>
{
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
	inline T Add( const T& elem ) {
		this->push_back( elem );
		return elem;
	}
	void Del( T& elem ) {
		auto iter = std::find( this->begin(), this->end(), elem );
		if( iter != this->end() )
			this->erase(iter);
	}
	void Del( T&& elem ) {
		auto iter = std::find( this->begin(), this->end(), elem );
		if( iter != this->end() )
			this->erase( iter );
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
	template<typename F>
	void DelIf( F func ) {
		auto iter = std::find_if(this->begin(), this->end(), func);
		if (iter != this->end())
			this->erase(iter);
	}
	template<typename F>
	T* FindIf( F func ) {
		auto iter = std::find_if(this->begin(), this->end(), func);
		if (iter != this->end())
			return &(*iter);
		return nullptr;
	}
	T* Find( const T& elem  ) {
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
	T* FindByID( ID idNode ) {
		auto iter = std::find_if(this->begin(), this->end(),
			[idNode](T pElem)->bool {  
			return pElem->getid() == idNode;
		} );
		if (iter != this->end())
			return &(*iter);
		return nullptr;
	}
	// T가 shared_ptr일때 전용
	T FindValueByID( ID idNode ) {
		auto iter = std::find_if( this->begin(), this->end(),
			[idNode]( T pElem )->bool {
			return pElem->getid() == idNode;
		} );
		if( iter != this->end() )
			return (*iter);
		return T();
	}
	inline T FindspByID( ID idNode ) {
		return FindValueByID( ID idNode );
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
	T* GetFirst() {
		return &(*this->begin());
	}
	T* GetLast() {
		return &(*--this->end());
	}
	T* GetByIndex( int idx ) {
		int i = 0;
		for (auto iter = this->begin(); iter != this->end(); ++iter) {
			if( i++ == idx )
				return &(*iter);
		}
		return nullptr;
	}
	// 값으로 리턴하는 버전 shared_ptr도 이걸로 사용
	T GetValueByIndex( int idx ) {
		int i = 0;
		for( auto iter = this->begin(); iter != this->end(); ++iter ) {
			if( i++ == idx )
				return ( *iter );
		}
		return T();
	}
	typename XList4<T>::iterator GetItorByIndex( int idx ) {
		int i = 0;
		for( auto iter = this->begin(); iter != this->end(); ++iter ) {
			if( i++ == idx )
				return iter;
		}
		return this->end();
	}
	T* GetFromRandom() {
		int max = this->size();
		return GetByIndex( xRandom(max) );
	}
	// 리스트내에서 랜덤으로 한 노드의 이터레이터를 꺼낸다.
	typename XList4<T>::iterator GetItorFromRandom() {
		int max = this->size();
		return GetItorByIndex( xRandom(max) );
	}
	typename XList4<T>::iterator GetItorByID( ID idNode ) {
		for( auto iter = this->begin(); iter != this->end(); ++iter ) {
			if( (*iter)->getid() == idNode )
				return iter;
		}
		return this->end();
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
		sort( func );
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

template<typename T, int N>
class XArrayN2 : public std::vector < T >
{
public:
	XArrayN2() : std::vector<T>( N ) {}
};

#include "XList4.inl"

#pragma warning( default : 4700 )	
