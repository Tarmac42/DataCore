/** @file contracts.cpp
 * @brief Verify typed writer dispatch, owned values and observable output errors.
 */
#include <tarmac/DataCore/Error.hpp>
#include <tarmac/DataCore/models/RecordEnvelope.hpp>
#include <tarmac/DataCore/writers/RecordWriter.hpp>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace {
/** @brief Test payload with no library base class or runtime type tag. */
struct Reading { double value; };
using Record = tarmac::DataCore::RecordEnvelope<Reading>;
/** @brief Test-only sink owning copies; deliberately fails explicit flush. */
class Sink final : public tarmac::DataCore::RecordWriter<Record> {
public:
    std::vector<Record> records;
    /** @copydoc tarmac::DataCore::RecordWriter::write */
    void write(const Record& record) override { records.push_back(record); }
    /** @copydoc tarmac::DataCore::RecordWriter::flush */
    void flush() override { throw tarmac::DataCore::Error("flush failed"); }
};
/** @brief Reject a failed contract without dependence on NDEBUG.
 * @param condition Contract expression. @throws std::runtime_error On failure.
 */
void require(bool condition) { if (!condition) throw std::runtime_error("contract failed"); }
}
/** @brief Exercise record ownership, order and error propagation. @return Zero on success. */
int main() {
    static_assert(std::is_aggregate_v<Record>);
    static_assert(std::has_virtual_destructor_v<tarmac::DataCore::RecordWriter<Record>>);
    Sink sink;
    tarmac::DataCore::RecordWriter<Record>& writer = sink;
    Record record{"sensor-a", {}, {12.5}};
    writer.write(record);
    record.source = "changed";
    record.payload.value = 7.0;
    writer.write(record);
    require(sink.records.size() == 2);
    require(sink.records[0].source == "sensor-a" && sink.records[0].payload.value == 12.5);
    require(sink.records[1].source == "changed" && sink.records[1].payload.value == 7.0);
    bool caught = false;
    try { writer.flush(); } catch (const tarmac::DataCore::Error& error) {
        caught = std::string(error.what()) == "flush failed";
    }
    require(caught);
}
