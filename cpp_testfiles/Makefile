.PHONY: all clean

CXX=em++
CXXFLAGS= -std=c++17 -Wall -s WASM=1 -O2

SRC_DIR= .
WASM_DIR= ../wasm_tests

SRC_FILES=$(wildcard $(SRC_DIR)/*.cpp)
PROGRAMS=$(patsubst $(SRC_DIR)/%.cpp,$(WASM_DIR)/%.wasm,$(SRC_FILES))

# PROGRAMS=fairsum-2N fairsum-ON2 fairsum-ON3 fairsum \
#   longsumk-ON2 longsumk-ON3 longsumk \
#   samenum-ON2-better samenum-ON2 samenum \
#   cnteven-loop cnteven third-if third twobase vicky

$(WASM_DIR)/%.wasm: $(SRC_DIR)/%.cpp
	@echo "EM++   "$@
	@$(CXX) $(CXXFLAGS) $< -o $@  

all: $(PROGRAMS)

clean:
	$(RM) $(PROGRAMS)
	$(RM) *~
