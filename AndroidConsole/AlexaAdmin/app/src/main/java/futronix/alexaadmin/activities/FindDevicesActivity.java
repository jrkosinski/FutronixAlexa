package futronix.alexaadmin.activities;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import java.util.Hashtable;

import futronix.alexaadmin.Global;
import futronix.alexaadmin.R;
import futronix.alexaadmin.presenters.FindDevicesPresenter;
import futronix.alexaadmin.presenters.FindDevicesPresenterListener;
import futronix.alexaadmin.presenters.Screen;

public class FindDevicesActivity extends FutronixActivity
{
    private final FindDevicesPresenter presenter = new FindDevicesPresenter();
    private Button findButton;

    @Override
    protected int getLayoutResId() { return R.layout.activity_find_devices;}

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        this.findButton = (Button)findViewById(R.id.findButton);

        this.presenter.setListener(new PresenterListener());

        this.findButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(final View v) {
                presenter.findDevices();
            }
        });
    }


    private final class PresenterListener extends FindDevicesPresenterListener
    {
        @Override
        public void requestViewState(Hashtable<String, Object> values)
        {
        }

        @Override
        public void showErrorMessage(String message)
        {
        }

        @Override
        public void redirect(Screen screen)
        {
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
