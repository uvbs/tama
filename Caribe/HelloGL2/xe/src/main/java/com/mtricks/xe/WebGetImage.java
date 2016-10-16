package com.mtricks.xe;

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.net.URL;
import java.net.URLConnection;
import java.nio.Buffer;

import org.apache.http.Header;
import org.apache.http.HttpResponse;
import org.apache.http.client.methods.HttpGet;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.BitmapFactory;
import android.net.http.AndroidHttpClient;
import android.os.AsyncTask;
import android.util.Log;

public class WebGetImage extends AsyncTask<Void, Void, Void>{
    
	public static final String TAG_FB = "xfacebook";
	String strUserID;
	Context m_context;
	final boolean bDebug = false;

	public WebGetImage(String strID, Context context)
	{
		strUserID = strID;
		m_context = context;
	}

	@Override
	protected Void doInBackground(Void... params) {
		// TODO Auto-generated method stub
			//사진의 url을 만듦 : 페이스북링크 + 유저 페이지 ID + 사진 타입
			//타입은 square, small, normal, large가 있으며 square가 디폴트인듯
			// 
			String uri1="http://graph.facebook.com/";
			String uri2="/picture?type=normal";
			String uri = uri1 + strUserID + uri2;
			if( bDebug )
				Log.d(TAG_FB, "WebGetImage:facebook: Request profile img:" + uri );
			//리다이렉트 되는 리소스의 실제 uri을 얻어옴
			AndroidHttpClient client = AndroidHttpClient.newInstance(uri);
		try {
			final HttpGet getRequest = new HttpGet(uri);
			HttpResponse response = client.execute(getRequest);
			Header redirect = response.getFirstHeader("Location");
			if( redirect == null ) {
				((XeActivity)m_context).onRecvProfileImage( strUserID, null, 0, 0, 0);
				client.close();
				return null;
			}
			//웹사이트에 접속 (사진이 있는 주소로 접근)
			if( bDebug )
				Log.d(TAG_FB, "new URL(redirect.getValue())");
			URL Url = new URL(redirect.getValue());
			// 웹사이트에 접속 설정
			if( bDebug )
				Log.d(TAG_FB, "Url.openConnection()");
			URLConnection urlcon = Url.openConnection();
			// 연결하시오
			if( bDebug )
				Log.d(TAG_FB, "urlcon.connect();");
			urlcon.connect();
			// 이미지 길이 불러옴
			int imagelength = urlcon.getContentLength();
			if( bDebug )
				Log.d(TAG_FB, "WebGetImage:facebook: Received profile img: lenImg=" + imagelength );
			// 스트림 클래스를 이용하여 이미지를 불러옴
			BufferedInputStream bis = new BufferedInputStream(urlcon.getInputStream(), imagelength);
			// 스트림에 정보를 이용해 비트맵을 만들고
			Bitmap _bit = BitmapFactory.decodeStream(bis);
			int ow = _bit.getWidth();
			float ratio = 200 / (float)ow;
			int sh = (int)(_bit.getHeight() * ratio);
			Bitmap bit = Bitmap.createScaledBitmap(_bit, 200, sh, true );
			bis.close();
			int w = bit.getWidth();
			int h = bit.getHeight();
			if( h > 200 )
				h = 200;
			int[] pixels = new int[w * h];
			bit.getPixels(pixels, 0, w, 0, 0, w, h );
			client.close();
//			ByteArrayOutputStream stream = new ByteArrayOutputStream() ;  
//			bit.compress( CompressFormat.JPEG, 100, stream) ;  
//			byte[] byteArray = stream.toByteArray() ;
			
//			Log.d("xuzhu", "" + byteArray.length);
//			Log.d("xuzhu", "end");
			if( bDebug )
				Log.d(TAG_FB, "WebGetImage:profile img size:(" + w + "x" + h );
			int bpp = 4;
			((XeActivity)m_context).onRecvProfileImage( strUserID, pixels, w, h, bpp);
			//바이너리 어레이와 어레이 길이를 전달함
//			((XeActivity)m_context).onRecvProfileImage(byteArray, byteArray.length);
		} catch (Exception e) {
			client.close();
			Log.e(TAG_FB, "WebGetImage:exception" );
			e.printStackTrace();
		}
		return null;
	}
}