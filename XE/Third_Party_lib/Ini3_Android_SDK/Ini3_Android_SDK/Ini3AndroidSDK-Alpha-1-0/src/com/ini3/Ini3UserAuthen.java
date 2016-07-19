package com.ini3;

import java.io.IOException;
import java.net.URL;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParserFactory;

import android.content.res.XmlResourceParser;
import android.os.AsyncTask;
import android.util.Log;

public class Ini3UserAuthen extends AsyncTask<Object, Object, Boolean> {
	
	private static final String test_url = "http://event.ini3.co.th/_test_billing_event/android/authenvisa.asp";
	private static String strUserName, strEmail, strGender, retMsg;
	private int retVal, intUserId;
	private Boolean onExecute;
	private AuthenTaskListener _listener;
	private String CON_LIB_VERSION = "alpha v.1.0";
	
	public Ini3UserAuthen() {
		// TODO Auto-generated constructor stub
		setExecuteStatus(false);
		setReturnMsg(null);
		setUserId(-1);
		setUserName(null);
		setEmail(null);
		setGender(null);
	}
	
	public void authenUser(String userid, String password, AuthenTaskListener listener) {
		
		// return if onExecute process...
		try {
			if(getExecuteStatus()) {
				listener.onExecute(true);
				return;
			}
		} catch(Exception e) {
			e.printStackTrace();
		}
		_listener = listener;		
		this.execute(userid, password);
	}
	
	public void authenUserFacebook(String userid, AuthenTaskListener listener) {
		Log.i("DEBUG", "authenUserFacebook: ".concat(userid));
		// return if onExecute process...
		try {
			if(getExecuteStatus()) {
				listener.onExecute(true);
				return;
			}
		} catch(Exception e) {
			e.printStackTrace();
		}
		_listener = listener;		
		this.execute(userid, "facebook");
	}

	@Override
	protected Boolean doInBackground(Object... params) {
		String userid = params[0].toString();
		String password = params[1].toString();
		
		XmlPullParser result;
		
		try {
			String authen_url = test_url.concat("?userid=");
			authen_url = authen_url.concat(userid);
			authen_url = authen_url.concat("&pass=");
			authen_url = authen_url.concat(password);
			
            URL xmlUrl = new URL(authen_url);
            result = XmlPullParserFactory.newInstance().newPullParser();
            result.setInput(xmlUrl.openStream(), null);
        } catch (XmlPullParserException e) {
        	result = null;
        	Log.i("DEBUG", "doInBackground: ".concat((String)e.getMessage()));
        	return false;
        } catch (IOException e) {
        	result = null;
        	Log.i("DEBUG", "doInBackground: ".concat((String)e.getMessage()));
        	return false;
        } catch (Exception e) {
        	result = null;
        	Log.i("DEBUG", "doInBackground: ".concat((String)e.getMessage()));
        	return false;
        }
				
		if (result != null) {
            try {
                xmlResult(result);
            } catch (XmlPullParserException e) {
                Log.e("DEBUG", "Pull Parser failure", e);
                return false;
            } catch (IOException e) {
                Log.e("DEBUG", "IO Exception parsing XML", e);
                return false;
            }
        }
        
		return true;
	}
	
	private void xmlResult(XmlPullParser result) throws XmlPullParserException, IOException {
		int eventType = result.getEventType();		
		while (eventType != XmlResourceParser.END_DOCUMENT) {
			if (eventType == XmlResourceParser.START_TAG) {
				if(result.getName().equals("result")) {
					retVal = Integer.parseInt(result.getAttributeValue(null, "ret"));
				} else if(result.getName().equals("msg")) {
					eventType = result.next();
					setReturnMsg(result.getText());
				} else if(result.getName().equals("userid")) {
					eventType = result.next();
					setUserId(Integer.parseInt(result.getText()));
				} else if(result.getName().equals("username")) {
					eventType = result.next();
					setUserName(result.getText());
				} else if(result.getName().equals("email")) {
					eventType = result.next();
					setEmail(result.getText());
				} else if(result.getName().equals("gender")) {
					eventType = result.next();
					setGender(result.getText());
				}
			}
			eventType = result.next();
		}
	}
	
	@Override
    protected void onPreExecute() {
		setExecuteStatus(true);
	}
	
	@Override
    protected void onPostExecute(Boolean result) {
		if(retVal==1) {
			Log.e("DEBUG", "Fail to Authentication!!!");
			_listener.onComplete(-100);
		} else if(retVal==0) {			
			_listener.onComplete(0);
		} else {
			Log.e("DEBUG", "Error to Authentication, Try again.");
			_listener.onComplete(-100);
		}
		setExecuteStatus(false);
    }

	//@SuppressWarnings("hiding")
	public interface AuthenTaskListener {
		public void onComplete(int result);
		
		public void onExecute(Boolean result);
	}
	
	private void setUserId(int var) {
		intUserId = var;
	}
	
	private void setUserName(String var) {
		strUserName = var;
	}
	
	private void setEmail(String var) {
		strEmail = var;
	}
	
	private void setGender(String var) {
		strGender = var;
	}
	
	private void setReturnMsg(String var) {
		retMsg = var;
	}
	
	private void setExecuteStatus(Boolean var) {
		onExecute = var;
	}
	
	public int getUserId() {
		return intUserId;
	}
	
	public String getUserName() {
		return strUserName;
	}
	
	public String getEmail() {
		return strEmail;
	}
	
	public String getGender() {
		return strGender;
	}
	
	public String getReturnMsg() {
		return retMsg;
	}
	
	public Boolean getExecuteStatus() {
		return onExecute;
	}
	
	public String getversion() {
		return CON_LIB_VERSION;
	}
	
}