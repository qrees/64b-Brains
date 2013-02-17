package info.plocharz.android.kaleidoscope;

import java.util.Vector;
import javax.microedition.khronos.opengles.GL10;

public class Group extends Mesh {
	
	private final Vector<Mesh> mChildren = new Vector<Mesh>();
	
    @Override
    public void draw(GL10 gl) {
    	gl.glMatrixMode(GL10.GL_MODELVIEW);
    	gl.glPushMatrix();
		gl.glTranslatef(_x, _y, _z);
		gl.glRotatef(_rx, 1, 0, 0);
		gl.glRotatef(_ry, 0, 1, 0);
		gl.glRotatef(_rz, 0, 0, 1);
		gl.glScalef(_sx, _sy, _sz);
        int size = mChildren.size();
        for (int i = 0; i < size; i++)
                mChildren.get(i).draw(gl);
    	gl.glMatrixMode(GL10.GL_MODELVIEW);
    	gl.glPopMatrix();
    }
    
    public boolean add(Mesh object) {
            return mChildren.add(object);
    }

    public void clear() {
            mChildren.clear();
    }

	public void click(float clickX, float clickY) {
		float destX = clickX - _x;
		float destY = clickY - _x;
        int size = mChildren.size();
        for (int i = 0; i < size; i++)
        	;
	}

}
