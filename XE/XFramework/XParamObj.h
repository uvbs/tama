/********************************************************************
	@date:	2016/04/20 13:43
	@file: 	C:\xuzhu_work\Project\iPhone_zero\XE\XFramework\XParamObj.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once

/****************************************************************
* @brief
* @author xuzhu
* @date	2016/01/12 12:39
*****************************************************************/
template<typename T>
class XParamType
{
public:
	XParamType() {
		Init();
	}
	~XParamType() {
		Destroy();
	}
	// get/setter
	// public member
	void Set( const char* cKey, const T& val ) {
		const std::string strKey( cKey );
		auto itor = m_mapVal.find( strKey );
		if( XBREAK( itor != m_mapVal.end() ) )	// 중복된 키
			return;
		m_mapVal[strKey] = val;
	}
	inline void Set( const std::string& strKey, const T& val ) {
		Set( strKey.c_str(), val );
	}
	const T& Get( const char* cKey ) const {
		auto itor = m_mapVal.find( std::string( cKey ) );
		if( itor == m_mapVal.end() )
			return m_Empty;
		return ( *itor ).second;
	}
	inline bool IsEmpty( const char* cKey ) const {
		return  m_mapVal.find( std::string( cKey ) ) == m_mapVal.end();
	}
	inline bool IsEmpty() const {
		return m_mapVal.empty();
	}
	void Clear() {
		m_mapVal.clear();
	}
private:
	// private member
	std::map<std::string, T> m_mapVal;
	T m_Empty;
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XParamObj

/****************************************************************
* @brief
* @author xuzhu
* @date	2016/01/12 14:33
*****************************************************************/
class XParamObj
{
public:
	XParamObj() {
		Init();
	}
	~XParamObj() {
		Destroy();
	}
	// get/setter
	// public member
	void Set( const char* cKey, int val ) {
		m_paramInt.Set( cKey, val );
	}
	void Set( const char* cKey, WORD val ) {
		m_paramWord.Set( cKey, val );
	}
	void Set( const char* cKey, DWORD val ) {
		m_paramDword.Set( cKey, val );
	}
	void Set( const char* cKey, float val ) {
		m_paramFloat.Set( cKey, val );
	}
	void Set( const char* cKey, char val ) {
		m_paramChar.Set( cKey, val );
	}
	void Set( const char* cKey, const std::string& val ) {
		m_paramStrc.Set( cKey, val );
	}
#ifdef WIN32
	void Set( const char* cKey, const _tstring& val ) {
		m_paramStrt.Set( cKey, val );
	}
#endif // WIN32
	int GetInt( const char* cKey ) const {
		if( m_paramInt.IsEmpty( cKey ) )
			return 0;
		return m_paramInt.Get( cKey );
	}
	DWORD GetDword( const char* cKey ) const {
		if( m_paramDword.IsEmpty( cKey ) )
			return 0;
		return m_paramDword.Get( cKey );
	}
	WORD GetWord( const char* cKey ) const {
		if( m_paramWord.IsEmpty( cKey ) )
			return 0;
		return m_paramWord.Get( cKey );
	}
	float GetFloat( const char* cKey ) const {
		if( m_paramFloat.IsEmpty( cKey ) )
			return 0;
		return m_paramFloat.Get( cKey );
	}
	char GetChar( const char* cKey ) const {
		return m_paramChar.Get( cKey );
	}
	std::string GetStrc( const char* cKey ) const {
		return m_paramStrc.Get( cKey );
	}
	_tstring GetStrt( const char* cKey ) const {
#ifdef WIN32
		return m_paramStrt.Get( cKey );
#else	// WIN32
		return m_paramStrc.Get( cKey );
#endif // not win32
	}
	bool IsEmpty() const {
		return m_paramInt.IsEmpty() && m_paramDword.IsEmpty() && m_paramWord.IsEmpty()
			&& m_paramFloat.IsEmpty() && m_paramChar.IsEmpty() && m_paramStrc.IsEmpty()
#ifdef WIN32
			&& m_paramStrt.IsEmpty()
#endif // WIN32
			;
	}
private:
	// private member
	XParamType<int> m_paramInt;
	XParamType<WORD> m_paramWord;
	XParamType<DWORD> m_paramDword;
	XParamType<float> m_paramFloat;
	XParamType<char> m_paramChar;
	XParamType<std::string> m_paramStrc;
#ifdef WIN32
	XParamType<_tstring> m_paramStrt;
#endif // WIN32
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XParam

/**
 @brief 
*/
class XParamObj2
{
public:
	XParamObj2() {
		Init();
	}
	~XParamObj2() {
		Destroy();
	}
	// get/setter
	// public member
	inline void Set( const char* cKey, int val ) {
		const std::string strVal = XE::Format( "%d", val );
//		const std::string strVal = std::to_string( val );	// 문자열화 시켜서 넣음.
		m_params.Set( cKey, strVal );
	}
	inline void Set( const char* cKey, const std::string& strVal ) {
		m_params.Set( cKey, strVal );
	}
	inline void Set( const std::string& strKey, const std::string& strVal ) {
		m_params.Set( strKey.c_str(), strVal );
	}
// 	template<typename T>
// 	inline void Set( const std::string& strKey, const T& val ) {
//		const std::string strVal = std::to_string( val );	// 문자열화 시켜서 넣음.
// 		m_params.Set( strKey, strVal );
// 	}
	inline int GetInt( const char* cKey ) const {
		const std::string strVal = m_params.Get( cKey );
		int num = ::atoi( strVal.c_str() );
		return ( strVal.empty() ) ? XE::INVALID_INT : num;
// 		return (strVal.empty())? XE::INVALID_INT : std::stoi( strVal );
	}
	inline DWORD GetDword( const char* cKey ) const {
		const std::string strVal = m_params.Get( cKey );
		DWORD num = (DWORD)::atoi( strVal.c_str() );
		return num;
// 		return (DWORD)std::stol( strVal );
	}
	inline WORD GetWord( const char* cKey ) const {
		const std::string strVal = m_params.Get( cKey );
		WORD num = (WORD)::atoi( strVal.c_str() );
		return num;
//		return (WORD)std::stoi( strVal );
	}
	inline float GetFloat( const char* cKey ) const {
		const std::string strVal = m_params.Get( cKey );
		float num = (float)::atof( strVal.c_str() );
		return num;
//		return std::stof( strVal );
	}
	inline char GetChar( const char* cKey ) const {
		const std::string strVal = m_params.Get( cKey );
		return strVal[0];
	}
	inline std::string GetStrc( const char* cKey ) const {
		return m_params.Get( cKey );
	}
	inline _tstring GetStrt( const char* cKey ) const {
#ifdef WIN32
		return _tstring( C2SZ( m_params.Get( cKey ) ) );
#else	// WIN32
		return GetStrc( cKey );
#endif // not win32
	}
	inline bool IsEmpty() const {
		return m_params.IsEmpty();
	}
	void Clear() {
		m_params.Clear();
	}
private:
	// private member
	XParamType<std::string> m_params;
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XParam