#pragma once
#include "etc/XGraphicsDef.h"

namespace xSplit {
class XNode;
}
namespace xnTexAtlas {
struct xAtlas {
	ID m_idTex;
	XE::VEC2 m_Size;		// 텍스쳐 전체 사이즈
	XE::xtPixelFormat m_FormatSurface;		// 아틀라스 서피스의 포맷
	xSplit::XNode* m_pRoot;			// 아틀라스 트리의 루트
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
	static std::shared_ptr<XTextureAtlas>& sGet();
	static void sDestroyInstance();
public:
	XTextureAtlas();
	~XTextureAtlas() { Destroy(); }
	//
	ID ArrangeImg( ID idTex,
								 XE::xRect2* pOut,
								 const void* pImgSrc,
								 const XE::VEC2& sizeMemSrc,
								 XE::xtPixelFormat formatImgSrc,
								 XE::xtPixelFormat formatSurface );
private:
	static std::shared_ptr<XTextureAtlas> s_spInstance;
	void Init() {}
	void Destroy() {}
	XSPAtlas AddAtlas( const XE::VEC2& size, XE::xtPixelFormat formatSurface );
	xSplit::XNode* InsertElem( XSPAtlasConst spAtlas,
														 const XE::VEC2& sizeElem );
	XSPAtlas GetspAtlas( ID idTex );
//	void UpdateSub( const DWORD* pImg, const XE::VEC2& vLT, const XE::VEC2& sizeImg, ID glTex );
private:
	XList4<XSPAtlas> m_listAtlas;		// 커다란 아틀라스들의 리스트
}; // class XTextureAtlas

