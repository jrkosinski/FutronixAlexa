package futronix.alexaadmin.api;

import futronix.alexaadmin.callbacks.ApiStatusCallback;
import futronix.alexaadmin.callbacks.GetSceneCallback;

/**
 * Created by Home on 24/4/2560.
 */
public interface IApiInterface {
    public ApiStatus getStatus();
    public void getStatusAsync(final ApiStatusCallback callback);
    public void setupAsync(String wifiSsid, String wifiPasswd, final ApiStatusCallback callback);
    public void setSceneAsync(int scene, final ApiStatusCallback callback);
    public void getSceneAsync(final GetSceneCallback callback);
}
