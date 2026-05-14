// Copyright 2026 Rob Caelers
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "handler/crash_summary.h"

#include <sstream>
#include <string>

#include "build/build_config.h"
#include "snapshot/exception_snapshot.h"
#include "snapshot/module_snapshot.h"
#include "snapshot/process_snapshot.h"
#include "snapshot/thread_snapshot.h"

namespace crashpad {

namespace {

// Returns a human-readable name for the exception code, if known.
// The mapping is OS-specific; an empty string is returned for unknown codes.
std::string ExceptionCodeName(uint32_t code) {
#if BUILDFLAG(IS_WIN)
  switch (code) {
    case 0x80000001: return "EXCEPTION_GUARD_PAGE";
    case 0x80000002: return "EXCEPTION_DATATYPE_MISALIGNMENT";
    case 0x80000003: return "EXCEPTION_BREAKPOINT";
    case 0x80000004: return "EXCEPTION_SINGLE_STEP";
    case 0xC0000005: return "EXCEPTION_ACCESS_VIOLATION";
    case 0xC0000006: return "EXCEPTION_IN_PAGE_ERROR";
    case 0xC0000008: return "EXCEPTION_INVALID_HANDLE";
    case 0xC000000D: return "EXCEPTION_INVALID_PARAMETER";
    case 0xC000001D: return "EXCEPTION_ILLEGAL_INSTRUCTION";
    case 0xC0000025: return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
    case 0xC0000094: return "EXCEPTION_INT_DIVIDE_BY_ZERO";
    case 0xC0000095: return "EXCEPTION_INT_OVERFLOW";
    case 0xC0000096: return "EXCEPTION_PRIV_INSTRUCTION";
    case 0xC00000FD: return "EXCEPTION_STACK_OVERFLOW";
    case 0xC0000409: return "EXCEPTION_STACK_BUFFER_OVERRUN";
    case 0xE06D7363: return "C++ exception";
    case 0x40010005: return "CONTROL_C_EXIT";
    default:         return "";
  }
#elif BUILDFLAG(IS_APPLE)
  // Mach exception types (EXC_*)
  switch (code) {
    case 1:  return "EXC_BAD_ACCESS";
    case 2:  return "EXC_BAD_INSTRUCTION";
    case 3:  return "EXC_ARITHMETIC";
    case 4:  return "EXC_EMULATION";
    case 5:  return "EXC_SOFTWARE";
    case 6:  return "EXC_BREAKPOINT";
    case 7:  return "EXC_SYSCALL";
    case 8:  return "EXC_MACH_SYSCALL";
    case 9:  return "EXC_RPC_ALERT";
    case 10: return "EXC_CRASH";
    case 11: return "EXC_RESOURCE";
    case 12: return "EXC_GUARD";
    default: return "";
  }
#elif BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS) || BUILDFLAG(IS_ANDROID)
  // POSIX signal numbers
  switch (code) {
    case 1:  return "SIGHUP";
    case 2:  return "SIGINT";
    case 3:  return "SIGQUIT";
    case 4:  return "SIGILL";
    case 5:  return "SIGTRAP";
    case 6:  return "SIGABRT";
    case 7:  return "SIGBUS";
    case 8:  return "SIGFPE";
    case 9:  return "SIGKILL";
    case 11: return "SIGSEGV";
    case 12: return "SIGSYS";
    case 13: return "SIGPIPE";
    case 15: return "SIGTERM";
    default: return "";
  }
#else
  (void)code;
  return "";
#endif
}

}  // namespace

CrashSummary BuildCrashSummary(const ProcessSnapshot& snapshot) {
  CrashSummary summary;

  const ExceptionSnapshot* exception = snapshot.Exception();
  if (!exception) {
    return summary;
  }

  summary.exception_code    = exception->Exception();
  summary.exception_address = exception->ExceptionAddress();
  summary.exception_name    = ExceptionCodeName(summary.exception_code);
  summary.crashing_thread_id = exception->ThreadID();

  // Find crashing thread name and stack frames.
  for (const ThreadSnapshot* thread : snapshot.Threads()) {
    if (thread->ThreadID() != summary.crashing_thread_id) {
      continue;
    }
    summary.crashing_thread_name = thread->ThreadName();
#ifdef CLIENT_STACKTRACES_ENABLED
    for (const auto& frame : thread->StackTrace()) {
      summary.stack_frames.emplace_back(frame.InstructionAddr(), frame.Symbol());
    }
#endif
    break;
  }

  // Find the module that contains the exception address.
  for (const ModuleSnapshot* module : snapshot.Modules()) {
    const uint64_t base = module->Address();
    const uint64_t size = module->Size();
    if (size > 0 &&
        summary.exception_address >= base &&
        summary.exception_address < base + size) {
      // Derive the basename from the full module path.
      const std::string full_name = module->Name();
      const auto slash = full_name.find_last_of("/\\");
      summary.module_name =
          (slash != std::string::npos) ? full_name.substr(slash + 1) : full_name;
      summary.module_offset = summary.exception_address - base;
      break;
    }
  }

  return summary;
}

}  // namespace crashpad
