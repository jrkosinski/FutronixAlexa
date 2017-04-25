package futronix.alexaadmin.activities;

import android.os.Bundle;
import android.widget.ProgressBar;

import futronix.alexaadmin.api.ApiStatus;
import futronix.alexaadmin.Global;
import futronix.alexaadmin.R;
import futronix.alexaadmin.callbacks.ApiStatusCallback;
import futronix.alexaadmin.storage.LocalStorage;
import futronix.alexaadmin.tasks.UdpBroadcastTask;

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

        //if we have a saved IP, attempt to get status of that...
        if (Global.device.ipAddress != null)
        {
            this.getApiStatus();
        }
        else
        {
            new FindDevicesTask().execute(this.getApplicationContext());
        }
    }

    private final void getApiStatus()
    {
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


    private final class FindDevicesTask extends UdpBroadcastTask
    {
        @Override
        protected void onPostExecute(ApiStatus result)
        {
            switch(result)
            {
                case NotFound:
                    directToDiscover();
                    break;
                default:
                    LocalStorage.saveDevice(Global.device);
                    getApiStatus();
                    break;
            }
        }
    }
}
