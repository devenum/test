#include "ball_icon.hpp"
#include <memory>
#include <type_traits>
#include <utility>
// NOLINTNEXTLINE(readability-duplicate-include)
#include "ball_icon.hpp"  // Ensure include guards
#include "doctest.h"
#include "test_utils.hpp"

// NOLINTBEGIN(readability-function-cognitive-complexity)
// NOLINTBEGIN(misc-use-anonymous-namespace)

#ifdef TEST_BALL_ICON
static_assert(!std::is_convertible_v<int, widgets::ball_icon>);
static_assert(!std::is_copy_constructible_v<widgets::ball_icon>);
static_assert(!std::is_copy_assignable_v<widgets::ball_icon>);
static_assert(!std::is_move_constructible_v<widgets::ball_icon>);
static_assert(!std::is_move_assignable_v<widgets::ball_icon>);

TEST_CASE("ball_icon works") {
    widgets::ball_icon ico(10);
    widgets::widget &icow = ico;
    CHECK_DIMENSIONS(ico, 21, 21);
    CHECK(std::as_const(ico).radius() == 10);
#ifdef TEST_PARENT
    CHECK(std::as_const(ico).parent() == nullptr);
#endif
#ifdef TEST_CHILD_AT
    CHECK(icow.child_at(0, 0) == nullptr);
    CHECK(icow.child_at(5, 5) == &ico);
    CHECK(icow.child_at(20, 10) == &ico);
    CHECK(icow.child_at(21, 10) == nullptr);
#endif

    ico.radius(20);
    CHECK_DIMENSIONS(ico, 41, 41);
    CHECK(ico.radius() == 20);
}

TEST_CASE("make_ball_icon") {
    const std::unique_ptr<widgets::ball_icon> ico = widgets::make_ball_icon(10);
    CHECK_DIMENSIONS(*ico, 21, 21);
}

TEST_CASE("Euclidean distance vs Manhattan distance") {
    widgets::ball_icon ico(5);
    widgets::widget &icow = ico;
    CHECK(std::as_const(ico).radius() == 5);
#ifdef TEST_CHILD_AT
    // if Euclidean, then &ico (2^2 + 4^2 = 20 < 25 = 5^5)
    // if Manhattan, then nullptr (2 + 4 > 5)
    CHECK(icow.child_at(2, 4) == &ico);
#endif
}

#endif  // TEST_BALL_ICON

// NOLINTEND(misc-use-anonymous-namespace)
// NOLINTEND(readability-function-cognitive-complexity)
