import java.io.IOException;
import java.net.Socket;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Base64;
import java.util.Scanner;

public class httpRequest {
    String filename;
    String swk;
    String encodedRequest;
    boolean isWebSocket = false;

   public httpRequest(Socket socket) throws IOException, NoSuchAlgorithmException {
        Scanner scnr = new Scanner(socket.getInputStream());
        while(true) {
            String line = scnr.nextLine();
            String[] lineArray = line.split(" ");


            if (lineArray[0].equals("GET")) {
                filename = lineArray[1];
            }
            if (lineArray[0].equals("Sec-WebSocket-Key:")) {
                swk = lineArray[1];
                encodedRequest = handshakeResponse(swk);
                isWebSocket = true;
            }
            if (line.equals("")) {
                break;
            }

        }

    }


    public static String handshakeResponse(String swk) throws NoSuchAlgorithmException {

        String magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        String key = swk + magic;
        MessageDigest md = MessageDigest.getInstance("SHA-1");
        md.update(key.getBytes());
        byte[] keyByte = md.digest();
        String ret = Base64.getEncoder().encodeToString(keyByte);
        return ret;
    }


    public String getFilename(){
        return filename;
    }

    public void setFileName(String input){
       filename = input;
    }

    public String getSwk(){
        return swk;
    }

    public String getEncodedRequest() {
        return encodedRequest;
    }

    public Boolean isWebSocket(){
       return isWebSocket;
    }



}