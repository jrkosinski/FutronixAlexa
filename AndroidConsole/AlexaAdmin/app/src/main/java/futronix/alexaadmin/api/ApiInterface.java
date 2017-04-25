package futronix.alexaadmin.api;

import android.util.Log;

import java.io.*;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

import futronix.alexaadmin.Global;
import futronix.alexaadmin.callbacks.ApiStatusCallback;
import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;

/**
 * Created by UserZ150 on 23/4/2560.
 */
public class ApiInterface implements  IApiInterface
{
    public ApiStatus getStatus()
    {
        try {
            Global.device.status = ApiStatus.Unknown;
            OkHttpClient client = new OkHttpClient();

            Request request = new Request.Builder()
                    .url(this.buildUrl("status"))
                    .build();

            Response response = client.newCall(request).execute();
            switch (response.code())
            {
                case 200:
                    Global.device.status = ApiStatus.Running;
                    break;
                case 404:
                    Global.device.status = ApiStatus.NotSetUp;
                    break;
                default:
                    Global.device.status = ApiStatus.NotFound;
                    break;
            }

            return Global.device.status;
        }
        catch (Exception e)
        {
            //TODO: log error
        }

        Global.device.status = ApiStatus.NotFound;
        return ApiStatus.NotFound;
    }

    public void getStatusAsync(final ApiStatusCallback callback)
    {
        try {
            Global.device.status = ApiStatus.Unknown;
            OkHttpClient client = new OkHttpClient();

            Request request = new Request.Builder()
                    .url(this.buildUrl("status"))
                    .build();

            client.newCall(request).enqueue(new Callback() {
                @Override
                public void onFailure(Call call, IOException e)
                {
                    e.printStackTrace();
                    Global.device.status = ApiStatus.NotFound;

                    if (callback != null)
                        callback.execute(Global.device.status);
                }

                @Override
                public void onResponse(Call call, Response response) throws IOException
                {
                    if (!response.isSuccessful())
                    {
                        Global.device.status = ApiStatus.NotFound;
                    }
                    else {
                        switch (response.code()) {
                            case 200:
                                Global.device.status = ApiStatus.Running;
                                break;
                            case 404:
                                Global.device.status = ApiStatus.NotSetUp;
                                break;
                            default:
                                Global.device.status = ApiStatus.NotFound;
                                break;
                        }
                    }

                    if (callback != null)
                        callback.execute(Global.device.status);
                }
            });
        }
        catch (Exception e)
        {
            //TODO: log error
            Log.e(e.getMessage(), e.toString());
        }
    }

    public void setupAsync(String wifiSsid, String wifiPasswd, final ApiStatusCallback callback)
    {
        try {
            OkHttpClient client = new OkHttpClient();

            Request request = new Request.Builder()
                    .url(this.buildUrl("setup?wifiSsid=" + wifiSsid + "&wifiPasswd=" + wifiPasswd))
                    .build();

            client.newCall(request).enqueue(new Callback() {
                @Override
                public void onFailure(Call call, IOException e) {
                    e.printStackTrace();
                    if (callback != null)
                        callback.execute(ApiStatus.NotFound);
                }

                @Override
                public void onResponse(Call call, Response response) throws IOException
                {
                    if (!response.isSuccessful())
                    {
                        Global.device.status = ApiStatus.NotFound;
                    }
                    else {
                        switch (response.code()) {
                            case 200:
                                Global.device.status = ApiStatus.Running;
                                break;
                            case 404:
                                Global.device.status = ApiStatus.NotSetUp;
                                break;
                            default:
                                Global.device.status = ApiStatus.NotFound;
                                break;
                        }
                    }

                    if (callback != null)
                        callback.execute(Global.device.status);
                }
            });
        }
        catch (Exception e)
        {
            //TODO: log error
        }
    }

    private String buildUrl(String suffix)
    {
        String output = "";
        if (Global.device.ipAddress != null) {
            output = "http://" + Global.device.ipAddress.toString() + "/admin/" + suffix;
        }

        return output;
    }

    public void setSceneAsync(int scene, final ApiStatusCallback callback)
    {
        try {
            OkHttpClient client = new OkHttpClient();

            Request request = new Request.Builder()
                    .url(this.buildUrl("setScene?scene=" + scene))
                    .build();

            client.newCall(request).enqueue(new Callback() {
                @Override
                public void onFailure(Call call, IOException e) {
                    e.printStackTrace();
                    if (callback != null)
                        callback.execute(ApiStatus.NotFound);
                }

                @Override
                public void onResponse(Call call, Response response) throws IOException
                {
                    if (!response.isSuccessful())
                    {
                        Global.device.status = ApiStatus.NotFound;
                    }
                    else {
                        switch (response.code()) {
                            case 200:
                                Global.device.status = ApiStatus.Running;
                                break;
                            case 404:
                                Global.device.status = ApiStatus.NotSetUp;
                                break;
                            default:
                                Global.device.status = ApiStatus.NotFound;
                                break;
                        }
                    }

                    if (callback != null)
                        callback.execute(Global.device.status);
                }
            });
        }
        catch (Exception e)
        {
            //TODO: log error
        }
    }
}
