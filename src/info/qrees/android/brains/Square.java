
package info.qrees.android.brains;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import javax.microedition.khronos.opengles.GL10;

import android.util.Log;

@SuppressWarnings("unused")
public class Square extends Mesh {
    // The order we like to connect them.
	protected float mVertices[] = { 0.0f, 1.0f, 0.0f, // 0, Top Left
			0.0f, 0.0f, 0.0f, // 1, Bottom Left
			1.0f, 0.0f, 0.0f, // 2, Bottom Right
			1.0f, 1.0f, 0.0f, // 3, Top Right
	};
    private short[] mIndices = { 0, 1, 2, 0, 2, 3 };
	private float textureCoordinates[] = { 
			0.0f, 1.0f, //
            0.0f, 0.0f, //
            1.0f, 0.0f, //
            1.0f, 1.0f, //
	};

    public Square() {
        this.setVertices(mVertices);
        this.setIndices(mIndices);
    	setTextureCoordinates(textureCoordinates);
    }
    
}
