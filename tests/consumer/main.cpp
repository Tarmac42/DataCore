/** @file main.cpp
 * @brief Verify installed public headers and transitive C++20 requirement.
 */
#include <tarmac/DataCore/models/RecordEnvelope.hpp>
#include <tarmac/DataCore/writers/RecordWriter.hpp>
/** @brief Construct an installed model. @return Zero if the value is preserved. */
int main() { tarmac::DataCore::RecordEnvelope<int> value{"consumer", {}, 42}; return value.payload == 42 ? 0 : 1; }
