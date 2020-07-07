import javax.crypto.BadPaddingException;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.Mac;
import javax.crypto.NoSuchPaddingException;
import java.io.*;
import java.math.BigInteger;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.charset.StandardCharsets;
import java.security.*;
import java.security.cert.Certificate;
import java.security.cert.CertificateException;
import java.security.spec.InvalidKeySpecException;
import java.util.ArrayList;
import java.util.Arrays;


public class Server extends TLSBase {

    ServerSocket serverSocket;
    byte[] nonce;

    private Server() throws IOException {
        serverSocket = new ServerSocket(8080);
        nonce = new byte[32];
    }


    public static void main(String[] args) throws CertificateException, IOException, NoSuchAlgorithmException, SignatureException, InvalidKeySpecException, InvalidKeyException, ClassNotFoundException, NoSuchPaddingException, BadPaddingException, InvalidAlgorithmParameterException, IllegalBlockSizeException {

        Server server = new Server();

        //create certificate
        Certificate certificateServer = getCertificate("CASignedServerCertificate.pem");
        //TA server public key

        BigInteger SA = generateSaOrSb();
        BigInteger TA = generateDHPublicKey(SA);
        PrivateKey privateKey = generatePrivKey("serverPrivateKey.der");
        byte[] signedPublicKey = generateSignature(TA.toByteArray(), privateKey);

        Socket connection = server.serverSocket.accept();
        System.out.println("Connected");
        System.out.println("----------------------------------------------------------------");


        ObjectOutputStream oServerOut = new ObjectOutputStream(connection.getOutputStream());
        ObjectInputStream oServerIn = new ObjectInputStream(connection.getInputStream());


        server.nonce = (byte[]) oServerIn.readObject();
        System.out.println("Client Nonce received.");
        System.out.println("----------------------------------------------------------------");
        server.history.writeBytes(server.nonce);

        //send first message from server to client
        oServerOut.writeObject(certificateServer);
        server.history.writeBytes(certificateServer.getEncoded());
        oServerOut.writeObject(TA);
        server.history.writeBytes(TA.toByteArray());
        oServerOut.writeObject(signedPublicKey);
        server.history.writeBytes(signedPublicKey);


        Certificate clientCert = (Certificate) oServerIn.readObject();
        server.history.writeBytes(clientCert.getEncoded());
        BigInteger TB = (BigInteger) oServerIn.readObject();
        server.history.writeBytes(TB.toByteArray());
        byte[] clientSignedPublicKey = (byte[]) oServerIn.readObject();
        server.history.writeBytes(clientSignedPublicKey);
        System.out.println("Client Certificate, DiffieHellman public key, Signed DiffieHellman public key received.");
        System.out.println("----------------------------------------------------------------");
        boolean clientCertified = checkKey(clientCert, TB.toByteArray(), clientSignedPublicKey);
        System.out.print("Client Certied? ");
        System.out.println(clientCertified);
        System.out.println("----------------------------------------------------------------");

        Certificate CACheck = getCertificate("CAcertificate.pem");
        try {
            clientCert.verify(CACheck.getPublicKey());
        } catch (Exception e) {
            e.printStackTrace();
            System.exit(0);
        }

        //create private keys
        BigInteger secretKey = generateDHPrivateKey(TB, SA, getN());
        System.out.println("DH keys created.");
        System.out.println("----------------------------------------------------------------");
        server.makeSecretKeys(server.nonce, secretKey.toByteArray());

        System.out.println("Secret keys created.");
        System.out.println("----------------------------------------------------------------");


        //server sends first mac
        Mac serverMac1 = createMac(server.serverMAC);
        byte[] serverMacArray = serverMac1.doFinal(server.history.toByteArray());
        oServerOut.writeObject(serverMacArray);
//                server.history.writeBytes(serverMacArray);

        byte[] clientMacArray = (byte[]) oServerIn.readObject();

        if (!Arrays.equals(clientMacArray, serverMacArray)) {
            System.out.println("Macs are not equal.");
            PrintStream err = System.err;
        } else {
            System.out.println("Macs are the same.");
        }
        System.out.println("----------------------------------------------------------------");
        File file = new File("The Odyssey.txt");
        //init array with file length
        byte[] messageArray = new byte[(int) file.length()];

        FileInputStream fis = new FileInputStream(file);
        fis.read(messageArray); //read file into bytes[]
        fis.close();
        System.out.println(messageArray.length);


//                byte[] decryptedBytes = Message.decrypt(messageArray,server.clientEncrypt,server.clientIV,server.clientMAC);

        ArrayList<byte[]> encryptedBytes = Message.formatMessage(messageArray, server.serverMAC, server.serverEncrypt, server.serverIV);
        ByteArrayOutputStream test = new ByteArrayOutputStream();

        for (byte[] eb : encryptedBytes) {
            oServerOut.writeObject(eb);
            test.writeBytes(eb);
        }

        byte[] decryptedMessage = (byte[]) oServerIn.readObject();
        if (!Arrays.equals(decryptedMessage, messageArray)) {
            System.out.println("Messages do not match");
        } else {
            System.out.println("Messages match");
        }
        System.out.println("----------------------------------------------------------------");
        byte[] currentArray = (byte[]) oServerIn.readObject();
        ByteArrayOutputStream bIn = new ByteArrayOutputStream();
        while (currentArray.length != 0) {
            bIn.writeBytes(Message.decrypt(currentArray, server.clientEncrypt, server.clientIV, server.clientMAC));
            currentArray = (byte[]) oServerIn.readObject();
        }
        String str = new String(bIn.toByteArray(), StandardCharsets.UTF_8);
        System.out.println("Confirmation message from client: " + str);
    }
}
