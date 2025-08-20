#include <memory>
#include <utility>
#include "abstract_widgets.hpp"
#include "ball_icon.hpp"
#include "box.hpp"
#include "button.hpp"
#include "doctest.h"
#include "grid.hpp"
#include "placeholder.hpp"
#include "test_utils.hpp"

// NOLINTBEGIN(readability-function-cognitive-complexity)
// NOLINTBEGIN(misc-use-anonymous-namespace)

#ifdef TEST_BOX
TEST_CASE("Combine inside a box") {
    widgets::box bx1(widgets::box::kind::VERTICAL);
    auto *ball = bx1.add(widgets::make_ball_icon(4));
    auto *btn = bx1.add(widgets::make_button("h"));

    auto bx2 = widgets::make_box(widgets::box::kind::HORIZONTAL);
    auto *p1 = bx2->add(widgets::make_placeholder(7, 5));
    auto *p2 = bx2->add(widgets::make_placeholder(7, 6));
    auto *p3 = bx2->add(widgets::make_placeholder(7, 7));
    bx1.add(std::move(bx2));

    CHECK_DIMENSIONS(bx1, 21, 9 + 22 + 7);
    CHECK(bx1.child_at(5, 4) == nullptr);
    CHECK(bx1.child_at(10, 4) == ball);
    CHECK(bx1.child_at(10, 9 + 11) == btn);
    CHECK(bx1.child_at(10, 9 + 22) == p2);
    CHECK(bx1.child_at(0, 9 + 22) == nullptr);
    CHECK(bx1.child_at(0, 9 + 22 + 1) == p1);
    CHECK(bx1.child_at(15, 9 + 22) == p3);

    // All coordinates in `child_at` are relative, widget does not know about
    // its parents.
    CHECK(p1->child_at(6, 0) == p1);
    CHECK(p1->child_at(7, 0) == nullptr);
    CHECK(p3->child_at(-1, -1) == nullptr);
    CHECK(p3->child_at(0, 0) == p3);
}

#ifdef TEST_GRID
TEST_CASE("Combine inside a grid inside a box") {
    auto bx1 = widgets::make_box(widgets::box::kind::HORIZONTAL);
    auto *ball = bx1->add(widgets::make_ball_icon(4));
    bx1->add(widgets::make_placeholder(0, 101));

    auto bx2 = widgets::make_box(widgets::box::kind::VERTICAL);
    auto *btn = bx2->add(widgets::make_button("h"));
    bx2->add(widgets::make_placeholder(200, 0));

    widgets::grid gr(2, 2);
    gr.add(std::move(bx1), 0, 0);
    gr.add(std::move(bx2), 1, 1);

    CHECK_DIMENSIONS(gr, 209, 123);
    CHECK(gr.child_at(0, 25) == nullptr);
    CHECK(gr.child_at(0, 50) == ball);
    CHECK(gr.child_at(109, 112) == btn);
    CHECK(gr.child_at(150, 112) == nullptr);
}
#endif  // TEST_GRID
#endif  // TEST_BOX

// NOLINTEND(misc-use-anonymous-namespace)
// NOLINTEND(readability-function-cognitive-complexity)
