import org.junit.jupiter.api.Test;

import java.security.NoSuchAlgorithmException;

import static org.junit.jupiter.api.Assertions.*;

class httpRequestTest {


    @Test
    void handshakeResponse() throws NoSuchAlgorithmException {
        String swk = "dGhlIHNhbXBsZSBub25jZQ==";
        String answer = "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=";
        String encoded = httpRequest.handshakeResponse(swk);
        assertEquals(encoded, answer);
    }

}