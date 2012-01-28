package info.qrees.android.brains;

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
