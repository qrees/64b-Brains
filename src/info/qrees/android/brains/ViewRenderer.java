package info.qrees.android.brains;

import java.io.IOException;
import java.io.InputStream;
import java.nio.IntBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.opengles.GL11;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLSurfaceView;
import android.opengl.GLU;

@SuppressWarnings("unused")
public class ViewRenderer implements GLSurfaceView.Renderer {
	Square mSquare;
	Group root = new Group();
	LabelMaker mLabels;
	private AssetManager _assets;
	private int rawWidth, rawHeight;
	
	public void onDrawFrame(GL10 gl) {
		Log.d("redrawing frame");
		//if (gl instanceof GL11) {
			GL11 gl11 = (GL11) gl;
			drawGL11(gl11);
		//}
	}
	
	private void drawGL11(GL11 gl){
        gl.glClear(GL11.GL_COLOR_BUFFER_BIT);
		gl.glLoadIdentity();
        //gl.glTranslatef(0, 0, -4); 
        root.draw(gl);
	}
	
	public ViewRenderer(AssetManager assets){
		mSquare = new Square();
		root.add(mSquare);
		_assets = assets;
		loadBitmaps();
	}
	
    private void loadBitmaps() {
		InputStream background_file;
		try {
			background_file = _assets.open("images/background.png");
			Bitmap background = BitmapFactory.decodeStream(background_file);
			background = Bitmap.createScaledBitmap(background, 256, 512, false);
			
			Log.d("Bitmap: %dx%d", background.getWidth(), background.getHeight());
			mSquare.loadBitmap(background);
		} catch (IOException e) {
			Log.e("Failed to load assets", e);
		}
	}

	public void onSurfaceChanged(GL10 gl, int width, int height) {
    	if (gl instanceof GL11) {
			GL11 gl11 = (GL11) gl;
			rawWidth = width;
			rawHeight = height;
			gl11.glViewport(0, 0, width, height);
			float ratio = (float)height/(float)width;
			Log.d("Surface Changed: %dx%d -> %f", width, height, ratio);
			mSquare.setSize(1, ratio);
			// Select the projection matrix
			gl11.glMatrixMode(GL10.GL_PROJECTION);
			gl11.glLoadIdentity();
			gl11.glOrthof(0, 1, ratio, 0, -1, 1);
			gl11.glMatrixMode(GL10.GL_MODELVIEW);
			gl11.glLoadIdentity();
		}
    }

	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		mLabels = new LabelMaker(true, 256, 64);
		if (gl instanceof GL11) {
			GL11 gl11 = (GL11)gl;
			gl11.glDisable(GL11.GL_DITHER);
			gl11.glDisable(GL11.GL_DEPTH_TEST);
			gl11.glDisable(GL11.GL_DEPTH_BUFFER_BIT);
			gl11.glEnable(GL10.GL_TEXTURE_2D);
			IntBuffer buf = IntBuffer.allocate(1);
			gl11.glGetIntegerv(GL11.GL_MAX_TEXTURE_SIZE, buf);
			Log.d("Max texture size %d", buf.get(0));
		}
	}
	
	public void click(int x, int y){
		float dest_y = (float)(rawHeight-y)/(float)(rawHeight);
		float dest_x = (float)x/(float)(rawWidth);
		Log.d("Click: %f, %f", dest_x, dest_y);
		root.click(dest_x, dest_y);
	}
}
