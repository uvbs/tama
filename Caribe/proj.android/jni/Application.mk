# 안드로이드 google결제용 빌드
NDK_TOOLCHAIN_VERSION=4.8
APP_STL := gnustl_static
APP_CPPFLAGS := -frtti -std=c++11 -D_XGMO -D_XPATCH
APP_PLATFORM := android-8
# -Wliteral-suffix -Wparentheses