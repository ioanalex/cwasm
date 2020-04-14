#! /bin/sh

# set -v

ROOT_DIR=$PWD
TEST_DIR=$ROOT_DIR/tests
SRCS_DIR=$TEST_DIR/srcs
CPP_DIR=$SRCS_DIR/cpp_testfiles
CORE_TEST_DIR=$SRCS_DIR/core-tests
WASM_DIR=$TEST_DIR/bins

# colors for output
RED='\033[91;1m'
GREEN='\033[92;1m'
NC='\033[0m' # No Color

# check for emscripten compiler
em++ -v &>/dev/null
EXITCODE=$?
if [ $EXITCODE -eq 0 ]; then
    echo -e "${GREEN}emcc is available${NC}"
else
    echo -e "${RED}emcc is missing${NC}, make sure to source the emsdk_env.sh"
    exit $EXITCODE
fi

# check if bins dir is present
if [ ! -d "$WASM_DIR" ]; then
    echo "MKDIR wasm_test"
    mkdir $WASM_DIR
fi

# compile all cpp files to wasm
cd $CPP_DIR
# WASM_DIR=$WASM_DIR make clean &>/dev/null
WASM_DIR=$WASM_DIR make
cd $ROOT_DIR

# compile all core-tests (2 steps)
# Step 1: *.wast -> *.bin.wast
# This step needs two env variables
# WASMI=/absolute/path/to/wasm/interpreter
# SPEC_CORE_DIR=/absolute/path/to/spec/test/core

# There are 74 tests (which expand to more later).
# Let's count the number of .bin.wast files
num=$(ls -1 *.bin.wast | wc -l)
if [ num -neq 74 ]; then
    printf "${GREEN}The bin.wast files need a refresh fou you want to remake them?[y/N](default: N):${NC}"
    read -r ans
    if [ ans -eq 'y' ]; then
        printf "absolute path to SPEC WASM interpreter: " && read WASMI
        printf "absolute path to spec tests (core): " && read SPEC_CORE_DIR

        cd $CORE_TEST_DIR
        WASMI=$WASMI SPEC_CORE_DIR=$SPEC_CORE_DIR make clean
        WASMI=$WASMI SPEC_CORE_DIR=$SPEC_CORE_DIR make
        cd $ROOT_DIR
    fi
fi

# Step 2: get the hex code from each .bin.wast file
# This is done by specTest.py

# run the tests
echo "------------------------------"
echo -e "${GREEN}Running tests:${NC}"
echo "------------------------------"

FAILED_TESTS=" "

col=0
tempfile=$(mktemp)
for prog in $WASM_DIR/*.wasm; do
    >tempfile # truncate file
    ./cwasm $prog &>tempfile
    EXITCODE=$?
    if [ $EXITCODE -eq 0 ]; then
        ((col++))
        test $col -eq 31 && echo "" && col=1
        printf '.'
    else
        printf 'X'
        NAME=$(basename $prog)
        FAILED_TESTS="${FAILED_TESTS}\n ${NAME}"
        echo
        cat tempfile
        break
    fi
done

echo -e "\n\n"

# Print results
if [ "$FAILED_TESTS" == " " ]; then
    echo -e "${GREEN}All tests passed!${NC}"
else
    echo -e "${RED}-- FAILED TESTS --${NC}"
    echo -e $FAILED_TESTS
    echo -e "${RED}------------------${NC}"
fi
