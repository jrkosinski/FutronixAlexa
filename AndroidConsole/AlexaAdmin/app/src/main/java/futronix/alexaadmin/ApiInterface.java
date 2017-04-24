package futronix.alexaadmin;

import java.net.URL;
import java.net.HttpURLConnection;
import java.io.*;
import android.net.http.*; 

/**
 * Created by UserZ150 on 23/4/2560.
 */
public class ApiInterface {
    public static void GetStatus() throws java.net.MalformedURLException, java.io.IOException
    {
        URL url = new URL("http://192.168.1.34/admin/status");

        HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
        try {
            InputStream in = new BufferedInputStream(urlConnection.getInputStream());
            String output = readHttpStream(in);
        } finally {
            urlConnection.disconnect();
        }
    }

    private static String readHttpStream(InputStream stream) throws java.io.IOException
    {
        BufferedReader r = new BufferedReader(new InputStreamReader(stream));
        StringBuilder output = new StringBuilder();
        String line;

        while ((line = r.readLine()) != null) {
            output.append(line).append('\n');
        }

        return output.toString();
    }
}
