package futronix.alexaadmin.activities;

import android.os.Bundle;
import android.widget.ProgressBar;

import java.util.Hashtable;

import futronix.alexaadmin.api.ApiStatus;
import futronix.alexaadmin.Global;
import futronix.alexaadmin.R;
import futronix.alexaadmin.callbacks.ApiStatusCallback;
import futronix.alexaadmin.presenters.MainPresenter;
import futronix.alexaadmin.presenters.MainPresenterListener;
import futronix.alexaadmin.presenters.Screen;
import futronix.alexaadmin.storage.LocalStorage;
import futronix.alexaadmin.tasks.UdpBroadcastTask;

public class MainActivity extends FutronixActivity
{
    private final MainPresenter presenter = new MainPresenter();

    @Override
    protected int getLayoutResId() { return R.layout.activity_main;}

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        this.presenter.setListener(new PresenterListener());
    }

    @Override
    protected void onStart()
    {
        super.onStart();
        this.presenter.start();
    }


    private final class PresenterListener extends MainPresenterListener
    {
        @Override
        public void requestViewState(Hashtable<String, Object> values) {
            values.put("context", getApplicationContext());
        }

        @Override
        public void showErrorMessage(String message) {
        }

        @Override
        public void redirect(Screen screen) {
            navigateTo(screen);
        }

        @Override
        public void showProgress(boolean show)
        {
            if (show == true)
                showSpinner();
            else
                dismissSpinner();
        }
    }
}
