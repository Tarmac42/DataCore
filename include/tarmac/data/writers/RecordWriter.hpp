/** @file RecordWriter.hpp
 * @brief Typed synchronous writer extension contract without backend selection.
 */
#pragma once

namespace tarmac::data {
/** @brief Consumer of one concrete record type; backend chosen by composition.
 * @tparam Record Caller-defined record value or RecordEnvelope specialization.
 * Calls are synchronous and non-reentrant; callers serialize access. The caller
 * owns records; implementations must not retain borrowed references after write.
 * Implementations own their output resources through RAII. Destruction releases
 * resources without throwing; callers explicitly flush to observe final errors.
 * No atomicity, retry safety or crash durability is implied by this base contract.
 * Each future backend must document partial writes and durability separately.
 */
template<class Record>
class RecordWriter {
public:
    /** @brief Release implementation resources without throwing or implicit retry. */
    virtual ~RecordWriter() = default;
    /** @brief Synchronously accept a typed record, copying any retained data.
     * @param record Borrowed for this call only.
     * @throws Error On adapter-reported output or serialization failure.
     * @throws std::bad_alloc On allocation failure; record-specific errors may propagate.
     * Failure may follow partial output; callers must not assume retry is safe.
     */
    virtual void write(const Record& record) = 0;
    /** @brief Finish buffered output so callers can observe pending failures.
     * @throws Error On output failure; std::bad_alloc may also propagate.
     * Flush is not a universal fsync, commit or durability guarantee.
     */
    virtual void flush() = 0;
};
}
