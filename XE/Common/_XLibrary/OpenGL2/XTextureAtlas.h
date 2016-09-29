#pragma once
#include "etc/XGraphicsDef.h"

#ifdef XSPR_LOAD
#error "스프라이트 비동기 로딩중(멀티스레드)에 못쓰게 하려고 막아둠. 로딩중에 sGetCurrMng()가 의미가 없다."
#endif // XSPR_LOAD

// #define SET_ATLASES( ATLASMNG ) \
// {	XBREAK( ATLASES == nullptr); \
// 	ATLASMNG->PushAtlasMng();
// 
// #define END_ATLASES \
// 	ATLASMNG->PopAtlasMng(); \

/****************************************************************
* @brief 생성시 push를 하고 파괴시 pop을 자동으로 하는 객체
* @author xuzhu
* @date	2016/09/23 18:37
*****************************************************************/

namespace xSplit {
class XNode;
}
namespace xnTexAtlas {
struct xAtlas {
	ID m_idTex;
	XE::VEC2 m_Size;		// 텍스쳐 전체 사이즈
	XE::xtPixelFormat m_FormatSurface;		// 아틀라스 서피스의 포맷
	xSplit::XNode* m_pRoot;			// 아틀라스 트리의 루트
	XE::VEC2 m_maxFill;					// 배치된 아틀라스의 최대크기
	int m_refCnt = 0;						// 이 아틀라스를 가리키는 객체수.
	int m_glFmt = 0;
	int m_glType = 0;
#ifdef _CHEAT
	std::string m_strTag;
#endif // _CHEAT
//	bool m_bDestroy = false;
		//
	xAtlas( const XE::VEC2& size, XE::xtPixelFormat formatSurface );
	~xAtlas();
	inline ID getid() const {
		return m_idTex;
	}
	void DestroyDevice();
	inline bool IsEmpty() const {
		return m_idTex == 0;
	}
	void UpdateSubToDevice( const void* pImg, const XE::VEC2& vLT, const XE::VEC2& sizeImg, XE::xtPixelFormat fmtImg );
	bool ResizeAtlas();
	int GetBytes() const;
};
}

class XTextureAtlas;
/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/09/05 15:44
*****************************************************************/
class XTextureAtlas : public std::enable_shared_from_this<XTextureAtlas>
{
public:
	class XAutoPushObj {
	public:
		XAutoPushObj( XSPAtlasMng spMng )
			: m_spMng( spMng ) {
			XBREAK( spMng == nullptr );
			spMng->PushAtlasMng();
		}
		~XAutoPushObj() {
			m_spMng->PopAtlasMng();
		}
	private:
		// private member
		XSPAtlasMng m_spMng;
	}; // class XPushAtlasMng
	//////////////////////////////////////////////////////////////////////////
public:
	static XSPAtlasMng _sSetpCurrMng( XSPAtlasMng spMng ) {
		auto spPrev = s_spCurrAtlasMng;
		s_spCurrAtlasMng = spMng;
		return spPrev;
	}
	static XSPAtlasMng sGetspCurrMng() {
		return s_spCurrAtlasMng;
	}
	static void sRelease( ID idTex, ID idNode );
	static void sFlushAtlasMng();
	static void sFlushAtlasSurface();
	static XSPAtlasMng sCreateAtlasMng( const char* cTag );
	static int sGetBytesAll();
	static XList4<XSPAtlas> s_listSurfaceAll;
	static XSPAtlasConst sGetspAtlasByIdx( int idx ) {
		return s_listSurfaceAll.GetByIndexConst( idx );
	}
	static int sGetNumAtlas() {
		return s_listSurfaceAll.size();
	}
	static void sAddSizeVM( int addbytes ) {
		s_sizeVM += addbytes;
	}
	static int sGetSizeVM() {
		return s_sizeVM;
	}
	//////////////////////////////////////////////////////////////////////////
public:
	XTextureAtlas( const char* cTag );
	~XTextureAtlas() {		Destroy();	}
	//
	GET_ACCESSOR_CONST( ID, idMng );
	inline ID getid() const {
		return m_idMng;
	}
	void Release( ID idTex, ID idNode );
	void DestroyDevice();
	void OnPause();
	//
	ID ArrangeImg( ID idTex,
								 XE::xRect2* pOut,
								 ID* pOutID,
								 const void* pImgSrc,
								 const XE::VEC2& sizeMemSrc,
								 XE::xtPixelFormat formatImgSrc,
								 XE::xtPixelFormat formatSurface,
								 XE::VEC2* pOutSizeAtlas );
	inline int GetnumAtlas() const {
		return m_listAtlas.size();
	}
	ID GetidTex( int idxAtlas );
	inline XE::xtPixelFormat GetfmtByidxAtlas( int idxAtlas ) {
		auto spAtlas = m_listAtlas.GetByIndex( idxAtlas );
		return (spAtlas)? spAtlas->m_FormatSurface : XE::xPF_NONE;
	}
	void PushAtlasMng();
	void PopAtlasMng();
private:
	void Init() {}
	void Destroy();
	XSPAtlas AddAtlas( const XE::VEC2& size, XE::xtPixelFormat formatSurface );
	bool ResizeAtlas( XSPAtlas spAtlas );
	xSplit::XNode* InsertElem( XSPAtlas spAtlas,
														 const XE::VEC2& sizeElem ) const;
	XSPAtlas GetspAtlas( ID idTex );
	XSPAtlasConst GetspAtlasConst( ID idTex ) const;
	void DestroyAtlas( XSPAtlas spAtlas );
	//	void UpdateSub( const DWORD* pImg, const XE::VEC2& vLT, const XE::VEC2& sizeImg, ID glTex );
private:
	static XSPAtlasMng s_spCurrAtlasMng;
	static XList4<XSPAtlasMng> s_listAtlasLayer;
	static XE::VEC2 s_sizeDefault;
	static int s_sizeVM;
	ID m_idMng = 0;
	std::string m_strTag;
	XList4<XSPAtlas> m_listAtlas;		// 커다란 아틀라스들의 리스트
	XSPAtlasMng m_spMngPrev = nullptr;
//	bool m_bDestroy = false;
}; // class XTextureAtlas

