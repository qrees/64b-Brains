package info.qrees.android.brains;

import java.io.IOException;
import java.io.InputStream;

import android.content.res.AssetManager;

// Wrapper for native library

public class GL2JNILib {

     static {
    	 //System.loadLibrary("stlport_shared");
         System.loadLibrary("gl2jni");
     }
     static final int GL_FRAGMENT_SHADER = 0x8B30;
     static final int GL_VERTEX_SHADER   = 0x8B31;
     public static AssetManager assets;
     
     public static String loadAsset(String source){
    	 try {
			return readFile(source);
		} catch (IOException e) {
			Log.e("Failed to load asset %s", e, source);
			return "";
		}
     };

     public static String readFile(String name) throws IOException{
     	StringBuffer sbuffer = new StringBuffer();
 		InputStream stream = assets.open(name);
 		byte[] buffer = new byte[1024];
 		int n;
 		while ((n = stream.read(buffer, 0, 1024)) != -1)
 			sbuffer.append(new String(buffer, 0, n));
 		return sbuffer.toString();
     }
     
     public static native void init(int width, int height);
     public static native void step();
     public static native int loadShader(String source, int type);
     public static native int createProgram(int vertexShader, int fragmentShader);
}
