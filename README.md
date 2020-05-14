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
3. Now [test.sh](test.sh) can run the tests

For the second type of tests the oficial github repo of the WebAssembly spec must cloned, because we need their interpreter implementation as well as their test files. 

#### Disclaimer
In order for the spec tests to run an older version of the spec is needed. This is what the `git checkout` command does. It creates a branch with the latest compatible version. 

```
git clone https://github.com/WebAssembly/spec/
git checkout -b old-state 704d9d9e9c861fdb957c3d5e928f1d046a31497e
cd interpreter
make
```

If you have done that then the final thing is to run [test.sh](test.sh) located in the cwasm directory. This should begin by compilling the CPP tests, and then it will check for the spec tests. You will be asked if you want to download the wast test files. You can specify the specific commit you want in the [wasmCommit](tests/wasmCommit) file.

You wil also be prompted for an absolute path to the **spec interpreter** (`/<path_to_spec>/interpreter/wasm`). This is necessary in order to prepare the test files for cwasm.

#### But why?

The spec interpreter provided by WebAssembly can understand a specific S-expr language specified [here](https://github.com/WebAssembly/spec/tree/master/interpreter#s-expression-syntax). Our implementation is able to understand only wasm files, so it cannot process the test files directly.

We use the spec interpreter to first translate the `.wast` files to `.bin.wast`. This format is also an S-expr language (subset of the previous one) that contains blocks of wasm. We use [specTests.py](specTests.py) to extract these blocks and transfer each one to a file.

So, for example:
1. We get `address.wast` from WebAssembly's github repo. 

2. We call their interpreter to transform it to `address.bin.wast` and store it in `tests/srcs/core-tests`.

3. We use [specTests.py](specTests.py) to extract the bytecode. The first module declared in `address.wast` is put in a file named `address_module_0.wasm`. These files are in the `tests/bins` directory.

There are many types of modules such as valid, invalid, malformed etc.. We only care about the three types mentioned above. All valid modules are placed in the `tests/bins`, while invalid and malformed ones are placed in `tests/bins/fail`.

*The testing process is fast (about 7 - 10 seconds)*
   

