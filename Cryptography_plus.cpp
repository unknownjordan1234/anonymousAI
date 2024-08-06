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
        ec_key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
        if (!ec_key || !EC_KEY_generate_key(ec_key)) {
            throw std::runtime_error("Failed to generate EC key.");
        }
    }

    ~Cryptography() {
        EC_KEY_free(ec_key);
    }

    void encrypt_file(const std::string& input_file, const std::string& output_file) {
        std::ifstream infile(input_file, std::ios::binary);
        if (!infile.is_open()) throw std::runtime_error("Could not open input file.");

        // Generate AES Key and IV
        unsigned char key[32], iv[16];
        if (!RAND_bytes(key, sizeof(key)) || !RAND_bytes(iv, sizeof(iv))) {
            throw std::runtime_error("Failed to generate AES key or IV.");
        }

        std::ofstream outfile(output_file, std::ios::binary);
        outfile.write(reinterpret_cast<char*>(key), sizeof(key)); // Write key to file
        outfile.write(reinterpret_cast<char*>(iv), sizeof(iv));   // Write IV to file

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

        unsigned char buffer[4096];
        unsigned char outbuf[4096 + EVP_CIPHER_block_size(EVP_aes_256_cbc())];
        int outlen;

        while (infile.read(reinterpret_cast<char*>(buffer), sizeof(buffer))) {
            EVP_EncryptUpdate(ctx, outbuf, &outlen, buffer, infile.gcount());
            outfile.write(reinterpret_cast<char*>(outbuf), outlen);
        }

        int tmplen;
        EVP_EncryptFinal_ex(ctx, outbuf + outlen, &tmplen);
        outlen += tmplen;
        outfile.write(reinterpret_cast<char*>(outbuf), outlen);

        EVP_CIPHER_CTX_free(ctx);
        infile.close();
        outfile.close();
    }

    void decrypt_file(const std::string& input_file, const std::string& output_file) {
        std::ifstream infile(input_file, std::ios::binary);
        if (!infile.is_open()) throw std::runtime_error("Could not open input file.");

        unsigned char key[32], iv[16];
        infile.read(reinterpret_cast<char*>(key), sizeof(key)); // Read key from file
        infile.read(reinterpret_cast<char*>(iv), sizeof(iv));   // Read IV from file

        std::ofstream outfile(output_file, std::ios::binary);
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

        unsigned char buffer[4096];
        unsigned char outbuf[4096];
        int outlen;

        while (infile.read(reinterpret_cast<char*>(buffer), sizeof(buffer)) || infile.gcount() > 0) {
            EVP_DecryptUpdate(ctx, outbuf, &outlen, buffer, infile.gcount());
            outfile.write(reinterpret_cast<char*>(outbuf), outlen);
        }

        int tmplen;
        EVP_DecryptFinal_ex(ctx, outbuf + outlen, &tmplen);
        outlen += tmplen;
        outfile.write(reinterpret_cast<char*>(outbuf), outlen);

        EVP_CIPHER_CTX_free(ctx);
        infile.close();
        outfile.close();
    }

    void log_to_file(const std::string& message) {
        std::ofstream log_file("cryptography.log", std::ios_base::app);
        if (log_file.is_open()) {
            log_file << message << std::endl;
        } else {
            throw std::runtime_error("Failed to open log file.");
        }
    }
};

int main() {
    try {
        Cryptography crypto;

        // Replace "input.dat" with your input file name
        crypto.encrypt_file("input.dat", "encrypted.dat");
        crypto.log_to_file("File encryption successful: encrypted.dat");

        // Replace "encrypted.dat" with your encrypted file name
        crypto.decrypt_file("encrypted.dat", "decrypted.dat");
        crypto.log_to_file("File decryption successful: decrypted.dat");

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::ofstream log_file("cryptography.log", std::ios_base::app);
        log_file << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
