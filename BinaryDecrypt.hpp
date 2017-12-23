#ifndef BINARY_DECRYPT_INCLUDED
#define BINARY_DECRYPT_INCLUDED

#include "aes256.h"

extern "C" {
extern CryptoPP::byte *Executable;
extern unsigned ExecLen;
}

class BinaryDecrypt {
  Cipher::AES256_decryptor Dec;

public:
  BinaryDecrypt(const std::string &Psw):
    Dec(Psw) {}

  void retrieveBinary() {
    Dec.decrypt(Executable, ExecLen);
  }
};

#endif
