/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.gemtree2.caribe;

import com.google.android.gms.gcm.GoogleCloudMessaging;

import android.app.IntentService;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.SystemClock;
import android.support.v4.app.NotificationCompat;
import android.util.Log;

import com.gemtree2.caribe.R;

/**
 * 이 IntentService는 GCM 메시지의 실제 처리를 수행합니다. 서비스가 작업을 수행하는 동안 GcmBroadcastReceiver
 * (WakefulBroadcastReceiver)이 서비스에 대한 부분 웨이크 잠금을 보유하고있다. 서비스가 완료하면, 웨이크 잠금을 해제
 * completeWakefulIntent() 부른다.
 * 
 * This {@code IntentService} does the actual handling of the GCM message.
 * {@code GcmBroadcastReceiver} (a {@code WakefulBroadcastReceiver}) holds a
 * partial wake lock for this service while the service does its work. When the
 * service is finished, it calls {@code completeWakefulIntent()} to release the
 * wake lock.
 */

public class GcmIntentService extends IntentService {
	public static final int NOTIFICATION_ID = 1;
	private NotificationManager mNotificationManager;
	NotificationCompat.Builder builder;

	public GcmIntentService() {
		super("GcmIntentService");
	}

	public static final String TAG = "Caribe GCM";

	@Override
	protected void onHandleIntent(Intent intent) {
		Bundle extras = intent.getExtras();
		GoogleCloudMessaging gcm = GoogleCloudMessaging.getInstance(this);

		// getMessageType() 의도 매개 변수를 사용하여 브로드 캐스트 리시버에서 수신 된 의도를해야합니다.
		// The getMessageType() intent parameter must be the intent you received
		// in your BroadcastReceiver.
		String messageType = gcm.getMessageType(intent);
		Log.i(TAG, "Received:=================================================== ");

		// unparcelling 번들의 효과가. has effect of unparcelling Bundle
		if (!extras.isEmpty()) {
			/*
			 * 메시지 유형에 따라 메시지를 필터링 할 수 있습니다. 그것은 GCM은 당신이에 관심이있어 어떤 메시지 유형을
			 * 무시하고, 새로운 메시지 유형과 향후 확장, 또는 당신이 인식하지 않는 될 가능성이 높습니다 때문에.
			 * 
			 * Filter messages based on message type. Since it is likely that
			 * GCM will be extended in the future with new message types, just
			 * ignore any message types you're not interested in, or that you
			 * don't recognize.
			 */
			if (GoogleCloudMessaging.MESSAGE_TYPE_SEND_ERROR
					.equals(messageType)) {
				sendNotification("Send error: " + extras.toString());
			} else if (GoogleCloudMessaging.MESSAGE_TYPE_DELETED
					.equals(messageType)) {
				sendNotification("Deleted messages on server: "
						+ extras.toString());
				// 정규 GCM 메시지의 경우, 몇 가지 작업을 수행.
				// If it's a regular GCM message, do some work.
			} else if (GoogleCloudMessaging.MESSAGE_TYPE_MESSAGE
					.equals(messageType)) {
				// 이 루프는 몇 가지 작업을 수행하는 서비스를 나타냅니다.
				// This loop represents the service doing some work.
				/*
				for (int i = 0; i < 5; i++) {
					Log.i(TAG,
							"Working... " + (i + 1) + "/5 @ "
									+ SystemClock.elapsedRealtime());
					try {
						Thread.sleep(5000);
					} catch (InterruptedException e) {
					}
				}*/
				//Log.i(TAG, "Completed work @ " + SystemClock.elapsedRealtime());
				// 수신 된 메시지에 대한 알림을 게시합니다.
//				Log.i(TAG, "Received2: " + extras.toString());
				// Post notification of received message.
//				sendNotification("Received: " + extras.toString());
				sendNotification(extras.getString("msg"));	// "msg" : "정복자들/특성연구가 완료됨"
//				Log.i(TAG, "Received: " + extras.toString());
			}
		}
		// WakefulBroadcastReceiver에서 제공하는 웨이크 잠금을 해제합니다.
		// Release the wake lock provided by the WakefulBroadcastReceiver.
		GcmBroadcastReceiver.completeWakefulIntent(intent);
	}

	// 알림에 메시지를 넣어 게시 할 수 있습니다. 이것은 당신이 GCM 메시지와 함께 수행하도록 선택할 수 있습니다
	// 무엇을 하나의 간단한 예입니다.
	// Put the message into a notification and post it.
	// This is just one simple example of what you might choose to do with
	// a GCM message.
	private void sendNotification(String msg) {
		mNotificationManager = (NotificationManager) this
				.getSystemService(Context.NOTIFICATION_SERVICE);

		PendingIntent contentIntent = PendingIntent.getActivity(this, 0,
				new Intent(this, MyMainActivity.class), 0);
		
		String [] strArr = msg.split("/");	// "msg" : "정복자들/특성연구가 완료됨"에서 /단위로 제목과 본문을 분리함.
		NotificationCompat.Builder mBuilder = new NotificationCompat.Builder(
				this).setSmallIcon(R.drawable.ic_launcher)
				.setContentTitle(strArr[0])
//				.setContentTitle("GCM Notification")
				.setStyle(new NotificationCompat.BigTextStyle().bigText(strArr[1]))
//				.setStyle(new NotificationCompat.BigTextStyle().bigText(msg))
				.setVibrate(new long[] { 100, 100, 100, 100})
				.setContentText(strArr[1]);
/*		NotificationCompat.Builder mBuilder =
                new NotificationCompat.Builder(this)
                        .setSmallIcon(R.drawable.ic_launcher)
                        .setContentTitle("GCM Notification")
                        .setStyle(new NotificationCompat.BigTextStyle()
                                .bigText(msg))
                        .setContentText(msg); */
		
		mBuilder.setContentIntent(contentIntent);
		mNotificationManager.notify(NOTIFICATION_ID, mBuilder.build());
	}
}
