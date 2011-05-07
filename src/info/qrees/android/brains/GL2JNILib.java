package info.qrees.android.brains;

// Wrapper for native library

public class GL2JNILib {

     static {
    	 //System.loadLibrary("stlport_shared");
         System.loadLibrary("gl2jni");
     }
     static final int GL_FRAGMENT_SHADER = 0x8B30;
     static final int GL_VERTEX_SHADER   = 0x8B31;

     public static native void init(int width, int height);
     public static native void step();
     public static native int loadShader(String source, int type);
     public static native int createProgram(int vertexShader, int fragmentShader);
}
