#include "aes256.h"
#include "encrypt_commons.h"

int main (int argc, char * argv[]) {
    if (argc != 4) {
        printf("Usage: %s <filepath to ciphered object> <password path> <out_path>\n", argv[0]);
        exit(1);
    }

    using namespace Cipher;
    std::string pass;
    loadfile_into_str(argv[2], Mode::binary, pass);

    std::vector<CryptoPP::byte> obj;
    loadfile_into_vector(argv[1], Mode::binary, obj);

    AES256_decryptor dec(pass);
    dec.decrypt(obj.data(), obj.size());

    std::ofstream out(argv[3], Mode::binary & Mode::out);
    for (auto i: obj) {
        out << i;
    }
    return 0;
}
