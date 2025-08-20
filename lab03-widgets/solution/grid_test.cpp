#include "grid.hpp"
#include <type_traits>
#include <utility>
#include "doctest.h"
// NOLINTNEXTLINE(readability-duplicate-include)
#include "grid.hpp"  // Ensure include guards
#include "placeholder.hpp"
#include "test_utils.hpp"

// NOLINTBEGIN(readability-function-cognitive-complexity)
// NOLINTBEGIN(misc-use-anonymous-namespace)

#ifdef TEST_GRID
static_assert(std::is_convertible_v<widgets::grid *, widgets::container *>);

TEST_CASE("grid works and mutates") {
    widgets::grid gr(2, 3);
#ifdef TEST_PARENT
    CHECK(gr.parent() == nullptr);
#endif
    CHECK_DIMENSIONS(gr, 0, 0);
    CHECK(std::as_const(gr).rows() == 2);
    CHECK(std::as_const(gr).columns() == 3);

    auto *pa = dynamic_cast<widgets::placeholder *>(
        gr.add(widgets::make_placeholder(10, 15), 0, 0)
    );
    auto *pb = dynamic_cast<widgets::placeholder *>(
        gr.add(widgets::make_placeholder(20, 10), 1, 1)
    );
    auto *pc = dynamic_cast<widgets::placeholder *>(
        gr.add(widgets::make_placeholder(30, 5), 1, 2)
    );

    CHECK(std::as_const(gr).get(0, 0) == pa);
    CHECK(std::as_const(gr).get(0, 1) == nullptr);
    CHECK(std::as_const(gr).get(1, 1) == pb);
    CHECK(std::as_const(gr).get(1, 2) == pc);
    {
        widgets::widget *w = std::as_const(gr).get(0, 0);
        CHECK(w == pa);
    }
#ifdef TEST_PARENT
    CHECK(std::as_const(*pa).parent() == &gr);
    CHECK(std::as_const(*pb).parent() == &gr);
    CHECK(std::as_const(*pc).parent() == &gr);
#endif
    CHECK_DIMENSIONS(gr, 60, 25);

    CHECK(gr.child_at(0, 0) == pa);
    CHECK(gr.child_at(10, 14) == nullptr);
    CHECK(gr.child_at(10, 15) == pb);
    CHECK(gr.child_at(10, 24) == pb);
    CHECK(gr.child_at(30, 14) == nullptr);
    CHECK(gr.child_at(30, 15) == pc);
    CHECK(gr.child_at(30, 24) == nullptr);

    pb->m_height = 1;
    static_cast<widgets::container &>(gr).update_layout();
    CHECK_DIMENSIONS(gr, 60, 20);
    CHECK(gr.child_at(10, 14) == nullptr);
    CHECK(gr.child_at(10, 15) == pb);
    CHECK(gr.child_at(10, 16) == nullptr);
    CHECK(gr.child_at(30, 14) == nullptr);
    CHECK(gr.child_at(30, 15) == pc);
    CHECK(gr.child_at(30, 20) == nullptr);

    const std::unique_ptr<widgets::widget> pb_owned = gr.remove(1, 1);
    CHECK(pb_owned.get() == pb);
    CHECK(gr.get(0, 0) == pa);
    CHECK(gr.get(0, 1) == nullptr);
    CHECK(gr.get(1, 1) == nullptr);
    CHECK(gr.get(1, 2) == pc);
#ifdef TEST_PARENT
    CHECK(pa->parent() == &gr);
    CHECK(pb->parent() == nullptr);
    CHECK(pc->parent() == &gr);
#endif
    CHECK_DIMENSIONS(gr, 40, 20);
    CHECK(gr.child_at(10, 15) == pc);
}

TEST_CASE("make_grid") {
    const std::unique_ptr<widgets::grid> gr = widgets::make_grid(5, 5);
    CHECK_DIMENSIONS(*gr, 0, 0);
}
#endif  // TEST_GRID

// NOLINTEND(misc-use-anonymous-namespace)
// NOLINTEND(readability-function-cognitive-complexity)
