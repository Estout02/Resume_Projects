import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;
import java.io.*;
import java.math.BigInteger;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.security.*;
import java.security.cert.Certificate;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.PKCS8EncodedKeySpec;
import java.util.Arrays;
import java.util.Random;

public class TLSBase {


    BigInteger N;
    ByteArrayOutputStream history = new ByteArrayOutputStream();

    byte[] serverEncrypt;
    byte[] clientEncrypt;
    byte[] serverMAC;
    byte[] clientMAC;
    byte[] serverIV;
    byte[] clientIV;


//    byte[] readMessage(ObjectInputStream in) throws IOException, ClassNotFoundException {
//        byte[] message = (byte[]) in.readObject();
//        history.write(message);
//        return message;
//    }
//
//    void sendMessage(ObjectOutputStream out, Object messageObj) throws IOException {
//        out.writeObject(messageObj);
//        history.writeBytes((byte[]) messageObj);
//    }

    static byte[] getNonce() {
        java.security.SecureRandom nonce1 = new SecureRandom();
        byte[] nonce = new byte[32];
        nonce1.nextBytes(nonce);
        return nonce;
    }

    static Certificate getCertificate(String string) throws CertificateException, FileNotFoundException {
        CertificateFactory certificateFactory = CertificateFactory.getInstance("X.509");

        InputStream certificateInputStream = new FileInputStream(string);

        Certificate certificate = certificateFactory.generateCertificate(certificateInputStream);

        return certificate;

    }

    static BigInteger generateSaOrSb() {
        Random random = new Random();
        return new BigInteger(256, random);
    }

    static BigInteger getN() {
        String modp2048 = (
                "FFFFFFFF FFFFFFFF C90FDAA2 2168C234 C4C6628B 80DC1CD1" +
                        "29024E08 8A67CC74 020BBEA6 3B139B22 514A0879 8E3404DD" +
                        "EF9519B3 CD3A431B 302B0A6D F25F1437 4FE1356D 6D51C245" +
                        "E485B576 625E7EC6 F44C42E9 A637ED6B 0BFF5CB6 F406B7ED" +
                        "EE386BFB 5A899FA5 AE9F2411 7C4B1FE6 49286651 ECE45B3D" +
                        "C2007CB8 A163BF05 98DA4836 1C55D39A 69163FA8 FD24CF5F" +
                        "83655D23 DCA3AD96 1C62F356 208552BB 9ED52907 7096966D" +
                        "670C354E 4ABC9804 F1746C08 CA18217C 32905E46 2E36CE3B" +
                        "E39E772C 180E8603 9B2783A2 EC07A28F B5C55DF0 6F4C52C9" +
                        "DE2BCBF6 95581718 3995497C EA956AE5 15D22618 98FA0510" +
                        "15728E5A 8AACAA68 FFFFFFFF FFFFFFFF")
                .replaceAll("\\s", "");

        return new BigInteger(modp2048, 16);
    }

    static BigInteger generateDHPublicKey(BigInteger SaOrSb) {
//        g^ks % N. ks is secret (never sent to anyone) and is randomly generated. The other 2 numbers come from the MODP group.
//        You use these 3 numbers in that formula to get the DH public key which is transmitted (and also signed).
//        g is 2 (the "generator") and that large number is N
        int g = 2;
        BigInteger bigOne = new BigInteger(String.valueOf(g));
        BigInteger publickey = bigOne.modPow(SaOrSb, getN());
//        System.out.println("this is the key" + "\n" + key);
//            return key.toByteArray();
        return publickey;

    }

    static BigInteger generateDHPrivateKey(BigInteger TAorTB, BigInteger SAorSB, BigInteger N) {
        return TAorTB.modPow(SAorSB, N);

    }

    static PrivateKey generatePrivKey(String derfile) throws InvalidKeySpecException, NoSuchAlgorithmException, IOException {

        Path path = Paths.get(derfile);
        byte[] privKeyByteArray = Files.readAllBytes(path);

        PKCS8EncodedKeySpec keySpec = new PKCS8EncodedKeySpec(privKeyByteArray);

        KeyFactory keyFactory = KeyFactory.getInstance("RSA");

        PrivateKey myPrivKey = keyFactory.generatePrivate(keySpec);
        return myPrivKey;

    }

    static byte[] generateSignature(byte[] publicKey, PrivateKey privateKey) throws InvalidKeyException, NoSuchAlgorithmException, SignatureException {
        Signature signature = Signature.getInstance("SHA256WithRSA");

        SecureRandom secureRandom = new SecureRandom();

        signature.initSign(privateKey, secureRandom);

        signature.update(publicKey);
        //sign server public key
        return signature.sign();
    }

    static boolean checkKey(Certificate certificate, byte[] publicKey, byte[] signedPublicKey) throws NoSuchAlgorithmException, InvalidKeyException, SignatureException {
        Signature signature = Signature.getInstance("SHA256WithRSA");
        signature.initVerify(certificate.getPublicKey());
        signature.update(publicKey);
        return signature.verify(signedPublicKey);
    }

    static byte[] hkdfExpand(byte[] input, byte[] tag) throws NoSuchAlgorithmException, InvalidKeyException {
        byte[] data = new byte[tag.length + 1];
        for (int i = 0; i < tag.length; i++) {
            data[i] = tag[i];
        }
        data[tag.length - 1] = 1;
        Mac mac = TLSBase.createMac(input);
        return Arrays.copyOfRange(mac.doFinal(data), 0, 16);
    }

    void makeSecretKeys(byte[] nonce, byte[] DHPrivateKey) throws NoSuchAlgorithmException, InvalidKeyException {
        Mac mac = createMac(DHPrivateKey);
        byte[] prk = mac.doFinal(nonce);
        serverEncrypt = hkdfExpand(prk, ("server encrypt").getBytes());
        clientEncrypt = hkdfExpand(serverEncrypt, ("client encrypt").getBytes());
        serverMAC = hkdfExpand(clientEncrypt, ("server MAC").getBytes());
        clientMAC = hkdfExpand(serverMAC, ("client MAC").getBytes());
        serverIV = hkdfExpand(clientMAC, ("server IV").getBytes());
        clientIV = hkdfExpand(serverIV, ("client IV").getBytes());
    }

    static Mac createMac(byte[] bytes) throws NoSuchAlgorithmException, InvalidKeyException {
        SecretKeySpec serverKeySpec = new SecretKeySpec(bytes, "hmacSHA256");
        Mac mac = Mac.getInstance("hmacSHA256");
        mac.init(serverKeySpec);
        return mac;
    }


}
