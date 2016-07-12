package com.mtricks.xe;
import android.view.MotionEvent;
import android.util.Log;
import android.util.FloatMath;

public class PinchZoom{
    private float cx, cy;
 
    // ��ġ�� ����ǥ���� �Ÿ� ����
    float oldDist = 1f;
    float newDist = 1f;
    float scale = 1f;
     
    // �巡�� ������� ��ġ�� ������� ����
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
            case MotionEvent.ACTION_DOWN:    //ù��° �հ��� ��ġ
//                Log.d("zoom", "mode=DRAG" );
                mode = DRAG;                    
                break;
            case MotionEvent.ACTION_UP:    // ù��° �հ����� ������ ���
            case MotionEvent.ACTION_POINTER_UP:  // �ι�° �հ����� ������ ���
                mode = NONE;
                break;
            case MotionEvent.ACTION_POINTER_DOWN:  
            //�ι�° �հ��� ��ġ(�հ��� 2���� �ν��Ͽ��� ������ ��ġ ������ �Ǻ�)
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
                if(mode == DRAG) {   // �巡�� ���̸�, �̹����� X,Y���� ��ȯ��Ű�鼭 ��ġ �̵�.
                } else if (mode == ZOOM) {    // ��ġ�� ���̸�, �̹����� �Ÿ��� ����ؼ� Ȯ�븦 �Ѵ�.
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
    //Rect ���·� �Ѱ��ش�.
     private float spacing(MotionEvent event) {
        float x = event.getX(0) - event.getX(1);
        float y = event.getY(0) - event.getY(1);
        return FloatMath.sqrt(x * x + y * y);
 
    }
}
