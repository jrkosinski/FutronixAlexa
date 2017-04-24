package futronix.alexaadmin.api;

import futronix.alexaadmin.callbacks.GetStatusCallback;

/**
 * Created by Home on 24/4/2560.
 */
public interface IApiInterface {
    public ApiStatus getStatus();
    public void getStatusAsync(final GetStatusCallback callback);
    public void setupAsync(String wifiSsid, String wifiPasswd, final GetStatusCallback callback);
}
