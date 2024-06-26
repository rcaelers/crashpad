// Copyright 2015 The Crashpad Authors. All rights reserved.
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

#ifndef CRASHPAD_COMPAT_MINGW_WERAPI_H_
#define CRASHPAD_COMPAT_MINGW_WERAPI_H_

typedef HANDLE HREPORT;

#ifndef WER_MAX_PREFERRED_MODULES_BUFFER
#define WER_MAX_PREFERRED_MODULES_BUFFER 256
#endif

#include_next <werapi.h>

#ifndef PWER_SUBMIT_RESULT
#define PWER_SUBMIT_RESULT WER_SUBMIT_RESULT*
#endif

#endif  // CRASHPAD_COMPAT_MINGW_WERAPI_H_
