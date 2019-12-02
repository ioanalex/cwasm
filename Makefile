# Add .d to Make's recognized suffixes.
SUFFIXES += .d
#We don't need to clean up when we're making these targets
NODEPS:=clean distclean

CXX=c++

CXXFLAGS=-std=c++11 -Wall
LDFLAGS=

SRCDIR=src
OBJDIR=obj
BINDIR=.

SRCS=$(wildcard $(SRCDIR)/*.cpp)
DEPS=$(patsubst %.cpp,%.d,$(SRCS))
OBJS=$(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

EXEC=$(BINDIR)/cwasm

RM=rm -rf

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJS) $(LDFLAGS)
	@echo Compilation complete. Run the thing named "$@"...

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(SRCDIR)/%.d
	$(CXX) $(CXXFLAGS) -c $< -o $@
# @echo "Compiled "$<" succesfully!"

clean:
	$(RM) $(OBJDIR)/* $(EXEC)

distclean: clean
	$(RM) $(EXEC)

#Don't create dependencies when we're cleaning, for instance
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
	#Chances are, these files don't exist.  GMake will create them and
	#clean up automatically afterwards
	-include $(DEPS)
endif

#This is the rule for creating the dependency files
$(SRCDIR)/%.d: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -MM -MT '$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$<)' $< -MF $@
