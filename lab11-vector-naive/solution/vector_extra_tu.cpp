#include <memory>
#include <string>
#include "vector.hpp"  // Ensure that including the header in a separate TU does not induce ODR violation.
// NOLINTNEXTLINE(readability-duplicate-include)
#include "vector.hpp"  // Ensure that double inclusion does not break anything.

namespace {
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
[[maybe_unused]] lab_vector_naive::vector<std::string> vec_string;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
[[maybe_unused]] lab_vector_naive::vector<int> vec_int;
}  // namespace
