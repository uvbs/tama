#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "md5.h"


void md5test(const char *in)
{
	unsigned char out[16];
	MD5Simple(in, (unsigned int)strlen(in), out);
	printf("\"%s\" = ", in);
	MDPrint(out);
	printf("\n");
}

void main()
{
	FILE* fp;
	fopen_s( &fp, "a.spr", "rb" );
	if( fp ) {
		auto offset = ftell( fp );
		fseek( fp, 0, SEEK_END );
		int size = ftell( fp );
		fseek( fp, offset, SEEK_SET );
		char* pBuff = new char[ size ];
		fread( pBuff, size, 1, fp );
		fclose(fp);
		md5test( pBuff );
	}

	md5test("");
	md5test("a");
	md5test("abc");
}
