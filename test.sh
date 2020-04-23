#! /bin/sh

# set -v

print_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "  OPTIONS:"
    echo "      -h | --help               print this message"
    echo "      -r | --run  [cpp | spec]  select what tests you want to run"
    echo "                                by default all tests are run"
    echo "      -f | --fail               run tests that should fail. This will"
    echo "                                not happen by default. Should be used"
    echo "                                when spec tests are enabled."
    echo "      -v | --verbose            enable verbose output (set -v)"
    echo ""
}

RUN_CPP=1
RUN_SPEC=1
RUN_FAIL=0

# check options
while [[ $# -gt 0 ]]; do
    option=$1
    case "$option" in
    "-h" | "--help")
        print_usage
        exit 0
        ;;
    "-r" | "--run")
        test $2 == cpp && RUN_SPEC=0
        test $2 == spec && RUN_CPP=0
        shift
        shift
        ;;
    "-v" | "--verbose")
        set -v
        shift
        ;;
    "-f" | "--fail")
        RUN_FAIL=1
        shift
        ;;
    *)
        echo "Unknown option: $1"
        print_usage
        exit 1
        ;;
    esac
done

ROOT_DIR=$PWD
TEST_DIR=$ROOT_DIR/tests
SRCS_DIR=$TEST_DIR/srcs
CPP_DIR=$SRCS_DIR/cpp_testfiles
CORE_TEST_DIR=$SRCS_DIR/core-tests
WASM_DIR=$TEST_DIR/bins

# colors for output
RED='\033[31;1m'
GREEN='\033[32;1m'
YELLOW='\033[33;1m'
NC='\033[0m' # No Color

# check if bins dir is present
if [ ! -d "$WASM_DIR" ]; then
    echo "MKDIR bins"
    mkdir $WASM_DIR
fi

# check if bins/fail dir is present
if [ ! -d "$WASM_DIR/fail" ]; then
    echo "MKDIR bins/fail"
    mkdir "$WASM_DIR/fail"
fi

# clea bins dir
rm $WASM_DIR/*.wasm &>/dev/null
rm $WASM_DIR/fail/*.wasm &>/dev/null

if [ $RUN_CPP -eq 1 ]; then
    # check for emscripten compiler
    em++ -v &>/dev/null
    EXITCODE=$?
    if [ $EXITCODE -eq 0 ]; then
        echo -e "${GREEN}emcc is available${NC}"
    else
        echo -e "${RED}emcc is missing${NC}, make sure to source the emsdk_env.sh"
        exit $EXITCODE
    fi

    # compile all cpp files to wasm
    echo -e "⚒ ${YELLOW}Generating cpp (emscripten) tests${NC}"
    cd $CPP_DIR
    # WASM_DIR=$WASM_DIR make clean &>/dev/null
    WASM_DIR=$WASM_DIR make
    cd $ROOT_DIR
fi
if [ $RUN_SPEC -eq 1 ]; then
    # compile all core-tests (2 steps)
    # Step 1: *.wast -> *.bin.wast
    # This step needs two env variables
    # WASMI=/absolute/path/to/wasm/interpreter
    # SPEC_CORE_DIR=/absolute/path/to/spec/test/core

    # There are 74 tests (which expand to more later).
    # Let's count the number of .wast files
    num=$(ls -1 $CORE_TEST_DIR/ | grep .wast | wc -l)
    if [ $num != "74" ]; then
        printf "${YELLOW}The bin.wast files need a refresh fou you want to remake them?[y/N](default: N):${NC}"
        read -r ans
        if [ $ans == 'y' ]; then
            printf "absolute path to SPEC WASM interpreter: " && read WASMI
            printf "absolute path to spec tests (core): " && read SPEC_CORE_DIR

            echo -e "⚒ Generating .bin.wast files"
            cd $CORE_TEST_DIR
            WASMI=$WASMI SPEC_CORE_DIR=$SPEC_CORE_DIR make clean
            WASMI=$WASMI SPEC_CORE_DIR=$SPEC_CORE_DIR make
            cd $ROOT_DIR
        fi
    fi

    # Step 2: get the hex code from each .bin.wast file
    echo -e "⚒ ${YELLOW}Generating spec core tests${NC} $PWD"
    ./specTests.py

    # check that all is good
    # script that checks that all failed tests are under bins/fail
    for file in $CORE_TEST_DIR/*; do
        name=$(basename --suffix=.bin.wast $file)
        a=$(grep -e 'assert_malformed' -e 'assert_invalid' $file | wc -l)
        if [ $a -ne 0 ]; then
            b=$(ls -1 tests/bins/fail/${name}_module_* | wc -l)
            if [ ! $a -eq $b ]; then
                echo -e "\033[31;1m👎 error:\033[0m file: ${name} has not been processed correctly"
                exit 1
            fi
        fi
    done
    echo -e "\033[32;1m👍\033[0m all test files were processed correctly"
fi

test $RUN_CPP -eq 0 -a $RUN_SPEC -eq 0 && exit 0

# run the tests
echo "----------------------------------------------------"
echo -e "${GREEN}Running tests:${NC}"
echo "----------------------------------------------------"

FAILED_TESTS=" "

count=0
total=$(ls -1 $WASM_DIR/*.wasm | wc -l)
col=0
tempfile=$(mktemp)
for prog in $WASM_DIR/*.wasm; do
    >tempfile # truncate file
    ./cwasm $prog &>tempfile
    EXITCODE=$?
    if [ $EXITCODE -eq 0 ]; then
        ((col++))
        test $col -eq 51 && printf " (${count} / ${total})" && echo "" && col=1
        printf '.'
    else
        printf 'X'
        NAME=$(basename $prog)
        FAILED_TESTS="${FAILED_TESTS}\n ${NAME}"
        echo
        cat tempfile
        break
    fi
    ((count++))
done

echo ""
echo ""

if [ $RUN_FAIL -eq 1 ]; then
    # run the tests that should fail
    echo "----------------------------------------------------"
    echo -e "${GREEN}Running tests that should fail:${NC}"
    echo "----------------------------------------------------"

    count2=0
    total2=$(ls -1 $WASM_DIR/fail/*.wasm | wc -l)
    col2=0
    for prog in $WASM_DIR/fail/*.wasm; do
        # >tempfile # truncate file
        timeout 2 ./cwasm $prog &>/dev/null #tempfile
        EXITCODE=$?
        if [ $EXITCODE -ne 0 ]; then
            ((count2++))
            test $col2 -eq 51 && printf " (${count2} / ${total2})" && echo "" && col2=1
            printf '.'
        else
            printf '|'
            NAME=$(basename $prog)
            FAILED_TESTS="${FAILED_TESTS}\n ${NAME}"
            # echo
            # cat tempfile
            # TODO?: This would be nice in the future
            # msg=$(cat $WASM_DIR/fail/msg/$prog)
            # echo "test should fail: $msg"
            # break
        fi
        ((col2++))
    done
else
    count2=0
    total2=0
fi

# Print results
if [ "$FAILED_TESTS" == " " ]; then
    all_count=$((count + count2))
    all_total=$((total + total2))
    echo -e "${GREEN}\u2714 All ${all_count}/${all_total} tests passed!${NC}"
else
    echo ""
    echo -e "${GREEN}\u2714 ${count2}/${total2} tests passed!${NC}"
    # echo -e "${RED}-- 💩 FAILED TESTS 💩 --${NC}"
    # echo -e $FAILED_TESTS
    # echo -e "${RED}----------------------${NC}"
fi

echo ""
