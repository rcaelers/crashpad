// Copyright 2021 Rob Caelers
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

#ifndef CRASHPAD_HANDLER_USER_HOOK_H_
#define CRASHPAD_HANDLER_USER_HOOK_H_

#include <stdint.h>

#include <string>
#include <map>
#include <utility>
#include <vector>

#include "base/files/file_path.h"
#include "util/misc/uuid.h"

namespace crashpad {

//! \brief A summary of the crash extracted from the minidump snapshot.
//!
//! Passed to UserHook::requestUserConsent() so the user-facing dialog can
//! display meaningful information about the crash without the caller having
//! to parse the minidump file.
struct CrashSummary {
  //! \brief OS-specific exception/signal code (e.g. EXCEPTION_ACCESS_VIOLATION
  //!     on Windows, signal number on Linux, EXC_* on macOS).
  uint32_t exception_code = 0;

  //! \brief Human-readable name for \a exception_code where known, otherwise
  //!     an empty string.
  std::string exception_name;

  //! \brief Faulting instruction or memory address reported with the exception.
  uint64_t exception_address = 0;

  //! \brief Basename of the module that contains \a exception_address, or an
  //!     empty string if the address does not fall within any loaded module.
  std::string module_name;

  //! \brief Byte offset of \a exception_address from the start of
  //!     \a module_name.  Only meaningful when \a module_name is non-empty.
  uint64_t module_offset = 0;

  //! \brief Thread identifier of the thread that triggered the exception.
  uint64_t crashing_thread_id = 0;

  //! \brief Name of the crashing thread, or an empty string if unavailable.
  std::string crashing_thread_name;

  //! \brief Stack frames of the crashing thread.
  //!
  //! Each entry is a pair of {instruction_address, symbol_name}.  Only
  //! populated when the client was built with CLIENT_STACKTRACES_ENABLED.
  std::vector<std::pair<uint64_t, std::string>> stack_frames;
};

//! \brief Extensibility interface for embedders who wish to interact with the
//! user before submitting the crash report
class UserHook {
 public:
  virtual ~UserHook() {}

  //! \brief Interact with the user before submitting the crash report
  //!
  //! Called after \a process_snapshot has been initialized for the crashed
  //! process to (optionally) produce the contents of a user extension stream
  //! that will be attached to the minidump.
  //!
  //! \param[in] annotations A map of annotations to insert as
  //!     process-level annotations into each crash report that is written. Do
  //!     not confuse this with module-level annotations, which are under the
  //!     control of the crashing process, and are used to implement Chrome's
  //!     "crash keys." Process-level annotations are those that are beyond the
  //!     control of the crashing process, which must reliably be set even if
  //!     the process crashes before it's able to establish its own annotations.
  //!     To interoperate with Breakpad servers, the recommended practice is to
  //!     specify values for the `"prod"` and `"ver"` keys as process
  //!     annotations.
  //! \param[in,out] attachments A vector of file paths to be captured with
  //!     each report at the time of the crash.  The hook may append additional
  //!     paths (e.g. application log or config files) and may remove paths for
  //!     attachments that the user has chosen to exclude.  Only the paths that
  //!     remain in the vector after this call returns will be written to the
  //!     crash report.
  //!
  //! \param[in] summary A summary of the crash extracted from the minidump
  //!     snapshot.
  //!
  //! \return \c true if the user consents to submitting a report,
  //!     \c false otherwise
  virtual bool requestUserConsent(const std::map<std::string, std::string> &annotations,
                                  std::vector<base::FilePath> &attachments,
                                  const CrashSummary &summary) = 0;

  //! \brief Retrieve user provided text
  //!
  //! Called after \a requestUserConsent has been called to retrieve user provided text
  //! that will be added as attachment to the user report
  //!
  //! \return Text provided by the user to describe the crash
  virtual std::string getUserText() = 0;

  //! \brief Report crash UUID
  //!
  //! \param[in] uuid UUID of the created crash report.  This UUID is embedded
  //!     in the minidump, so the ingestion server will use it as the event ID.
  //!     It is suitable for constructing a direct link in the crash-reporting
  //!     web UI without waiting for the upload to complete.
  //!
  //! Called after \a requestUserConsent has been called to report back the UUID of
  //! the crash report.
  //!
  virtual void reportCompleted(const UUID &uuid) = 0;
};

}  // namespace crashpad

#endif  // CRASHPAD_HANDLER_USER_HOOK_H_
