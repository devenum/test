#ifndef TEST_UTILS_HPP_
#define TEST_UTILS_HPP_

#include <iostream>  // I/O for doctest

// Use macro so line numbers are preserved in doctest's output.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CHECK_DIMENSIONS(w, expected_width, expected_height)      \
    do {                                                          \
        CHECK(                                                    \
            static_cast<const ::widgets::widget &>(w).width() ==  \
            (expected_width)                                      \
        );                                                        \
        CHECK(                                                    \
            static_cast<const ::widgets::widget &>(w).height() == \
            (expected_height)                                     \
        );                                                        \
    } while (0)

#define TEST_CHILD_AT
#define TEST_BALL_ICON
#define TEST_BOX
#define TEST_GRID
#define TEST_PARENT

#endif  // TEST_UTILS_HPP_
