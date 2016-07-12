#pragma once
#ifndef _XOGG_EFFECT

#include <list>
#include "XSoundMng.h"
#include "XOpenAL.h"
#include "sound/windows/Ogg/XOgg.h"
//
struct ST_SOUND
{
	ID idSound;
	XOALObj *Sound;
	int Use;
	int alBuffer = 0;
};
//
typedef std::list<ST_SOUND*>           Sound_List;
typedef std::list<ST_SOUND*>::iterator Sound_Itor;
//
class XSoundMngOpenAL : public XSoundMng
{
private:
	XOpenAL		*m_pOpenAL;
	//	XOALObj		*m_pBGMusic;
	XOgg			*m_pOgg;
	XOggObj		*m_pBGMusic;

	Sound_List  m_SoundList;
public:
	XSoundMngOpenAL();
	virtual ~XSoundMngOpenAL();

	bool IsPlayableBGM() const override {
		if( !m_pOpenAL )
			return false;
		return true;
	}
	bool IsPlayableSound() const override {
		if( !m_pOpenAL )
			return false;
		return true;
	}
	// Background Music
	bool OpenBGMusic( const char* szFilename, bool repeat = true ) override;
	void PlayBGMusic() override;
	void StopBGMusic() override;
	void SetBGMVolumeLocal( float volLocal ) override;
	void SetBGMVolumeFinal( float volFinal ) override;
	// Effect sound
	bool OpenSound( ID idSound ) override;
	void CloseSound( ID idSound ) override;
	ID PlaySound( ID idSound, bool bRepeat = false ) override;
	void StopSound( ID idSound ) override;
// 	void SetSoundVolume( ID idStream, float vol ) override;
// 	void SetSoundVolumeAllStream( ID idSound, float vol ) override;
	void RemoveAll() override;
	void StopAllSound() override {
		RemoveAll();
	}
// 	int GetalBuffer( ID idSnd ) {
// 		for( auto pSound : m_SoundList ) {
// 			if( pSound->idSound == idSnd )
// 				return pSound->alBuffer;
// 		}
// 	}
};


//------------------------------------------------------------------------------------------------------------

#endif // not _XOGG_EFFECT
