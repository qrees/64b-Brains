package info.plocharz.android.kaleidoscope;

public class Log {
    private static final String TAG = "Brains";
	
	static public void d(String msg, Object ... args){
		String formatted = String.format(msg, args);
		android.util.Log.d(TAG, formatted);
	}
	static public void w(String msg, Object ... args){
		String formatted = String.format(msg, args);
		android.util.Log.w(TAG, formatted);
	}
	static public void e(String msg, Throwable tr, Object ... args){
		String formatted = String.format(msg, args);
		android.util.Log.e(TAG, formatted, tr);
	}
}
