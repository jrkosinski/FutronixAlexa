package futronix.alexaadmin.api.testing;

import futronix.alexaadmin.api.*;
import futronix.alexaadmin.callbacks.ApiStatusCallback;

/**
 * Created by UserZ150 on 23/4/2560.
 */
public class DummyApiInterface implements IApiInterface
{
    public ApiStatus getStatus()
    {
        return ApiStatus.NotFound;
    }

    public void getStatusAsync(final ApiStatusCallback callback)
    {
        new TestThread(callback, ApiStatus.NotFound).start();
    }

    public void setupAsync(String wifiSsid, String wifiPasswd, final ApiStatusCallback callback)
    {
        new TestThread(callback, ApiStatus.Running).start();
    }


    public class TestThread extends Thread
    {
        public ApiStatusCallback callback;
        public ApiStatus returnValue;

        public TestThread(ApiStatusCallback callback, ApiStatus returnValue)
        {
            this.callback = callback;
            this.returnValue = returnValue;
        }

        public void run() {
            try {
                Thread.sleep(5000);
            }
            catch (InterruptedException e) {
            }

            if (callback != null)
                callback.execute(returnValue);
        }
    }
}