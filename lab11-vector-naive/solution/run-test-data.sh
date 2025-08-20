#!/bin/bash
set -euo pipefail
TIMEOUT=30s  # Careful of Valgrind

FAIL=0
# Warning: use \033 instead of \e for compatibility with old Bash on macOS.

echo "Ensuring there is no 'noexcept', just 'VECTOR_NOEXCEPT'..."
if grep -H "noexcept .*{" vector.hpp; then
    FAIL=1
else
    echo "    OK"
fi

echo "Testing std::vector..."
if ! timeout --foreground -k 0.1s "$TIMEOUT" "$@" ./vector-test-std; then
    FAIL=1
fi

echo "Testing lab_vector_naive::vector..."
if ! timeout --foreground -k 0.1s "$TIMEOUT" "$@" ./vector-test; then
    FAIL=1
fi

if [[ "$FAIL" == "0" ]]; then
    echo -e "===== \033[32;1mALL PASS\033[0m ====="
else
    echo -e "===== \033[31;1mSOME FAIL\033[0m ====="
fi
exit $FAIL
