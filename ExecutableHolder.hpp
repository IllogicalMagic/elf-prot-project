#ifndef EXEC_HOLDER_HPP_INCLUDED
#define EXEC_HOLDER_HPP_INCLUDED

#include <elfio/elfio.hpp>

#include <cstdint>

class ExeHolder {
  // begin of aligned storage
  void *AllocPtr;
  uint64_t Base;
  int PageSize;
  int RetCode;

  void memoryInitSegment(const ELFIO::segment &S, int AllocSize);

public:
  ExeHolder():
    AllocPtr(nullptr), Base(0), PageSize(0), RetCode(0) {}

  void memoryInit(ELFIO::elfio &Reader);
  void transferControl(ELFIO::elfio &Reader);
  int getRetCode() { return RetCode; }

  ~ExeHolder();
};

#endif
