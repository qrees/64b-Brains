package info.plocharz.android.kaleidoscope;

public class Application extends android.app.Application {

	@Override
	public void onCreate() {
		super.onCreate();
        GL2JNILib.onApplicationCreate();
	}

	@Override
	public void onTerminate() {
		super.onTerminate();
        GL2JNILib.onApplicationTerminate();
	}
}
