﻿//
//  File.h
//  Game
//
//  Created by 성정우 on 11. 6. 1..
//  Copyright 2011 LINKS CO.,LTD. All rights reserved.
//

#pragma once

#ifdef _VER_IOS
void AVLoad( LPCTSTR szFilename, BOOL repeat );
void AVPlay( void );
void AVSetVolume( float vol );
void AVDestroy( void );
void AVStop( void );

#endif // _VER_IOS
