/** @file RecordEnvelope.hpp
 * @brief Optional typed observation envelope, independent of storage format.
 */
#pragma once
#include <chrono>
#include <string>

namespace tarmac::DataCore {
/** @brief Owning metadata and a statically typed payload.
 * @tparam Payload Application-defined owning value type.
 * Aggregate initialization and value semantics follow Payload. UTC observation
 * time is supplied by the caller, never inferred from a writer or monotonic clock.
 * source is an opaque identifier, not a filesystem path. An envelope is optional:
 * RecordWriter also accepts ordinary application-defined record structures.
 * Mutable instances require external synchronization if shared across threads.
 */
template<class Payload>
struct RecordEnvelope {
    std::string source;
    std::chrono::sys_time<std::chrono::nanoseconds> observed_at{};
    Payload payload;
};
}
