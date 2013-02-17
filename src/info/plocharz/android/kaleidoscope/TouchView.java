package info.plocharz.android.kaleidoscope;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;

public class TouchView extends GLSurfaceView{
	private final boolean DEBUG = false;
	
    public TouchView(Context context, ViewRenderer renderer) {
        super(context);
        _renderer = renderer;
        setRenderer(renderer);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }
    
    public boolean onTouchEvent (MotionEvent event){
    	this.requestRender();
    	if(DEBUG){
	    	for(int i = 0; i < event.getHistorySize(); i++){
	    		Log.d(" ---- ");
	    		Log.d("Hitory %d:", i);
				Log.d(" - time: %d", event.getHistoricalEventTime(i));
	    		Log.d(" - press: %f", event.getPressure(i));
	    		Log.d(" - size: %f", event.getSize(i));
	    		Log.d(" - x: %f", event.getX(i));
	    		Log.d(" - y: %f", event.getY(i));
	    		for(int j = 0; j < event.getPointerCount(); j++){
	        		Log.d(" - Pointer %d:", j);
		    		Log.d(" - - press: %f", event.getHistoricalPressure(j, i));
		    		Log.d(" - - size: %f", event.getHistoricalSize(j, i));
		    		Log.d(" - - x: %f", event.getHistoricalX(j, i));
		    		Log.d(" - - y: %f", event.getHistoricalY(j, i));
		    	}
	    	}
			Log.d(" ---- ");
	    	Log.d("press: %f", event.getPressure());
	    	Log.d("x: %f", event.getX());
	    	Log.d("y: %f", event.getY());
			Log.d("raw x: %f", event.getRawX());
			Log.d("raw y: %f", event.getRawY());
			Log.d("size: %f", event.getSize());
			Log.d("x prec: %f", event.getXPrecision());
			Log.d("y prec: %f", event.getYPrecision());
    	}
		_renderer.click(Math.round(event.getX()), Math.round(event.getY()));
    	return true;
    }
    
    private ViewRenderer _renderer;
}
