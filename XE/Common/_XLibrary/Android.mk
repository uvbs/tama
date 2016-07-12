LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_MODULE := xe_static

LOCAL_MODULE_FILENAME := libxlib

LOCAL_SRC_FILES := \
XE.cpp \
XResMng.cpp \
../../XFramework/XParamObj.cpp \
../../XFramework/XEOption.cpp \
../../XFramework/XInApp.cpp \
../../XFramework/XReceiverCallback.cpp \
../../XFramework/Game/XEComponentMng.cpp \
../../XFramework/client/XEmitter.cpp \
../../XFramework/client/XPropParticle.cpp \
XHSLMap.cpp \
xVector.cpp \
XXMLDoc.cpp \
XArchive.cpp \
Sound/Android/XSoundMngAndroid2.cpp \
XLock.cpp \
XPThread.cpp \
XRefObj.cpp \
XLockPThreadMutex.cpp \
XSystem.cpp \
XLibrary.cpp \
XCrypto.cpp \
XImage.cpp \
XImageLibPng.cpp \
XLua.cpp \
lua_tinker.cpp \
XResObj.cpp \
XAutoPtr.cpp \
XBaseFontDat.cpp \
XBaseFontObj.cpp \
XFontMng.cpp \
XFontSpr.cpp \
XParticle.cpp \
XParticleMng.cpp \
XParticleFramework.cpp \
XPool.cpp \
XRefRes.cpp \
XSoundMng.cpp \
XTransition.cpp \
XImageMng.cpp \
android_xe/ConvertString.cpp \
android_xe/CrashHandler.cpp \
etc/Alert.cpp \
etc/Debug.cpp \
etc/InputMng.cpp \
etc/path.cpp \
etc/Timer.cpp \
etc/Token.cpp \
etc/XGraphics.cpp \
etc/xLang.cpp \
etc/xMath.cpp \
etc/XSurface.cpp \
etc/XTimer2.cpp \
etc/xUtil.cpp \
etc/iOS/xStringiOS.cpp \
etc/XSurfaceMem.cpp \
Sprite/Key.cpp \
Sprite/Layer.cpp \
Sprite/SprDat.cpp \
Sprite/Sprite.cpp \
Sprite/SprMng.cpp \
Sprite/SprObj.cpp \
Sprite/XBaseObj.cpp \
OpenGL2/XGraphicsOpenGL.cpp \
OpenGL2/XSurfaceOpenGL.cpp \
OpenGL2/XShader.cpp \
OpenGL2/XFontDatFTGL.cpp \
OpenGL2/XFontObjFTGL.cpp \
OpenGL2/XParticleOpenGL.cpp \
_Wnd2/XWnd.cpp \
_Wnd2/XWndButton.cpp \
_Wnd2/XWndButtonRadio.cpp \
_Wnd2/XWndEdit.cpp \
_Wnd2/XWndCtrls.cpp \
_Wnd2/XWndSprObj.cpp \
_Wnd2/XWndImage.cpp \
_Wnd2/XWndPageSlide.cpp \
_Wnd2/XWndText.cpp \
_Wnd2/XWndView.cpp \
_Wnd2/XWndPopup.cpp \
_Wnd2/XWndList.cpp \
_Wnd2/XWndProgressbar.cpp \
_Wnd2/XWndScrollview.cpp \
Map/XEMap.cpp \
Map/XEMapLayer.cpp \
Map/XEMapLayerImage.cpp \
Network/XPacket.cpp \
Network/XSocket.cpp \
Network/XDownloader.cpp \
Network/XDownloaderCurl.cpp \
Network/XDownloadTask.cpp \
Network/client/XNetworkConnection.cpp \
Network/client/XSocketClient.cpp \
Network/XNetworkDelegate.cpp \
Network/android/XBSDSocketClient.cpp \
Sound/Android/XSoundMngAndroid.cpp \
../../XFramework/XEToolTip.cpp \
../../XFramework/XDBAccount.cpp \
../../XFramework/XConstant.cpp \
../../XFramework/XTextTable.cpp \
../../XFramework/XTextTableUTF8.cpp \
../../XFramework/XF.cpp \
../../XFramework/XConsoleMain.cpp \
../../XFramework/XScroll.cpp \
../../XFramework/XSoundTable.cpp \
../../XFramework/XGlobal.cpp \
../../XFramework/XFacebook.cpp \
../../XFramework/XEBaseScene.cpp \
../../XFramework/XEBaseProp.cpp \
../../XFramework/XESceneMng.cpp \
../../XFramework/XEProfile.cpp \
../../XFramework/Game/XFLevel.cpp \
../../XFramework/Game/XEBaseWorldObj.cpp \
../../XFramework/Game/XEWndWorld.cpp \
../../XFramework/Game/XEWndWorldImage.cpp \
../../XFramework/Game/XEObjMng.cpp \
../../XFramework/Game/XEWorld.cpp \
../../XFramework/Game/XEComponents.cpp \
../../XFramework/client/XApp.cpp \
../../XFramework/client/XEContent.cpp \
../../XFramework/client/XClientMain.cpp \
../../XFramework/client/XLayout.cpp \
../../XFramework/client/XLayoutObj.cpp \
../../XFramework/client/XTimeoutMng.cpp \
../../Third_Party_lib/zip_support/ioapi.cpp \
../../Third_Party_lib/zip_support/unzip.cpp \
../../Third_Party_lib/tinyxml/tinystr.cpp \
../../Third_Party_lib/tinyxml/tinyxml.cpp \
../../Third_Party_lib/tinyxml/tinyxmlerror.cpp \
../../Third_Party_lib/tinyxml/tinyxmlparser.cpp \
../XDrawGraph.cpp \
../XGameUtil.cpp \
../XInterpolationObj.cpp \




LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
						$(LOCAL_PATH)/../ \
						$(LOCAL_PATH)/../../ \
						$(LOCAL_PATH)/android_xe/ \
						$(LOCAL_PATH)/android_xe/prebuilt/libpng/include/ \
						$(LOCAL_PATH)/android_xe/prebuilt/libcurl/include/curl/ \
						$(LOCAL_PATH)/../../Third_Party_lib/Oolong_Engine2/include \
						$(LOCAL_PATH)/../../Third_Party_lib/zip_support \
						$(LOCAL_PATH)/../../Third_Party_lib/lua514/src/ \
						$(LOCAL_PATH)/../../Third_Party_lib/ftgles/ftgles2/ftgles/src/ \
						$(LOCAL_PATH)/../../Third_Party_lib/ftgles/freetype2_android/include/ \
						$(LOCAL_PATH)/../../Third_Party_lib/tinyxml/ \
						$(LOCAL_PATH)/../../Third_Party_lib \
						$(LOCAL_PATH)/../../Third_Party_lib\boost_1_55_0/ \
						

LOCAL_EXPORT_LDLIBS := -llog\
                       -lz \
                       -lGLESv2 \
                       

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../ \
					$(LOCAL_PATH)/android_xe/ \
					$(LOCAL_PATH)/android_xe/prebuilt/libpng/include/ \
					$(LOCAL_PATH)/android_xe/prebuilt/libcurl/include/curl/ \
					$(LOCAL_PATH)/../../Third_Party_lib/Oolong_Engine2/include \
					$(LOCAL_PATH)/../../Third_Party_lib/zip_support \
					$(LOCAL_PATH)/../../Third_Party_lib/lua514/src/ \
					$(LOCAL_PATH)/../../Third_Party_lib/ftgles/ftgles2/ftgles/src/ \
					$(LOCAL_PATH)/../../Third_Party_lib/ftgles/freetype2_android/include/ \
					$(LOCAL_PATH)/../../Third_Party_lib/tinyxml/ \
					$(LOCAL_PATH)/../../Third_Party_lib\boost_1_55_0/ \
					$(LOCAL_PATH)/../../Third_Party_lib \
					$(LOCAL_PATH)/../../ \

					
LOCAL_LDLIBS := -lGLESv2 \
                -lEGL \
                -llog \
                -lz \

LOCAL_CFLAGS += $(COMMON_CFLAGS)

LOCAL_WHOLE_STATIC_LIBRARIES := cocos_libpng_static cocos_curl_static

include $(BUILD_STATIC_LIBRARY)

$(call import-module,libpng)
$(call import-module,libcurl)
