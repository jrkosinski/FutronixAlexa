package futronix.alexaadmin.presenters;

import java.util.Hashtable;

/**
 * Created by Home on 25/4/2560.
 */
public abstract class PresenterBase
{
    protected Hashtable<String, Object> formValues = new Hashtable<>();
    protected PresenterListenerBase listener;

    public void setListener(PresenterListenerBase listener)
    {
        this.listener = listener;
    }

    public void start() {

    }

    protected void requestViewState()
    {
        this.formValues.clear();
        this.listener.requestViewState(this.formValues);
    }

    protected Object getFormValue(String key)
    {
        Object out = null;
        if (this.formValues.contains(key))
        {
            out = this.formValues.get(key);
        }

        return out;
    }

    protected String getFormValueString(String key)
    {
        String out = null;
        Object o = this.getFormValue(key);
        if (o != null)
            out = o.toString();

        return out;
    }

    protected int getFormValueInt(String key, int defaultValue)
    {
        String s = this.getFormValueString(key);
        if (s != null && s.length() > 0)
        {
            return Integer.parseInt(s);
        }

        return defaultValue;
    }
}
