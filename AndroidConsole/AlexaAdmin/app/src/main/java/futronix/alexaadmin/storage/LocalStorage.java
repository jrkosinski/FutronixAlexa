package futronix.alexaadmin.storage;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.util.Log;

import java.net.InetAddress;
import java.net.UnknownHostException;

import futronix.alexaadmin.Global;
import futronix.alexaadmin.model.Device;


/**
 * Created by UserZ150 on 25/4/2560.
 */
public class LocalStorage
{
    private static SharedPreferences sharedPreferences;
    private static SharedPreferences.Editor editor;

    private static final String IpAddressKey = "username";
    private static final String PortNumberKey = "Password";

    private static String ipAddress;
    private static int portNumber;

    public static String getIpAddress()
    {
        if (sharedPreferences != null) {
            if (ipAddress == null)
                ipAddress = sharedPreferences.getString(IpAddressKey, "");
        }

        return ipAddress;
    }

    public static void setIpAddress(String value)
    {
        ipAddress = value;
        if (editor != null) {
            editor.putString(IpAddressKey, value);
            editor.commit();
        }
    }

    public static void setIpAddress(InetAddress value)
    {
        if (value != null)
            setIpAddress(value.toString());
        else
            setIpAddress("");
    }

    public static int getPortNumber()
    {
        if (sharedPreferences != null)
        {
            if (portNumber == 0)
            {
                String value = sharedPreferences.getString(PortNumberKey, "");
                if (value != null && value.length() > 0)
                    portNumber = Integer.parseInt(value);
            }
        }

        return portNumber;
    }

    public static void setPortNumber(int value)
    {
        portNumber = value;
        if (editor != null)
        {
            editor.putString(PortNumberKey, Integer.toString(value));
            editor.commit();
        }
    }

    /*
    Clears the database.
     */
    public static void clear()
    {
        setIpAddress("");
        setPortNumber(0);
    }

    public static void restoreDevice(Device device)
    {
        String ip = getIpAddress();
        if (ip != null && ip.length() > 0) {
            try {
                device.ipAddress = InetAddress.getByName(ip);
            }
            catch (UnknownHostException e){}
        }

        device.tcpPort = getPortNumber();
        Log.d("Restoring device", Global.device.toString());
    }

    public static void saveDevice(Device device)
    {
        setIpAddress(device.ipAddress);
        setPortNumber(device.tcpPort);
        Log.d("Saving device", Global.device.toString());
    }

    public static void initialize(Context context)
    {
        sharedPreferences = context.getSharedPreferences("FutronixStorage", Activity.MODE_PRIVATE);
        editor = sharedPreferences.edit();
    }
}
