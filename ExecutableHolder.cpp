#include "ExecutableHolder.hpp"
#include "Error.hpp"

#include <elfio/elfio.hpp>

#include <unistd.h>
#include <sys/mman.h>

#include <cstdlib>
#include <cstdio>

#include <thread>

static int formMMapProt(int Flags) {
  int result = 0;
  result |= (Flags & PF_X) ? PROT_EXEC : 0;
  result |= (Flags & PF_R) ? PROT_READ : 0;
  result |= (Flags & PF_W) ? PROT_WRITE : 0;
  return result;
}

static bool isLoadable(const ELFIO::segment &S) {
  int Flags = S.get_type();
  return Flags == PT_LOAD;
}

static uint64_t getSegmentOffset(const ELFIO::segment &S, uint64_t Base, int PageSize) {
  return (Base & (PageSize - 1)) + (S.get_virtual_address() - Base);
}

static void memProtect(void *Ptr, uint64_t MemSize, int Prot) {
  errno = 0;
  if (mprotect(Ptr, MemSize, Prot))
    reportErrorAndExit("mprotect error");
}

uint64_t getBaseAddress(ELFIO::elfio &Reader) {
  using namespace ELFIO;
  int SegNum = Reader.segments.size();
  uint64_t Base = -1;
  bool Found = false;
  for (int i = 0; i < SegNum; ++i) {
    segment &S = *Reader.segments[i];
    if (isLoadable(S)) {
      Base = std::min(Base, S.get_virtual_address());
      Found = true;
    }
  }
  if (!Found) {
    reportErrorAndExit("Not found base address");
  }

  return Base;
}

uint64_t getFullSize(ELFIO::elfio &Reader, uint64_t Base) {
 using namespace ELFIO;
  int SegNum = Reader.segments.size();
  uint64_t MaxAddr = 0;
  for (int i = 0; i < SegNum; ++i) {
    segment &S = *Reader.segments[i];
    if (isLoadable(S))
      MaxAddr = std::max(MaxAddr, S.get_virtual_address() + S.get_memory_size());
  }
  return MaxAddr - Base;
}

static uint64_t getAlignedVal(uint64_t Value, uint64_t Align) {
  return Value & ~(Align - 1);
}

static uint64_t getAlignedOffset(const ELFIO::segment &S, uint64_t Base, int PageSize) {
  return getAlignedVal(S.get_virtual_address() - Base, PageSize);
}

void
ExeHolder::memoryInitSegment(const ELFIO::segment &S, int AllocSize) {
  uint64_t Offset = getSegmentOffset(S, Base, PageSize);
  char *StartAddr = (char *)AllocPtr + Offset;
  int FileSize = S.get_file_size();
  int MemSize = S.get_memory_size();
  // copy segment into memory
  std::memcpy(StartAddr, S.get_data(), FileSize);
  // nullify rest of allocated space
  std::memset(StartAddr + FileSize, 0, MemSize - FileSize);

  int MProtFlags = formMMapProt(S.get_flags());
  Offset = getAlignedOffset(S, Base, PageSize);
  memProtect((char *)AllocPtr + Offset, MemSize, MProtFlags);
}

void ExeHolder::memoryInit(ELFIO::elfio &Reader) {
  using namespace ELFIO;

  Base = getBaseAddress(Reader);
  uint64_t FullSize = getFullSize(Reader, Base);

  PageSize = sysconf(_SC_PAGE_SIZE);
  int AllocSize = ((FullSize - 1) / PageSize + 1) * PageSize;
  AllocPtr = aligned_alloc(getAlignedVal(Base, PageSize), AllocSize);
  if (AllocPtr == nullptr)
    reportErrorAndExit("Can't alloc memory");

  int SegNum = Reader.segments.size();
  for (int i = 0; i < SegNum; ++i) {
    const segment &S = *Reader.segments[i];
    if (isLoadable(S)) {
      memoryInitSegment(S, AllocSize);
    }
  }
}

void nop() {}

void initAll(void *Entry, int &Result) {
  // Pass this to _start function.
  struct Args {
    long argc;
    char **argv;
    char **envp;
    // Auxiliary vector.
    int a_type;
    void *a_ptr;
  } A;

  typedef int (*EAddrFn)(void(*)(), Args);
  A.argc = 0;
  A.argv = 0;
  A.envp = nullptr;
  A.a_type = 0;
  A.a_ptr = nullptr;

  // Pass function to be called at exit in %rdi and
  // additional args on the stack.
  Result = reinterpret_cast<EAddrFn>(Entry)(nop, A);
}

void ExeHolder::transferControl(ELFIO::elfio &Reader) {
  uint64_t Entry = Reader.get_entry() - Base;
  uint64_t Offset = (Base & (PageSize - 1));

  char *EAddr = (char *)AllocPtr + Offset + Entry;
  {
    std::thread T(initAll, EAddr, std::ref(RetCode));
    T.join();
  }
}

ExeHolder::~ExeHolder() {
  free(AllocPtr);
}
