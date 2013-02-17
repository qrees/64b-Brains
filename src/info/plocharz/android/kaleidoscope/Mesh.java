package info.plocharz.android.kaleidoscope;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

import javax.microedition.khronos.opengles.GL10;

import android.graphics.Bitmap;
import android.opengl.GLUtils;

public class Mesh {

	float textureCoordinates[] = { 0.0f, 1.0f, //
		1.0f, 1.0f, //
		0.0f, 0.0f, //
		1.0f, 0.0f //
	};
	protected FloatBuffer mVertexBuffer;
	protected ShortBuffer mIndexBuffer;
	protected FloatBuffer mColorBuffer;
	protected float _x=0, _y=0, _z=0;
	protected float _rx=0, _ry=0, _rz=0;
	protected float _sx=1, _sy=1, _sz=1;
	private final float[] mRGBA = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };
	protected int mNumOfIndices;
	protected FloatBuffer mTextureCoords;
	protected Texture mTexture = new Texture();

	public Mesh() {
		super();
	}
    /**
     * Render the mesh.
     * 
     * @param gl
     *            the OpenGL context to render to.
     */
    public void draw(GL10 gl) {
		gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);

		gl.glVertexPointer(3, GL10.GL_FLOAT, 0, mVertexBuffer);

		gl.glColor4f(mRGBA[0], mRGBA[1], mRGBA[2], mRGBA[3]);

		if (mColorBuffer != null) {
			// Enable the color array buffer to be used during rendering.
			gl.glEnableClientState(GL10.GL_COLOR_ARRAY);
			gl.glColorPointer(4, GL10.GL_FLOAT, 0, mColorBuffer);
		}

		loadGLTexture(gl);
		int texId = mTexture.getId();
		if (texId != -1 && mTextureCoords != null) {
			gl.glEnable(GL10.GL_TEXTURE_2D);
			// Enable the texture state
			gl.glEnableClientState(GL10.GL_TEXTURE_COORD_ARRAY);

			// Point to our buffers
			gl.glTexCoordPointer(2, GL10.GL_FLOAT, 0, mTextureCoords);
			gl.glBindTexture(GL10.GL_TEXTURE_2D, texId);
		}
		gl.glMatrixMode(GL10.GL_MODELVIEW);
		gl.glPushMatrix();
		gl.glTranslatef(_x, _y, _z);
		gl.glRotatef(_rx, 1, 0, 0);
		gl.glRotatef(_ry, 0, 1, 0);
		gl.glRotatef(_rz, 0, 0, 1);
		gl.glScalef(_sx, _sy, _sz);

		// Point out the where the color buffer is.
		gl.glDrawElements(GL10.GL_TRIANGLES, mNumOfIndices,
				GL10.GL_UNSIGNED_SHORT, mIndexBuffer);
		// gl.glDisableClientState(GL10.GL_VERTEX_ARRAY);
		if (texId != -1 && mTextureCoords != null) {
			// gl.glDisableClientState(GL10.GL_TEXTURE_COORD_ARRAY);
		}
		gl.glPopMatrix();
    }


	public void setSize(float sx, float sy) {
		_sx = sx;
		_sy = sy;
	}

	public void setColor(float[] color) {
		if(mColorBuffer == null){
	    	ByteBuffer cbb = ByteBuffer.allocateDirect(mNumOfIndices *4 * 4);
	    	cbb.order(ByteOrder.nativeOrder());
	    	mColorBuffer = cbb.asFloatBuffer();
		}
		mColorBuffer.put(color);
    	mColorBuffer.position(0);
	}

	protected void setColor(int vertex, float[] color) {
		if(mColorBuffer == null){
	    	ByteBuffer cbb = ByteBuffer.allocateDirect(mNumOfIndices *4 * 4);
	    	cbb.order(ByteOrder.nativeOrder());
	    	mColorBuffer = cbb.asFloatBuffer();
		}
		mColorBuffer.position(vertex * 4);
		mColorBuffer.put(color);
    	mColorBuffer.position(0);
	}

	protected void setVertices(float[] vertices) {
		ByteBuffer vbb = ByteBuffer.allocateDirect(vertices.length * 4);
		vbb.order(ByteOrder.nativeOrder());
		mVertexBuffer = vbb.asFloatBuffer();
		mVertexBuffer.put(vertices);
		mVertexBuffer.position(0);
	}

	protected void setIndices(short[] indices) {
		ByteBuffer ibb = ByteBuffer.allocateDirect(indices.length * 2);
		ibb.order(ByteOrder.nativeOrder());
		mIndexBuffer = ibb.asShortBuffer();
		mIndexBuffer.put(indices);
		mIndexBuffer.position(0);
		mNumOfIndices = indices.length;
	}

	protected void setTextureCoordinates(float[] textureCoords) {
		ByteBuffer byteBuf = ByteBuffer
				.allocateDirect(textureCoords.length * 4);
		byteBuf.order(ByteOrder.nativeOrder());
		mTextureCoords = byteBuf.asFloatBuffer();
		mTextureCoords.put(textureCoords);
		mTextureCoords.position(0);
	}

    /**
     * Set the bitmap to load into a texture.
     * 
     * @param bitmap
     */
    public void loadBitmap(Bitmap bitmap) {
    	mTexture.loadBitmap(bitmap);
    }

    /**
     * Loads the texture.
     * 
     * @param gl
     */
    private void loadGLTexture(GL10 gl) { // New function
    	mTexture.loadGLTexture(gl);
    }
    
    
}