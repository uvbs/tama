set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -frtti -fno-omit-frame-pointer -fexceptions -Wparentheses -fpermissive")

add_library(xlib STATIC
XE.cpp 
XResMng.cpp 
XHSLMap.cpp 
xVector.cpp 
XXMLDoc.cpp 
XArchive.cpp 
Sound/Android/XSoundMngAndroid2.cpp 
XLock.cpp 
XPThread.cpp 
XRefObj.cpp 
XLockPThreadMutex.cpp 
XSystem.cpp 
XLibrary.cpp 
XCrypto.cpp 
XImage.cpp 
XImageLibPng.cpp 
XLua.cpp 
lua_tinker.cpp 
XResObj.cpp 
XAutoPtr.cpp 
XBaseFontDat.cpp 
XBaseFontObj.cpp 
XFontMng.cpp 
XFontSpr.cpp 
XParticle.cpp 
XParticleMng.cpp 
XParticleFramework.cpp 
XPool.cpp 
XRefRes.cpp 
XSoundMng.cpp 
XTransition.cpp 
XImageMng.cpp 
android_xe/ConvertString_android.cpp 
android_xe/CrashHandler.cpp 
Map/XEMap.cpp 
Map/XEMapLayer.cpp 
Map/XEMapLayerImage.cpp 
Sound/Android/XSoundMngAndroid.cpp 
../XDrawGraph.cpp 
../XGameUtil.cpp 
../XInterpolationObj.cpp 
            )

include_directories(
    ./
    ./android_xe
    ../../Third_Party_lib
    ../../Third_Party_lib/ftgles/ftgles2/ftgles/src
    ../../Third_Party_lib/ftgles/freetype2_android/include
    ../../Third_Party_lib/lua514/src
    ../../
    )

