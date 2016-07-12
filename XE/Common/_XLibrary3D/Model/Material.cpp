
#include <stdio.h>
#include "material.h"
#include "path.h"
#include "XImage.h"
#include "xUtil.h"
//#include "vutil.h"
//#include "xutil.h"


CTextureManager		g_TextureMng;

CTextureManager :: CTextureManager()
{

	memset( m_pMaterial, 0, sizeof(m_pMaterial) );
	m_nMaxTexture = 0;
}

CTextureManager :: ~CTextureManager()
{
	DeleteDeviceObjects();
}
xRESULT CTextureManager::DeleteDeviceObjects()
{
	int		i;
	for( i = 0; i < MAX_MATERIAL; i ++ )
	{
		if( m_pMaterial[i].m_bActive )
			glDeleteTextures( 1, &m_pMaterial[i].m_nGL_Texture );
		m_pMaterial[i].m_bActive = FALSE;
		m_pMaterial[i].strBitMapFileName[0] = 0;
	}
	m_nMaxTexture = 0;
	return  xSUCCESS;
}	

// pTexture를 사용하는 매터리얼을 찾아 삭제한다.
// 공유되어 있는 텍스쳐라면 사용카운터를 보고 1인것만 삭제한다..
int CTextureManager::DeleteMaterial( GLuint nGL_Texture )
{
	int		i;

	if( nGL_Texture == 0 )	return FALSE;
	if( m_nMaxTexture == 0 )	return FALSE;

	for( i = 0; i < MAX_MATERIAL; i ++ )
	{
		if( m_pMaterial[i].m_bActive )
		{
			if( m_pMaterial[i].m_nGL_Texture == nGL_Texture )		// pTexture를 찾았다.
			{
				if( m_pMaterial[i].m_nUseCnt == 1 )			// 공유된게 아니다(usecnt == 1)
				{
					glDeleteTextures( 1, &m_pMaterial[i].m_nGL_Texture );
					m_pMaterial[i].m_bActive = FALSE;			// 텍스쳐 관리자에서도 삭제.
					m_pMaterial[i].strBitMapFileName[0] = 0;
					m_nMaxTexture --;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

MATERIAL*	CTextureManager :: AddMaterial( D3DMATERIAL9 *pMaterial, LPCTSTR strFileName, LPCTSTR szPath )
{
	int		i;
	MATERIAL	*pMList = m_pMaterial;
	GLuint      nGL_Texture = 0;

	// 이미 읽은건지 검사.
	for( i = 0; i < MAX_MATERIAL; i ++ )
	{
		if( pMList->m_bActive )
		{
			if( strcmpi(strFileName, pMList->strBitMapFileName) == 0 )	// 이미 읽은건 다시 읽지 않음.  역시 땜빵 -_-;;
			{
				pMList->m_nUseCnt ++;	// 이미로딩한걸 공유하고 있다면 카운트 올림.
				return pMList;
			}
		}
		pMList ++;
	}
	pMList = NULL;

	LPCTSTR szFullPath;
	if( szPath == NULL )
	{
		szFullPath = MakePath( DIR_MODELTEX, strFileName );
	}
	else
	{
		szFullPath = MakePath( szPath, strFileName );		// 경로가 지정되어 있을땐 그걸쓴다.
	}
	
	glGenTextures( 1, &nGL_Texture );
	
	XImage *pImage = new XImage;
	pImage->Load32( szFullPath );

	glBindTexture(GL_TEXTURE_2D, nGL_Texture );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 
				 0, 
				 GL_RGBA, 
				 pImage->GetWidth(), 
				 pImage->GetHeight(), 
				 0, 
				 GL_RGBA, 
				 GL_UNSIGNED_BYTE, 
				 pImage->GetTextureData() );
	SAFE_DELETE( pImage );
//	{
// #ifdef	__XDEBUG
//		if( !IsEmpty(strFileName) )
//			XERROR( "%s 텍스쳐 비트맵 못읽음", szFullPath );
// #endif
//	}

	// 빈 슬롯이 있는지 검사.
	pMList = m_pMaterial;
	for( i = 0; i < MAX_MATERIAL; i ++ )
	{
		if( pMList->m_bActive == FALSE )	break;
		pMList++;
	}
	if( i >= MAX_MATERIAL )
	{
		XERROR( "CTextureManager::AddMaterial : 텍스쳐 갯수를 넘어섰다." );
		return NULL;
	}

	pMList->m_bActive = TRUE;
	pMaterial->Ambient[0] = 1;
	pMaterial->Ambient[1] = 1;
	pMaterial->Ambient[2] = 1;
	pMaterial->Diffuse[0] = 1;
	pMaterial->Diffuse[1] = 1;
	pMaterial->Diffuse[2] = 1;
	pMaterial->Specular[0] = 1;
	pMaterial->Specular[1] = 1;
	pMaterial->Specular[2] = 1;
	pMaterial->Emissive[0] = 0;
	pMaterial->Emissive[1] = 0;
	pMaterial->Emissive[2] = 0;
	pMaterial->Power = 0.0f;
	pMList->m_Material = *pMaterial;
#ifdef	_XDEBUG
	if( strlen(strFileName)+1 > 32 )
		XERROR( "CTextureManager::AddMaterial() : %s의 길이가 너무 길다", strFilename );
#endif
	strcpy( pMList->strBitMapFileName, strFileName );		// 텍스쳐 파일명 카피
	pMList->m_nGL_Texture = nGL_Texture;
	pMList->m_nUseCnt = 1;	// 처음 등록된것이기땜에 1부터 시작.
	m_nMaxTexture ++;


	return pMList;
}
D3DMATERIAL9* CTextureManager::GetMaterial( int nIdx )
{
	return &m_pMaterial[ nIdx ].m_Material; 
}
GLuint	CTextureManager::GetTexture( int nIdx )
{
	return m_pMaterial[ nIdx ].m_nGL_Texture; 
}

