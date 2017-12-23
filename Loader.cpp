#include "ExecutableHolder.hpp"
#include "BinaryDecrypt.hpp"
#include "Error.hpp"
#include "DebugChecker.hpp"

#include <elfio/elfio.hpp>

#include <termios.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <string>
#include <sstream>

void getPass(std::string &S) {
  std::cout << "Enter password:" << std::endl;

  // Hide input on terminal
  termios nw, old;
  if (tcgetattr(fileno(stdin), &old) != 0)
    reportErrorAndExit("Can't hide password");

  nw = old;
  nw.c_lflag &= ~ECHO;
  if (tcsetattr(fileno(stdin), TCSAFLUSH, &nw) != 0)
    reportErrorAndExit("Can't hide password");
 
  std::cin >> S;

  tcsetattr(fileno(stdin), TCSAFLUSH, &old);
}

int main() {
  // checkDebug();
  std::string Psw;
  getPass(Psw);
  // Decrypt.
  BinaryDecrypt Dec(Psw);
  Dec.retrieveBinary();

  std::string Loaded {reinterpret_cast<const char*>(Executable), ExecLen};
  std::istringstream SS {Loaded, std::ios_base::in | std::ios_base::binary};

  // Load decrypted image.
  ELFIO::elfio Reader;
  if (!Reader.load(SS)) {
    reportErrorAndExit("Can't load image\n");
  }

  ExeHolder EH;
  // Map segments.
  EH.memoryInit(Reader);
  // Run!
  EH.transferControl(Reader);

  std::cout << "Result is " << EH.getRetCode() << std::endl;
  return 0;
}
