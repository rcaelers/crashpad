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

#ifndef CRASHPAD_HANDLER_CRASH_SUMMARY_H_
#define CRASHPAD_HANDLER_CRASH_SUMMARY_H_

#include "handler/user_hook.h"

namespace crashpad {

class ProcessSnapshot;

//! \brief Build a CrashSummary from a ProcessSnapshot.
//!
//! Extracts the exception code, faulting address, crashing thread info, module
//! name, and (when CLIENT_STACKTRACES_ENABLED is defined) the stack frames of
//! the crashing thread.
//!
//! \param[in] snapshot A fully-initialized ProcessSnapshot for the crashed
//!     process.
//! \return A populated CrashSummary.
CrashSummary BuildCrashSummary(const ProcessSnapshot& snapshot);

}  // namespace crashpad

#endif  // CRASHPAD_HANDLER_CRASH_SUMMARY_H_
