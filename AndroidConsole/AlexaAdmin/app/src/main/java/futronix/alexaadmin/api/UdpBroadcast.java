package futronix.alexaadmin.api;

import android.content.Context;
import android.net.DhcpInfo;
import android.net.wifi.WifiManager;
import android.os.StrictMode;
import android.provider.SyncStateContract;
import android.util.Log;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.Timer;
import java.util.TimerTask;

import futronix.alexaadmin.Global;

/**
 * Created by UserZ150 on 24/4/2560.
 */
public class UdpBroadcast
{
    private Context context;
    private Timer timer;
    private DatagramSocket socket;

    public UdpBroadcast(Context context)
    {
        this.context = context;
    }

    public void sendBroadcast(String message)
    {
        if (this.timer == null) {
            this.timer = new Timer();

            // Hack Prevent crash (sending should be done using an async task)
            StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
            StrictMode.setThreadPolicy(policy);

            //start timer
            this.timer.scheduleAtFixedRate(new TimerTask() {
                @Override
                public void run()
                {
                    //timeout exceeded
                    if (socket != null)
                    {
                        Log.i("UDP socket timeout", "timeout");
                        if (socket != null) {
                            socket.close();
                            socket.disconnect();
                        }
                        socket = null;
                        timer.cancel();
                    }
                }
            }, 10000, 10000);

            try {
                //Open a random port to send the package
                this.socket = socket = new DatagramSocket();
                this.socket.setBroadcast(true);
                byte[] sendData = message.getBytes();

                //TODO: make the port number a setting
                //broadcast packet to find device
                DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, getBroadcastAddress(), 2001);
                this.socket.send(sendPacket);
                Log.i(getClass().getName(), "Broadcast packet sent to: " + getBroadcastAddress().getHostAddress());

                //receive packet from server
                byte[] buffer = new byte[512];
                DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
                this.socket.receive(packet);

                //get IP and port from response
                InetAddress serverAddress = packet.getAddress();
                Log.i("Found device at", serverAddress.toString());

                //set the response data globally
                timer.cancel();
                Global.device.found(serverAddress, 80);

                if (socket != null) {
                    socket.close();
                    socket.disconnect();
                }
            } catch (IOException e) {
                //TODO: log
                Log.e("UDP Error", e.toString());
                System.out.println(e.toString());
                timer.cancel();
            }
        }
    }

    InetAddress getBroadcastAddress() throws IOException
    {
        WifiManager wifi = (WifiManager) this.context.getSystemService(Context.WIFI_SERVICE);
        DhcpInfo dhcp = wifi.getDhcpInfo();
        // handle null somehow

        int broadcast = (dhcp.ipAddress & dhcp.netmask) | ~dhcp.netmask;
        byte[] quads = new byte[4];
        for (int k = 0; k < 4; k++)
            quads[k] = (byte) ((broadcast >> k * 8) & 0xFF);

        InetAddress addr = InetAddress.getByAddress(quads);
        Log.i("Broadcasting on", addr.toString());
        return addr;
    }
}
