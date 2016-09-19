package com.mtricks.caribe;

import android.util.Log;

class Version {
	// 
	public static final boolean _XKOREA = true;
	public static final boolean _XTHAILAND = false;
	public static final boolean _XINDONESIA = false;
<<<<<<< HEAD
	public static final boolean _SOFTNYX = false;
=======
>>>>>>> a0ed401913e2273227457e089b4100928406b35c
	// 
	public static final boolean _XUNIVERSAL = true;		// 
	public static boolean _XFACEBOOK;		// 
	public static boolean _XGOOGLE_IAP;		// 
	public static boolean _XINI3;			// INI3
<<<<<<< HEAD
	public static boolean _XSOFTNYX;
	Version() {
	    Log.d("xuzhu", "Version()");
		if(_XKOREA==true ) {
			_XFACEBOOK = true;
			_XGOOGLE_IAP = true;
			_XINI3 = false;
			_XSOFTNYX = false;
=======
	Version() {
	    Log.d("xuzhu", "Version()");
		if(_XKOREA==true ) {
			_XFACEBOOK = false;
			_XGOOGLE_IAP = true;
			_XINI3 = false;
>>>>>>> a0ed401913e2273227457e089b4100928406b35c
		} else
		if(_XINDONESIA==true) {
			_XFACEBOOK = true;
			_XGOOGLE_IAP = true;
			_XINI3 = false;
<<<<<<< HEAD
			_XSOFTNYX = false;
=======
>>>>>>> a0ed401913e2273227457e089b4100928406b35c
		} else
		if(_XTHAILAND==true) {
			_XFACEBOOK = false;
			_XGOOGLE_IAP = true;
			_XINI3 = true;
<<<<<<< HEAD
			_XSOFTNYX = false;
		} else
		if(_SOFTNYX == true) {
			_XFACEBOOK = true;
			_XGOOGLE_IAP = false;
			_XINI3 = false;
			_XSOFTNYX = true;
=======
>>>>>>> a0ed401913e2273227457e089b4100928406b35c
		}
	}
}