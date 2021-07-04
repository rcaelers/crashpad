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
  //! \param[in] info Human readable information on the crash that can be 
  //!     reported to the user.
  //!
  //! \return \c true if the user consents to submitting a report,
  //!     \c false otherwise
  virtual bool reportCrash(const std::string &info) = 0;

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
