/** @file contracts.cpp
 * @brief Verify typed writer dispatch, owned values and observable output errors.
 */
#include <tarmac/data/Error.hpp>
#include <tarmac/data/models/RecordEnvelope.hpp>
#include <tarmac/data/writers/RecordWriter.hpp>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace {
/** @brief Test payload with no library base class or runtime type tag. */
struct Reading { double value; };
using Record = tarmac::data::RecordEnvelope<Reading>;
/** @brief Test-only sink owning copies; deliberately fails explicit flush. */
class Sink final : public tarmac::data::RecordWriter<Record> {
public:
    std::vector<Record> records;
    /** @copydoc tarmac::data::RecordWriter::write */
    void write(const Record& record) override { records.push_back(record); }
    /** @copydoc tarmac::data::RecordWriter::flush */
    void flush() override { throw tarmac::data::Error("flush failed"); }
};
/** @brief Reject a failed contract without dependence on NDEBUG.
 * @param condition Contract expression. @throws std::runtime_error On failure.
 */
void require(bool condition) { if (!condition) throw std::runtime_error("contract failed"); }
}
/** @brief Exercise record ownership, order and error propagation. @return Zero on success. */
int main() {
    static_assert(std::is_aggregate_v<Record>);
    static_assert(std::has_virtual_destructor_v<tarmac::data::RecordWriter<Record>>);
    Sink sink;
    tarmac::data::RecordWriter<Record>& writer = sink;
    Record record{"sensor-a", {}, {12.5}};
    writer.write(record);
    record.source = "changed";
    record.payload.value = 7.0;
    writer.write(record);
    require(sink.records.size() == 2);
    require(sink.records[0].source == "sensor-a" && sink.records[0].payload.value == 12.5);
    require(sink.records[1].source == "changed" && sink.records[1].payload.value == 7.0);
    bool caught = false;
    try { writer.flush(); } catch (const tarmac::data::Error& error) {
        caught = std::string(error.what()) == "flush failed";
    }
    require(caught);
}
