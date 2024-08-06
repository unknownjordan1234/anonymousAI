#include <iostream>
#include <fstream>
#include <stdexcept>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

class Cryptography {
public:
    Cryptography() {
        // Prepares elliptic curve to generate key pair
        ec_key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
        if (!ec_key || !EC_KEY_generate_key(ec_key)) {
            throw std::runtime_error("Failed to generate EC key.");
        }
    }

    ~Cryptography() {
        EC_KEY_free(ec_key);
    }

    void encrypt(const std::string& plaintext, std::string& ciphertext) {
        // AES Encryption
        unsigned char key[32];
        unsigned char iv[16];
        if (!RAND_bytes(key, sizeof(key)) || !RAND_bytes(iv, sizeof(iv))) {
            throw std::runtime_error("Failed to generate AES key or IV.");
        }

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

        unsigned char outbuf[plaintext.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc())];
        int outlen;

        EVP_EncryptUpdate(ctx, outbuf, &outlen, (const unsigned char*)plaintext.c_str(), plaintext.length());
        int tmplen;
        EVP_EncryptFinal_ex(ctx, outbuf + outlen, &tmplen);
        outlen += tmplen;

        ciphertext.assign((char*)outbuf, outlen);
        EVP_CIPHER_CTX_free(ctx);
    }

    void log_to_file(const std::string& message) {
        std::ofstream log_file("cryptography.log", std::ios_base::app);
        if (log_file.is_open()) {
            log_file << message << std::endl;
        } else {
            throw std::runtime_error("Failed to open log file.");
        }
    }

private:
    EC_KEY* ec_key;
};

int main() {
    try {
        Cryptography crypto;
        std::string plaintext = "Hello, World!";
        std::string ciphertext;

        crypto.encrypt(plaintext, ciphertext);
        crypto.log_to_file("Encryption successful. Ciphertext length: " + std::to_string(ciphertext.size()));
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::ofstream log_file("cryptography.log", std::ios_base::app);
        log_file << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
