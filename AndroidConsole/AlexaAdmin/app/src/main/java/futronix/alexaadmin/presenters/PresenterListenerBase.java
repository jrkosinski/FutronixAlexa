package futronix.alexaadmin.presenters;

import java.util.Hashtable;

/**
 * Created by Home on 25/4/2560.
 */
public abstract class PresenterListenerBase
{
    public void showErrorMessage(String message) {}
    public void requestViewState(Hashtable<String, Object> values) {}
    public void showProgress(boolean show) {}
    public void redirect(Screen screen){}
}
