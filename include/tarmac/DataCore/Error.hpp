/** @file Error.hpp
 * @brief Application-independent data operation error boundary.
 */
#pragma once
#include <stdexcept>

namespace tarmac::DataCore {
/** @brief Recoverable data-operation failure carrying a diagnostic.
 * Owns its message; no shared state. Concrete adapters may derive more specific
 * errors. Allocation errors and user-record exceptions may propagate separately.
 */
class Error : public std::runtime_error {
public:
    /** @brief Own a diagnostic message.
     * @param message Human-readable context; must point to a terminated string.
     * @throws std::bad_alloc If message allocation fails.
     */
    explicit Error(const char* message) : std::runtime_error(message) {}
};
}
