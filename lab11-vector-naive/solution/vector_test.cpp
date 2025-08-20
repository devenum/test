#include "vector.hpp"
#include <cstddef>
#include <iostream>  // https://github.com/onqtam/doctest/issues/356
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#define DOCTEST_CONFIG_VOID_CAST_EXPRESSIONS
#include "doctest.h"

#define TEST_ADDITIONAL_OPS
#define TEST_EXTEND_OPS
#define TEST_CAREFUL_CONSTRUCTORS
#define TEST_VERY_FORMALLY_EFFICIENT

// NOLINTBEGIN(readability-function-cognitive-complexity)
// NOLINTBEGIN(misc-use-anonymous-namespace)

#ifdef TEST_STD_VECTOR
using std::vector;
#else
using lab_vector_naive::vector;
#endif

#ifdef TEST_STD_VECTOR
TEST_CASE("Default-initialize std::vector<std::string>")
#else
TEST_CASE("Default-initialize lab_vector_naive::vector<std::string>")
#endif
{
    // `const` ensures all fields are initialized.
    const vector<std::string> x;
    CHECK(x.empty());
    CHECK(x.size() == 0);
    CHECK(x.capacity() == 0);
#ifdef TEST_ADDITIONAL_OPS
    CHECK_THROWS_AS(x.at(0), std::out_of_range);
    CHECK_THROWS_AS(x.at(1), std::out_of_range);
#endif  // TEST_ADDITIONAL_OPS
}

TEST_CASE("Default-copy-initialize") {
    const vector<std::string> x = {};
    CHECK(x.empty());
    CHECK(x.size() == 0);
    CHECK(x.capacity() == 0);
#ifdef TEST_ADDITIONAL_OPS
    CHECK_THROWS_AS(x.at(0), std::out_of_range);
    CHECK_THROWS_AS(x.at(1), std::out_of_range);
#endif  // TEST_ADDITIONAL_OPS
}

TEST_CASE("Constructor from size_t is explicit") {
    CHECK(std::is_constructible_v<vector<std::string>, std::size_t>);
    CHECK(!std::is_convertible_v<std::size_t, vector<std::string>>);
}

TEST_CASE("Constructor from (size_t, T) is implicit (since C++11)") {
    // For compatibility with std::vector<>.
    const vector<std::string> vec = {5, std::string("hi")};
    CHECK(vec.size() == 5);
}

struct MinimalObj {
    int id;  // NOLINT(misc-non-private-member-variables-in-classes)

    // Draft check for leaks, double-frees and non-inits.
    // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
    std::string data = std::string(500U, 'x');

    explicit MinimalObj(int id_) : id(id_) {
        CHECK(data.size() == 500U);
    }

    MinimalObj(MinimalObj &&) = default;
    MinimalObj &operator=(MinimalObj &&) = default;

#ifdef TEST_CAREFUL_CONSTRUCTORS
    MinimalObj(const MinimalObj &) = delete;
    MinimalObj &operator=(const MinimalObj &) = delete;
#else
    MinimalObj() : MinimalObj(100) {
    }

    MinimalObj(const MinimalObj &) = default;
    MinimalObj &operator=(const MinimalObj &) = default;
#endif  // TEST_CAREFUL_CONSTRUCTORS

    ~MinimalObj() = default;
};

struct ObjWithDefaultCtor : MinimalObj {
    using MinimalObj::MinimalObj;

    explicit ObjWithDefaultCtor() : MinimalObj(100) {
    }
};

static_assert(std::is_default_constructible_v<ObjWithDefaultCtor>);

struct ObjWithCopyCtor : MinimalObj {
    using MinimalObj::MinimalObj;

    ObjWithCopyCtor(const ObjWithCopyCtor &other) : MinimalObj(other.id) {
    }
#ifdef TEST_CAREFUL_CONSTRUCTORS
    ObjWithCopyCtor &operator=(const ObjWithCopyCtor &) = delete;
#else
    ObjWithCopyCtor &operator=(const ObjWithCopyCtor &) = default;
#endif  // TEST_CAREFUL_CONSTRUCTORS

    ObjWithCopyCtor(ObjWithCopyCtor &&) = default;
    ObjWithCopyCtor &operator=(ObjWithCopyCtor &&) = default;

    ~ObjWithCopyCtor() = default;
};

struct ObjWithCopyAssignment : MinimalObj {
    using MinimalObj::MinimalObj;

    ObjWithCopyAssignment(const ObjWithCopyAssignment &other)
        : MinimalObj(other.id) {
    }

    ObjWithCopyAssignment &operator=(const ObjWithCopyAssignment &other) {
        id = other.id;
        return *this;
    }

    ObjWithCopyAssignment(ObjWithCopyAssignment &&) = default;
    ObjWithCopyAssignment &operator=(ObjWithCopyAssignment &&) = default;

    ~ObjWithCopyAssignment() = default;
};

TEST_CASE("Construct empty") {
    SUBCASE("explicit") {
        const vector<MinimalObj> v;

        CHECK(v.empty());
    }

    SUBCASE("implicit") {
        const vector<MinimalObj> v = {};

        CHECK(v.empty());
    }
}

TEST_CASE("Construct zero elements") {
    const vector<ObjWithDefaultCtor> v(0);
    CHECK(v.empty());
    CHECK(v.size() == 0);
#ifndef TEST_STD_VECTOR
    CHECK(v.capacity() == 0);
#endif
}

TEST_CASE("Construct n elements and read") {
    const vector<ObjWithDefaultCtor> v(5);

    CHECK(!v.empty());
    REQUIRE(v.size() == 5);
#ifndef TEST_STD_VECTOR
    CHECK(v.capacity() == 8);
#endif

    CHECK(v[0].id == 100);
    CHECK(v[1].id == 100);
    CHECK(v[2].id == 100);
    CHECK(v[3].id == 100);
    CHECK(v[4].id == 100);
}

TEST_CASE("Construct n copies elements and read") {
    const ObjWithCopyCtor obj(10);
    vector<ObjWithCopyCtor> v(5, obj);

    CHECK(!v.empty());
    REQUIRE(v.size() == 5);
#ifndef TEST_STD_VECTOR
    CHECK(v.capacity() == 8);
#endif

    CHECK(v[0].id == 10);
    CHECK(v[1].id == 10);
    CHECK(v[2].id == 10);
    CHECK(v[3].id == 10);
    CHECK(v[4].id == 10);
}

TEST_CASE("Construct n copies of temporary and read") {
    const vector<std::string> v(5, std::string(1000, 'x'));

    CHECK(!v.empty());
    REQUIRE(v.size() == 5);
#ifndef TEST_STD_VECTOR
    CHECK(v.capacity() == 8);
#endif
    CHECK(v[0] == std::string(1000, 'x'));
    CHECK(v[1] == std::string(1000, 'x'));
    CHECK(v[2] == std::string(1000, 'x'));
    CHECK(v[3] == std::string(1000, 'x'));
    CHECK(v[4] == std::string(1000, 'x'));
}

struct OverloadedAmpersand {
    // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
    std::string s = std::string(1000, 'x');

    // Technically possible, almost never used, has to be handled.
    // NOLINTNEXTLINE(google-runtime-operator)
    void operator&() const {
    }
};

TEST_CASE("Construct n elements with overloaded ampersand and read") {
    const vector<OverloadedAmpersand> v(5);
    CHECK(!v.empty());
    REQUIRE(v.size() == 5);
#ifndef TEST_STD_VECTOR
    CHECK(v.capacity() == 8);
#endif
    CHECK(v[0].s == std::string(1000, 'x'));
    CHECK(v[1].s == std::string(1000, 'x'));
    CHECK(v[2].s == std::string(1000, 'x'));
    CHECK(v[3].s == std::string(1000, 'x'));
    CHECK(v[4].s == std::string(1000, 'x'));
}

#ifdef TEST_EXTEND_OPS
TEST_CASE("push_back moves") {
    vector<MinimalObj> v;
    v.push_back(MinimalObj(10));
    v.push_back(MinimalObj(11));
    v.push_back(MinimalObj(12));
    v.push_back(MinimalObj(13));
    v.push_back(MinimalObj(14));

    CHECK(!v.empty());
    REQUIRE(v.size() == 5);
#ifndef TEST_STD_VECTOR
    CHECK(v.capacity() == 8);
#endif
    CHECK(v[0].id == 10);
    CHECK(v[1].id == 11);
    CHECK(v[2].id == 12);
    CHECK(v[3].id == 13);
    CHECK(v[4].id == 14);
}

TEST_CASE("push_back copies") {
    vector<ObjWithCopyCtor> v;
    const ObjWithCopyCtor obj(10);
    v.push_back(obj);
    v.push_back(obj);
    v.push_back(obj);

    CHECK(!v.empty());
    REQUIRE(v.size() == 3);
#ifndef TEST_STD_VECTOR
    CHECK(v.capacity() == 4);
#endif
    CHECK(v[0].id == 10);
    CHECK(v[1].id == 10);
    CHECK(v[2].id == 10);
}

TEST_CASE("push_back copies from itself") {
    vector<std::string> v(2);
    v[0] = std::string(1000, 'x');
    v[1] = std::string(1000, 'y');

    v.push_back(v[0]);

    REQUIRE(v.size() == 3);
    CHECK(v[0] == std::string(1000, 'x'));
    CHECK(v[1] == std::string(1000, 'y'));
    CHECK(v[2] == std::string(1000, 'x'));
}

TEST_CASE(
    "push_back reallocates non-trivially-copyable non-trivially-relocatable "
    "types"
) {
    struct AlwaysHoldsData {
        // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
        std::string s = std::string(500, 'A');
        // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
        AlwaysHoldsData *me = this;

        AlwaysHoldsData() = default;

        AlwaysHoldsData(AlwaysHoldsData &&) noexcept {
        }

        AlwaysHoldsData &operator=(AlwaysHoldsData &&) noexcept {
            return *this;
        }

        AlwaysHoldsData(const AlwaysHoldsData &) noexcept {
        }

        // NOLINTNEXTLINE(bugprone-unhandled-self-assignment)
        AlwaysHoldsData &operator=(const AlwaysHoldsData &) noexcept {
            return *this;
        }

        ~AlwaysHoldsData() = default;
    };

    vector<AlwaysHoldsData> v;
    v.push_back(AlwaysHoldsData());
    v.push_back(AlwaysHoldsData());
    v.push_back(AlwaysHoldsData());
    v.push_back(AlwaysHoldsData());
    v.push_back(AlwaysHoldsData());
    CHECK(v[0].me == &v[0]);
    CHECK(v[1].me == &v[1]);
    CHECK(v[2].me == &v[2]);
    CHECK(v[3].me == &v[3]);
    CHECK(v[4].me == &v[4]);
}
#endif  // TEST_EXTEND_OPS

#ifdef TEST_ADDITIONAL_OPS
TEST_CASE("Copy-construct") {
    vector<ObjWithCopyCtor> orig(5, ObjWithCopyCtor(123));
    orig[0].id = 10;
    orig[1].id = 11;
    orig[2].id = 12;
    orig[3].id = 13;
    orig[4].id = 14;
    // To check whether copy chooses minimal possible capacity.
    orig.pop_back();
    orig.pop_back();

    auto check_vec = [](const vector<ObjWithCopyCtor> &v) {
        CHECK(!v.empty());
        REQUIRE(v.size() == 3);
        CHECK(v[0].id == 10);
        CHECK(v[1].id == 11);
        CHECK(v[2].id == 12);
    };

    const auto &orig_const = orig;
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    const vector<ObjWithCopyCtor> v = orig_const;

    INFO("target");
    check_vec(v);

    INFO("origin");
    check_vec(orig);

#ifndef TEST_STD_VECTOR
    CHECK(orig.capacity() == 8);
    CHECK(v.capacity() == 4);
#endif
}

TEST_CASE("Copy-assign") {
    vector<ObjWithCopyAssignment> orig(5, ObjWithCopyAssignment(123));
    orig[0].id = 10;
    orig[1].id = 11;
    orig[2].id = 12;
    orig[3].id = 13;
    orig[4].id = 14;
    // To check whether copy chooses minimal possible capacity.
    orig.pop_back();
    orig.pop_back();

    auto check_vec = [](const vector<ObjWithCopyAssignment> &v) {
        CHECK(!v.empty());
        REQUIRE(v.size() == 3);
        CHECK(v[0].id == 10);
        CHECK(v[1].id == 11);
        CHECK(v[2].id == 12);
    };

    auto test_copy_to = [&](vector<ObjWithCopyAssignment> &v) {
        const auto &orig_const = orig;
        CHECK(&(v = orig_const) == &v);
        INFO("target");
        check_vec(v);
        INFO("origin");
        check_vec(orig);
    };

    SUBCASE("to empty") {
        vector<ObjWithCopyAssignment> v;
        test_copy_to(v);
#ifndef TEST_STD_VECTOR
        CHECK(orig.capacity() == 8);
        CHECK(v.capacity() == 4);
#endif
    }

    SUBCASE("to shorter non-empty") {
        vector<ObjWithCopyAssignment> v(3, ObjWithCopyAssignment(20));
        test_copy_to(v);
#ifndef TEST_STD_VECTOR
        CHECK(orig.capacity() == 8);
        CHECK(v.capacity() == 4);
#endif
    }

    SUBCASE("to longer non-empty") {
        vector<ObjWithCopyAssignment> v(7, ObjWithCopyAssignment(20));
        [[maybe_unused]] void *old_buffer = &v[0];
        test_copy_to(v);
#ifndef TEST_STD_VECTOR
        CHECK(orig.capacity() == 8);
#ifdef TEST_VERY_FORMALLY_EFFICIENT
        // We don't have to re-create buffer because assignment cannot throw.
        CHECK(v.capacity() == 8);
        CHECK(old_buffer == &v[0]);
#else
        // We may recreate buffer if needed
        if (old_buffer == &v[0]) {
            CHECK(v.capacity() == 8);
        } else {
            CHECK(v.capacity() == 4);
        }
#endif
#endif
    }

    SUBCASE("to self") {
        [[maybe_unused]] ObjWithCopyAssignment *orig_buf = &orig[0];
        test_copy_to(orig);
#ifdef TEST_CAREFUL_CONSTRUCTORS
        // Ensure there were no extra copies.
        CHECK(&orig[0] == orig_buf);
#ifndef TEST_STD_VECTOR
        CHECK(orig.capacity() == 8);
#endif
#endif  // TEST_CAREFUL_CONSTRUCTORS
    }
}
#endif  // TEST_ADDITIONAL_OPS

TEST_CASE("Move-construct") {
#ifdef TEST_CAREFUL_CONSTRUCTORS
    vector<MinimalObj> orig;
    orig.push_back(MinimalObj(10));
    orig.push_back(MinimalObj(11));
    orig.push_back(MinimalObj(12));
    orig.push_back(MinimalObj(13));
    orig.push_back(MinimalObj(14));
#else
    vector<MinimalObj> orig(5);
    orig[0].id = 10;
    orig[1].id = 11;
    orig[2].id = 12;
    orig[3].id = 13;
    orig[4].id = 14;
#endif

    MinimalObj *orig_buf = &orig[0];

    const vector<MinimalObj> v = std::move(orig);
    CHECK(!v.empty());
    REQUIRE(v.size() == 5);
#ifndef TEST_STD_VECTOR
    CHECK(v.capacity() == 8);
#endif
    CHECK(v[0].id == 10);
    CHECK(v[1].id == 11);
    CHECK(v[2].id == 12);
    CHECK(v[3].id == 13);
    CHECK(v[4].id == 14);

    // Ensure there were no extra copies.
    CHECK(&v[0] == orig_buf);

#ifndef TEST_STD_VECTOR
    // NOLINTNEXTLINE(bugprone-use-after-move)
    CHECK(orig.empty());
    // NOLINTNEXTLINE(bugprone-use-after-move)
    CHECK(orig.size() == 0);
    // NOLINTNEXTLINE(bugprone-use-after-move)
    CHECK(orig.capacity() == 0);
#endif
}

TEST_CASE("Move-assign") {
#ifdef TEST_CAREFUL_CONSTRUCTORS
    vector<MinimalObj> orig;
    orig.push_back(MinimalObj(10));
    orig.push_back(MinimalObj(11));
    orig.push_back(MinimalObj(12));
    orig.push_back(MinimalObj(13));
    orig.push_back(MinimalObj(14));
#else
    vector<MinimalObj> orig(5);
    orig[0].id = 10;
    orig[1].id = 11;
    orig[2].id = 12;
    orig[3].id = 13;
    orig[4].id = 14;
#endif

    MinimalObj *orig_buf = &orig[0];

    auto test_move_to_with_expected_capacity =
        [&](vector<MinimalObj> &v,
            [[maybe_unused]] std::size_t expected_capacity) {
            CHECK(&(v = std::move(orig)) == &v);

            CHECK(!v.empty());
            REQUIRE(v.size() == 5);
#ifndef TEST_STD_VECTOR
            CHECK(v.capacity() == 8);
#endif
            CHECK(v[0].id == 10);
            CHECK(v[1].id == 11);
            CHECK(v[2].id == 12);
            CHECK(v[3].id == 13);
            CHECK(v[4].id == 14);

            // Ensure there were no extra copies.
            CHECK(&v[0] == orig_buf);

#ifndef TEST_STD_VECTOR
            // NOLINTNEXTLINE(bugprone-use-after-move)
            CHECK(orig.empty());
            // NOLINTNEXTLINE(bugprone-use-after-move)
            CHECK(orig.size() == 0);
            // NOLINTNEXTLINE(bugprone-use-after-move)
            CHECK(orig.capacity() == expected_capacity);
#endif
        };

    SUBCASE("to empty") {
        vector<MinimalObj> v;
        test_move_to_with_expected_capacity(v, 0);
    };

    SUBCASE("to non-empty") {
#ifdef TEST_CAREFUL_CONSTRUCTORS
        vector<MinimalObj> v;
        v.push_back(MinimalObj(100));
        v.push_back(MinimalObj(101));
        v.push_back(MinimalObj(102));
#else
        vector<MinimalObj> v(3);
        v[0].id = 100;
        v[1].id = 101;
        v[2].id = 102;
#endif
        // We have two buffers; no need to deallocate either of them.
        test_move_to_with_expected_capacity(v, 4);
    };

    SUBCASE("to self") {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
#endif
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wself-move"
#endif
        orig = std::move(orig);
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
#ifdef __clang__
#pragma clang diagnostic pop
#endif
        // No further checks on the state.
    };
}

TEST_CASE("Elements are consecutive") {
    vector<ObjWithDefaultCtor> v(5);

    CHECK(&v[0] + 1 == &v[1]);
    CHECK(&v[0] + 2 == &v[2]);
    CHECK(&v[0] + 3 == &v[3]);
    CHECK(&v[0] + 4 == &v[4]);
}

TEST_CASE("Write to non-const") {
    vector<ObjWithDefaultCtor> v(5);

    v[0].id = 15;
    CHECK(v[0].id == 15);

#ifdef TEST_ADDITIONAL_OPS
    v.at(1).id = 16;
    CHECK(v[1].id == 16);

    CHECK(&v[0] == &v.at(0));
    CHECK(&v[1] == &v.at(1));
    CHECK_THROWS_AS(v.at(5), std::out_of_range);
    CHECK_THROWS_AS(v.at(1'000'000'000), std::out_of_range);
#endif  // TEST_ADDITIONAL_OPS
}

TEST_CASE("Read from const") {
    vector<ObjWithDefaultCtor> orig(5);
    orig[0].id = 10;
    orig[1].id = 11;
    orig[2].id = 12;
    orig[3].id = 13;
    orig[4].id = 14;

    const vector<ObjWithDefaultCtor> &v = orig;
    CHECK(!v.empty());
    CHECK(v.size() == 5);
#ifndef TEST_STD_VECTOR
    CHECK(v.capacity() == 8);
#endif
    CHECK(v[0].id == 10);
#ifdef TEST_ADDITIONAL_OPS
    CHECK(v.at(0).id == 10);
    CHECK_THROWS_AS(v.at(5), std::out_of_range);
    CHECK_THROWS_AS(v.at(1'000'000'000), std::out_of_range);
#endif  // TEST_ADDITIONAL_OPS
}

#ifdef TEST_EXTEND_OPS
TEST_CASE("reserve") {
    vector<MinimalObj> v;
    v.push_back(MinimalObj(10));
    v.push_back(MinimalObj(11));
    v.push_back(MinimalObj(12));
    v.push_back(MinimalObj(13));
    v.push_back(MinimalObj(14));

    auto check_vec_and_capacity =
        [&v]([[maybe_unused]] std::size_t expected_capacity) {
            CHECK(!v.empty());
            REQUIRE(v.size() == 5);
#ifndef TEST_STD_VECTOR
            CHECK(v.capacity() == expected_capacity);
#endif
            CHECK(v[0].id == 10);
            CHECK(v[1].id == 11);
            CHECK(v[2].id == 12);
            CHECK(v[3].id == 13);
            CHECK(v[4].id == 14);
        };

    SUBCASE("reserve to size") {
        v.reserve(5);
        check_vec_and_capacity(8);
    }

    SUBCASE("reserve decreases") {
        v.reserve(1);
        check_vec_and_capacity(8);
    }

    SUBCASE("reserve to capacity") {
        v.reserve(8);
        check_vec_and_capacity(8);
    }

    SUBCASE("reserve bigger than capacity") {
        v.reserve(9);
        check_vec_and_capacity(16);
    }

    SUBCASE("reserve much bigger than capacity") {
        v.reserve(100);
        check_vec_and_capacity(128);
    }
}
#endif  // TEST_EXTEND_OPS

TEST_CASE("pop_back") {
#ifdef TEST_CAREFUL_CONSTRUCTORS
    vector<MinimalObj> v;
    v.push_back(MinimalObj(10));
    v.push_back(MinimalObj(11));
    v.push_back(MinimalObj(12));
    v.push_back(MinimalObj(13));
    v.push_back(MinimalObj(14));
#else
    vector<MinimalObj> v(5);
    v[0].id = 10;
    v[1].id = 11;
    v[2].id = 12;
    v[3].id = 13;
    v[4].id = 14;
#endif
    v.pop_back();
    v.pop_back();

    CHECK(!v.empty());
    REQUIRE(v.size() == 3);
#ifndef TEST_STD_VECTOR
    CHECK(v.capacity() == 8);
#endif
    CHECK(v[0].id == 10);
    CHECK(v[1].id == 11);
    CHECK(v[2].id == 12);
}

#ifdef TEST_EXTEND_OPS
TEST_CASE("pop_back with push_back") {
    vector<MinimalObj> v;
    v.push_back(MinimalObj(10));
    v.push_back(MinimalObj(11));
    v.push_back(MinimalObj(12));
    v.pop_back();
    v.pop_back();
    v.pop_back();

    CHECK(v.empty());
    CHECK(v.size() == 0);
#ifndef TEST_STD_VECTOR
    CHECK(v.capacity() == 4);
#endif

    v.push_back(MinimalObj(13));

    CHECK(!v.empty());
    REQUIRE(v.size() == 1);
#ifndef TEST_STD_VECTOR
    CHECK(v.capacity() == 4);
#endif
    CHECK(v[0].id == 13);

    v.pop_back();

    CHECK(v.empty());
    CHECK(v.size() == 0);
#ifndef TEST_STD_VECTOR
    CHECK(v.capacity() == 4);
#endif
}
#endif  // TEST_EXTEND_OPS

#ifdef TEST_ADDITIONAL_OPS
TEST_CASE("clear") {
#ifdef TEST_CAREFUL_CONSTRUCTORS
    vector<MinimalObj> v;
    v.push_back(MinimalObj(10));
    v.push_back(MinimalObj(11));
    v.push_back(MinimalObj(12));
    v.push_back(MinimalObj(13));
    v.push_back(MinimalObj(14));
#else
    vector<MinimalObj> v(5);
    v[0].id = 10;
    v[1].id = 11;
    v[2].id = 12;
    v[3].id = 13;
    v[4].id = 14;
#endif

    v.clear();

    CHECK(v.empty());
    CHECK(v.size() == 0);
#ifndef TEST_STD_VECTOR
    CHECK(v.capacity() == 8);
#endif
}
#endif  // TEST_ADDITIONAL_OPS

#ifdef TEST_EXTEND_OPS
TEST_CASE("resize default constructible") {
    vector<ObjWithDefaultCtor> v;
    v.push_back(ObjWithDefaultCtor(10));
    v.push_back(ObjWithDefaultCtor(11));
    v.push_back(ObjWithDefaultCtor(12));
    v.push_back(ObjWithDefaultCtor(13));
    v.push_back(ObjWithDefaultCtor(14));

    SUBCASE("to size") {
        v.resize(5);

        CHECK(!v.empty());
        REQUIRE(v.size() == 5);
#ifndef TEST_STD_VECTOR
        CHECK(v.capacity() == 8);
#endif
        CHECK(v[0].id == 10);
        CHECK(v[1].id == 11);
        CHECK(v[2].id == 12);
        CHECK(v[3].id == 13);
        CHECK(v[4].id == 14);
    }

    SUBCASE("to shorter") {
        v.resize(3);

        CHECK(!v.empty());
        REQUIRE(v.size() == 3);
#ifndef TEST_STD_VECTOR
        CHECK(v.capacity() == 8);
#endif
        CHECK(v[0].id == 10);
        CHECK(v[1].id == 11);
        CHECK(v[2].id == 12);
    }

    SUBCASE("to zero") {
        v.resize(0);

        CHECK(v.empty());
        CHECK(v.size() == 0);
#ifndef TEST_STD_VECTOR
        CHECK(v.capacity() == 8);
#endif
    }

    SUBCASE("to longer without reallocation") {
        v.resize(7);

        CHECK(!v.empty());
        REQUIRE(v.size() == 7);
#ifndef TEST_STD_VECTOR
        CHECK(v.capacity() == 8);
#endif
        CHECK(v[0].id == 10);
        CHECK(v[1].id == 11);
        CHECK(v[2].id == 12);
        CHECK(v[3].id == 13);
        CHECK(v[4].id == 14);
        CHECK(v[5].id == 100);
        CHECK(v[6].id == 100);
    }

    SUBCASE("to longer with reallocation") {
        v.resize(9);

        CHECK(!v.empty());
        REQUIRE(v.size() == 9);
#ifndef TEST_STD_VECTOR
        CHECK(v.capacity() == 16);
#endif
        CHECK(v[0].id == 10);
        CHECK(v[1].id == 11);
        CHECK(v[2].id == 12);
        CHECK(v[3].id == 13);
        CHECK(v[4].id == 14);
        CHECK(v[5].id == 100);
        CHECK(v[6].id == 100);
        CHECK(v[7].id == 100);
        CHECK(v[8].id == 100);
    }
}

TEST_CASE("resize with copy") {
#ifdef TEST_STD_VECTOR
    using Obj = ObjWithCopyAssignment;
#else
    using Obj = ObjWithCopyCtor;
#endif
    vector<Obj> v;
    v.push_back(Obj(10));
    v.push_back(Obj(11));
    v.push_back(Obj(12));
    v.push_back(Obj(13));
    v.push_back(Obj(14));

    SUBCASE("to size") {
        SUBCASE("from temporary") {
            v.resize(5, Obj(50));
        }
        SUBCASE("from const") {
            const Obj o(50);
            v.resize(5, o);
        }

        CHECK(!v.empty());
        REQUIRE(v.size() == 5);
#ifndef TEST_STD_VECTOR
        CHECK(v.capacity() == 8);
#endif
        CHECK(v[0].id == 10);
        CHECK(v[1].id == 11);
        CHECK(v[2].id == 12);
        CHECK(v[3].id == 13);
        CHECK(v[4].id == 14);
    }

    SUBCASE("to shorter") {
        SUBCASE("from temporary") {
            v.resize(3, Obj(50));
        }
        SUBCASE("from const") {
            const Obj o(50);
            v.resize(3, o);
        }

        CHECK(!v.empty());
        REQUIRE(v.size() == 3);
#ifndef TEST_STD_VECTOR
        CHECK(v.capacity() == 8);
#endif
        CHECK(v[0].id == 10);
        CHECK(v[1].id == 11);
        CHECK(v[2].id == 12);
    }

    SUBCASE("to zero") {
        SUBCASE("from temporary") {
            const Obj o(50);
            v.resize(0, o);
        }
        SUBCASE("from const") {
            const Obj o(50);
            v.resize(0, o);
        }

        CHECK(v.empty());
        CHECK(v.size() == 0);
#ifndef TEST_STD_VECTOR
        CHECK(v.capacity() == 8);
#endif
    }

    SUBCASE("to longer with 1 new element") {
        SUBCASE("from temporary") {
            v.resize(6, Obj(50));
        }
        SUBCASE("from const") {
            const Obj o(50);
            v.resize(6, o);
        }

        CHECK(!v.empty());
        REQUIRE(v.size() == 6);
#ifndef TEST_STD_VECTOR
        CHECK(v.capacity() == 8);
#endif
        CHECK(v[0].id == 10);
        CHECK(v[1].id == 11);
        CHECK(v[2].id == 12);
        CHECK(v[3].id == 13);
        CHECK(v[4].id == 14);
        CHECK(v[5].id == 50);
    }

    SUBCASE("to longer without reallocation") {
        SUBCASE("from temporary") {
            v.resize(7, Obj(50));
        }
        SUBCASE("from const") {
            const Obj o(50);
            v.resize(7, o);
        }

        CHECK(!v.empty());
        REQUIRE(v.size() == 7);
#ifndef TEST_STD_VECTOR
        CHECK(v.capacity() == 8);
#endif
        CHECK(v[0].id == 10);
        CHECK(v[1].id == 11);
        CHECK(v[2].id == 12);
        CHECK(v[3].id == 13);
        CHECK(v[4].id == 14);
        CHECK(v[5].id == 50);
        CHECK(v[6].id == 50);
    }

    SUBCASE("to longer with reallocation") {
        SUBCASE("from temporary") {
            v.resize(9, Obj(50));
        }
        SUBCASE("from const") {
            const Obj o(50);
            v.resize(9, o);
        }

        CHECK(!v.empty());
        REQUIRE(v.size() == 9);
#ifndef TEST_STD_VECTOR
        CHECK(v.capacity() == 16);
#endif
        CHECK(v[0].id == 10);
        CHECK(v[1].id == 11);
        CHECK(v[2].id == 12);
        CHECK(v[3].id == 13);
        CHECK(v[4].id == 14);
        CHECK(v[5].id == 50);
        CHECK(v[6].id == 50);
        CHECK(v[7].id == 50);
        CHECK(v[8].id == 50);
    }
}

TEST_CASE("resize copies temporary") {
    vector<std::string> v;

    v.resize(5, std::string(1000, 'x'));

    CHECK(!v.empty());
    REQUIRE(v.size() == 5);
#ifndef TEST_STD_VECTOR
    CHECK(v.capacity() == 8);
#endif
    CHECK(v[0] == std::string(1000, 'x'));
    CHECK(v[1] == std::string(1000, 'x'));
    CHECK(v[2] == std::string(1000, 'x'));
    CHECK(v[3] == std::string(1000, 'x'));
    CHECK(v[4] == std::string(1000, 'x'));
}

TEST_CASE("resize copies from itself") {
    vector<std::string> v(2);
    v[0] = std::string(1000, 'x');
    v[1] = std::string(1000, 'y');

    v.resize(5, v[0]);

    REQUIRE(v.size() == 5);
    CHECK(v[0] == std::string(1000, 'x'));
    CHECK(v[1] == std::string(1000, 'y'));
    CHECK(v[2] == std::string(1000, 'x'));
    CHECK(v[3] == std::string(1000, 'x'));
    CHECK(v[4] == std::string(1000, 'x'));
}
#endif  // TEST_EXTEND_OPS

TEST_CASE("operator[] and at() have lvalue/rvalue overloads") {
    struct TracingObj {
        // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
        int kind = 0;

        TracingObj() = default;

        TracingObj(const TracingObj &) : kind(1) {
        }

        TracingObj(TracingObj &&other) noexcept : kind(2) {
            other.kind = -2;
        }

        TracingObj &operator=(const TracingObj &) {
            kind = 3;
            return *this;
        }

        TracingObj &operator=(TracingObj &&other) noexcept {
            kind = 4;
            other.kind = -4;
            return *this;
        }

        ~TracingObj() = default;
    };

    vector<TracingObj> v(3);
    v[0].kind = 10;
    v[1].kind = 20;
#ifdef TEST_ADDITIONAL_OPS
    v.at(2).kind = 30;
#endif  // TEST_ADDITIONAL_OPS

    SUBCASE("operator[] &") {
        TracingObj o = v[0];
        CHECK(o.kind == 1);
        CHECK(v[0].kind == 10);
    }

#ifdef TEST_ADDITIONAL_OPS
    SUBCASE("at() &") {
        TracingObj o = v.at(0);
        CHECK(o.kind == 1);
        CHECK(v[0].kind == 10);
    }
#endif  // TEST_ADDITIONAL_OPS

#ifndef TEST_STD_VECTOR
#ifdef TEST_ADDITIONAL_OPS
    SUBCASE("operator[] &&") {
        TracingObj o = std::move(v)[0];
        CHECK(o.kind == 2);
        // NOLINTNEXTLINE(bugprone-use-after-move)
        CHECK(v[0].kind == -2);  // cppcheck-suppress accessMoved
    }

    SUBCASE("at() &&") {
        TracingObj o = std::move(v).at(0);
        CHECK(o.kind == 2);
        // NOLINTNEXTLINE(bugprone-use-after-move)
        CHECK(v[0].kind == -2);
    }
#endif  // TEST_ADDITIONAL_OPS
#endif
}

TEST_CASE("new elements are value-initialized") {
    SUBCASE("in constructor") {
        for (int step = 0; step < 10; step++) {
            vector<int> vec(1000);
            for (std::size_t i = 0; i < vec.size(); i++) {
                REQUIRE(vec[i] == 0);
                vec[i] = 10;
            }
        }
    }

#ifdef TEST_EXTEND_OPS
    SUBCASE("in resize with and without reallocation") {
        for (int step = 0; step < 10; step++) {
            vector<int> vec;
            vec.resize(500);
            for (std::size_t i = 0; i < 500; i++) {
                REQUIRE(vec[i] == 0);
                vec[i] = 10;
            }
            vec.resize(1000);
            for (std::size_t i = 500; i < 1000; i++) {
                REQUIRE(vec[i] == 0);
                vec[i] = 10;
            }
            vec.resize(0);
            vec.resize(1000);
            for (std::size_t i = 0; i < vec.size(); i++) {
                REQUIRE(vec[i] == 0);
                vec[i] = 10;
            }
        }
    }
#endif  // TEST_EXTEND_OPS
}

namespace {
struct Counters {
    std::size_t new_count = 0;
    std::size_t new_total_elems = 0;
    std::size_t delete_count = 0;
    std::size_t delete_total_elems = 0;
} global_counters;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
}  // namespace

Counters operator-(const Counters &a, const Counters &b) {
    return Counters{
        a.new_count - b.new_count, a.new_total_elems - b.new_total_elems,
        a.delete_count - b.delete_count,
        a.delete_total_elems - b.delete_total_elems};
}

template <typename Fn>
Counters with_counters(Fn fn) {
    const Counters start = global_counters;
    fn();
    return global_counters - start;
}

namespace {
template <typename T>
struct CounterAllocator {
    static_assert(alignof(T) <= __STDCPP_DEFAULT_NEW_ALIGNMENT__);

    using value_type = T;

    CounterAllocator() noexcept = default;

    template <typename U>
    // cppcheck-suppress noExplicitConstructor
    CounterAllocator(const CounterAllocator<U> &) noexcept {
    }

    template <typename U>
    bool operator==(const CounterAllocator<U> &) {
        return true;
    }

    template <typename U>
    bool operator!=(const CounterAllocator<U> &) {
        return false;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
    static inline std::size_t fail_next_allocations = 0;

    struct failed_allocation {};

    T *allocate(std::size_t count) {
        CHECK(count > 0);
        if (fail_next_allocations > 0) {
            fail_next_allocations--;
            throw failed_allocation();
        }
        T *result = static_cast<T *>(::operator new(count * sizeof(T)));
        global_counters.new_count++;
        global_counters.new_total_elems += count;
        return result;
    }

    void deallocate(T *ptr, std::size_t count) noexcept {
        CHECK(ptr != nullptr);
        CHECK(count > 0);
        ::operator delete(ptr);
        global_counters.delete_count++;
        global_counters.delete_total_elems += count;
    }
};
}  // namespace

#ifdef TEST_STD_VECTOR
TEST_CASE("custom allocator is used by std::vector<std::string>")
#else
TEST_CASE("custom allocator is used by lab_vector_naive::vector<std::string>")
#endif
{
    std::size_t capacity = 0;
    Counters res = with_counters([&capacity]() {
        struct S {
            char buf[40]{};
        };
        // Microsoft STL may actually allocate more data than we require.
        const vector<S, CounterAllocator<S>> vec_empty;
        vector<S, CounterAllocator<S>> vec(10);
        CHECK(vec[0].buf[0] == 0);
        capacity = vec.capacity();
    });
#ifndef TEST_STD_VECTOR
    CHECK(res.new_count == 1);
    CHECK(res.delete_count == 1);
#endif
    CHECK(res.new_total_elems == res.delete_total_elems);
#ifndef TEST_STD_VECTOR
    CHECK(res.new_total_elems == capacity);
    CHECK(capacity == 16);
#endif
}

#ifdef TEST_ADDITIONAL_OPS
#ifndef TEST_STD_VECTOR
TEST_CASE("operator= provides strong exception safety") {
    const vector<std::string, CounterAllocator<std::string>> from(
        5, std::string(1000, 'y')
    );
    vector<std::string, CounterAllocator<std::string>> to(
        3, std::string(1000, 'x')
    );
    REQUIRE(to.capacity() < from.size());

    CounterAllocator<std::string>::fail_next_allocations = 1;
    [[maybe_unused]] void *old_buffer = &to[0];
    CHECK_THROWS_AS(
        to = from, CounterAllocator<std::string>::failed_allocation
    );
    REQUIRE(to.size() == 3);
    CHECK(&to[0] == old_buffer);
    CHECK(to[0] == std::string(1000, 'x'));
    CHECK(to[1] == std::string(1000, 'x'));
    CHECK(to[2] == std::string(1000, 'x'));
}
#endif
#endif  // TEST_ADDITIONAL_OPS

#ifdef TEST_VERY_FORMALLY_EFFICIENT
#ifndef TEST_STD_VECTOR
TEST_CASE("Construct 1 move of temporary and read") {
    std::shared_ptr<int> data(new int(123));

    vector<std::shared_ptr<int>> v(1, std::move(data));

    CHECK(!v.empty());
    REQUIRE(v.size() == 1);
    CHECK(v.capacity() == 1);
    CHECK(*v[0] == 123);
    CHECK(data == nullptr);
}

TEST_CASE("resize can do 1 move of temporary") {
    std::shared_ptr<int> data(new int(123));
    vector<std::shared_ptr<int>> v;

    v.resize(1, std::move(data));

    CHECK(!v.empty());
    REQUIRE(v.size() == 1);
    CHECK(v.capacity() == 1);
    CHECK(*v[0] == 123);
    CHECK(data == nullptr);
}
#endif
#endif  // TEST_VERY_FORMALLY_EFFICIENT

// NOLINTEND(misc-use-anonymous-namespace)
// NOLINTEND(readability-function-cognitive-complexity)
