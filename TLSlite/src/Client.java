import javax.crypto.BadPaddingException;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.Mac;
import javax.crypto.NoSuchPaddingException;
import java.io.*;
import java.math.BigInteger;
import java.net.Socket;
import java.security.*;
import java.security.cert.Certificate;
import java.security.cert.CertificateException;
import java.security.spec.InvalidKeySpecException;
import java.util.ArrayList;
import java.util.Arrays;

public class Client extends TLSBase {

    private Socket clientSocket;
    private byte[] clientNonce;

    private Client() throws IOException {
        clientSocket = new Socket("127.0.0.1", 8080);
        clientNonce = getNonce();
    }

    public static void main(String[] args) throws IOException, ClassNotFoundException, CertificateException, NoSuchAlgorithmException, InvalidKeyException, SignatureException, InvalidKeySpecException, NoSuchProviderException, NoSuchPaddingException, BadPaddingException, InvalidAlgorithmParameterException, IllegalBlockSizeException {

        Client client = new Client();
        ObjectOutputStream oClientOut = new ObjectOutputStream(client.clientSocket.getOutputStream());
        ObjectInputStream oClientIn = new ObjectInputStream(client.clientSocket.getInputStream());

        oClientOut.writeObject(client.clientNonce);
        client.history.writeBytes(client.clientNonce);

        Certificate recServerCert = (Certificate) oClientIn.readObject();
        client.history.writeBytes(recServerCert.getEncoded());
        BigInteger TA = (BigInteger) oClientIn.readObject();
        client.history.writeBytes(TA.toByteArray());
        byte[] recSignedPublicKey = (byte[]) oClientIn.readObject();
        client.history.writeBytes(recSignedPublicKey);

        System.out.println("Server Certificate, DiffieHellman public key, Signed DiffieHellman public key received.");
        System.out.println("----------------------------------------------------------------");

        Certificate clientCertificate = getCertificate("clientsignedCertificate.pem");
        BigInteger sb = generateSaOrSb();
        BigInteger TB = generateDHPublicKey(sb);
        PrivateKey privateKey = generatePrivKey("clientPrivateKey.der");
        byte[] signedClientPublicKey = generateSignature(TB.toByteArray(), privateKey);


        boolean serverCertified = checkKey(recServerCert, TA.toByteArray(), recSignedPublicKey);
        System.out.print("ServerCertified? ");
        System.out.println(serverCertified);
        System.out.println("----------------------------------------------------------------");
        Certificate CACheck = getCertificate("CAcertificate.pem");
        try {
            recServerCert.verify(CACheck.getPublicKey());
        } catch (Exception e) {
            e.printStackTrace();
            System.exit(0);
        }

        //send client response, 2nd message

        oClientOut.writeObject(clientCertificate);
        client.history.writeBytes(clientCertificate.getEncoded());
        oClientOut.writeObject(TB);
        client.history.writeBytes(TB.toByteArray());
        oClientOut.writeObject(signedClientPublicKey);
        client.history.writeBytes(signedClientPublicKey);

        BigInteger secretKey = generateDHPrivateKey(TA, sb, getN());
        System.out.println("DH keys created.");
        System.out.println("----------------------------------------------------------------");
        //client receives mac 1
        client.makeSecretKeys(client.clientNonce, secretKey.toByteArray());
        System.out.println("Secret keys created.");
        System.out.println("----------------------------------------------------------------");
        Mac clientMac1 = createMac(client.serverMAC);
        byte[] clientMacArray = clientMac1.doFinal(client.history.toByteArray());
        oClientOut.writeObject(clientMacArray);


        byte[] serverMacArray = (byte[]) oClientIn.readObject();
        if (!Arrays.equals(clientMacArray, serverMacArray)) {
            System.out.println("Macs are not equal.");
            System.exit(0);
        } else {
            System.out.println("Macs are the same.");
        }
        System.out.println("----------------------------------------------------------------");

//            byte[] currentArray;
        byte[] currentArray = (byte[]) oClientIn.readObject();
        ByteArrayOutputStream bIn = new ByteArrayOutputStream();
        while (currentArray.length != 0) {
            bIn.writeBytes(Message.decrypt(currentArray, client.serverEncrypt, client.serverIV, client.serverMAC));
            currentArray = (byte[]) oClientIn.readObject();
        }
//            byte[] decrypt = Message.decrypt(bIn.toByteArray(), client.serverEncrypt, client.serverIV, client.serverMAC);

        oClientOut.writeObject(bIn.toByteArray());

        File file;
        file = new File("output.txt");
        FileOutputStream fos = new FileOutputStream(file);
        if (!file.exists()) {
            file.createNewFile();
        }
        fos.write(bIn.toByteArray());
        fos.flush();

        System.out.println("File Written Successfully");

        String response = "The message was received successfully. Also, I love you.";

        ArrayList<byte[]> encryptedBytes = Message.formatMessage(response.getBytes(), client.clientMAC, client.clientEncrypt, client.clientIV);
        for (byte[] eb : encryptedBytes) {
            oClientOut.writeObject(eb);
        }


    }


}
