#! /bin/bash

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

# gets the wast files from the web and stores them in the provided directory
# usage: getTheWasts <target-dir> <commit-id>?
getTheWasts() {
    if [ "$#" -eq 0 ]; then
        echo "no target dir passed"
        exit 1
    fi

    target="${1}"
    if [ ! -d $target ]; then
        echo "error: ${target} no such directory"
    fi

    # get the commit id
    printf "commit:"
    if [ "$#" -eq 1 ]; then
        echo "latest"
        commit="/master"
    else
        commit="/${2}"
        # curl returns a lot of stuff but we only care the image that
        # displays the 404 message
        stringToFind="<img alt=\"404"
        ret=$(curl -s "https://github.com/WebAssembly/spec/tree${commit}/test/core" | egrep -o "${stringToFind}")
        if [ "${ret}" == "${stringToFind}" ]; then
            echo "Invalid commit id"
            exit 1
        fi
        echo "${commit}"
    fi

    # create a temporary directory and navigate to it
    work_dir=$(mktemp -d)
    if [ ! -d $work_dir ]; then
        echo "failed to create tmp dir"
        exit 1
    fi
    cd $work_dir

    # get all Wast filenames
    stringToFind="/WebAssembly/spec/blob$commit"
    if [ commit!="/master" ]; then
        stringToFind=$stringToFind"[0-9abcdef]*"
    fi
    stringToFind=$stringToFind"/test/core/[^ >]*wast"

    curl -s https://github.com/WebAssembly/spec/tree${commit}/test/core |
        egrep -o "${stringToFind}" |
        sed 's/blob\///g' |
        sed -e 's/^/raw.githubusercontent.com/g' |
        xargs wget -q --show-progress

    echo $(ls -1 | grep .wast | wc -l) files downloaded

    # clean old wast files
    rm $target/*.wast >/dev/null 2>&1

    # move the wast files to the target dir
    mv *.wast $target/

    rmdir $work_dir
}

RUN_CPP=1
RUN_SPEC=1
RUN_FAIL=0
VERBOSE=0

# check options
while [[ $# -gt 0 ]]; do
    option=$1
    case "$option" in
    "-h" | "--help")
        print_usage
        exit 0
        ;;
    "-r" | "--run")
        test $2 = cpp && RUN_SPEC=0
        test $2 = spec && RUN_CPP=0
        shift
        shift
        ;;
    "-v" | "--verbose")
        set -v
        VERBOSE=1
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

    # cleaning up older wasm files
    rm $WASM_DIR/*_module_*.wasm &>/dev/null
    rm $WASM_DIR/fail/*.wasm &>/dev/null

    # compile all cpp files to wasm
    echo -e "⚒ ${YELLOW}Generating cpp (emscripten) tests${NC}"
    cd $CPP_DIR
    # WASM_DIR=$WASM_DIR make clean &>/dev/null
    WASM_DIR=$WASM_DIR make
    cd $ROOT_DIR
fi
if [ $RUN_SPEC -eq 1 ]; then

    printf "${YELLOW}Do you want to refresh the binaries?[y/N](default: N):${NC}"
    read -r ans

    if [ "${ans}" == "y" ]; then
        printf "absolute path to SPEC WASM interpreter: " && read WASMI

        commit=
        # read the commit in wasmCommit file if it exists
        if [ -f $TEST_DIR/wasmCommit ]; then
            commit=$(head -n 1 $TEST_DIR/wasmCommit)
        fi
        if [ "${commit}" == "" ]; then
            commit=
        fi

        echo "commit is ${commit}"
        # create a temporary dir to store the wasts
        WAST_DIR=$(mktemp -d)
        if [ ! -d $WAST_DIR ]; then
            echo "failed to create tmp dir"
            exit 1
        fi

        # Step 1: get the wast files from the web
        echo -e "⬇ ${YELLOW}Getting the .wast files${NC}"
        getTheWasts $WAST_DIR $commit

        # Step 2: use the interpreter to convert them to .bin.wast
        echo -e "⚒ ${YELLOW}Generating .bin.wast files${NC}"

        # cleaning up older wasm files
        if [ $RUN_CPP -eq 0 ]; then
            rm $WASM_DIR/*.wasm &>/dev/null
        else
            rm $WASM_DIR/*_module_*.wasm &>/dev/null
        fi
        rm $WASM_DIR/fail/*.wasm &>/dev/null

        cd $CORE_TEST_DIR
        WASMI=$WASMI SPEC_CORE_DIR=$WAST_DIR make
        cd $ROOT_DIR

        # Step 3: get the hex code from each .bin.wast file
        echo -e "⚒ ${YELLOW}Generating spec core tests${NC} $PWD"
        ./specTests.py

        # delete the WAST_DIR
        rm $WAST_DIR/*
        rmdir $WAST_DIR

    fi

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

col=0
passed=0
count=0
total=$(ls -1 $WASM_DIR/*.wasm | wc -l)

tempfile=$(mktemp)
for prog in $WASM_DIR/*.wasm; do
    ((col++))
    test $col -eq 51 && printf " (${count} / ${total})" && echo "" && col=1

    ((count++))

    ./cwasm $prog &>tempfile
    EXITCODE=$?
    if [ $EXITCODE -eq 0 ]; then
        ((passed++))
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
echo

col2=0
passed2=0
count2=0
total2=0
if [ $RUN_FAIL -eq 1 ] && [ $(ls -1 $WASM_DIR/fail | wc -l) -ne 0 ]; then
    # run the tests that should fail
    echo "----------------------------------------------------"
    echo -e "${GREEN}Running tests that should fail:${NC}"
    echo "----------------------------------------------------"

    total2=$(ls -1 $WASM_DIR/fail/*.wasm | wc -l)
    for prog in $WASM_DIR/fail/*.wasm; do
        ((col2++))
        test $col2 -eq 51 && printf " (${count2} / ${total2})" && echo && col2=1

        ((count2++))

        timeout 2 ./cwasm $prog &>/dev/null
        EXITCODE=$?
        if [ $EXITCODE -ne 0 ]; then
            ((passed2++))
            printf '.'
        else
            printf 'X'
            NAME=$(basename $prog)
            FAILED_TESTS="${FAILED_TESTS}\n ${NAME}"
            # echo
            # cat tempfile
            # TODO?: This would be nice in the future
            # msg=$(cat $WASM_DIR/fail/msg/$prog)
            # echo "test should fail: $msg"
            # break
        fi
    done
    echo
fi

# Print results
all_passed=$((passed + passed2))
all_total=$((total + total2))

if [ $all_passed -ne $all_total ] && [ $VERBOSE -eq 1 ]; then
    printf "show the failed tests?[y/N](default: N) " && read ans
    if [ -n $ans ]; then
        if [ "${ans}" == "y" ]; then
            echo -e $FAILED_TESTS
        fi
    fi
fi

echo
printf "\t %4d /%4d correct tests passed  \n" $passed $total
printf "\t %4d /%4d incorrect tests passed\n" $passed2 $total2
echo
echo -e "${GREEN}\u2714 ${all_passed}/${all_total} tests passed!${NC}"
echo
