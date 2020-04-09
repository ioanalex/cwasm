#! /bin/sh

# set -v

LANG=en_US.UTF-8

ROOT_DIR=$PWD
CPP_DIR=$ROOT_DIR/cpp_testfiles
WASM_DIR=$ROOT_DIR/wasm_tests

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

# check if wasm_tests dir is present
if [ ! -d "$WASM_DIR" ]; then
    echo "MKDIR wasm_test"
    mkdir $WASM_DIR
fi

# compile all cpp files to wasm
cd $CPP_DIR
make
cd $ROOT_DIR

# run the tests
echo "Running tests:"
printf "\t"
FAILED_TESTS=" "
for prog in $WASM_DIR/*.wasm; do

    ./cwasm $prog &>/dev/null
    EXITCODE=$?
    if [ $EXITCODE -eq 0 ]; then
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
