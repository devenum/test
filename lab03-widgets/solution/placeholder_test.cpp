#include "placeholder.hpp"
#include <memory>
#include <type_traits>
#include <utility>
#include "doctest.h"
// NOLINTNEXTLINE(readability-duplicate-include)
#include "placeholder.hpp"  // Ensure include guards
#include "test_utils.hpp"

// NOLINTBEGIN(readability-function-cognitive-complexity)
// NOLINTBEGIN(misc-use-anonymous-namespace)

// Make use of all 'placeholder' methods and helper functions.
static_assert(!std::is_convertible_v<int, widgets::placeholder>);
static_assert(!std::is_copy_constructible_v<widgets::placeholder>);
static_assert(!std::is_copy_assignable_v<widgets::placeholder>);
static_assert(!std::is_move_constructible_v<widgets::placeholder>);
static_assert(!std::is_move_assignable_v<widgets::placeholder>);

TEST_CASE("placeholder works") {
    widgets::placeholder ico(5, 10);
    widgets::widget &icow = ico;
    CHECK_DIMENSIONS(ico, 5, 10);
#ifdef TEST_PARENT
    CHECK(std::as_const(ico).parent() == nullptr);
#endif
#ifdef TEST_CHILD_AT
    CHECK(icow.child_at(-1, 0) == nullptr);
    CHECK(icow.child_at(0, -1) == nullptr);
    CHECK(icow.child_at(0, 0) == &ico);
    CHECK(icow.child_at(4, 5) == &ico);
    CHECK(icow.child_at(4, 9) == &ico);
    CHECK(icow.child_at(5, 9) == nullptr);
    CHECK(icow.child_at(4, 10) == nullptr);
#endif
}

TEST_CASE("make_placeholder") {
    const std::unique_ptr<widgets::placeholder> ico =
        widgets::make_placeholder(10, 5);
    CHECK_DIMENSIONS(*ico, 10, 5);
}

// NOLINTEND(misc-use-anonymous-namespace)
// NOLINTEND(readability-function-cognitive-complexity)
