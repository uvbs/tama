package com.mtricks.xe;
import android.view.MotionEvent;
import android.util.Log;
import android.util.FloatMath;

public class PinchZoom{
    private float cx, cy;
 
    // 핀치시 두좌표간의 거리 저장
    float oldDist = 1f;
    float newDist = 1f;
    float scale = 1f;
     
    // 드래그 모드인지 핀치줌 모드인지 구분
    static final int NONE = 0;
    static final int DRAG = 1;
    static final int ZOOM = 2;
 
    int mode = NONE;    
     
    public boolean IsZoom() {
    	return mode == ZOOM;
    }
    public float GetScale() {
    	return scale;
    }
    public float GetCenterX() {
    	return cx;
    }
    public float GetCenterY() {
    	return cy;
    }
    public void TouchProcess(MotionEvent event) { 
        int act = event.getAction();
         switch(act & MotionEvent.ACTION_MASK) {
            case MotionEvent.ACTION_DOWN:    //첫번째 손가락 터치
//                Log.d("zoom", "mode=DRAG" );
                mode = DRAG;                    
                break;
            case MotionEvent.ACTION_UP:    // 첫번째 손가락을 떼었을 경우
            case MotionEvent.ACTION_POINTER_UP:  // 두번째 손가락을 떼었을 경우
                mode = NONE;
                break;
            case MotionEvent.ACTION_POINTER_DOWN:  
            //두번째 손가락 터치(손가락 2개를 인식하였기 때문에 핀치 줌으로 판별)
                mode = ZOOM;
                cx = (event.getX(0) + event.getX(1)) / 2f;
                cy = (event.getY(0) + event.getY(1)) / 2f;
                newDist = spacing(event);
                oldDist = spacing(event);
//                Log.d("zoom", "newDist=" + newDist);
//                Log.d("zoom", "oldDist=" + oldDist);
//               Log.d("zoom", "mode=ZOOM");
             
                break;

            case MotionEvent.ACTION_MOVE: 
                if(mode == DRAG) {   // 드래그 중이면, 이미지의 X,Y값을 변환시키면서 위치 이동.
                } else if (mode == ZOOM) {    // 핀치줌 중이면, 이미지의 거리를 계산해서 확대를 한다.
                    newDist = spacing(event);
                    cx = (event.getX(0) + event.getX(1)) / 2f;
                    cy = (event.getY(0) + event.getY(1)) / 2f;
                	scale = newDist / oldDist;
                    oldDist = newDist;
//                    Log.d("zoom", "scale=" + scale );
                }
                break;
 
            case MotionEvent.ACTION_CANCEL:
            default : 
                break;
        }
 
    }
    //Rect 형태로 넘겨준다.
     private float spacing(MotionEvent event) {
        float x = event.getX(0) - event.getX(1);
        float y = event.getY(0) - event.getY(1);
        return FloatMath.sqrt(x * x + y * y);
 
    }
}
