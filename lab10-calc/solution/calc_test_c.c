#ifdef _MSC_VER
#include <crtdbg.h>
#endif
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "calc.h"
#include "tests_config.h"

void require_eq_neq(
    int is_eq,
    double a,
    double b,
    const char *file,
    int line,
    const char *astr,
    const char *bstr
) {
    if ((fabs(a - b) > 1e-6) == is_eq) {
        printf(
            "REQUIRE_EQ failed at %s:%d: %s != %s (%lf != %lf)\n", file, line,
            astr, bstr, a, b
        );
        abort();
    }
}

#define REQUIRE_EQ(a, b) \
    require_eq_neq(1, (double)(a), (double)(b), __FILE__, __LINE__, #a, #b)

#define REQUIRE_NEQ(a, b) \
    require_eq_neq(0, (double)(a), (double)(b), __FILE__, __LINE__, #a, #b)

#ifdef TEST_FUNCTIONS
double my_pi(void) {
    return 3.14;  // https://stackoverflow.com/questions/1727881/how-to-use-the-pi-constant-in-c
}

double my_hypot3(double a, double b, double c) {
    return sqrt(a * a + b * b + c * c);
}
#endif

int main(void) {
#ifdef _MSC_VER
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif

    {
        printf("=== simple expression ===\n");
        calc_function funcs[] = {CALC_FUNCTIONS_SENTINEL};
        calc_result res;
        REQUIRE_EQ(calc_evaluate("5*6", &res, funcs), 0);
        REQUIRE_EQ(res.value, 30);
#ifdef TEST_COMPLEX_EXPRESSIONS
        REQUIRE_EQ(
            calc_evaluate("5*", &res, funcs), CALC_ERROR_UNEXPECTED_CHAR
        );
        REQUIRE_EQ(res.error_position, 2);
#else
        REQUIRE_NEQ(calc_evaluate("5*", &res, funcs), 0);
#endif
        printf("PASS\n");
    }
#ifdef TEST_FUNCTIONS
    static_assert(
        CALC_MAX_ARITY >= 3,
        "CALC_MAX_ARITY is too small"
    );  // Lies in <assert.h> since C11,
        // message is optional since C23 only.
    {
        printf("=== no functions ===\n");
        calc_function funcs[] = {CALC_FUNCTIONS_SENTINEL};
        calc_result res;
#ifdef TEST_COMPLEX_EXPRESSIONS
        REQUIRE_EQ(
            calc_evaluate("5+pow(1,1)", &res, funcs),
            CALC_ERROR_UNKNOWN_FUNCTION
        );
        REQUIRE_EQ(res.error_position, 5);
#else
        REQUIRE_NEQ(calc_evaluate("5+pow(1,1)", &res, funcs), 0);
#endif
        printf("PASS\n");
    }
    {
        printf("=== custom functions ===\n");
        calc_function funcs[] = {
            {.name = "pi", .arity = 0, .func0 = my_pi},
            {.name = "hypot", .arity = 3, .func3 = my_hypot3},
            CALC_FUNCTIONS_SENTINEL,
        };
        calc_result res;
        REQUIRE_EQ(calc_evaluate("hypot(2,6,9)*100", &res, funcs), 0);
        REQUIRE_EQ(res.value, 1100);

        REQUIRE_EQ(calc_evaluate("pi()", &res, funcs), 0);
        REQUIRE_EQ(res.value, 3.14);
        printf("PASS\n");
    }
#ifdef TEST_COMPLEX_EXPRESSIONS
    {
        printf("=== complicated expression ===\n");
        calc_result res;
        REQUIRE_EQ(
            calc_evaluate(
                "2+3*(4+5*-8/   2* 3)/3-pow ( 2*5 , 1+1 ) ", &res, NULL
            ),
            0
        );
        REQUIRE_EQ(res.value, -154);
        printf("PASS\n");
    }
#endif  // TEST_COMPLEX_EXPRESSIONS
#endif  // TEST_FUNCTIONS
    printf("=== C TESTS PASS ===\n");
}
