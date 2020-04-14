.PHONY: all

WASMI?=
SPEC_CORE_DIR?=
BIN_WAST_DIR=.

# This doesn't work....
# define fix-files =
# sed -i 's:module \$.* binary:module binary:g' $(BIN_WAST_DIR)/*.bin.wast 
# sed -i 's:module binary \":module binary\n \":g' $(BIN_WAST_DIR)/*.bin.wast  
# sed -i 's:(module binary):(module binary\n):g' $(BIN_WAST_DIR)/*.bin.wast  
# endef

# This checks if spec/core exists
ifeq ($(SPEC_CORE_DIR),)
$(error "spec/test/core is not defined. Export its location before running)
else
$(info "Core dir OK!")	
endif

# this checks that we have the interpreter
ifeq ($(WASMI),)
$(error "Wasm Interpreter is not defined. Export its location before running)
else	
ifneq (,$(findstring wasm,$(shell $(WASMI) --help)))
$(info "Wasm Interpreter is probably the right one")
else
$(info "Wasm Interpreter is not the right one")
endif
endif

ifneq ($(SPEC_CORE_DIR),)
SRC_DIR ::= $(SPEC_CORE_DIR)
SRC_FILES ::= $(wildcard $(SRC_DIR)/*.wast)
BIN_WAST_FILES ::= $(patsubst $(SRC_DIR)/%.wast,$(BIN_WAST_DIR)/%.bin.wast,$(SRC_FILES))
endif

# Rule to transform the test files to binary
$(BIN_WAST_DIR)/%.bin.wast: $(SRC_DIR)/%.wast
	@echo "WAST2WASM "$@
	@$(WASMI) -d $< -o $@ &> /dev/null
	
# fix-files:
# 	$(shell BIN_WAST_DIR=$(BIN_WAST_DIR) ./fix_files.sh)

all: $(BIN_WAST_FILES)
	$(shell ./fix_files.sh)

clean: 
	$(RM) $(BIN_WAST_FILES)
