package com.gemtree2.caribe;

import android.util.Log;

class Version {
	// 
	public static final boolean _XKOREA = true;
	public static final boolean _XTHAILAND = false;
	public static final boolean _XINDONESIA = false;
	public static final boolean _SOFTNYX = false;
	// 
	public static final boolean _XUNIVERSAL = true;		// 
	public static boolean _XFACEBOOK;		// 
	public static boolean _XGOOGLE_IAP;		// 
	public static boolean _XINI3;			// INI3
	public static boolean _XSOFTNYX;
	Version() {
	    Log.d("xuzhu", "Version()");
		if(_XKOREA==true ) {
			_XFACEBOOK = true;
			_XGOOGLE_IAP = true;
			_XINI3 = false;
			_XSOFTNYX = false;
		} else
		if(_XINDONESIA==true) {
			_XFACEBOOK = true;
			_XGOOGLE_IAP = true;
			_XINI3 = false;
			_XSOFTNYX = false;
		} else
		if(_XTHAILAND==true) {
			_XFACEBOOK = false;
			_XGOOGLE_IAP = true;
			_XINI3 = true;
			_XSOFTNYX = false;
		} else
		if(_SOFTNYX == true) {
			_XFACEBOOK = true;
			_XGOOGLE_IAP = false;
			_XINI3 = false;
			_XSOFTNYX = true;
		}
	}
}