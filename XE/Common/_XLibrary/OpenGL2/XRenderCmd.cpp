﻿#include "stdafx.h"
#include "XRenderCmd.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xRenderCmd;

int xRenderCmd::xCmd::s_idGenerator = 0;

XRenderCmdMng* XRenderCmdMng::s_pCurrRenderer = nullptr;
XVector<XRenderCmdMng*> XRenderCmdMng::s_aryRenderer;
																	// std::shared_ptr<XRenderCmdMng> XRenderCmdMng::s_spInstance;
// int XRenderCmdMng::s_numDPCall = 0;
// int XRenderCmdMng::s_avgDPCall = 0;
// ///////////////////////////////////////////////////////////////
// std::shared_ptr<XRenderCmdMng>& XRenderCmdMng::sGet() {	if( s_spInstance == nullptr )		s_spInstance = std::shared_ptr<XRenderCmdMng>( new XRenderCmdMng );	return s_spInstance;}
// void XRenderCmdMng::sDestroyInstance() {
// 	s_spInstance.reset();
// }
////////////////////////////////////////////////////////////////
XRenderCmdMng::XRenderCmdMng( const char* cTag )
	: m_idRenderer( XE::GenerateID() )
	, m_strTag( cTag )
	, m_aryDPCall( 256 )
{
// 	XBREAK( s_spInstance != nullptr );
	Init();
}

void XRenderCmdMng::Destroy()
{
	for( const auto& dpcall : m_aryDPCall ) {
		const GLuint glBuffer = (GLuint)dpcall.m_idVertexBuffer;
		glDeleteBuffers( 1, &glBuffer );
	}
}

/**
 @brief 메인의 draw()에서 호출됨
*/
void XRenderCmdMng::RenderBatch()
{
	m_idTexPrev = 0;
	m_numDPCall = 0;
	std::sort( m_qCmds.begin(), m_qCmds.end(), 
						 []( const xCmd& cmd1, const xCmd& cmd2 )->bool{
		if( cmd1.m_Priority == cmd2.m_Priority ) {
			if( cmd1.m_glTex == cmd2.m_glTex ) {
				if( cmd1.m_pShader->GetglProgram() == cmd2.m_pShader->GetglProgram() ) {
					if( cmd1.m_glsFactor == cmd2.m_glsFactor ) {
						if( cmd1.m_gldFactor == cmd2.m_gldFactor ) {
							if( cmd1.m_glBlendEquation == cmd2.m_glBlendEquation ) {
								if( cmd1.m_bBlend == cmd2.m_bBlend ) {
									if( cmd1.m_bZBuffer == cmd2.m_bZBuffer ) {
										if( cmd1.m_ltViewport == cmd2.m_ltViewport ) {
											return cmd1.m_idCmd < cmd2.m_idCmd;
										} else {
											// 소팅을 하려면 불변의 값을 가져야하므로 x,y,w,h를 모두 합한 값으로 사용함.
											return (cmd1.GetSumViewport() > cmd2.GetSumViewport());
										}
									} else {
										int b1 = (cmd1.m_bZBuffer) ? 0 : 1;
										int b2 = (cmd2.m_bZBuffer) ? 0 : 1;
										return (b1 > b2);
									}
								} else {
									int b1 = (cmd1.m_bBlend) ? 0 : 1;
									int b2 = (cmd2.m_bBlend) ? 0 : 1;
									return (b1 > b2);
								}
							} else {
								return (cmd1.m_glBlendEquation > cmd2.m_glBlendEquation);
							}
						} else {
							return (cmd1.m_gldFactor > cmd2.m_gldFactor);
						}
					} else {
						return (cmd1.m_glsFactor > cmd2.m_glsFactor);
					}
				} else {
					return (cmd1.m_pShader->GetglProgram() > cmd2.m_pShader->GetglProgram());
				}
			} else {
				return (cmd1.m_glTex > cmd2.m_glTex);
			}
		} else {
			return (cmd1.m_Priority > cmd2.m_Priority);
		}
		return false;
	}); // sort lambda
	//////////////////////////////////////////////////////////////////////////
	////////// render ////////////////////////////////////////////////////////
	std::vector<int> aryNumsBatch;		// dpcall 횟수?
	int cnt2X = 0;
	int maxRect = 6 * 64;		// 6개정점이 한 사각형
//	XE::xVertex aryVertices[ 6 * 64 ];
#ifndef _DEBUG
	// 디버그모드에선 클리어를 하지 않아서 에러가 났을때 보이도록 함.
//	memset( aryVertices, 0, sizeof(XE::xVertex)* maxRect );
#endif // not _DEBUG
// 	XE::xVertex* pVertices = aryVertices;
	XE::xVertex* pVertices = new XE::xVertex[ maxRect ];
	const xCmd* pPrev = nullptr;
	int cndDPCall = 0;
	do {
		int idx = 0;
		// draw명령을 모두 순회하면서 같은속성끼리 모은다.
		for( const auto& cmd : m_qCmds ) {
			if( pPrev == nullptr ) {
				pPrev = &cmd;
			}
			bool bDiffAttr = false;
			do {
				if( cmd.m_bBlend != pPrev->m_bBlend ) {		// 이전명령과 비교해서 속성이 달라짐ㄴ
					bDiffAttr = true;
					break;
				}
				if( cmd.m_glTex != pPrev->m_glTex ) {
					bDiffAttr = true;
					break;
				}
				if( cmd.m_glsFactor != pPrev->m_glsFactor ) {
					bDiffAttr = true;
					break;
				}
				if( cmd.m_gldFactor != pPrev->m_gldFactor ) {
					bDiffAttr = true;
					break;
				}
				if( cmd.m_glBlendEquation != pPrev->m_glBlendEquation ) {
					bDiffAttr = true;
					break;
				}
				if( cmd.m_pShader != pPrev->m_pShader ) {
					bDiffAttr = true;
					break;
				}
				if( cmd.m_gldFactor != pPrev->m_gldFactor ) {
					bDiffAttr = true;
					break;
				}
				if( cmd.m_bZBuffer != pPrev->m_bZBuffer ) {
					bDiffAttr = true;
					break;
				}
				if( (cmd.m_ltViewport != pPrev->m_ltViewport)
						|| (cmd.m_sizeViewport != pPrev->m_sizeViewport) ) {
					bDiffAttr = true;
					break;
				}
			} while (0);
			if( bDiffAttr ) {
				Render( *pPrev, idx, pVertices );
				// 다시 초기화
//				if( cnt2X > 0 )
					SAFE_DELETE_ARRAY( pVertices );
				idx = 0;
#ifndef _DEBUG
				// 디버그모드에선 클리어를 하지 않아서 에러가 났을때 보이도록 함.
//				memset( aryVertices, 0, sizeof( XE::xVertex )* maxRect );
#endif // not _DEBUG
//				pVertices = aryVertices;
				maxRect = 6 * 64;
				pVertices = new XE::xVertex[maxRect];
				cnt2X = 0;
				++cndDPCall;
			}
			if( idx >= maxRect - 5 ) {
				ExtendBuffer( idx, cnt2X, &aryNumsBatch, &maxRect, &pVertices );
				++cnt2X;
			}
			pVertices[idx++] = cmd.m_aryVertices[0];
			pVertices[idx++] = cmd.m_aryVertices[2];
			pVertices[idx++] = cmd.m_aryVertices[1];
			pVertices[idx++] = cmd.m_aryVertices[1];
			pVertices[idx++] = cmd.m_aryVertices[2];
			pVertices[idx++] = cmd.m_aryVertices[3];

			pPrev = &cmd;
		}
		if( pPrev && idx > 0 ) {
			Render( *pPrev, idx, pVertices );
			// 다시 초기화
//			if( cnt2X > 0 ) {
				SAFE_DELETE_ARRAY( pVertices );
//			}
		}
	} while( 0 );
	m_qCmds.clear();
}

void XRenderCmdMng::ExtendBuffer( int idx, 
																	int cnt2X, 
																	std::vector<int>* pOutAryNumBatch,
																	int *pOutMaxRect, 
																	XE::xVertex** ppOutVertices )
{
	int maxRect = *pOutMaxRect;
	std::vector<int>& aryNumBatch = *pOutAryNumBatch;
	XE::xVertex* pVertices = *ppOutVertices;
	// 버텍스 버퍼 확장
	pOutAryNumBatch->push_back( idx );
	const int sizeNew = maxRect * 2;
	auto pNew = new XE::xVertex[sizeNew];
	memcpy_s( pNew, sizeNew * sizeof(XE::xVertex)
						, pVertices, sizeof(XE::xVertex) * maxRect );
//	*pNew = *pVertices;
	// 				memcpy_s( pNew, sizeNew * sizeof( XE::xVertex )
	// 									, pVertices, (int)maxRect * sizeof( XE::xVertex ) );
// 	if( cnt2X > 0 ) {
		SAFE_DELETE_ARRAY( pVertices );
// 	}
	pVertices = pNew;
	maxRect = sizeNew;
	*pOutMaxRect = maxRect;
	*ppOutVertices = pVertices;
}
void XRenderCmdMng::VertexAttribPointer( GLuint indx,
																				 GLint size,
																				 const void* ptr )
{
	glEnableVertexAttribArray( indx );
	CHECK_GL_ERROR();
	glVertexAttribPointer( indx, size, GL_FLOAT, GL_FALSE,
												 sizeof( XE::xVertex ),
												 ptr );
	CHECK_GL_ERROR();
}

void XRenderCmdMng::Render( const xCmd& cmd, 
														int idx, 
														const XE::xVertex* pVertices )
{
	const auto vpLT = GRAPHICS->GetViewportLT();		// 스택에 백업.
	const auto vpSize = GRAPHICS->GetViewportSize();
	GRAPHICS->SetViewport( cmd.m_ltViewport, cmd.m_sizeViewport );
	XE::SetProjection( cmd.m_sizeViewport );
	//	idx = (idx / 2);
	// blend
	(cmd.m_bBlend) ? glEnable( GL_BLEND ) : glDisable( GL_BLEND );
	glBlendEquation( (GLenum)cmd.m_glBlendEquation );
	glBlendFunc( (GLenum)cmd.m_glsFactor, (GLenum)cmd.m_gldFactor );
	// shader
	MATRIX mWorld;
	MatrixIdentity( mWorld );
	MATRIX mMVP;
	MatrixMultiply( mMVP, mWorld, XE::x_mViewProjection );
	cmd.m_pShader->SetShader( mMVP, cmd.m_v4Color );

//	int maxIndices = 4;
//	WORD indices[4] = { 0, 1, 2, 3 };
	// 버텍스 모으기가 끝남
//	GLuint glVertexBuffer = 0;
//	GLuint glIndexBuffer = 0;
#ifdef _XVAO
	glGenVertexArraysOES( 1, &xxxx );
	glBindVertexArrayOES( xxxx );
#endif // _XVAO
	// create vertex/idex buffer
	const GLuint glVertexBuffer = (GLuint)GetidVertexBuffer( m_numDPCall );
//	glGenBuffers( 1, &glVertexBuffer );
// 	glGenBuffers( 1, &glIndexBuffer );
	if( XBREAK( glVertexBuffer == 0 ) )
		return;
// 	if( XBREAK( glIndexBuffer == 0 ) )
// 		return;0
	const bool bEff = (cmd.m_glsFactor == GL_SRC_ALPHA && cmd.m_gldFactor == GL_ONE);
	if( cmd.m_bZBuffer )	// zbuff가 사용되어야 하는것이고 이펙트류가 아니어야 한다.
		glEnable( GL_DEPTH_TEST );
	else
		glDisable( GL_DEPTH_TEST );
	CHECK_GL_ERROR();
	// bind vertex buffer
	glBindBuffer( GL_ARRAY_BUFFER, glVertexBuffer );
	CHECK_GL_ERROR();
	const int size = sizeof( XE::xVertex ) * idx;
	glBufferData( GL_ARRAY_BUFFER, size, pVertices, GL_DYNAMIC_DRAW );
	CHECK_GL_ERROR();
	// 
	VertexAttribPointer( XE::ATTRIB_POS, 3,
		(void*)offsetof( XE::xVertex, pos ) );
	VertexAttribPointer( XE::ATTRIB_TEXTURE, 2,
		(void*)offsetof( XE::xVertex, uv ) );
	VertexAttribPointer( XE::ATTRIB_COLOR, 4,
		(void*)offsetof( XE::xVertex, rgba ) );

	// bind index buffer
// 	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, glIndexBuffer );
// 	CHECK_GL_ERROR();
// 	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( WORD ) * maxIndices
// 								, indices, GL_DYNAMIC_DRAW );
	// bind texture
//	if( m_idTexPrev != cmd.m_glTex )
		glBindTexture( GL_TEXTURE_2D, (GLuint)cmd.m_glTex );
	m_idTexPrev = cmd.m_glTex;
	CHECK_GL_ERROR();
	glDrawArrays( GL_TRIANGLES, 0, idx );
	CHECK_GL_ERROR();
	glDisable( GL_DEPTH_TEST );
	CHECK_GL_ERROR();
	//	glDeleteBuffers( 1, &glVertexBuffer );
	GRAPHICS->SetViewport( vpLT.ToPoint(), vpSize.ToPoint() );		// 백업받았던 뷰포트 복구.
	XE::SetProjection( vpSize.w, vpSize.h );
	++m_numDPCall;
}

ID XRenderCmdMng::GetidVertexBuffer( int idxDPCall )
{
	const int sizeAry = m_aryDPCall.size();
	if( sizeAry < idxDPCall + 1 ) {
		m_aryDPCall.resize( sizeAry * 2 );
	}
	ID idBuffer = m_aryDPCall[idxDPCall].m_idVertexBuffer;
	if( idBuffer == 0 ) {
		GLuint glBuffer = 0;
		glGenBuffers( 1, &glBuffer );
		m_aryDPCall[idxDPCall].m_idVertexBuffer = glBuffer;
		idBuffer = glBuffer;
	}
	return idBuffer;
}