import java.awt.image.DataBufferInt;
import java.io.*;
import java.net.Inet4Address;
import java.net.Socket;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Base64;
import java.util.Date;
import java.util.HashMap;

public class httpResponse {
    public static void response(Socket socket, httpRequest newRequest) throws IOException, InterruptedException {
        PrintWriter pw = new PrintWriter(socket.getOutputStream());
        //File file = new File("resources" + newRequest.getFilename());
        if (newRequest.getFilename().equals("/")) {
            //return index.html through pw writer
            newRequest.setFileName("/webchatentry.html");
        }

        newRequest.setFileName("resources" + newRequest.getFilename());
        File file = new File(newRequest.getFilename());

        if (file.exists()) {
            headerParam(socket, pw, newRequest);
        } else {
            newRequest.setFileName("resources/404.html");
            headerParam(socket, pw, newRequest);
        }

        pw.flush();
    }

    private static void headerParam(Socket socket, PrintWriter pw, httpRequest newRequest) throws IOException {
        File file = new File(newRequest.getFilename());
        FileInputStream is = new FileInputStream(file);

        if (newRequest.isWebSocket()) {
            String httpResponse = "HTTP/1.1 101 Switching Protocols\r\n" + "Upgrade: websocket\r\n" + "Connection: Upgrade\r\n"
                    + "Sec-WebSocket-Accept: " + newRequest.getEncodedRequest() + "\r\n" + "\r\n";
            pw.write(httpResponse);
            pw.flush();



        } else {
            Date today = new Date();
            String date = "Date: " + today + "\r\n\r\n";
            String contLength = "Content-Length: " + file.length() + "\r\n";
            String httpResponse = "HTTP/1.1 200 OK\r\n" + contLength + date;
            pw.write(httpResponse);
            pw.flush();
            is.transferTo(socket.getOutputStream());
        }


    }



}
