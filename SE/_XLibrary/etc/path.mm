//
//  path.mm
//  OmniHellMP
//
//  Created by xuzhu on 10. 6. 15..
//  Copyright 2010 LINKS CO.,LTD. All rights reserved.
//

#import "path.h"
#import "xString.h"

#ifdef _VER_IPHONE
LPCSTR _SetWorkDir( CHAR *szBuff, int maxlen )
{
#ifdef _BUNDLE
	NSBundle *mainBundle = [NSBundle mainBundle];
	NSString *path = [mainBundle bundlePath];
	if( [path length] >= maxlen )
		XERROR( "working path의 길이(%d)가 길어 버퍼에 담지 못한다", [path length] );
	strcpy( szBuff, [path UTF8String] );
	strcat( szBuff, "/Resource" );
#else
	strcpy( szBuff, WORKING_DIR );
#endif
	return szBuff;
}
#endif // VER_IPHONE

BOOL XE::GetDirectoryFiles( LPCSTR szPath, CHAR buffer[][MAX_PATH], int nMaxFiles )
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

void XE::GetDocumentPath( CHAR *szBuff )
{
	NSArray *arrayPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *docDir = [arrayPaths objectAtIndex:0];
	strcpy( szBuff, [docDir UTF8String] );
}

LPCSTR XE::MakeDocumentPath( LPCSTR szFilename )
{
	static CHAR szTemp[256];
	GetDocumentPath( szTemp );
	CHAR szLwr[64];
	strcpy( szLwr, szFilename );
	strlwr( szLwr );
	strcat( szTemp, "/" );
	strcat( szTemp, szLwr );
	return szTemp;
}
