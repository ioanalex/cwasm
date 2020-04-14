# Cwasm

This is an implementation of a Web Assembly Interpreter written in `C/C++`.

So far cwasm can validate WebAssembly modules in binary format.

## Build
```
git clone https://github.com/ioanalex/cwasm
cd cwasm
make
```

## Run tests
 Two types of tests are available. The first type is binaries produced by the emscripten compiler. In order for those tests to run:

1. Install Emscripten SDK from [here](https://emscripten.org/docs/getting_started/downloads.html)
2. Make sure em++ is in your path (remember to source `emsdk_env.sh`)
3. Now `test.sh` can run the tests

For the second type of tests the oficial github repo of the WebAssembly spec must cloned, because we need their interpreter implementation as well as their test files. 

```
git clone https://github.com/WebAssembly/spec/
cd interpreter
make
```

If you have done that then the final thing is to run `test.sh` located in the cwasm directory. This should begin by compilling the CPP tests, and then it will check for the spec tests. You will be prompted to enter the absolute path to the **spec interpreter** `/<path_to_spec>/interpreter/wasm` and the absolute path to core tests `/<path_to_spec>/tests/core`. 

*The testing process is fast (about 7 - 10 seconds)*

    

