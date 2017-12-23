#include "ExecutableHolder.hpp"
#include "BinaryDecrypt.hpp"
#include "Error.hpp"
#include "DebugChecker.hpp"

#include <elfio/elfio.hpp>

#include <unistd.h>
#include <sys/mman.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <fstream>
#include <string>
#include <sstream>

void getPass(std::string &S) {
  std::cin >> S;
}

int main() {
  checkDebug();
  std::string Psw;
  getPass(Psw);
  // Decrypt.
  BinaryDecrypt Dec(Psw);

  std::string Loaded {reinterpret_cast<const char*>(Executable), ExecLen};
  std::istringstream SS {Loaded, std::ios_base::in | std::ios_base::binary};

  // Load decrypted image.
  ELFIO::elfio Reader;
  if (!Reader.load(SS)) {
    reportErrorAndExit("Can't load image\n");
  }

  if (Reader.get_class() == ELFCLASS32)
    std::cout << "ELF32" << std::endl;
  else
    std::cout << "ELF64" << std::endl;

  ExeHolder EH;
  // Map segments.
  EH.memoryInit(Reader);
  // Run!
  EH.transferControl(Reader);
  
  return 0;
}
