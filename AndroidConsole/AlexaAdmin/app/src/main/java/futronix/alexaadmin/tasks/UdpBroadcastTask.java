package futronix.alexaadmin.tasks;

import android.content.Context;
import android.os.AsyncTask;

import futronix.alexaadmin.Global;
import futronix.alexaadmin.api.ApiStatus;
import futronix.alexaadmin.api.UdpBroadcast;

/**
 * Created by Home on 25/4/2560.
 */
public class UdpBroadcastTask extends AsyncTask<Context, Object, ApiStatus>
{
    protected ApiStatus doInBackground(Context... args)
    {
        UdpBroadcast udp = new UdpBroadcast(args[0]);
        udp.sendBroadcast("Futronix:012345678998765432102017:Futronix" );

        return Global.device.status;
    }

    protected void onProgressUpdate(Object... progress)
    {
    }

    protected void onPostExecute(ApiStatus result)
    {
    }
}
