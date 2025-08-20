#!/bin/bash
set -euo pipefail
# Array should not be empty on macOS (old Bash expands empty array to unset variable and errs)
BMP_TEST_COMMAND=("$@" ./bmp-test)
BMP_CROP_COMMAND=("$@" ./bmp)
TIMEOUT=15s  # Careful of Valgrind

FAIL=0
# Warning: use \033 instead of \e for compatibility with old Bash on macOS.

function test_ok {
    IN_F="test-data/$1.bmp"
    OUT_F="test-data/$1.out.bmp"
    CROP_ARGS=("$2" "$3" "$4" "$5")
    if [ "$#" -ne 5 ]; then
        echo "Invalid test-data: unexpected $# args"
        exit 1
    fi

    if [ -f "test-data/$1.sol.bmp" ]; then
        EXPECTED_EXITCODE=0
        EXPECTED_STDERR_F=/dev/null
        EXPECTED_F="test-data/$1.sol.bmp"

        function check_f() {
            # Use `cmp` instead of `diff` because files are binary;
            # we're interested in first different byte, not line-by-line.
            cmp "$OUT_F" "$EXPECTED_F"
        }
    elif [ -f "test-data/$1.sol.txt" ]; then
        EXPECTED_EXITCODE=1
        EXPECTED_STDERR_F="test-data/$1.sol.txt"

        function check_f() {
            if [ -f "$OUT_F" ]; then
                echo "File created, but should not be"
                return 1
            fi
        }
    else
        echo "Invalid test-data: no solution file"
        exit 1
    fi

    rm -rf "$OUT_F"
    set +e
    { timeout --foreground -k 0.1s "$TIMEOUT" "${BMP_CROP_COMMAND[@]}" crop-rotate "$IN_F" "$OUT_F" ${CROP_ARGS[@]} 2>&3 | head -c 100000 >stdout.out; echo ${PIPESTATUS[0]} >exit.out; } 3>&1 | head -c 100000 >stderr.out
    set -e
    if [[ $(cat exit.out) == "124" ]]; then
        echo -e "\033[31;1mTL\033[0m (or exit code $(cat exit.out))"
        FAIL=1
        return 1
    fi

    if [[ $(cat exit.out) != "$EXPECTED_EXITCODE" ]]; then
        echo -e "\033[31;1mWA or RTE\033[0m (exit code $(cat exit.out), expected $EXPECTED_EXITCODE)"
        FAIL=1
    elif [ -s stdout.out ]; then
        echo -e "\033[31;1mWA\033[0m: unexpected stdout"
        FAIL=1
    elif ! diff stderr.out "$EXPECTED_STDERR_F"; then 
        echo -e "\033[31;1mWA\033[0m: incorrect stderr"
        FAIL=1
    elif ! check_f; then 
        echo -e "\033[31;1mWA\033[0m: incorrect output file"
        FAIL=1
    else
        echo PASS
    fi
    return 0
}

echo "===== Running tests with ${BMP_TEST_COMMAND[@]} ====="
if ! timeout --foreground -k 0.1s "$TIMEOUT" "${BMP_TEST_COMMAND[@]}"; then
    FAIL=1
fi

while read -r tn; do
    tn=$(echo $tn)  # To remove \r from tests.txt on Windows
    echo ===== $tn =====
    # Expand all arguments
    if ! test_ok $tn; then break; fi
done < tests.txt
if [[ "$FAIL" == "0" ]]; then
    echo -e "===== \033[32;1mALL PASS\033[0m ====="
else
    echo -e "===== \033[31;1mSOME FAIL\033[0m ====="
fi
exit $FAIL
