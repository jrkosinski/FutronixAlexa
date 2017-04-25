package futronix.alexaadmin.presenters;

import futronix.alexaadmin.Global;

/**
 * Created by Home on 25/4/2560.
 */
public class FindDevicesPresenter extends PresenterBase
{
    public void findDevices()
    {
        Global.device.clear();
        this.listener.redirect(Screen.Main);
    }
}
