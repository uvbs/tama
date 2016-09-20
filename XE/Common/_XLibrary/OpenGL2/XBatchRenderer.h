#pragma once
#include "etc/XSurface.h"

#ifdef XSPR_LOAD
#error "스프라이트 비동기 로딩중(멀티스레드)에 RenderCmd못쓰게 하려고 막아둠. 로딩중에 XRenderCmd::sGetpCurrRenderer()가 의미가 없다."
#endif // XSPR_LOAD

#define SET_RENDERER( RENDERER ) \
{	XBREAK( RENDERER == nullptr ); \
	auto __pCurrRenderer = RENDERER; \
	auto __pPrev = XBatchRenderer::_sSetpCurrRenderer( __pCurrRenderer );

#define END_RENDERER \
	__pCurrRenderer->RenderBatch(); \
	XBatchRenderer::_sSetpCurrRenderer( __pPrev ); }

// #define END_RENDERER \
// 	XRenderCmdMng::sAddRenderer( __pCurrRenderer ); \
// 	XRenderCmdMng::_sSetpCurrRenderer( pPrev ); }


class XShader;
class XBatchRenderer;
namespace XE {
struct xVertex;
}

XE_NAMESPACE_START( xRenderCmd )
//
struct xCmd { 
	static int s_idGenerator;
	int m_Priority = 0;							// 찍기 우선순위. 값이 클수록 먼저찍는다.
	DWORD m_glTex = 0;
	XShader* m_pShader = nullptr;
	DWORD m_glsFactor = GL_SRC_ALPHA;
	DWORD m_gldFactor = GL_ONE_MINUS_SRC_ALPHA;
	DWORD m_glBlendEquation = GL_FUNC_ADD;		// gl디폴트값인듯.
	bool m_bZBuffer = false;				// zbuff 사용여부.(일반적으로 ui는 zbuff를 사용하지 않는다)
	XE::VEC4 m_v4Color;
	bool m_bBlend = false;
	XE::xVertex m_aryVertices[4];		// 월드변환이 끝난 버텍스들
	XE::POINT m_ltViewport, m_sizeViewport;		// 스크롤뷰영역 출력일때.
	ID m_idCmd = 0;
	//	bool m_bAlphaTest = 0;					// 알파테스팅 쉐이더 사용
	xCmd()
		: m_idCmd( ++s_idGenerator )
		, m_v4Color(1.f, 1.f, 1.f, 1.f) {}
	inline int GetSumViewport() const {
		return m_ltViewport.x + m_ltViewport.y + m_sizeViewport.w + m_sizeViewport.h;
	}
};
struct xDPCall {
	ID m_idVertexBuffer = 0;
};

//
XE_NAMESPACE_END; // xRenderCmd


/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/09/09 14:10
*****************************************************************/
class XBatchRenderer 
{
public:
	static std::shared_ptr<XBatchRenderer>& sGet();
	static void sDestroyInstance();
	static XBatchRenderer* sGetpCurrRenderer() {
		return s_pCurrRenderer;
	}
	static XBatchRenderer* _sSetpCurrRenderer( XBatchRenderer* pRenderer ) {
		auto pPrev = s_pCurrRenderer;
		s_pCurrRenderer = pRenderer;
		return pPrev;
	}
// 	static void sAddRenderer( XRenderCmdMng* pRenderer ) {
// 		s_aryRenderer.push_back( pRenderer );
// 	}
	// 각 윈도우 스택에서 쌓인 렌더러들을 한번에 렌더링 한다.
	static void sRenderBatchs() {
// 		for( auto pRenderer : s_aryRenderer ) {
// 			pRenderer->RenderBatch();
// 		}
		s_aryRenderer.clear();
	}
public:
	XBatchRenderer( const char* cTag );
	~XBatchRenderer() { Destroy(); }
	//
	//GET_ACCESSOR_CONST( int, avgDPCall );
	GET_ACCESSOR_CONST( int, numDPCall );
	GET_ACCESSOR_CONST( const XFps&, fpsDPCall );
	inline void PushCmd( const xRenderCmd::xCmd& cmd ) {
		m_qCmds.push_back( cmd );
	}
	inline int GetSize() const {
		return m_qCmds.size();
	}
	void RenderBatch();
private:
// 	static std::shared_ptr<XRenderCmdMng> s_spInstance;
	static XBatchRenderer* s_pCurrRenderer;
	static XVector<XBatchRenderer*> s_aryRenderer;
	ID m_idRenderer = 0;
	std::string m_strTag;			// 식별용 태그
	int m_avgDPCall = 0;		// 초당 평균 DP횟수
	int m_numDPCall = 0;
	ID m_idTexPrev = 0;
	XFps m_fpsDPCall;
	void Init() {}
	void Destroy();
	void VertexAttribPointer( GLuint indx, GLint size, const void* ptr );
	void Render( const xRenderCmd::xCmd& cmd, int idx, const XE::xVertex* pVertices );
	ID GetidVertexBuffer( int idxDPCall );
	void ExtendBuffer( int idx, int cnt2X, std::vector<int>* pOutAryNumBatch, int *pOutMaxRect, XE::xVertex** ppOutVertices );
private:
	std::vector<xRenderCmd::xCmd> m_qCmds;
	std::vector<xRenderCmd::xDPCall> m_aryDPCall;
}; // class XRenderCmdMng

