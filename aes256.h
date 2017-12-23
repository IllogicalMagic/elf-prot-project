#ifndef AES256_H_DEFINED
#define AES256_H_DEFINED

#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/randpool.h>
#include <cryptopp/secblock.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>

#include <assert.h>
#include <cstdio>

namespace Cipher{
    static const unsigned KEYLEN_B = 256/8;

    enum class WrapperType {
        Encryptor, Decryptor
    };

    class AES256_initter {
        CryptoPP::byte key[KEYLEN_B + 1];
        CryptoPP::byte iv[KEYLEN_B];

        static void calc_key(CryptoPP::byte * dst, const char * pass) {
            hash(dst, pass);
        }
        static void calc_iv(CryptoPP::byte * dst, const CryptoPP::byte * key) {
            hash(dst, (const char *) key);
            assert(key[KEYLEN_B] == '\0');
        }

    public:
        AES256_initter(const std::string & pass) {
            calc_key(key, pass.c_str());
            key[KEYLEN_B] = '\0';

            calc_iv(iv, key);
        }

        const CryptoPP::byte * getKey() const{
            return key;
        }
        const CryptoPP::byte * getIV() const {
            return iv;
        }

        static void hash(CryptoPP::byte * dst, const char * src, const unsigned len = KEYLEN_B, CryptoPP::SHA256 sha = CryptoPP::SHA256()) {
            using namespace CryptoPP;

            CryptoPP::ByteQueue * result = new CryptoPP::ByteQueue;
            HashFilter * hash_filter = new HashFilter(sha, result);
            StringSource s(src, true, hash_filter);
            result->Peek(dst, len);
        }

        ~AES256_initter() {}
    };

    class AES256_encryptor {
        CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption * enc;

    public:
        AES256_encryptor(const std::string & pass) {
            AES256_initter initter(pass);

            const CryptoPP::byte * key_p = initter.getKey(), * IV_p = initter.getIV();
            enc = new CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption(key_p, KEYLEN_B, IV_p);
        };
        ~AES256_encryptor() {
            delete enc;
        };

        void encrypt(CryptoPP::byte * array, const unsigned len) {
            enc->ProcessData(array, array, len);
        }
    };

    class AES256_decryptor {
        CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption * dec;

    public:
        AES256_decryptor(const std::string & pass) {
            AES256_initter initter(pass);

            const CryptoPP::byte * key_p = initter.getKey(), * IV_p = initter.getIV();
            dec = new CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption(key_p, KEYLEN_B, IV_p);
        };
        ~AES256_decryptor() {
            delete dec;
        };

        void decrypt(CryptoPP::byte * array, const unsigned len) {
            dec->ProcessData(array, array, len);
        }
    };
}

#endif //AES256_H_DEFINED
