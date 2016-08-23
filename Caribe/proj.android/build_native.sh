APPNAME="xlib_test"

NDK_ROOT="/Users/xuzhu/android/android-ndk-r8d"
# options

buildexternalsfromsource=

usage(){
cat << EOF
usage: $0 [options]

Build C/C++ code for $APPNAME using Android NDK

OPTIONS:
-s	Build externals from source
-h	this help
EOF
}

while getopts "sh" OPTION; do
case "$OPTION" in
s)
buildexternalsfromsource=1
;;
h)
usage
exit 0
;;
esac
done

# paths

if [ -z "${NDK_ROOT+aaa}" ];then
echo "please define NDK_ROOT"
exit 1
fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# ... use paths relative to current directory
XLIB_ROOT="$DIR/.."
APP_ROOT="$DIR"
APP_ANDROID_ROOT="$DIR"

echo "NDK_ROOT = $NDK_ROOT"
echo "XLIB_ROOT = $XLIB_ROOT"
echo "APP_ROOT = $APP_ROOT"
echo "APP_ANDROID_ROOT = $APP_ANDROID_ROOT"


    echo "Using prebuilt externals"
    "$NDK_ROOT"/ndk-build -C "$APP_ANDROID_ROOT" $* \
		"NDK_MODULE_PATH=${XLIB_ROOT}/../XE:${XLIB_ROOT}:${XLIB_ROOT}/T3/game_src/:${XLIB_ROOT}/../XE/Common/_XLibrary/android/prebuilt"

#        "NDK_MODULE_PATH=${XLIB_ROOT}:${XLIB_ROOT}/T3/game_src/:${XLIB_ROOT}/Common/_XLibrary/android/prebuilt"
