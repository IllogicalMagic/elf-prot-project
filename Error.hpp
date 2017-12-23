#ifndef ERROR_HPP_INCLUDED
#define ERROR_HPP_INCLUDED

#include <string>
#include <cerrno>
#include <cstdlib>
#include <iostream>

static void reportErrorAndExit(const std::string &S = "") {
  if (errno != 0) {
    const char *Err = nullptr;
    if (!S.empty())
      Err = S.c_str();
    perror(Err);
  } else {
    std::cerr << S << '\n';
  }
  exit(1);
}


#endif
