//
//  path.mm
//  OmniHellMP
//
//  Created by xuzhu on 10. 6. 15..
//  Copyright 2010 LINKS CO.,LTD. All rights reserved.
//

#import "path.h"
#import "xString.h"

namespace XIOS
{
    // 번들의 패스를 얻는다.
    LPCSTR _SetPackageDir( char *pOut, int sizeOut )
    {
        NSBundle *mainBundle = [NSBundle mainBundle];
        NSString *path = [mainBundle bundlePath];
        if( [path length] >= sizeOut )
            XERROR( "working path의 길이(%d)가 길어 버퍼에 담지 못한다", [path length] );
        strcpy_s( pOut, sizeOut, [path UTF8String] );
		strcat_s( pOut, sizeOut, "/" );
//        strcat_s( pOut, sizeOut, "/Resource/" );
        return pOut;
    }
    // 번들의 패스를 얻는다.
    LPCSTR GetBundleDir( char *pOut, int sizeOut )
    {
        NSBundle *mainBundle = [NSBundle mainBundle];
        NSString *path = [mainBundle bundlePath];
        if( [path length] >= sizeOut )
            XERROR( "working path의 길이(%d)가 길어 버퍼에 담지 못한다", [path length] );
        strcpy_s( pOut, sizeOut, [path UTF8String] );
		strcat_s( pOut, sizeOut, "/" );
        return pOut;
    }
    
    LPCSTR SetWorkDir( char *pOut, int sizeOut )
    {
        _GetDocumentPath( pOut, sizeOut );
        strcat_s( pOut, sizeOut, "/Resource/" );
        return pOut;
    }

    void _GetDocumentPath( CHAR *szBuff, int sizeBuff )
    {
        NSArray *arrayPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *docDir = [arrayPaths objectAtIndex:0];
        strcpy_s( szBuff, sizeBuff, [docDir UTF8String] );
    }

    BOOL GetDirectoryFiles( LPCSTR szPath, CHAR buffer[][MAX_PATH], int nMaxFiles )
    {
        NSFileManager *fileMng = [NSFileManager defaultManager];
        NSString *strPath = [NSString stringWithUTF8String:szPath];
        NSArray *fileList = [fileMng directoryContentsAtPath:strPath];
        int count = [fileList count];
        if( count >= nMaxFiles )
            count = nMaxFiles;
        NSString *strFile;
        for( int i = 0; i < count; i ++ )
        {
            strFile = [fileList objectAtIndex:i];
            if( [strFile length] < MAX_PATH )
                strcpy( buffer[i], [strFile UTF8String] );
        }
        return TRUE;
    }
}


