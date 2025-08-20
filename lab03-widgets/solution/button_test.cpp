#include "button.hpp"
#include <type_traits>
#include <utility>
// NOLINTNEXTLINE(readability-duplicate-include)
#include "button.hpp"  // Ensure include guards
#include "doctest.h"
#include "test_utils.hpp"

// NOLINTBEGIN(readability-function-cognitive-complexity)
// NOLINTBEGIN(misc-use-anonymous-namespace)

static_assert(!std::is_convertible_v<std::string, widgets::button>);
static_assert(!std::is_copy_constructible_v<widgets::button>);
static_assert(!std::is_copy_assignable_v<widgets::button>);
static_assert(!std::is_move_constructible_v<widgets::button>);
static_assert(!std::is_move_assignable_v<widgets::button>);

TEST_CASE("button works") {
    widgets::button btn("hello world");
    CHECK_DIMENSIONS(btn, 98, 22);
    CHECK(std::as_const(btn).label() == "hello world");
#ifdef TEST_PARENT
    widgets::container *parent = std::as_const(btn).parent();
    CHECK(parent == nullptr);
#endif
#ifdef TEST_CHILD_AT
    CHECK(btn.child_at(0, 0) == &btn);
    CHECK(btn.child_at(97, 21) == &btn);
    CHECK(btn.child_at(98, 22) == nullptr);
    {
        widgets::widget *w = btn.child_at(0, 0);
        CHECK(w == &btn);
    }
#endif

    btn.label("wow");
    CHECK_DIMENSIONS(btn, 34, 22);
    CHECK(btn.label() == "wow");

    btn.label("");
    CHECK_DIMENSIONS(btn, 10, 22);
    CHECK(btn.label() == "");
}

TEST_CASE("make_button") {
    const std::unique_ptr<widgets::button> btn = widgets::make_button("hi");
    CHECK_DIMENSIONS(*btn, 26, 22);
}

// NOLINTEND(misc-use-anonymous-namespace)
// NOLINTEND(readability-function-cognitive-complexity)
