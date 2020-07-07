import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;

class MessageHandling {



    static String getMessage(Socket socket) throws IOException {

        int length = 0;
        String message = "";

        DataInputStream bytes = new DataInputStream(socket.getInputStream());
        byte[] buffer = new byte[2];
        bytes.readFully(buffer);
//        System.out.println(buffer[0]);
//        System.out.println(buffer[1]);

        length = (buffer[1] & 0b01111111);
        int numberOfBytesLength = 1;

        if(length == 127){
            numberOfBytesLength = 8;
        }
        else if(length == 126){
            numberOfBytesLength = 2;
        }
        long messageLength;

        if(numberOfBytesLength == 2){
            messageLength = bytes.readShort();
        }else if(numberOfBytesLength == 8){
            messageLength = bytes.readLong();
        }else{
            messageLength = length;
        }

//        System.out.println(length);

        byte[] maskingKey = new byte[4];
        bytes.readFully(maskingKey);

        byte[] encoded = new byte[(int) messageLength];
        byte[] decoded = new byte[(int) messageLength];
        bytes.readFully(encoded);

        for (var i = 0; i < encoded.length; i++) {
            decoded[i] = (byte)(encoded[i] ^ maskingKey[i % 4]);
        }
        String msg = new String(decoded);
        System.out.println(msg);
        return msg;
    }

    static void sendMessage(String message, Socket socket) throws IOException {

        DataOutputStream dataOut = new DataOutputStream(socket.getOutputStream());
        byte[] messageArray = message.getBytes();
        int messageByteLength = messageArray.length;

        dataOut.write((byte)-127);

        if(messageByteLength <= 125){
            dataOut.write((byte)messageByteLength);
            dataOut.write(messageArray);

        }else if(messageByteLength <65535){
            dataOut.write((byte)126);
            dataOut.writeShort(messageByteLength);
            dataOut.write(messageArray);
        }

        else{
            dataOut.write((byte)127);
            dataOut.writeLong(messageByteLength);
            dataOut.write(messageArray);
        }
    }


}
