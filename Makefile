.PHONY: default all autogen clean distclean

CXX=c++
CXXFLAGS=-std=c++17 -Wall -g
LDFLAGS=

SRCDIR=src
OBJDIR=obj
BINDIR=.

SRCS=$(wildcard $(SRCDIR)/*.cpp)
OBJS=$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
DEPS=$(patsubst %.cpp,%.d,$(SRCS))

EXEC=$(BINDIR)/cwasm

default: $(EXEC)
all: default

$(EXEC): $(OBJS)
	@echo "LINK  "$@
	@$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJS) $(LDFLAGS)
	@echo Compilation complete. Run the thing named "$@"...

autogen:
	@echo "AUTOGEN"
	./instrs.py > opcode_switch.inc

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	@echo "MKDIR "$@
	@mkdir $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "C++   "$@
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) -r $(OBJDIR)
	$(RM) $(DEPS)
	$(RM) *~

distclean: clean
	$(RM) $(EXEC)

# We don't need to clean up when we're making these targets
NODEPS:=clean distclean autogen

# Autogeneated switch in instructions.cpp
# src/instructions.cpp: opcode_switch.inc

# opcode_switch.inc: instrs.py
# 	$(./instrs.py > opcode_switch.inc)

# Don't create dependencies when we're cleaning, for instance
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
	# Chances are, these files don't exist.  GMake will create them and
	# clean up automatically afterwards
	-include $(DEPS)
endif

# This is the rule for creating the dependency files
$(SRCDIR)/%.d: $(SRCDIR)/%.cpp
	@echo "DEP   "$@ $
	@$(CXX) $(CXXFLAGS) -MM \
		-MT '$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$<)' \
		-MT '$(patsubst $(SRCDIR)/%.cpp,$(SRCDIR)/%.d,$<)' $< \
		-MF $@

# Formatting --- assumes a working clang-format installation
SOURCES=$(SRCS) $(wildcard $(SRCDIR)/*.hpp)

check-format:
	@for src in $(SOURCES) ; do \
		var=`clang-format "$$src" | diff "$$src" - | wc -l` ; \
		if [ $$var -ne 0 ] ; then \
			echo "$$src does not respect the coding style (diff: $$var lines)" ; \
		fi ; \
	done

format:
	@for src in $(SOURCES) ; do \
		echo "Formatting $$src..." ; \
		clang-format -i "$$src" ; \
	done
	@echo "Done"
