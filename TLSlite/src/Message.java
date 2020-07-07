import javax.crypto.*;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;
import java.io.ByteArrayOutputStream;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.Arrays;

class Message extends TLSBase {


    private static byte[] encrypt(byte[] encryptionKey, byte[] message, byte[] IVkey) throws InvalidAlgorithmParameterException, InvalidKeyException, BadPaddingException, IllegalBlockSizeException, NoSuchPaddingException, NoSuchAlgorithmException {
        Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
        SecretKeySpec keySpec = new SecretKeySpec(encryptionKey, "AES");
        cipher.init(Cipher.ENCRYPT_MODE, keySpec, new IvParameterSpec(IVkey));
        return cipher.doFinal(message);
    }

    static byte[] decrypt(byte[] cipherText, byte[] encryptionKey, byte[] IVkey, byte[] macKey) throws InvalidAlgorithmParameterException, InvalidKeyException, NoSuchPaddingException, NoSuchAlgorithmException, BadPaddingException, IllegalBlockSizeException {
        Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
        SecretKeySpec keySpec = new SecretKeySpec(encryptionKey, "AES");
        cipher.init(Cipher.DECRYPT_MODE, keySpec, new IvParameterSpec(IVkey));
        byte[] decodedBytesWithMac = cipher.doFinal(cipherText);

        byte[] decodedMessage = Arrays.copyOfRange(decodedBytesWithMac, 0, decodedBytesWithMac.length - 32);
        byte[] mac = Arrays.copyOfRange(decodedBytesWithMac, decodedBytesWithMac.length - 32, decodedBytesWithMac.length);
        Mac messageHmac = createMac(macKey);
        byte[] messageHmacBytes = messageHmac.doFinal(decodedMessage);
        if (!Arrays.equals(mac, messageHmacBytes)) {
            System.out.println("Macs are not the same.");
            System.exit(0);
        }
        return decodedMessage;
    }

    /** Formats and encrypts messages, splits messages that are over 100,000 bytes
     *
     * @param message
     * @param macKey
     * @param encryptionKey
     * @param IVKey
     * @return
     * @throws InvalidKeyException
     * @throws NoSuchAlgorithmException
     * @throws IllegalBlockSizeException
     * @throws BadPaddingException
     * @throws InvalidAlgorithmParameterException
     * @throws NoSuchPaddingException
     */
    static ArrayList<byte[]> formatMessage(byte[] message, byte[] macKey, byte[] encryptionKey, byte[] IVKey) throws InvalidKeyException, NoSuchAlgorithmException, IllegalBlockSizeException, BadPaddingException, InvalidAlgorithmParameterException, NoSuchPaddingException {
        ArrayList<byte[]> byteLists = new ArrayList<>();
        ArrayList<byte[]> retList = new ArrayList<>();
        int total = message.length;
        int index = 0;
        int limit = 1;

        if (message.length > 100000) {
            while (total > 100000) {
                byte[] currentArray;
                currentArray = Arrays.copyOfRange(message, (index * 100000), (limit * 100000));
                index++;
                limit++;
                total -= 100000;
                byteLists.add(currentArray);
            }
            byte[] currentArray = Arrays.copyOfRange(message, (index * 100000), (message.length));
            byteLists.add(currentArray);
        } else {
            byteLists.add(message);
        }
        for (byte[] section : byteLists) {
            Mac messageHmac = createMac(macKey);
            byte[] messageHmacBytes = messageHmac.doFinal(section);

            ByteArrayOutputStream concatenatedMessage = new ByteArrayOutputStream();
            concatenatedMessage.writeBytes(section);
            concatenatedMessage.writeBytes(messageHmacBytes);
            byte[] encrypted = encrypt(encryptionKey, concatenatedMessage.toByteArray(), IVKey);
            retList.add(encrypted);
        }
        byte[] nullArray = new byte[0];
        retList.add(index + 1, nullArray);

        return retList;


    }


}
