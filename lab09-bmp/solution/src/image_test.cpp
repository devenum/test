#include "image.hpp"
#include <type_traits>
#include "doctest.h"

TEST_CASE("image is a struct") {
    [[maybe_unused]] lab_bmp::image *image = nullptr;
}

TEST_CASE("image is copyable and movable") {
    CHECK(std::is_copy_constructible_v<lab_bmp::image>);
    CHECK(std::is_move_constructible_v<lab_bmp::image>);
    CHECK(std::is_copy_assignable_v<lab_bmp::image>);
    CHECK(std::is_move_assignable_v<lab_bmp::image>);
}
