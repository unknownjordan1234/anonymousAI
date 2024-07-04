import org.bouncycastle.crypto.AsymmetricCipherKeyPair;
import org.bouncycastle.crypto.generators.ECKeyPairGenerator;
import org.bouncycastle.crypto.params.ECDomainParameters;
import org.bouncycastle.crypto.params.ECKeyGenerationParameters;
import org.bouncycastle.crypto.params.ECPrivateKeyParameters;
import org.bouncycastle.crypto.params.ECPublicKeyParameters;
import org.bouncycastle.crypto.params.ParametersWithRandom;
import org.bouncycastle.jce.ECNamedCurveTable;
import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.bouncycastle.util.encoders.Hex;
import javax.crypto.Cipher;
import java.security.KeyPair;
import java.security.Security;

public class ECCEncryption {

    public static void main(String[] args) throws Exception {
        Security.addProvider(new BouncyCastleProvider());

        // Generate ECC key pair
        AsymmetricCipherKeyPair keyPair = generateECKeyPair();

        // Encrypt plaintext
        byte[] plaintext = "Hello, ECC encryption!".getBytes();
        byte[] ciphertext = encryptECIES((ECPrivateKeyParameters) keyPair.getPrivate(), (ECPublicKeyParameters) keyPair.getPublic(), plaintext);

        System.out.println("Ciphertext: " + Hex.toHexString(ciphertext));

        // Decrypt ciphertext
        byte[] decryptedText = decryptECIES((ECPrivateKeyParameters) keyPair.getPrivate(), ciphertext);
        System.out.println("Decrypted text: " + new String(decryptedText));
    }

    public static AsymmetricCipherKeyPair generateECKeyPair() {
        ECDomainParameters ecParams = ECNamedCurveTable.getParameterSpec("secp256r1");
        ECKeyGenerationParameters keyGenParams = new ECKeyGenerationParameters(ecParams, null);
        ECKeyPairGenerator generator = new ECKeyPairGenerator();
        generator.init(keyGenParams);
        return generator.generateKeyPair();
    }

    public static byte[] encryptECIES(ECPrivateKeyParameters privateKey, ECPublicKeyParameters publicKey, byte[] plaintext) throws Exception {
        Cipher cipher = Cipher.getInstance("ECIES", "BC");
        ParametersWithRandom parameters = new ParametersWithRandom(publicKey, null);
        cipher.init(Cipher.ENCRYPT_MODE, parameters.getParameters());
        return cipher.doFinal(plaintext);
    }

    public static byte[] decryptECIES(ECPrivateKeyParameters privateKey, byte[] ciphertext) throws Exception {
        Cipher cipher = Cipher.getInstance("ECIES", "BC");
        ParametersWithRandom parameters = new ParametersWithRandom(privateKey, null);
        cipher.init(Cipher.DECRYPT_MODE, parameters.getParameters());
        return cipher.doFinal(ciphertext);
    }
}

//end of ECCEncryption.java
