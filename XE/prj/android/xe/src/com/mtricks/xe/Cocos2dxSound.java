/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:


he above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 ****************************************************************************/
package com.mtricks.xe;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.concurrent.Semaphore;
import android.content.Context;
import android.media.AudioManager;
import android.media.SoundPool;
import android.util.Log;
//import com.mtrics.epic.thl.R;
public class Cocos2dxSound {
	// ===========================================================
	// Constants
	// ===========================================================
	private static final String TAG = "xuzhu snd";
	// ===========================================================
	// Fields
	// ===========================================================
	private final Context mContext;
	private SoundPool mSoundPool;
	private float mLeftVolume;
	private float mRightVolume;
	// sound path and stream ids map
	// a file may be played many times at the same time
	// so there is an array map to a file path
	private final HashMap<String, ArrayList<Integer>> mPathStreamIDsMap = new HashMap<String, ArrayList<Integer>>();
	private final HashMap<String, Integer> mPathSoundIDMap = new HashMap<String, Integer>();
	private final ArrayList<SoundInfoForLoadedCompleted> mEffecToPlayWhenLoadedArray = new ArrayList<SoundInfoForLoadedCompleted>();
	private int mStreamIdSyn;
	private Semaphore mSemaphore;
	private static final int MAX_SIMULTANEOUS_STREAMS_DEFAULT = 50;
	private static final float SOUND_RATE = 1.0f;
	private static final int SOUND_PRIORITY = 1;
	private static final int SOUND_QUALITY = 5;
	private final static int INVALID_SOUND_ID = -1;
//	private final static int INVALID_STREAM_ID = -1;
	private final static int INVALID_STREAM_ID = 0;		// 메뉴얼엔 실패가 0이라고 나와있는데 왜 -1로 했지?
	// ===========================================================
	// Constructors
	// ===========================================================
	public Cocos2dxSound(final Context pContext) {
		this.mContext = pContext;
		this.initData();
	}
	private void initData() {
		this.mSoundPool = new SoundPool(
			Cocos2dxSound.MAX_SIMULTANEOUS_STREAMS_DEFAULT,
			AudioManager.STREAM_MUSIC, Cocos2dxSound.SOUND_QUALITY);
		this.mSoundPool
			.setOnLoadCompleteListener(new OnLoadCompletedListener());
		this.mLeftVolume = 0.5f;
		this.mRightVolume = 0.5f;
		this.mSemaphore = new Semaphore(0, true);
	}
	// ===========================================================
	// Getter & Setter
	// ===========================================================
	// ===========================================================
	// Methods for/from SuperClass/Interfaces
	// ===========================================================
	// ===========================================================
	// Methods
	// ===========================================================
	/*
	 * 개념주의: 
	 * 	soundID: 파일과 1:1 매칭
	 * streamID: soundID로 플레이중인 여러개의 소리스트림중의 하나.말하자면 소리 인스턴스. 
	 */
	// pPath의 파일을 읽어 soundID로 만든다. 파일당 soundID는 하나인것으로 보인다. 
	public int preloadEffect(final String pPath) {
		Integer soundID = this.mPathSoundIDMap.get(pPath);
		if (soundID == null) {
			soundID = this.createSoundIDFromAsset(pPath);
			Log.i(TAG, "preloadEffect:soundID=" + soundID.toString() );
			this.mPathSoundIDMap.put(pPath, soundID);
		}
		return soundID;
	}
	// pPath파일로 플레이중인 모든 streamID를 중지시키고 pPath파일을 unload한다.
	public void unloadEffect(final String pPath) {
		if( pPath == null ) {
			Log.e(TAG, "pPath == null" );
			return;
		}
		Log.d(TAG, "pPath:[" + pPath + "]" );
		// stop effects
		final ArrayList<Integer> streamIDs = this.mPathStreamIDsMap.get(pPath);
		if (streamIDs != null) {
			for (final Integer pStreamID : streamIDs) {
				this.mSoundPool.stop(pStreamID);
			}
		}
		this.mPathStreamIDsMap.remove(pPath);
		// unload effect
		final Integer soundID = this.mPathSoundIDMap.get(pPath);
		this.mSoundPool.unload(soundID);
		this.mPathSoundIDMap.remove(pPath);
	}
	// pPath 파일을 플레이한다.  플레이에 성공하면 새로운 스트림아이디를 얻어 리턴한다.
	// 플레이를 호출할때마다 스트림아이디는 새로 생기는것이므로 파일당 여러개가 있을 수 있다.
	// 플레이중인 어떤 소리를 제어하려면 이 스트림아이디를 이용한다.
	public int playEffect(final String pPath, final boolean pLoop) {
		// 파일이 로드되었는지 확인.
		Integer soundID = this.mPathSoundIDMap.get(pPath);
		int streamID = Cocos2dxSound.INVALID_STREAM_ID;
		if (soundID != null) {
			// 파일이 로딩되었으면 해당파일의 아이디로 플레이
			streamID = this.doPlayEffect(pPath, soundID.intValue(), pLoop);
		} else {
			// the effect is not prepared
			// 파일이 아직 로딩되지 않았으면 새로 로딩시켜 soundID를 만들고 streamID를 만들어낸다.
			soundID = this.preloadEffect(pPath);
			if (soundID == Cocos2dxSound.INVALID_SOUND_ID) {
				// can not preload effect
				return Cocos2dxSound.INVALID_SOUND_ID;
			}
			// only allow one playEffect at a time, or the semaphore will not
		// work correctly
			synchronized (this.mSoundPool) {
				// add this effect into mEffecToPlayWhenLoadedArray, and it will
			// be played when loaded completely
				mEffecToPlayWhenLoadedArray
					.add(new SoundInfoForLoadedCompleted(pPath, soundID
							.intValue(), pLoop));
				try {
					// wait OnloadedCompleteListener to set streamID
					this.mSemaphore.acquire();
					streamID = this.mStreamIdSyn;
				} catch (Exception e) {
					return Cocos2dxSound.INVALID_SOUND_ID;
				}
			}
		}
		Log.i(TAG, "playEffect:streamID=" + streamID );
		return streamID;
	}
	// soundID의 파일을 플레이한다. 플레이에 성공하면 스트림아이디를 만들어 내며, 같은파일명을 공유하는 배열에 스트림아이디를 추가시킨다.
	private int doPlayEffect(final String pPath, final int soundId, final boolean pLoop) {
		// play sound
		int streamID = this.mSoundPool.play(soundId, this.mLeftVolume,
				this.mRightVolume, Cocos2dxSound.SOUND_PRIORITY,
				pLoop ? -1 : 0, Cocos2dxSound.SOUND_RATE);
		// record stream id
		ArrayList<Integer> streamIDs = this.mPathStreamIDsMap.get(pPath);
		if (streamIDs == null) {
			streamIDs = new ArrayList<Integer>();
			this.mPathStreamIDsMap.put(pPath, streamIDs);
		}
		streamIDs.add(streamID);
		return streamID;
	}
	// 스트림아이디로 해당 소리를 중지시킨다. soundID가 아님에 주의
	public void stopEffect(final int pStreamID) {
		this.mSoundPool.stop(pStreamID);
		// remove record
		for (final String pPath : this.mPathStreamIDsMap.keySet()) {
			if (this.mPathStreamIDsMap.get(pPath).contains(pStreamID)) {
				this.mPathStreamIDsMap.get(pPath).remove(
					this.mPathStreamIDsMap.get(pPath).indexOf(pStreamID));
				break;
			}
		}
	}
	public void pauseEffect(final int pStreamID) {
		this.mSoundPool.pause(pStreamID);
	}
	public void resumeEffect(final int pStreamID) {
		this.mSoundPool.resume(pStreamID);
	}
	public void pauseAllEffects() {
		this.mSoundPool.autoPause();
	}
	public void resumeAllEffects() {
		// can not only invoke SoundPool.autoResume() here, because
		// it only resumes all effects paused by pauseAllEffects()
		if (!this.mPathStreamIDsMap.isEmpty()) {
			final Iterator<Entry<String, ArrayList<Integer>>> iter = this.mPathStreamIDsMap
				.entrySet().iterator();
			while (iter.hasNext()) {
				final Entry<String, ArrayList<Integer>> entry = iter.next();
				for (final int pStreamID : entry.getValue()) {
					this.mSoundPool.resume(pStreamID);
				}
			}
		}
	}
	// 플레이중인 모든 사운드 스트림을 중지시킨다. 
	@SuppressWarnings("unchecked")
	public void stopAllEffects() {
		// stop effects
		if (!this.mPathStreamIDsMap.isEmpty()) {
			final Iterator<?> iter = this.mPathStreamIDsMap.entrySet()
					.iterator();
			while (iter.hasNext()) {
				final Map.Entry<String, ArrayList<Integer>> entry = (Map.Entry<String, ArrayList<Integer>>) iter
					.next();
				for (final int pStreamID : entry.getValue()) {
					this.mSoundPool.stop(pStreamID);
				}
			}
		}
		// remove records
		this.mPathStreamIDsMap.clear();
	}
	public float getEffectsVolume() {
		return (this.mLeftVolume + this.mRightVolume) / 2;
	}
	// idStream사운드의 볼륨을 조절한다.,
	public void setEffectsVolumeByStream( final int idStream, float pVolume) {
		// pVolume should be in [0, 1.0]
		if (pVolume < 0) {
			pVolume = 0;
		}
		if (pVolume > 1) {
			pVolume = 1;
		}
//   		Log.d(Cocos2dxSound.TAG, "setEffectsVolumeByStream:" + idStream + "," + pVolume );
	    this.mSoundPool.setVolume(idStream, pVolume, pVolume);
	}
	// 플레이중인 모든 스트림의 볼륨을 조정한다.
	public void setEffectsVolume(float pVolume) {
		// pVolume should be in [0, 1.0]
		if (pVolume < 0) {
			pVolume = 0;
		}
		if (pVolume > 1) {
			pVolume = 1;
	    }
        // 여기서 왼쪽오른쪽 볼륨은 마스터 볼륨으로 사용
		this.mLeftVolume = this.mRightVolume = pVolume;
		// change the volume of playing sounds
		if (!this.mPathStreamIDsMap.isEmpty()) {
			final Iterator<Entry<String, ArrayList<Integer>>> iter = this.mPathStreamIDsMap.entrySet().iterator();
			while (iter.hasNext()) {
				final Entry<String, ArrayList<Integer>> entry = iter.next();
				for (final int pStreamID : entry.getValue()) {
					this.mSoundPool.setVolume(pStreamID, this.mLeftVolume,
							this.mRightVolume);
				}
			}
		}
	}
	public void end() {
		this.mSoundPool.release();
		this.mPathStreamIDsMap.clear();
		this.mPathSoundIDMap.clear();
		this.mEffecToPlayWhenLoadedArray.clear();
		this.mLeftVolume = 0.5f;
		this.mRightVolume = 0.5f;
		this.initData();
	}
	// 에셋으로부터 pPath파일을 로딩하고 그에 대응하는 soundID를 만든다.
	public int createSoundIDFromAsset(final String pPath) {
		int soundID = Cocos2dxSound.INVALID_SOUND_ID;
		try {
			if (pPath.startsWith("/")) {
				soundID = this.mSoundPool.load(pPath, 0);
			} else {
				// 파일은 내부에서 한번만 로딩하고 idSound만 새로 생성하는듯?
				soundID = this.mSoundPool.load(this.mContext.getAssets()
					.openFd(pPath), 0);
			}
		} catch (final Exception e) {
			soundID = Cocos2dxSound.INVALID_SOUND_ID;
			Log.e(Cocos2dxSound.TAG, "error: " + e.getMessage(), e);
		}
		return soundID;
	}
	// ===========================================================
	// Inner and Anonymous Classes
	// ===========================================================
	public class SoundInfoForLoadedCompleted {
		public int soundID;
		public boolean isLoop;
		public String path;
		public SoundInfoForLoadedCompleted(String path, int soundId,
			boolean isLoop) {
			this.path = path;
			this.soundID = soundId;
			this.isLoop = isLoop;
		}
	}
	public class OnLoadCompletedListener implements
			SoundPool.OnLoadCompleteListener {
		@Override
		public void onLoadComplete(SoundPool soundPool, int sampleId, int status) {
			if (status == 0)
			{
				// only play effect that are in mEffecToPlayWhenLoadedArray
				for (SoundInfoForLoadedCompleted info : mEffecToPlayWhenLoadedArray) {
					if (sampleId == info.soundID) {
						// set the stream id which will be returned by
					// playEffect()
						mStreamIdSyn = doPlayEffect(info.path, info.soundID,
							info.isLoop);
						// remove it from array, because we will break here
						// so it is safe to do
						mEffecToPlayWhenLoadedArray.remove(info);
						break;
					}
				}
			} else {
				mStreamIdSyn = Cocos2dxSound.INVALID_SOUND_ID;
			}
			mSemaphore.release();
		}
	}
}
