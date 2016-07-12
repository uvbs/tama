package com.mtricks.caribe;

import android.util.Log;

class Version {
	// 
	public static final boolean _XKOREA = true;
	public static final boolean _XTHAILAND = false;
	public static final boolean _XINDONESIA = false;
	// 
	public static final boolean _XUNIVERSAL = true;		// 
	public static boolean _XFACEBOOK;		// 
	public static boolean _XGOOGLE_IAP;		// 
	public static boolean _XINI3;			// INI3
	Version() {
	    Log.d("xuzhu", "Version()");
		if(_XKOREA==true ) {
			_XFACEBOOK = false;
			_XGOOGLE_IAP = true;
			_XINI3 = false;
		} else
		if(_XINDONESIA==true) {
			_XFACEBOOK = true;
			_XGOOGLE_IAP = true;
			_XINI3 = false;
		} else
		if(_XTHAILAND==true) {
			_XFACEBOOK = false;
			_XGOOGLE_IAP = true;
			_XINI3 = true;
		}
	}
}