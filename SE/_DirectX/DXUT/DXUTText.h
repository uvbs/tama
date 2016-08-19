//--------------------------------------------------------------------------------------
// File: SDKMisc.h
//
// Various helper functionality that is shared between SDK samples
//
// Copyright (c) Microsoft Corporation. All rights reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef DXUTTEXT_H
#define DXUTTEXT_H

// Direct3D9 includes
#include <d3d9.h>
#include <d3dx9.h>

// Direct3D10 includes
#include <dxgi.h>
#include <d3d10_1.h>
#include <d3d10.h>
#include <d3dx10.h>

// XInput includes
#include <xinput.h>

// HRESULT translation for Direct3D10 and other APIs 
#include <dxerr.h>

//--------------------------------------------------------------------------------------
// Manages the insertion point when drawing text
//--------------------------------------------------------------------------------------
class CDXUTTextHelper
{
public:
            CDXUTTextHelper( ID3DXFont* pFont9 = NULL, ID3DXSprite* pSprite9 = NULL, ID3DX10Font* pFont10 = NULL,
                             ID3DX10Sprite* pSprite10 = NULL, int nLineHeight = 15 );
            CDXUTTextHelper( ID3DXFont* pFont9, ID3DXSprite* pSprite9, int nLineHeight = 15 );
            CDXUTTextHelper( ID3DX10Font* pFont10, ID3DX10Sprite* pSprite10, int nLineHeight = 15 );
            ~CDXUTTextHelper();

    void    Init( ID3DXFont* pFont9 = NULL, ID3DXSprite* pSprite9 = NULL, ID3DX10Font* pFont10 = NULL,
                  ID3DX10Sprite* pSprite10 = NULL, int nLineHeight = 15 );

    void    SetInsertionPos( int x, int y )
    {
        m_pt.x = x; m_pt.y = y;
    }
    void    SetForegroundColor( D3DXCOLOR clr )
    {
        m_clr = clr;
    }

    void    Begin();
    HRESULT DrawFormattedTextLine( const WCHAR* strMsg, ... );
    HRESULT DrawTextLine( const WCHAR* strMsg );
    HRESULT DrawFormattedTextLine( RECT& rc, DWORD dwFlags, const WCHAR* strMsg, ... );
    HRESULT DrawTextLine( RECT& rc, DWORD dwFlags, const WCHAR* strMsg );
    void    End();

protected:
    ID3DXFont* m_pFont9;
    ID3DXSprite* m_pSprite9;
    ID3DX10Font* m_pFont10;
    ID3DX10Sprite* m_pSprite10;
    D3DXCOLOR m_clr;
    POINT m_pt;
    int m_nLineHeight;

    ID3D10BlendState* m_pFontBlendState10;
};



#endif
