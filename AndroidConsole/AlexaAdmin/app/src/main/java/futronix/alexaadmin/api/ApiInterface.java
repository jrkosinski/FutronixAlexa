package futronix.alexaadmin.api;

import java.io.*;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

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
            OkHttpClient client = new OkHttpClient();

            Request request = new Request.Builder()
                    .url("http://192.168.l.34/admin/status")
                    .build();

            Response response = client.newCall(request).execute();
            switch (response.code())
            {
                case 200:
                    return ApiStatus.Running;
                case 404:
                    return ApiStatus.NotSetUp;
                default:
                    return ApiStatus.NotFound;
            }
        }
        catch (Exception e)
        {
            //TODO: log error
        }

        return ApiStatus.NotFound;
    }

    public void getStatusAsync(final ApiStatusCallback callback)
    {
        try {
            OkHttpClient client = new OkHttpClient();

            Request request = new Request.Builder()
                    .url("http://192.168.l.34/admin/status")
                    .build();

            client.newCall(request).enqueue(new Callback() {
                @Override
                public void onFailure(Call call, IOException e) {
                    e.printStackTrace();
                    if (callback != null)
                        callback.execute(ApiStatus.NotFound);
                }

                @Override
                public void onResponse(Call call, Response response) throws IOException {
                    if (!response.isSuccessful())
                    {
                        if (callback != null)
                            callback.execute(ApiStatus.NotFound);
                    }

                    switch (response.code())
                    {
                        case 200:
                            if (callback != null)
                                callback.execute(ApiStatus.Running);
                            break;
                        case 404:
                            if (callback != null)
                                callback.execute(ApiStatus.NotSetUp);
                            break;
                        default:
                            if (callback != null)
                                callback.execute(ApiStatus.NotFound);
                            break;
                    }
                }
            });
        }
        catch (Exception e)
        {
            //TODO: log error
        }
    }

    public void setupAsync(String wifiSsid, String wifiPasswd, final ApiStatusCallback callback)
    {
        try {
            OkHttpClient client = new OkHttpClient();

            Request request = new Request.Builder()
                    .url("http://192.168.l.34/admin/setup?wifiSsid=" + wifiSsid + "&wifiPasswd=" + wifiPasswd)
                    .build();

            client.newCall(request).enqueue(new Callback() {
                @Override
                public void onFailure(Call call, IOException e) {
                    e.printStackTrace();
                    if (callback != null)
                        callback.execute(ApiStatus.NotFound);
                }

                @Override
                public void onResponse(Call call, Response response) throws IOException {
                    if (!response.isSuccessful())
                    {
                        if (callback != null)
                            callback.execute(ApiStatus.NotFound);
                    }

                    switch (response.code())
                    {
                        case 200:
                            if (callback != null)
                                callback.execute(ApiStatus.Running);
                            break;
                        case 404:
                            if (callback != null)
                                callback.execute(ApiStatus.NotSetUp);
                            break;
                        default:
                            if (callback != null)
                                callback.execute(ApiStatus.NotFound);
                            break;
                    }
                }
            });
        }
        catch (Exception e)
        {
            //TODO: log error
        }
    }

    public void findDeviceAsync(ApiStatusCallback callback)
    {
        try {
            String messageStr = "Hello Android!";
            int server_port = 1001;
            DatagramSocket s = new DatagramSocket();
            InetAddress local = InetAddress.getByName("192.168.1.102");
            int msg_length = messageStr.length();
            byte[] message = messageStr.getBytes();
            DatagramPacket p = new DatagramPacket(message, msg_length, local, server_port);
            s.send(p);
        }
        catch (Exception e)
        {
            //TODO: log
        }
    }
}
