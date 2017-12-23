#include "DebugChecker.hpp"
#include "Error.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <string>

void my_ptrace(enum __ptrace_request req, pid_t pid, void *addr, void *data,
               const std::string &Msg) {
  errno = 0;
  if (ptrace(req, pid, addr, data)) {
    reportErrorAndExit(Msg);
  }
}

void checkDebug() {
  my_ptrace(PTRACE_TRACEME, 0, 0, 0, "Under debugger. Leaving.");
}
