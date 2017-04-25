package futronix.alexaadmin.presenters;

import futronix.alexaadmin.Global;
import futronix.alexaadmin.api.ApiStatus;
import futronix.alexaadmin.callbacks.ApiStatusCallback;

/**
 * Created by Home on 25/4/2560.
 */
public class SetupPresenter extends PresenterBase
{
    public void doSetup() {
        this.requestViewState();

        Global.apiInterface.setupAsync(
                this.getFormValueString("ssid"),
                this.getFormValueString("passwd"),
                new ApiStatusCallback() {
                    @Override
                    public void execute(ApiStatus status) {
                        switch (status) {
                            case NotFound:
                                listener.redirect(Screen.Discover);
                                break;
                            case NotSetUp:
                                break;
                            case Running:
                                listener.redirect(Screen.Scenes);
                                break;
                        }
                    }
                });
    }
}
