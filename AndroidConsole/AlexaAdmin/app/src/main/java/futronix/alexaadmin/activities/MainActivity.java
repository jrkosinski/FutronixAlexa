package futronix.alexaadmin.activities;

import android.app.ProgressDialog;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ProgressBar;

import futronix.alexaadmin.api.ApiStatus;
import futronix.alexaadmin.Global;
import futronix.alexaadmin.R;
import futronix.alexaadmin.callbacks.GetStatusCallback;

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

        Global.ApiInterface.getStatusAsync(new GetStatusCallback() {
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
