package futronix.alexaadmin;

import java.net.InetAddress;
import java.net.UnknownHostException;

import futronix.alexaadmin.api.*;
import futronix.alexaadmin.api.testing.DummyApiInterface;
import futronix.alexaadmin.model.Device;

/**
 * Created by Home on 24/4/2560.
 */
public final class Global
{
    public final static IApiInterface apiInterface = new DummyApiInterface();
    public final static Device device = new Device();

    public static void initializeDevice(String ip, int port)
    {
        try {
            if (ip != null) {
                device.ipAddress = InetAddress.getByName(ip);
            }
        }
        catch (UnknownHostException e){}

        device.tcpPort = port;
        device.status = ApiStatus.Unknown;
    }
}


