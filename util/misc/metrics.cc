// Copyright 2016 The Crashpad Authors. All rights reserved.
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

#include "util/misc/metrics.h"

#include "base/metrics/histogram_functions.h"
#include "base/metrics/histogram_macros.h"
#include "base/numerics/safe_conversions.h"
#include "build/build_config.h"

#if BUILDFLAG(IS_APPLE)
#define METRICS_OS_NAME "Mac"
#elif BUILDFLAG(IS_WIN)
#define METRICS_OS_NAME "Win"
#elif BUILDFLAG(IS_ANDROID)
#define METRICS_OS_NAME "Android"
#elif BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS)
#define METRICS_OS_NAME "Linux"
#elif BUILDFLAG(IS_FUCHSIA)
#define METRICS_OS_NAME "Fuchsia"
#endif

namespace crashpad {

namespace {

//! \brief Metrics values used to track the start and completion of a crash
//!     handling. These are used as metrics values directly, so
//!     enumeration values so new values should always be added at the end,
//!     before kMaxValue.
enum class ExceptionProcessingState {
  //! \brief Logged when exception processing is started.
  kStarted = 0,

  //! \brief Logged when exception processing completes.
  kFinished = 1,

  //! \brief An invalid value.
  kMaxValue,
};

void ExceptionProcessing(ExceptionProcessingState state) {
  UMA_HISTOGRAM_ENUMERATION("Crashpad.ExceptionEncountered",
                            state,
                            ExceptionProcessingState::kMaxValue);
}

}  // namespace

// static
void Metrics::CrashReportPending(PendingReportReason reason) {
  UMA_HISTOGRAM_ENUMERATION(
      "Crashpad.CrashReportPending", reason, PendingReportReason::kMaxValue);
}

// static
void Metrics::CrashReportSize(FileOffset size) {
  UMA_HISTOGRAM_CUSTOM_COUNTS("Crashpad.CrashReportSize",
                              base::saturated_cast<uint32_t>(size),
                              0,
                              20 * 1024 * 1024,
                              50);
}

// static
void Metrics::CrashUploadAttempted(bool successful) {
  UMA_HISTOGRAM_BOOLEAN("Crashpad.CrashUpload.AttemptSuccessful", successful);
}

#if BUILDFLAG(IS_APPLE)
// static
void Metrics::CrashUploadErrorCode(int error_code) {
  base::UmaHistogramSparse("Crashpad.CrashUpload.ErrorCode", error_code);
}
#endif

// static
void Metrics::CrashUploadSkipped(CrashSkippedReason reason) {
  UMA_HISTOGRAM_ENUMERATION(
      "Crashpad.CrashUpload.Skipped", reason, CrashSkippedReason::kMaxValue);
}

// static
void Metrics::ExceptionCaptureResult(CaptureResult result) {
  ExceptionProcessing(ExceptionProcessingState::kFinished);
  UMA_HISTOGRAM_ENUMERATION(
      "Crashpad.ExceptionCaptureResult", result, CaptureResult::kMaxValue);
}

// static
void Metrics::ExceptionCode(uint32_t exception_code) {
  base::UmaHistogramSparse("Crashpad.ExceptionCode." METRICS_OS_NAME,
                           exception_code);
}

// static
void Metrics::ExceptionEncountered() {
  ExceptionProcessing(ExceptionProcessingState::kStarted);
}

// static
void Metrics::HandlerLifetimeMilestone(LifetimeMilestone milestone) {
  UMA_HISTOGRAM_ENUMERATION("Crashpad.HandlerLifetimeMilestone",
                            milestone,
                            LifetimeMilestone::kMaxValue);
}

// static
void Metrics::HandlerCrashed(uint32_t exception_code) {
  base::UmaHistogramSparse(
      "Crashpad.HandlerCrash.ExceptionCode." METRICS_OS_NAME, exception_code);
}

#if BUILDFLAG(IS_IOS)
// static
void Metrics::MissingIntermediateDumpKey(
    const internal::IntermediateDumpKey& key) {
  base::UmaHistogramSparse("Crashpad.IntermediateDump.Reader.MissingKey",
                           static_cast<uint16_t>(key));
}

// static
void Metrics::InvalidIntermediateDumpKeySize(
    const internal::IntermediateDumpKey& key) {
  base::UmaHistogramSparse("Crashpad.IntermediateDump.Reader.InvalidKeySize",
                           static_cast<uint16_t>(key));
}
#endif

// static

}  // namespace crashpad
