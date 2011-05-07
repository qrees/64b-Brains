package info.qrees.android.brains;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import info.qrees.android.brains.GL2JNILib;

import android.opengl.GLSurfaceView.Renderer;

public class JNIRenderer implements Renderer {

	public void onDrawFrame(GL10 gl) {
        GL2JNILib.step();
    }

    public void onSurfaceChanged(GL10 gl, int width, int height) {
        GL2JNILib.init(width, height);
    }

    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        // Do nothing.
    }
}
