#!/bin/bash
set -euo pipefail
TIMEOUT=15s  # Careful of Valgrind

FAIL=0
# Warning: use \033 instead of \e for compatibility with old Bash on macOS.

COMMAND=("$@" ./calc-cli)

function normalize_t3() {
    sed -e 's/^  \.*\^$/  ..^[REDACTED]^/g' -e 's/^Error [1-7]:$/Error [REDACTED]:/g' < "$1"
}

function test_ok {
    IN_F="test-data/$1.in"
    OUT_F="test-data/$1.out"

    if [ -f "test-data/$1.sol" ]; then
        EXPECTED_EXITCODE=0
        EXPECTED_STDERR_F=/dev/null
        EXPECTED_F="test-data/$1.sol"

        if [[ "$1" =~ "t3-" ]]; then
            function check_f() {
                diff <(normalize_t3 "$OUT_F") <(normalize_t3 "$EXPECTED_F")
            }
        else
            function check_f() {
                diff "$OUT_F" "$EXPECTED_F"
            }
        fi
    elif [ -f "test-data/$1.err" ]; then
        EXPECTED_EXITCODE=1
        EXPECTED_STDERR_F="test-data/$1.err"

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
    { timeout --foreground -k 0.1s "$TIMEOUT" "${COMMAND[@]}" "$IN_F" "$OUT_F" 2>&3 | head -c 100000 >stdout.out; echo ${PIPESTATUS[0]} >exit.out; } 3>&1 | head -c 100000 >stderr.out
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

echo -n "Detecting tasks... "
TASKS=$(./tasks-info)
echo "$TASKS"

echo "===== calc-test ====="
if ! timeout --foreground -k 0.1s "$TIMEOUT" "$@" ./calc-test; then
    FAIL=1
fi

echo "===== calc-test-c ====="
if ! timeout --foreground -k 0.1s "$TIMEOUT" "$@" ./calc-test-c; then
    FAIL=1
fi

if [[ $TASKS -ge 3 ]]; then
    echo "===== t3-smoke ====="
    test_ok t3-smoke

    echo "===== t3-bad-input ====="
    test_ok t3-bad-input
fi

if [[ $TASKS -ge 4 ]]; then
    echo "===== t4-smoke ====="
    test_ok t4-smoke
fi

if [[ $TASKS -ge 5 ]]; then
    echo "===== t5-big ====="
    ./test-data/t5-big.gen.py >./test-data/t5-big.in
    test_ok t5-big
fi

if [[ "$FAIL" == "0" ]]; then
    echo -e "===== \033[32;1mALL PASS\033[0m ====="
else
    echo -e "===== \033[31;1mSOME FAIL\033[0m ====="
fi
exit $FAIL
