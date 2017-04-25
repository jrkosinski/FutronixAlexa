package futronix.alexaadmin.presenters;

import java.util.Hashtable;

import futronix.alexaadmin.Global;
import futronix.alexaadmin.api.ApiStatus;
import futronix.alexaadmin.callbacks.ApiStatusCallback;
import futronix.alexaadmin.callbacks.GetSceneCallback;

/**
 * Created by Home on 25/4/2560.
 */
public class ScenesPresenter extends PresenterBase
{
    public void setScene()
    {
        this.requestViewState();
        final int scene = this.getFormValueInt("scene", 0);
        listener.showProgress(true);

        Global.apiInterface.setSceneAsync(scene, new ApiStatusCallback()
        {
            @Override
            public void execute(ApiStatus status) {
                listener.showProgress(false);

                switch (status) {
                    case NotFound:
                        listener.redirect(Screen.Discover);
                        break;
                    case NotSetUp:
                        listener.redirect(Screen.Setup);
                        break;
                    case Running:
                        ((ScenesPresenterListener)listener).onSetSceneSuccess(scene);
                        break;
                }
            }
        });
    }

    @Override
    public void start()
    {
        Global.apiInterface.getSceneAsync(new GetSceneCallback()
        {
            @Override
            public void execute(ApiStatus status) {
                this.execute(status, 0);
            }

            @Override
            public void execute(ApiStatus status, int scene) {
                listener.showProgress(false);

                switch (status) {
                    case NotFound:
                        listener.redirect(Screen.Discover);
                        break;
                    case NotSetUp:
                        listener.redirect(Screen.Setup);
                        break;
                    case Running:
                        ((ScenesPresenterListener) listener).showScene(scene);
                        break;
                }
            }
        });
    }
}
