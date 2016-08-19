#include "stdafx.h"
#include <stdio.h>
#include <assert.h>
#include <list>
#include "XSoundMngOpenAL.h"
#include "path.h"
#include "ConvertString.h"

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
	m_pBGMusic = nullptr;
	m_pOgg = nullptr;
	m_pOgg = new XOgg;

}

//------------------------------------------------------------------------------------------------------------

XSoundMngOpenAL::~XSoundMngOpenAL()
{
	RemoveAll();
	SAFE_DELETE( m_pOpenAL );
	SAFE_DELETE( m_pBGMusic );
	SAFE_DELETE( m_pOgg );
}

//------------------------------------------------------------------------------------------------------------
// 패스는 넣지말고 파일타이틀만 넘길것.
BOOL XSoundMngOpenAL::OpenBGMusic( const char* _filename, BOOL repeat)
{	
	if( !m_pOpenAL )	return FALSE;


	char filename[ 1024 ];
	sprintf( filename, "%s.ogg", _filename );
	strcpy_s( m_szBGMFilename, _filename );

	if (m_pBGMusic != nullptr) 
		SAFE_DELETE( m_pBGMusic );

#ifdef _XTOOL
	m_pBGMusic = new XOggObj( filename );
#else
	m_pBGMusic = new XOggObj( XE::MakePath( DIR_BGM, filename ) );
#endif
	if( m_pBGMusic->GetError() )
	{
		SAFE_DELETE( m_pBGMusic );
		return FALSE;
	}

	m_pBGMusic->SetRepeat(repeat);
    m_BGMVolumeTemp = m_BGMVolume;
	CONSOLE( "BGM:%s .......loaded", Convert_char_To_TCHAR( filename ) );

	return TRUE;
}

//------------------------------------------------------------------------------------------------------------
void XSoundMngOpenAL::SetBGMVolume(float volume)
{	
	if(!m_pOpenAL) return;
	XSoundMng::SetBGMVolume( volume );
	if( m_pBGMusic )
		m_pBGMusic->SetVolume( volume );
}
//------------------------------------------------------------------------------------------------------------
void XSoundMngOpenAL::PlayBGMusic()
{
	if( !m_pOpenAL )
		return;
	if (m_bMuteBGM == FALSE && m_pBGMusic != nullptr)
		m_pBGMusic->Play();
}
//------------------------------------------------------------------------------------------------------------
void XSoundMngOpenAL::StopBGMusic()
{
	if(!m_pOpenAL) return;
	if( m_pBGMusic == nullptr ) return;
	m_pBGMusic->Stop();
	SAFE_DELETE( m_pBGMusic );
}
//------------------------------------------------------------------------------------------------------------

void XSoundMngOpenAL::FadeOutBGM( float fLevel)
{
//	if(!m_pOpenAL) return;
	if( m_pBGMusic == nullptr )	return;          
	XSoundMng::FadeOutBGM( fLevel );
	m_pBGMusic->SetVolume( m_BGMVolumeTemp );
}
//------------------------------------------------------------------------------------------------------------
BOOL XSoundMngOpenAL::IsBGMusic()
{	
	if (m_pBGMusic != nullptr ) return TRUE;
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------
BOOL XSoundMngOpenAL::OpenSound( int FileID )
{	
	if( m_pOpenAL == nullptr )
		return FALSE;
	if( FileID == 0 )		// 0번 사운드는 없는걸로 침. 툴에ㅐ서 사용.
		return TRUE;
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

	pSound->Sound = nullptr;
	char filename[100];
	sprintf_s(filename, "%04d.wav", FileID);

	pSound->Sound = new XOALObj( MakePath( DIR_SND, filename) );
	if (pSound->Sound != nullptr)
	{
		pSound->FileID = FileID;
	    pSound->Use  =  1;
		m_SoundList.push_back(pSound);
		CONSOLE( "sound:%s .......loaded", Convert_char_To_TCHAR( filename ) );
		return TRUE;
	}
	if (pSound != nullptr)
		delete pSound;
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------
void XSoundMngOpenAL::CloseSound(int FileID)
{
	if( m_pOpenAL == nullptr )
		return;

    ST_SOUND *pSound;
	pSound = nullptr;

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

	if (pSound != nullptr)

	{

        pSound->Use--;

		if(pSound->Use == 0)

		{

			m_SoundList.erase(it);

			SAFE_DELETE( pSound->Sound );

			SAFE_DELETE( pSound )

			pSound = nullptr;

		}

	}	

}



//------------------------------------------------------------------------------------------------------------



void XSoundMngOpenAL::PlaySound(int FileID)

{	
	if( m_pOpenAL == nullptr ) return;

	if (m_bMuteSound) return;



	Sound_Itor it = m_SoundList.begin();

	while(it != m_SoundList.end()) {

	    if ((*it)->FileID == FileID)

		{		    

			(*it)->Sound->SetVolume( m_SoundVolume );
			(*it)->Sound->Play();

			break;

		}

		it++;

	}

}



void XSoundMngOpenAL::StopSound( int FileID )

{
	if( m_pOpenAL == nullptr ) return;

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
	if( m_pOpenAL == nullptr ) return;
  
   ST_SOUND *pSound;
   pSound = nullptr;

   for(Sound_Itor itor = m_SoundList.begin(); itor != m_SoundList.end(); )
   {
	   pSound = (*itor);
	   m_SoundList.erase(itor++);
	   if (pSound != nullptr) 
	   {
		   pSound->Sound->Stop();
		   SAFE_DELETE( pSound->Sound );
		   SAFE_DELETE( pSound );
	   }
   }   
}



