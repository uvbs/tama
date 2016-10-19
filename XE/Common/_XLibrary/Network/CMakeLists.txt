add_library(xe_network STATIC
XPacket.cpp 
XSocket.cpp 
XDownloader.cpp 
XDownloaderCurl.cpp 
XDownloadTask.cpp 
client/XNetworkConnection.cpp 
client/XSocketClient.cpp 
XNetworkDelegate.cpp 
android/XBSDSocketClient.cpp 
            )
include_directories(
    ../android_xe
    ../
    ../../../Third_Party_lib
    ../../../Third_Party_lib/ftgles/ftgles2/ftgles/src
    ../../../Third_Party_lib/ftgles/freetype2_android/include
    ../../../Common/_XLibrary/android_xe/prebuilt/libcurl/include
    ../../../
        )

