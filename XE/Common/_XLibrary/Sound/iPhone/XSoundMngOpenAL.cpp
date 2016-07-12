#include "stdafx.h"
#ifdef _VER_IOS

#include <stdio.h>

#include <assert.h>

#include <list>

#include "XSoundMngOpenAL.h"

#include "XOpenAL.h"

#include "path.h"
#include "XResMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


using namespace XE;



//------------------------------------------------------------------------------------------------------------



XSoundMngOpenAL::XSoundMngOpenAL()

{

	m_pOpenAL = new XOpenAL;

	m_pBGMusic = NULL;

}



//------------------------------------------------------------------------------------------------------------

#include "AVAudio.h"



XSoundMngOpenAL::~XSoundMngOpenAL()

{

	RemoveAll();

	SAFE_DELETE( m_pOpenAL );

	SAFE_DELETE( m_pBGMusic );

	AVDestroy();

}



//------------------------------------------------------------------------------------------------------------

BOOL XSoundMngOpenAL::OpenBGMusic( const char* _filename, BOOL repeat)

{	
	// 이미 같은 파일명이 플레이 되고 있다면 그냥 리턴
	if( XE::IsSame( m_szBGMFilename, _filename ) )
		return FALSE;
    char filename[1024];
    sprintf( filename, "%s.mp3", _filename );

	strcpy_s(m_szBGMFilename, _filename);
    TCHAR szPath[1024]; // fullpath
    XE::SetReadyRes( szPath, XE::MakePath( DIR_BGM, filename ) );
	AVLoad( szPath, repeat );

	return 1;

}

//------------------------------------------------------------------------------------------------------------



void XSoundMngOpenAL::SetBGMVolume(float volume)

{	

	AVSetVolume( volume );

	return;
/*
	if(!m_pOpenAL) return;



	XSoundMng::SetBGMVolume( volume );

	m_pBGMusic->SetVolume( volume );
*/
}



//------------------------------------------------------------------------------------------------------------



void XSoundMngOpenAL::PlayBGMusic()

{

	AVPlay();

	return;
/*
	if( !m_pOpenAL )

		return;



	if (m_bMuteBGM == FALSE && m_pBGMusic != NULL)

		m_pBGMusic->Play();
*/
}



//------------------------------------------------------------------------------------------------------------



void XSoundMngOpenAL::StopBGMusic()

{
	m_szBGMFilename[0] = 0;
	AVStop();

	return;
/*
	if(!m_pOpenAL) return;

	if( m_pBGMusic == NULL ) return;

	m_pBGMusic->Stop();

	SAFE_DELETE( m_pBGMusic );
*/
}



//------------------------------------------------------------------------------------------------------------



void XSoundMngOpenAL::FadeOutBGM( float fLevel)

{

//	if(!m_pOpenAL) return;

	if( m_pBGMusic == NULL )	return;          

	XSoundMng::FadeOutBGM( fLevel );



	m_pBGMusic->SetVolume( m_BGMVolumeTemp );

}



//------------------------------------------------------------------------------------------------------------



BOOL XSoundMngOpenAL::IsBGMusic( void )

{	

	if (m_pBGMusic != NULL ) return TRUE;

	return FALSE;

}



//------------------------------------------------------------------------------------------------------------



BOOL XSoundMngOpenAL::OpenSound( ID FileID )

{	
	return TRUE;
	if( m_pOpenAL == NULL )

		return FALSE;



	// 중복 사운드를 찾음.

	Sound_Itor it = m_SoundList.begin();

	while(it != m_SoundList.end())

	{

	    if ( (*it)->FileID == FileID)

		{

			(*it)->Use++;

			return TRUE;

		}

		it++;

	}



    ST_SOUND *pSound;

	pSound = new ST_SOUND;



	pSound->Sound = NULL;

	char filename[100];

	sprintf(filename, "%04lu.wav", FileID);


    TCHAR szFullpath[1024];
    XE::SetReadyRes( szFullpath, MakePath( DIR_SND, filename) );
	pSound->Sound = new XOALObj( szFullpath );

	if (pSound->Sound != NULL)

	{

		pSound->FileID = FileID;

	    pSound->Use  =  1;

		m_SoundList.push_back(pSound);

		return TRUE;

	}

	if (pSound != NULL)

		delete pSound;



	return FALSE;

}



//------------------------------------------------------------------------------------------------------------



void XSoundMngOpenAL::CloseSound(ID FileID)

{

	if( m_pOpenAL == NULL )

		return;



    ST_SOUND *pSound;

	pSound = NULL;

    Sound_Itor it = m_SoundList.begin();

	while(it != m_SoundList.end())

	{

	    if ((*it)->FileID == FileID) 

		{

		    pSound = (*it);

		    break;

		}

		it++;

	}	



    //

	if (pSound != NULL)

	{

        pSound->Use--;

		if(pSound->Use == 0)

		{

			m_SoundList.erase(it);

			SAFE_DELETE( pSound->Sound );

			delete pSound;

			pSound = NULL;

		}

	}	

}



//------------------------------------------------------------------------------------------------------------



void XSoundMngOpenAL::PlaySound(ID FileID, BOOL bRepeat )

{	
	return;
	if( m_pOpenAL == NULL ) return;

	if (m_bMuteSound) return;



	Sound_Itor it = m_SoundList.begin();

	while(it != m_SoundList.end()) {

	    if ((*it)->FileID == FileID)

		{		    

			(*it)->Sound->SetVolume( m_SoundVolume );
            (*it)->Sound->SetRepeat( bRepeat );

			(*it)->Sound->Play();

			break;

		}

		it++;

	}

}



void XSoundMngOpenAL::StopSound( ID FileID )

{

	if( m_pOpenAL == NULL ) return;

	if (m_bMuteSound) return;

	

	Sound_Itor it = m_SoundList.begin();

	while(it != m_SoundList.end()) {

	    if ((*it)->FileID == FileID)

		{		    

			(*it)->Sound->Stop();

			break;

		}

		it++;

	}	

}



//------------------------------------------------------------------------------------------------------------



void XSoundMngOpenAL::RemoveAll()

{	

	if( m_pOpenAL == NULL ) return;

   

   ST_SOUND *pSound;

   pSound = NULL;

//   for(Sound_Itor itor = m_SoundList.begin(); itor != m_SoundList.end(); ++itor )

   for(Sound_Itor itor = m_SoundList.begin(); itor != m_SoundList.end(); )

   {

	   pSound = (*itor);

	   m_SoundList.erase(itor++);

//	   itor = m_SoundList.begin();

	   if (pSound != NULL) 

	   {

		   pSound->Sound->Stop();

		   SAFE_DELETE( pSound->Sound );

		   delete pSound;

		   pSound = NULL;

	   }

   }   

}



#endif // _VER_IOS
