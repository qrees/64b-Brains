package info.qrees.android.brains;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

// Wrapper for native library

public class GL2JNILib {

    static {
        System.loadLibrary("gl2jni");
    }
    static final int GL_FRAGMENT_SHADER = 0x8B30;
    static final int GL_VERTEX_SHADER = 0x8B31;
    public static AssetManager assets;

    public static byte[] loadBitmap(String source) {
        /*
         * Loads png file. File is decoded into bitmap and array of pixels is returned.
         */
        try {
            return readBitmap(source);
        } catch (IOException e) {
            Log.e("Failed to load asset %s", e, source);
            return null;
        } catch (Exception e) {
            Log.e("Failed to load asset %s", e, source);
            return null;
        }
    };

    public static byte[] loadRaw(String source) {
        InputStream stream;
        try {
            stream = assets.open(source);
        } catch (IOException e) {
            Log.e("Failed to load asset %s", e, source);
            return null;
        }
        ByteArrayOutputStream arr = new ByteArrayOutputStream();
        try {
            byte[] buffer = new byte[1024];
            int read;
            while((read = stream.read(buffer, 0, 1024)) > 0){
                arr.write(buffer, 0, read);
            }
        } catch (IOException e) {
            Log.e("Failed to load asset %s", e, source);
            return null;
        }
        Log.d("Loaded raw file %s",  source);
        return arr.toByteArray();
    };

    public static String loadAsset(String source) {
        try {
            return readFile(source);
        } catch (IOException e) {
            Log.e("Failed to load asset %s", e, source);
            return "";
        }
    };
    
    public static final int HEADER_SIZE = 4*2+4;
    
    public static byte[] readBitmap(String name) throws IOException {
        InputStream stream = assets.open(name);
        Bitmap bitmap = BitmapFactory.decodeStream(stream);
        Bitmap.Config config = bitmap.getConfig();
        short mult;
        switch (config) {
        case ALPHA_8:
            Log.d("Loaded 8 bit image ALPHA_8");
            mult = 1;
            break;
        case ARGB_4444:
            Log.d("Loaded 16 bit image ARGB_4444");
            mult = 2;
            break;
        case ARGB_8888:
            Log.d("Loaded 32 bit image ARGB_8888");
            mult = 4;
            break;
        case RGB_565:
            Log.d("Loaded 16 bit image RGB_565");
            mult = 2;
            break;
        default:
            Log.d("Error: unknown file format");
            return null;
        }
        Log.d("img %dx%d", bitmap.getWidth(), bitmap.getHeight());
        
        ByteBuffer dst = ByteBuffer.allocate(bitmap.getWidth()
                * bitmap.getHeight() * mult);
        ByteBuffer dst2 = ByteBuffer.allocate(bitmap.getWidth()
                * bitmap.getHeight() * mult + HEADER_SIZE);
        dst.order(ByteOrder.nativeOrder());
        dst2.order(ByteOrder.nativeOrder());
        dst2.putInt(bitmap.getWidth());
        dst2.putInt(bitmap.getHeight());
        dst2.putInt(config.ordinal());
        bitmap.copyPixelsToBuffer(dst);
        dst.position(0);
        dst2.put(dst);
        Log.d("img data size %d", dst.capacity());
        stream.close();
        return dst2.array();
    }

    public static String readFile(String name) throws IOException {
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

    public static native void touch(int x, int y);
    
    public static native void motionevent(int x, int y, int action);

    public static native void runEventLoop();

    public static native void onPause();

    public static native void onResume();
}
