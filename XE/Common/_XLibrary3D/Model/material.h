#ifndef		__MATERIAL_H__
#define		__MATERIAL_H__

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

typedef struct _D3DCOLORVALUE {
    float r;
    float g;
    float b;
    float a;
} D3DCOLORVALUE;

typedef struct _D3DMATERIAL9 {
	GLfloat Diffuse[4];
	GLfloat Ambient[4];
	GLfloat Specular[4];
	GLfloat Emissive[4];
    float           Power;          /* Sharpness if specular highlight */
} D3DMATERIAL9;

struct MATERIAL
{
	D3DMATERIAL9			m_Material;
	GLuint		m_nGL_Texture;		// 텍스쳐메모리 포인터
	char			strBitMapFileName[64];	// 텍스쳐 파일명
	int						m_nUseCnt;		// 같은 텍스쳐가 몇번 사용됐는지 ...
	BOOL					m_bActive;
};

#define		MAX_MATERIAL		2048

class CTextureManager
{
protected:
	int		m_nMaxTexture;
	
public:
	MATERIAL	m_pMaterial[ MAX_MATERIAL ];		// 게임에서 사용하는 모든 매트리얼이 들어간다.
	CTextureManager();
	~CTextureManager();

	xRESULT DeleteDeviceObjects();

	int		DeleteMaterial( GLuint nGL_Texture );	// pTexture를 사용하는 매터리얼을 찾아 삭제한다.

	D3DMATERIAL9			*GetMaterial( int nIdx );
	GLuint		GetTexture( int nIdx );
		
	MATERIAL	*AddMaterial( D3DMATERIAL9 *pMaterial, LPCTSTR strFileName, LPCTSTR szPath = NULL );		// 매터리얼 하나를 추가하고 그 포인터를 되돌린다.
};

extern CTextureManager		g_TextureMng;


#endif

