#include <iostream>
#include "tests_config.h"

int main() {
    std::cout <<
#if !defined(TEST_FUNCTIONS)
        1
#elif !defined(TEST_CLI_SIMPLE)
        2
#elif !defined(TEST_COMPLEX_EXPRESSIONS)
        3
#elif !defined(TEST_CLI_LONG_LINES)
        4
#else
        5
#endif
        ;
}
