package futronix.alexaadmin.activities;

import android.os.Bundle;
import android.widget.ProgressBar;

import futronix.alexaadmin.api.ApiStatus;
import futronix.alexaadmin.Global;
import futronix.alexaadmin.R;
import futronix.alexaadmin.callbacks.ApiStatusCallback;

public class MainActivity extends FutronixActivity
{
    private ProgressBar progressSpinner;

    @Override
    protected int getLayoutResId() { return R.layout.activity_main;}

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    protected void onStart()
    {
        super.onStart();
    }

    protected void onResume()
    {
        super.onResume();
        this.showSpinner();

        Global.apiInterface.getStatusAsync(new ApiStatusCallback() {
            @Override
            public void execute(ApiStatus status) {
                dismissSpinner();

                switch (status) {
                    case NotFound:
                        directToDiscover();
                        break;
                    case NotSetUp:
                        directToSetup();
                        break;
                    case Running:
                        directToScenes();
                        break;
                }
            }
        });
    }
}
