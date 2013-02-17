package info.plocharz.android.kaleidoscope;

import java.io.File;
import java.util.ArrayList;

import android.app.Activity;
import android.content.res.AssetManager;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Environment;
import android.text.TextUtils;
import android.view.KeyEvent;
import android.view.Window;
import android.view.WindowManager;


public class Kaleidoscope extends Activity implements SensorEventListener {
    private SensorManager mSensorManager;
    private Sensor mAccelerometer;
	private GLSurfaceView mGLSurfaceView;
	AssetManager assets;
	
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        initSensor();
        createStorageDirectory();
        
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.setRequestedOrientation(1);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, 
                                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        //mGLSurfaceView = new TouchView(this, new ViewRenderer(getAssets()));
        mGLSurfaceView = new GL2JNIView(getApplication(), assets);
        mGLSurfaceView.setRenderMode(GL2JNIView.RENDERMODE_CONTINUOUSLY);
        setContentView(mGLSurfaceView);
        
        assets = getAssets();
        GL2JNILib.assets = assets;
        GL2JNILib.onCreate();
    }
    
    private void initSensor() {
        mSensorManager = (SensorManager)getSystemService(SENSOR_SERVICE);
        mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
    }
    
	private void createStorageDirectory() {
        boolean mExternalStorageWriteable = false;
        String state = Environment.getExternalStorageState();
        
    	if (Environment.MEDIA_MOUNTED.equals(state)) {
            mExternalStorageWriteable = true;
        } else if (Environment.MEDIA_MOUNTED_READ_ONLY.equals(state)) {
            mExternalStorageWriteable = false;
        } else {
            mExternalStorageWriteable = false;
        }
        if(mExternalStorageWriteable){
        	Log.d(Environment.getExternalStorageDirectory().getAbsolutePath());
        	File dir = new File(Environment.getExternalStorageDirectory(), "64b");
        	if(dir.mkdirs())
        		Log.d("Created storage directory");
        	else
        		Log.d("failed to create storage directory");
        	if(!dir.exists())
        		Log.w("Data directory does not exist");
        }else{
        	Log.d("Storage is not writable");
        }
	}
	
	@Override
    protected void onResume() {
        super.onResume();
        mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_GAME);
        mGLSurfaceView.onResume();
        GL2JNILib.onResume();
    }

    @Override
    protected void onPause() {
    	Log.d("onPause");
        GL2JNILib.onPause();
        mSensorManager.unregisterListener(this);
        mGLSurfaceView.onPause();
        super.onPause();
    }
    
    @Override
    protected void onDestroy() {
    	Log.d("onDestroy");
        GL2JNILib.onDestroy();
        super.onDestroy();
    }
    
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if ((keyCode == KeyEvent.KEYCODE_BACK)) {
            finish();
        }
        return super.onKeyDown(keyCode, event);
    }
    
    
	public void onAccuracyChanged(Sensor arg0, int arg1) {
		// TODO Auto-generated method stub
		
	}

	public void onSensorChanged(SensorEvent event) {
		ArrayList<String> values = new ArrayList<String>();
		for(int i = 0; i < event.values.length; i++){
			values.add(String.valueOf(event.values[i]));
		}
		//Log.d(TextUtils.join(", ", values.toArray()));
		if(event.values.length > 2)
			GL2JNILib.sensorevent(event.values[0], event.values[1], event.values[2]);
		else
			Log.w("Incorrect sensor values length, expected at least 3");
	}
}