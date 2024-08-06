#include <iostream>
#include <fstream>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/rand.h>

class MorseSoundTranslator {
public:
    void encryptFile(const std::string& inputFile, const std::string& outputFile) {
        // 生成 AES 的密钥和 IV
        unsigned char key[32]; // AES-256 密钥
        unsigned char iv[16];  // AES 块大小
        if (!RAND_bytes(key, sizeof(key)) || !RAND_bytes(iv, sizeof(iv))) {
            std::cerr << "生成随机密钥或 IV 时出错。" << std::endl;
            return;
        }

        std::ifstream infile(inputFile, std::ios::binary);
        if (!infile.is_open()) {
            std::cerr << "打开输入文件时出错。" << std::endl;
            return;
        }

        std::ofstream outfile(outputFile, std::ios::binary);
        // 首先将密钥和 IV 写入输出文件
        outfile.write(reinterpret_cast<char*>(key), sizeof(key));
        outfile.write(reinterpret_cast<char*>(iv), sizeof(iv));

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

    void decryptFile(const std::string& inputFile, const std::string& outputFile) {
        std::ifstream infile(inputFile, std::ios::binary);
        if (!infile.is_open()) {
            std::cerr << "打开输入文件时出错。" << std::endl;
            return;
        }

        unsigned char key[32], iv[16];
        infile.read(reinterpret_cast<char*>(key), sizeof(key)); // 读取密钥
        infile.read(reinterpret_cast<char*>(iv), sizeof(iv));   // 读取 IV

        std::ofstream outfile(outputFile, std::ios::binary);
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
};

int main() {
    MorseSoundTranslator morseSoundTranslator;
    std::string inputFile = "input.txt";
    std::string encryptedFile = "encrypted.dat";
    std::string decryptedFile = "decrypted.txt";

    // 加密文件
    morseSoundTranslator.encryptFile(inputFile, encryptedFile);

    // 解密文件
    morseSoundTranslator.decryptFile(encryptedFile, decryptedFile);

    // 在解密后的文件上执行摩尔斯编码翻译
    // morseSoundTranslator.textFileToMorseSound(decryptedFile, "output_sound_file.wav");

    return 0;
}
