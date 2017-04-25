package futronix.alexaadmin.model;

import java.net.InetAddress;

import futronix.alexaadmin.api.ApiStatus;

/**
 * Created by UserZ150 on 24/4/2560.
 */
public class Device
{
    public java.net.InetAddress ipAddress = null;
    public int tcpPort = 0;
    public ApiStatus status = ApiStatus.Unknown;

    public void clear()
    {
        this.ipAddress = null;
        this.tcpPort = 0;
        this.status = ApiStatus.NotFound;
    }

    public void found(InetAddress ip, int port)
    {
        this.ipAddress = ip;
        this.tcpPort = port;
        this.status = ApiStatus.Unknown;
    }
}
