/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
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

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.util.Locale;

import com.mtricks.trivialdrivesample.util.IabException;

import android.app.ActivityManager;
import android.app.ActivityManager.MemoryInfo;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.res.AssetManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Build;
import android.os.Environment;
import android.util.Log;

//import com.mtrics.epic.thl.R;

public class Cocos2dxHelper {
	// ===========================================================
	// Constants
	// ===========================================================

	// ===========================================================
	// Fields
	// ===========================================================

	public static final String TAG = "xuzhu";
	private static Cocos2dxMusic sCocos2dMusic;
	private static Cocos2dxSound sCocos2dSound;
	private static AssetManager sAssetManager;
	private static boolean sAccelerometerEnabled;
	private static String sPackageName;

	private static Cocos2dxHelperListener sCocos2dxHelperListener;

	// ===========================================================
	// Constructors
	// ===========================================================

	public static void init(final Context pContext,
			final Cocos2dxHelperListener pCocos2dxHelperListener) {
		final ApplicationInfo applicationInfo = pContext.getApplicationInfo();

		Cocos2dxHelper.sCocos2dxHelperListener = pCocos2dxHelperListener;

		Cocos2dxHelper.sPackageName = applicationInfo.packageName;
		Cocos2dxHelper.nativeSetApkPath(applicationInfo.sourceDir);
		// Cocos2dxHelper.nativeSetExternalAssetPath(Cocos2dxHelper.getAbsolutePathOnExternalStorage(applicationInfo,
		// "assets/"));
		// String path =
		// Cocos2dxHelper.getAbsolutePathOnExternalStorage(applicationInfo,
		// "assets/");
		// Cocos2dxHelper.nativeSetExternalResourcePath(Cocos2dxHelper.getAbsolutePathOnExternalPrivateStorage(pContext,
		// "Resource/"));
		Cocos2dxHelper.nativeSetInternalResourcePath(Cocos2dxHelper
				.getAbsolutePathOnInternalStorage(pContext, "Resource/"));

		Cocos2dxHelper.sCocos2dMusic = new Cocos2dxMusic(pContext);
		Cocos2dxHelper.sCocos2dSound = new Cocos2dxSound(pContext);

		Cocos2dxHelper.sAssetManager = pContext.getAssets();
	}

	// Environment.getDataDirectory().getAbsolutePath()
	private static int saveDefaultModule(final Context pContext,
			String assetsfile) {
		// AssetManager mgr = getResources().getAssets();
		AssetManager mgr = getAssetManager();
		if (mgr != null) {
			FileInputStream fis = null;
			BufferedInputStream bis = null;
			FileOutputStream fos = null;

			try {
				InputStream is = mgr.open("spr/" + assetsfile);
				bis = new BufferedInputStream(is);
				// fis = new FileInputStream(bis);

				Log.d("xuzhu", "package path: "
						+ pContext.getFilesDir().getAbsolutePath());

				String path = pContext.getFilesDir().getAbsolutePath()
						.replace("files", "module");
				// String path = pContext.getFilesDir().getAbsolutePath();
				File f = new File(path);
				if (!f.exists())
					f.mkdirs();

				String outfile = path + "/" + assetsfile;
				Log.d("xuzhu", "path: " + outfile);

				f = new File(outfile);
				if (!f.exists()) {
					fos = new FileOutputStream(f);

					int readBytes = 0;
					byte[] buf = new byte[1024];
					while ((readBytes = bis.read(buf, 0, 1024)) != -1) {
						fos.write(buf, 0, readBytes);
					}
				}

				if (fos != null)
					fos.close();
				if (bis != null)
					bis.close();
				mgr.close();
			} catch (IOException e) {
				// TODO: handle exception
				Log.e("xuzhu", e.getMessage());

				try {
					if (fos != null)
						fos.close();
					if (bis != null)
						bis.close();
					mgr.close();
				} catch (Exception e2) {
					// TODO: handle exception
				}
			}
		}
		return 1;
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

	private static native void nativeSetApkPath(final String pApkPath);

	private static native void nativeSetExternalAssetPath(
			final String pExternalAssetPath);

	private static native void nativeSetExternalResourcePath(
			final String pExternalResourcePath);

	private static native void nativeSetInternalResourcePath(
			final String pInternalResourcePath);

	// private static native void nativeOnKeyDown( int keyCode, int keyAction );

	private static native void nativeSetEditTextDialogResult(final byte[] pBytes);

	// public static void onKeyDown( int keyCode, int keyAction ) {
	// nativeOnKeyDown( keyCode, keyAction );
	// }
	public static String getCocos2dxPackageName() {
		return Cocos2dxHelper.sPackageName;
	}

	public static String getCurrentLanguage() {
		return Locale.getDefault().getLanguage();
	}

	public static String getDeviceModel() {
		return Build.MODEL;
	}

	public static AssetManager getAssetManager() {
		return Cocos2dxHelper.sAssetManager;
	}

	private static String getAbsolutePathOnExternalPrivateStorage(
			final Context pContext, final String pPath) {
		String pathPrivate = pContext.getExternalFilesDir(null)
				.getAbsolutePath();
		Log.i("xuzhu", "external private storage:" + pathPrivate + "/" + pPath);
		return pathPrivate + "/" + pPath;
	}

	private static String getAbsolutePathOnInternalStorage(
			final Context pContext, final String pPath) {
		String path = pContext.getFilesDir().getAbsolutePath()
				.replace("files", pPath);
		return path;
	}

	public static byte[] Callback_test(byte[] byteStr) {
		System.out.print("callback test");
		String tmp = "hi xuzhu";
		byte[] result = tmp.getBytes();
		return result;
	}

	public static byte[] Callback_GetUnicodeString(byte[] byteStr) {
		byte[] result = null;
		try {
			String tmpStr = new String(byteStr, "KSC5601");
			result = tmpStr.getBytes("UTF-8");
			tmpStr = null;
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		return result;
	}

	public static byte[] Callback_utf16_to_utf8(byte[] byteStr) {
		byte[] result = null;
		try {
			String tmpStr = new String(byteStr, "UTF-16");
			result = tmpStr.getBytes("UTF-8");
			tmpStr = null;
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		return result;
	}

	public static byte[] Callback_euckr_to_utf8(byte[] byteStr) {
		byte[] result = null;
		try {
			String tmpStr = new String(byteStr, "KSC5601");
			result = tmpStr.getBytes("UTF-8");
			tmpStr = null;
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		return result;
	}

	public static byte Callback_isExistDir(byte[] bytePath) {
		byte exist = 0;
		try {
			String path = new String(bytePath, "KSC5601");
			File f = new File(path);
			if (f.exists())
				exist = 1;
			else
				exist = 0;
			// f.mkdirs();
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		return exist;
	}

	public static void Callback_MakeDir(byte[] bytePath) {
		try {
			String path = new String(bytePath, "KSC5601");
			File f = new File(path);
			if (!f.exists())
				f.mkdirs();
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
	}

	private static void Callback_OnClickedBuyItem(final String stridProduct, final String strPayload ) {
		Log.d("xuzhu", "java Callback_OnClickedBuyItem." + stridProduct + "payload:" + strPayload );
		Cocos2dxHelper.sCocos2dxHelperListener.onBuyProduct(stridProduct, strPayload );
	}
//	public static void Callback_GetMemoryInfo() {
//		try {
//			ActivityManager activityMng = (ActivityManager)getSystemService
//			MemoryInfo memInfo = new Ac
//		} catch (UnsupportedEncodingException e) {
//			e.printStackTrace();
//		}
//	}

	/*
	 * private static void Callback_OnSendAuthenIni3ByIDPW(final String strId,
	 * final String strPassword) { Log.d("xuzhu",
	 * "java Callback_OnClickedLogin." + strId + "," + strPassword);
	 * Cocos2dxHelper.sCocos2dxHelperListener.onSendAuthenIni3ByIDPW(strId,
	 * strPassword); } private static void Callback_DoLoginIni3ByFacebook() {
	 * Log.d("xuzhu", "java Callback_DoLoginIni3ByFacebook.");
	 * Cocos2dxHelper.sCocos2dxHelperListener.DoLoginIni3ByFacebook(); } private
	 * static void Callback_DoLoginLytoByFacebook() { Log.d("xuzhu",
	 * "java Callback_DoLoginLytoByFacebook.");
	 * Cocos2dxHelper.sCocos2dxHelperListener.DoLoginLytoByFacebook(); }
	 */
	private static void Callback_DoLoginFacebook() {
		Log.d("xuzhu", "java Callback_DoLoginFacebook.");
		Cocos2dxHelper.sCocos2dxHelperListener.DoLoginFacebook();
	}
	private static void Callback_DoLogoutFacebook() {
		Log.d("xuzhu", "java Callback_DoLogoutFacebook.");
		Cocos2dxHelper.sCocos2dxHelperListener.DoLogoutFacebook();
	}

	private static void Callback_GetUserProfileImage(final String strID) {
		//Log.d("xuzhu", "java Callback_GetUserProfileImage.");
		Cocos2dxHelper.sCocos2dxHelperListener.GetUserProfileImage(strID);
	}

	public static int Callback_renameFile(final String oldFile,
			final String newFile) {
		File fileOld = new File(oldFile);
		File fileNew = new File(newFile);
		if (!fileOld.renameTo(fileNew)) {
			return 0;
		}
		return 1;
	}

	public static void Callback_DoExitApp() {
		Log.d("xuzhu", "java Callback_ExitApp");
		Cocos2dxHelper.sCocos2dxHelperListener.DoExitApp();
	}

	public static boolean Callback_check3G() {
		Log.d("xuzhu", "java Callback_check3G");
		boolean flag = Cocos2dxHelper.sCocos2dxHelperListener.CheckCDMA();
		Log.d("xuzhu", "is 3g? :" + Boolean.toString(flag));
		return flag;
	}

	public static boolean Callback_checkWiFi() {
		Log.d("xuzhu", "java Callback_checkWiFi");
		boolean flag = Cocos2dxHelper.sCocos2dxHelperListener.CheckWiFi();
		Log.d("xuzhu", "is WiFi? :" + Boolean.toString(flag));
		return flag;
	}
	// 구글 인앱시스템 초기화
	private static void Callback_InitGoogleIAP(final String publicKey ) {
		Log.d("xuzhu", "Java:Callback_InitGoogleIAP:" );
		Cocos2dxHelper.sCocos2dxHelperListener.InitGoogleIAP( publicKey );
	}
	public static void Callback_DoConsumeItem(final String idsProduct ) {
		Log.d(TAG, "xeActivity.DoConsumeItem:" + idsProduct );
		Cocos2dxHelper.sCocos2dxHelperListener.DoConsumeItem( idsProduct );
	}
	public static void Callback_CheckUnConsumedItem() {
		Log.d(TAG, "xeActivity.DoCheckUnconsumedItem:" );
		Cocos2dxHelper.sCocos2dxHelperListener.CheckUnConsumedItem();
	}
	public static void Callback_SoftnyxLogin() {
		Log.d(TAG, "Callback_SoftnyxLogin:" );
		Cocos2dxHelper.sCocos2dxHelperListener.PGMAN_LoginFromJni();
	}
	public static void Callback_SoftnyxLogout() {
		Log.d(TAG, "Callback_SoftnyxLogout:" );
		Cocos2dxHelper.sCocos2dxHelperListener.PGMAN_LogoutFromJni();
	}
	public static void Callback_SoftnyxBuyItem( final String idsProduct
											  , final String strPayload
											  , final int price ) {
		Log.d(TAG, "Callback_SoftnyxBuyItem:ids=" + idsProduct
//				+ " payload=" + strPayload 
				+ " price=" + price 
				);
		Cocos2dxHelper.sCocos2dxHelperListener.PGMAN_BuyItemFromJni( idsProduct, price, strPayload );
	}
	public static void Callback_ShowAdmob( final boolean bShow, final int dpX, final int dpY ) {
		Log.d(TAG, "Java: Callback_ShowAdmob" );
		Cocos2dxHelper.sCocos2dxHelperListener.ShowAdViewToMsg( bShow, dpX, dpY );
	}
	
	public static void Callback_DoTest() {
		Log.d(TAG, "Java: callback_Test" );
		Cocos2dxHelper.sCocos2dxHelperListener.DoTest();
	}
	/*
	 * public static void terminateProcess() {
	 * android.os.Process.killProcess(android.os.Process.myPid()); }
	 * 
	 * private static void showDialog(final String pTitle, final String
	 * pMessage) { Cocos2dxHelper.sCocos2dxHelperListener.showDialog(pTitle,
	 * pMessage); }
	 */
	private static void showDialog(final String pTitle, final String pMessage) { 
		 Cocos2dxHelper.sCocos2dxHelperListener.showDialog(pTitle, pMessage); 
	}

	private static void showEditTextDialog(final String pTitle,
			final String pMessage, final int pInputMode, final int pInputFlag,
			final int pReturnType, final int pMaxLength) {
		Cocos2dxHelper.sCocos2dxHelperListener.showEditTextDialog(pTitle,
				pMessage, pInputMode, pInputFlag, pReturnType, pMaxLength);
	}

	public static void setEditTextDialogResult(final String pResult) {
		try {
			final byte[] bytesUTF8 = pResult.getBytes("UTF8");

			Cocos2dxHelper.sCocos2dxHelperListener
					.runOnGLThread(new Runnable() {
						@Override
						public void run() {
							Cocos2dxHelper
									.nativeSetEditTextDialogResult(bytesUTF8);
						}
					});
		} catch (UnsupportedEncodingException pUnsupportedEncodingException) {
			// Nothing.
		}
	}

	private static String getAbsolutePathOnExternalStorage(
			final ApplicationInfo pApplicationInfo, final String pPath) {
		return Environment.getExternalStorageDirectory() + "/Android/data/"
				+ pApplicationInfo.packageName + "/files/" + pPath;
	}

	// Music
	public static void preloadBackgroundMusic(final String pPath) {
		Cocos2dxHelper.sCocos2dMusic.preloadBackgroundMusic(pPath);
	}

	public static void playBackgroundMusic(final String pPath,
			final boolean isLoop) {
		Cocos2dxHelper.sCocos2dMusic.playBackgroundMusic(pPath, isLoop);
	}

	public static void resumeBackgroundMusic() {
		Cocos2dxHelper.sCocos2dMusic.resumeBackgroundMusic();
	}

	public static void pauseBackgroundMusic() {
		Cocos2dxHelper.sCocos2dMusic.pauseBackgroundMusic();
	}

	public static void stopBackgroundMusic() {
		Cocos2dxHelper.sCocos2dMusic.stopBackgroundMusic();
		Log.d("xuzhu", "java stopBackgroundMusic");
	}

	public static void rewindBackgroundMusic() {
		Cocos2dxHelper.sCocos2dMusic.rewindBackgroundMusic();
	}

	public static boolean isBackgroundMusicPlaying() {
		return Cocos2dxHelper.sCocos2dMusic.isBackgroundMusicPlaying();
	}

	public static float getBackgroundMusicVolume() {
		return Cocos2dxHelper.sCocos2dMusic.getBackgroundVolume();
	}

	public static void setBackgroundMusicVolume(final float volume) {
		Cocos2dxHelper.sCocos2dMusic.setBackgroundVolume(volume);
	}

	public static void preloadEffect(final String path) {
		Cocos2dxHelper.sCocos2dSound.preloadEffect(path);
	}

	public static int playEffect(final String path, final boolean isLoop) {
		return Cocos2dxHelper.sCocos2dSound.playEffect(path, isLoop);
	}

	public static void resumeEffect(final int soundId) {
		Cocos2dxHelper.sCocos2dSound.resumeEffect(soundId);
	}

	public static void pauseEffect(final int soundId) {
		Cocos2dxHelper.sCocos2dSound.pauseEffect(soundId);
	}

	public static void stopEffect(final int soundId) {
		Cocos2dxHelper.sCocos2dSound.stopEffect(soundId);
	}

	public static float getEffectsVolume() {
		return Cocos2dxHelper.sCocos2dSound.getEffectsVolume();
	}

	public static void setEffectsVolume(final float volume) {
		Cocos2dxHelper.sCocos2dSound.setEffectsVolume(volume);
	}
	
	public static void setEffectsVolumeByStream( final int idStream, final float volume) {
		Cocos2dxHelper.sCocos2dSound.setEffectsVolumeByStream(idStream, volume);
	}

	public static void unloadEffect(final String path) {
		Cocos2dxHelper.sCocos2dSound.unloadEffect(path);
	}

	public static void pauseAllEffects() {
		Cocos2dxHelper.sCocos2dSound.pauseAllEffects();
	}

	public static void resumeAllEffects() {
		Cocos2dxHelper.sCocos2dSound.resumeAllEffects();
	}

	public static void stopAllEffects() {
		Cocos2dxHelper.sCocos2dSound.stopAllEffects();
	}

	public static void end() {
		Cocos2dxHelper.sCocos2dMusic.end();
		Cocos2dxHelper.sCocos2dSound.end();
	}

	// ===========================================================
	// Inner and Anonymous Classes
	// ===========================================================

	public static interface Cocos2dxHelperListener {
		public void showDialog(final String pTitle, final String pMessage);
		public void showEditTextDialog(final String pTitle,
				final String pMessage, final int pInputMode,
				final int pInputFlag, final int pReturnType,
				final int pMaxLength);
		public void onBuyProduct(final String stridProduct, final String strPayload );
		// public void onSendAuthenIni3ByIDPW(final String strId, final String
		// strPassword);
		// public void onSendAuthenLytoByDeviceID(final String strDeviceID);
		public void runOnGLThread(final Runnable pRunnable);
		public void DoLoginFacebook();
		public void DoLogoutFacebook();
		// public void DoLoginIni3ByFacebook();
		// public void DoLoginLytoByFacebook();
		public void PGMAN_LoginFromJni();
		public void PGMAN_LogoutFromJni();
		public void PGMAN_BuyItemFromJni( final String idsProduct, int price, final String strPayload );
		public void DoExitApp();
		public void DoTest();
		public void ShowAdViewToMsg( boolean bShow, int dpX, int dpY );
		public boolean CheckCDMA();
		public boolean CheckWiFi();
		public void GetUserProfileImage(final String strID);
		public void InitGoogleIAP( final String publicKey ); 
		public void DoConsumeItem( final String idsProduct ); 
		public void CheckUnConsumedItem();

	}
}
