/*
 * Copyright 2013 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.mtricks.caribe;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.support.v4.content.WakefulBroadcastReceiver;

/**
 * 이 WakefulBroadcastReceiver는 앱에 대한 부분 웨이크 잠금을 생성 및 관리를 처리합니다. 장치 전환에 잠에 가지 않는
 * 것을 보장하면서, IntentService에 GCM 메시지를 처리하는 작업을 전달합니다. 그것은 웨이크 잠금을 해제 할 준비가되면
 * IntentService는 GcmBroadcastReceiver.completeWakefulIntent()를 호출합니다.
 * 
 * This {@code WakefulBroadcastReceiver} takes care of creating and managing a
 * partial wake lock for your app. It passes off the work of processing the GCM
 * message to an {@code IntentService}, while ensuring that the device does not
 * go back to sleep in the transition. The {@code IntentService} calls
 * {@code GcmBroadcastReceiver.completeWakefulIntent()} when it is ready to
 * release the wake lock.
 */

public class GcmBroadcastReceiver extends WakefulBroadcastReceiver {
	
	static boolean isAppOn = false;		// 테스트를 위해 우선 false

	@Override
	public void onReceive(Context context, Intent intent) {
		if(!isAppOn)
		{
			// 명시 적으로 GcmIntentService는 의도를 처리하도록 지정합니다.
			// Explicitly specify that GcmIntentService will handle the intent.
			ComponentName comp = new ComponentName(context.getPackageName(),
					GcmIntentService.class.getName());
			// 이 실행되는 동안 깨어있는 장치를 유지하는 서비스를 시작합니다.
			// Start the service, keeping the device awake while it is launching.
			startWakefulService(context, (intent.setComponent(comp)));
			setResultCode(Activity.RESULT_OK);
		}
	}
}
