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

#include <string>
#include <map>
#include <vector>

#include "base/files/file_path.h"

namespace crashpad {

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
  //! \param[in] attachments A vector of file paths that should be captured with
  //!     each report at the time of the crash.
  //!
  //! \return \c true if the user consents to submitting a report,
  //!     \c false otherwise
  virtual bool reportCrash(const std::map<std::string, std::string> &annotations,
                           const std::vector<base::FilePath> &attachments) = 0;

  //! \brief Interact with the user before submitting the crash report
  //!
  //! Called after \a process_snapshot has been initialized for the crashed
  //! process to (optionally) produce the contents of a user extension stream
  //! that will be attached to the minidump.
  //!
  //! \return Text provided by the user to describe the crash
  virtual std::string getUserText() = 0;
};

}  // namespace crashpad

#endif  // CRASHPAD_HANDLER_USER_HOOK_H_
