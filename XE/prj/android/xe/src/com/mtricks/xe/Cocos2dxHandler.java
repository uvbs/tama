/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 ****************************************************************************/

package com.mtricks.xe;

import com.mtricks.xe.XeActivity;
//import com.mtrics.epic.thl.R;

import java.lang.ref.WeakReference;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

public class Cocos2dxHandler extends Handler {
	// ===========================================================
	// Constants
	// ===========================================================
	public final static int HANDLER_SHOW_DIALOG = 1;
	public final static int HANDLER_SHOW_EDITBOX_DIALOG = 2;
	public final static int HANDLER_IAP_CONSUME = 3;
	public final static int HANDLER_IAP_QUERY_INVENTORY = 4;
	public final static int HANDLER_SOFTNYX_LOGIN = 5;
	public final static int HANDLER_SOFTNYX_GETUSERINFO = 6;
	public final static int HANDLER_SOFTNYX_BUYITEM = 7;
	public final static int HANDLER_SOFTNYX_LOGOUT = 8;

	// ===========================================================
	// Fields
	// ===========================================================
	private WeakReference<XeActivity> mActivity;

	// ===========================================================
	// Constructors
	// ===========================================================
	public Cocos2dxHandler(XeActivity activity) {
		this.mActivity = new WeakReference<XeActivity>(activity);
	}

	// ===========================================================
	// Getter & Setter
	// ===========================================================

	// ===========================================================
	// Methods for/from SuperClass/Interfaces
	// ===========================================================

	// ===========================================================
	// Methods
	// ===========================================================

	public void handleMessage(Message msg) {
		// 이곳은 메인 스레드임.
		switch (msg.what) {
		case Cocos2dxHandler.HANDLER_SHOW_DIALOG:
			showDialog(msg);
			break;
		case Cocos2dxHandler.HANDLER_SHOW_EDITBOX_DIALOG:
			showEditBoxDialog(msg);
			break;
		case Cocos2dxHandler.HANDLER_IAP_CONSUME:
			IAPConsume(msg);
			break;
		case Cocos2dxHandler.HANDLER_IAP_QUERY_INVENTORY:
			IAPQueryInventory(msg);
			break;
		case Cocos2dxHandler.HANDLER_SOFTNYX_LOGIN:
			SoftnyxLogin(msg);
			break;
		case Cocos2dxHandler.HANDLER_SOFTNYX_LOGOUT:
			SoftnyxLogout(msg);
			break;
		case Cocos2dxHandler.HANDLER_SOFTNYX_BUYITEM:
			SoftnyxBuyItem(msg);
			break;
		}
	}

	private void showDialog(Message msg) {
		XeActivity theActivity = this.mActivity.get();
		DialogMessage dialogMessage = (DialogMessage) msg.obj;
		Log.e("xuzhu", "Handler:showDialog: " + dialogMessage.title + ", " + dialogMessage.message );
		new AlertDialog.Builder(theActivity).setTitle(dialogMessage.title)
				.setMessage(dialogMessage.message)
				.setPositiveButton("Ok", new DialogInterface.OnClickListener() {

					@Override
					public void onClick(DialogInterface dialog, int which) {
						// TODO Auto-generated method stub

					}
				}).create().show();
	}

	private void showEditBoxDialog(Message msg) {
		EditBoxMessage editBoxMessage = (EditBoxMessage) msg.obj;
		new Cocos2dxEditBoxDialog(this.mActivity.get(), editBoxMessage.title,
				editBoxMessage.content, editBoxMessage.inputMode,
				editBoxMessage.inputFlag, editBoxMessage.returnType,
				editBoxMessage.maxLength).show();
	}

	private void IAPConsume( Message msg ) {
		Log.d("xuzhu iap", "handlerMessage: IAPConsume" );
		XeActivity theActivity = this.mActivity.get();
		IAPConsumeMessag iapConsume = (IAPConsumeMessag) msg.obj;
		theActivity.DoConsumeItemHandler( iapConsume.idsProduct );
	}
	private void IAPQueryInventory( Message msg ) {
		Log.d("xuzhu iap", "handlerMessage: IAPQueryInventory" );
		XeActivity theActivity = this.mActivity.get();
		theActivity.DoQueryInventory();
	}
	private void SoftnyxLogin( Message msg ) {
		Log.d("softnyx", "handlerMessage: SoftnyxLogin" );
		XeActivity theActivity = this.mActivity.get();
		theActivity.PGMAN_Login();
	}
	private void SoftnyxLogout( Message msg ) {
		Log.d("softnyx", "handlerMessage: SoftnyxLogout" );
		XeActivity theActivity = this.mActivity.get();
		theActivity.PGMAN_Logout();
	}
	private void SoftnyxBuyItem( Message msg ) {
		Log.d("softnyx", "handlerMessage: SoftnyxBuyItem" );
		XeActivity theActivity = this.mActivity.get();
		xSoftnyxBuyItem buyParam = (xSoftnyxBuyItem) msg.obj;
		theActivity.PGMAN_BuyItem( buyParam.idsProduct
								, buyParam.price
								, buyParam.strPayload );
	}
	// ===========================================================
	// Inner and Anonymous Classes
	// ===========================================================

	public static class DialogMessage {
		public String title;
		public String message;

		public DialogMessage(String title, String message) {
			this.title = title;
			this.message = message;
		}
	}

	public static class EditBoxMessage {
		public String title;
		public String content;
		public int inputMode;
		public int inputFlag;
		public int returnType;
		public int maxLength;

		public EditBoxMessage(String title, String content, int inputMode,
				int inputFlag, int returnType, int maxLength) {
			this.content = content;
			this.title = title;
			this.inputMode = inputMode;
			this.inputFlag = inputFlag;
			this.returnType = returnType;
			this.maxLength = maxLength;
		}
	}
	public static class IAPConsumeMessag {
		public String idsProduct; 
		public IAPConsumeMessag( final String idsProduct ) {
			this.idsProduct = idsProduct;
		}
	}
	public static class xSoftnyxBuyItem {
		public String idsProduct; 
		public String strPayload;
		public int price;
	}
}
