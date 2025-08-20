#include "box.hpp"
#include <type_traits>
#include <utility>
// NOLINTNEXTLINE(readability-duplicate-include)
#include "box.hpp"  // Ensure include guards
#include "doctest.h"
#include "placeholder.hpp"
#include "test_utils.hpp"

// NOLINTBEGIN(readability-function-cognitive-complexity)
// NOLINTBEGIN(misc-use-anonymous-namespace)
#ifdef TEST_BOX
static_assert(!std::is_convertible_v<widgets::box::kind, widgets::box>);
static_assert(std::is_convertible_v<widgets::box *, widgets::container *>);

TEST_CASE("horizontal box works and mutates") {
    widgets::box bx(widgets::box::kind::HORIZONTAL);
#ifdef TEST_PARENT
    CHECK(bx.parent() == nullptr);
#endif
    CHECK_DIMENSIONS(bx, 0, 0);
    CHECK(std::as_const(bx).size() == 0);

    auto *pa = dynamic_cast<widgets::placeholder *>(
        bx.add(widgets::make_placeholder(10, 15))
    );
    auto *pb = dynamic_cast<widgets::placeholder *>(
        bx.add(widgets::make_placeholder(20, 10))
    );

    CHECK(std::as_const(bx).size() == 2);
    CHECK(std::as_const(bx).get(0) == pa);
    CHECK(std::as_const(bx).get(1) == pb);
    {
        widgets::widget *w = std::as_const(bx).get(0);
        CHECK(w == pa);
    }
#ifdef TEST_PARENT
    CHECK(std::as_const(*pa).parent() == &bx);
    CHECK(std::as_const(*pb).parent() == &bx);
#endif
    CHECK_DIMENSIONS(bx, 30, 15);

    CHECK(bx.child_at(0, 0) == pa);
    CHECK(bx.child_at(10, 0) == nullptr);
    CHECK(bx.child_at(10, 8) == pb);
    CHECK(bx.child_at(10, 14) == nullptr);

    pb->m_height = 50;
    static_cast<widgets::container &>(bx).update_layout();
    CHECK_DIMENSIONS(bx, 30, 50);
    CHECK(bx.child_at(0, 0) == nullptr);
    CHECK(bx.child_at(10, 14) == pb);

    const std::unique_ptr<widgets::widget> pa_owned = bx.remove(0);
    CHECK(pa_owned.get() == pa);
    CHECK(bx.size() == 1);
    CHECK(bx.get(0) == pb);
#ifdef TEST_PARENT
    CHECK(pa->parent() == nullptr);
    CHECK(pb->parent() == &bx);
#endif
    CHECK_DIMENSIONS(bx, 20, 50);
    CHECK(bx.child_at(0, 0) == pb);
}

TEST_CASE("vertical box has correct dimensions") {
    widgets::box bx(widgets::box::kind::VERTICAL);
#ifdef TEST_PARENT
    CHECK(bx.parent() == nullptr);
#endif
    CHECK_DIMENSIONS(bx, 0, 0);
    CHECK(std::as_const(bx).size() == 0);

    bx.add(widgets::make_placeholder(10, 15));
    bx.add(widgets::make_placeholder(20, 10));

    CHECK_DIMENSIONS(bx, 20, 25);
}

TEST_CASE("box inside box") {
    widgets::box bx(widgets::box::kind::VERTICAL);
    widgets::widget *bx2 =
        bx.add(widgets::make_box(widgets::box::kind::HORIZONTAL));
#ifdef TEST_PARENT
    CHECK(bx2->parent() == &bx);
#endif
    CHECK_DIMENSIONS(*bx2, 0, 0);
    CHECK_DIMENSIONS(bx, 0, 0);
}

TEST_CASE("make_box") {
    const std::unique_ptr<widgets::box> bx =
        widgets::make_box(widgets::box::kind::HORIZONTAL);
    CHECK_DIMENSIONS(*bx, 0, 0);
}
#endif  // TEST_BOX
// NOLINTEND(misc-use-anonymous-namespace)
// NOLINTEND(readability-function-cognitive-complexity)
