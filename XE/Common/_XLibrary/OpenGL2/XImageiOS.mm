/*
 *  XImage.cpp
 *  OmniHellMP
 *
 *  Created by xuzhu on 10. 6. 16..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#include "XImageiOS.h"
#include "etc/xGraphics.h"

BOOL LoadImageiOS( LPCTSTR szFilename, int *pOutW, int *pOutH, DWORD **pOutImg ) 
{
    NSString *strPath = [NSString stringWithUTF8String:szFilename];
    UIImage *pImage = [[UIImage alloc] initWithContentsOfFile:strPath];
    CGImageRef textureImage = [pImage CGImage];
    if (textureImage == nil) {
//        XERROR( "Failed to load texture image %s", szFilename );
        return FALSE;
    }
    NSInteger texWidth = CGImageGetWidth(textureImage);
    NSInteger texHeight = CGImageGetHeight(textureImage);
    //	m_nWidth = texWidth;
    //	m_nHeight = texHeight;
    *pOutW = texWidth;
    *pOutH = texHeight;
    BYTE *pTemp = new BYTE[texWidth * texHeight * sizeof(DWORD)];
    DWORD *textureData = (DWORD*)pTemp;
    memset( textureData, 0, texWidth * texHeight * sizeof(DWORD) );
    CGContextRef textureContext = CGBitmapContextCreate( textureData,
                                                        texWidth, texHeight,
                                                        8, texWidth * 4,
                                                        CGImageGetColorSpace(textureImage),
                                                        kCGImageAlphaPremultipliedLast);
    CGContextDrawImage(textureContext, CGRectMake(0.0, 0.0, (float)texWidth, (float)texHeight), textureImage);
    CGContextRelease(textureContext);
    
    *pOutImg = textureData;
    return TRUE;
}

/*
BOOL XImageiOS::Load16( LPCTSTR szFilename )
{
    XBREAKF( 1, "다시 구현할것" );
	NSString *strPath = [NSString stringWithUTF8String:szFilename];
	UIImage *pImage = [[UIImage alloc] initWithContentsOfFile:strPath];
	CGImageRef textureImage = [pImage CGImage];
//	CGImageRef textureImage = [UIImage imageNamed:strPath].CGImage;
    if (textureImage == nil) {
        XERROR( "Failed to load texture image %s", szFilename );
		return FALSE;
    }
    NSInteger texWidth = CGImageGetWidth(textureImage);
    NSInteger texHeight = CGImageGetHeight(textureImage);
	m_nWidth = texWidth;
	m_nHeight = texHeight;
    SetvSize( texWidth, texHeight );

	DWORD *textureData = new DWORD[texWidth * texHeight];
    CGContextRef textureContext = CGBitmapContextCreate( textureData,
														texWidth, texHeight,
														8, texWidth * 4,
														CGImageGetColorSpace(textureImage),
														kCGImageAlphaPremultipliedLast);
	CGContextDrawImage(textureContext, CGRectMake(0.0, 0.0, (float)texWidth, (float)texHeight), textureImage);
	CGContextRelease(textureContext);
	[pImage release];
	
	WORD *pBuffer = new WORD[ m_nWidth * m_nHeight ];
	int i, j;
	WORD *_pBuffer = pBuffer;
	DWORD dwData;
	for( i = 0; i < m_nHeight; i ++ )
		for( j = 0; j < m_nWidth; j ++ )
		{
			dwData = textureData[i*m_nWidth+j];
			BYTE r = XCOLOR_RGB_R(dwData);
			BYTE g = XCOLOR_RGB_G(dwData);
			BYTE b = XCOLOR_RGB_B(dwData);
			*_pBuffer++ = XCOLOR_RGB565(r, g, b);
		}
	m_pTextureData = pBuffer;
	m_bpp = sizeof(WORD);
	SAFE_DELETE_ARRAY( textureData );
//	free( textureData );
	textureData = NULL;	
	return TRUE;
}
 */	
