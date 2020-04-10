#! /bin/sh

# set -v

ROOT_DIR=$PWD
TEST_DIR=$ROOT_DIR/tests
SRCS_DIR=$TEST_DIR/srcs
CPP_DIR=$SRCS_DIR/cpp_testfiles
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
export WASM_DIR
make
cd $ROOT_DIR

# run the tests
echo "------------------------------"
echo -e "${GREEN}Running tests:${NC}"
echo "------------------------------"

FAILED_TESTS=" "

col=0

for prog in $WASM_DIR/*.wasm; do

    ./cwasm $prog &>/dev/null
    EXITCODE=$?
    if [ $EXITCODE -eq 0 ]; then
        ((col++))
        test $col -eq 31 && echo "" && col=1
        printf '.'
    else
        printf 'X'
        NAME=$(basename $prog)
        FAILED_TESTS="${FAILED_TESTS}\n ${NAME}"
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
