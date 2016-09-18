#pragma once
#include "etc/XGraphicsDef.h"

#define SET_ATLASES( ATLASES ) \
{	XBREAK( ATLASES == nullptr); \
 auto pPrev = XTextureAtlas::_sSetpCurrMng( ATLASES );

#define END_ATLASES \
	XTextureAtlas::_sSetpCurrMng( pPrev ); }

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
	//
	xAtlas( const XE::VEC2& size, XE::xtPixelFormat formatSurface );
	~xAtlas();
	inline bool IsEmpty() const {
		return m_idTex == 0;
	}
	void UpdateSubToDevice( const void* pImg, const XE::VEC2& vLT, const XE::VEC2& sizeImg, XE::xtPixelFormat fmtImg );
};
}

typedef std::shared_ptr<xnTexAtlas::xAtlas> XSPAtlas;
typedef std::shared_ptr<xnTexAtlas::xAtlas const> XSPAtlasConst;
/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/09/05 15:44
*****************************************************************/
class XTextureAtlas
{
public:
// 	static std::shared_ptr<XTextureAtlas>& sGet();
// 	static void sDestroyInstance();
	static XTextureAtlas* _sSetpCurrMng( XTextureAtlas* pMng ) {
		auto pPrev = s_pCurrAtlasMng;
		s_pCurrAtlasMng = pMng;
		return pPrev;
	}
	static XTextureAtlas* sGetpCurrMng() {
		return s_pCurrAtlasMng;
	}
public:
	XTextureAtlas( const char* cTag );
	~XTextureAtlas() {		Destroy();	}
	void Release( ID idTex );
	void DestroyAtlas( XSPAtlas spAtlas );
	//
	ID ArrangeImg( ID idTex,
								 XE::xRect2* pOut,
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
private:
// 	static std::shared_ptr<XTextureAtlas> s_spInstance;
	static XTextureAtlas* s_pCurrAtlasMng;
	void Init() {}
	void Destroy() {}
	XSPAtlas AddAtlas( const XE::VEC2& size, XE::xtPixelFormat formatSurface );
	xSplit::XNode* InsertElem( XSPAtlas spAtlas,
														 const XE::VEC2& sizeElem ) const;
	XSPAtlas GetspAtlas( ID idTex );
	XSPAtlasConst GetspAtlasConst( ID idTex ) const;
	//	void UpdateSub( const DWORD* pImg, const XE::VEC2& vLT, const XE::VEC2& sizeImg, ID glTex );
private:
	static XE::VEC2 s_sizeDefault;
	ID m_idMng = 0;
	std::string m_strTag;
	XList4<XSPAtlas> m_listAtlas;		// 커다란 아틀라스들의 리스트
}; // class XTextureAtlas

