/********************************************************************
	@date:	2016/06/16 15:43
	@file: 	C:\xuzhu_work\Project\iPhone_zero\XE\XFramework\client\XASyncLoad.h
	@author:	xuzhu
	
	@brief:	비동기로딩 시험버전
	구현의 용이성을 위해 메인스레드에서 로딩하는것으로 한다.
	어느곳에든지 비동기로딩을 등록하면 일단 리스트에 받아두고
	메인스레드의 Process()를 하는중에 로딩을 하는데 터치중에는 로딩을 하지 않는다.
	그리고 1프레임에 1개의 파일만 로딩한다.

	ex)
	auto pSurface = IMAGE_MNG->Load( "test.png", format, bAsync );
*********************************************************************/
#pragma once

