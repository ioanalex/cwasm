CXX=c++

CXXFLAGS=-std=c++11 -Wall
LDFLAGS=

SRCDIR=src
OBJDIR=obj
BINDIR=.

SRCS= $(wildcard $(SRCDIR)/*.cpp)
OBJS= $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

EXEC=$(BINDIR)/cwasm

RM=rm -rf

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJS) $(LDFLAGS)
	@echo Compilation complete. Run the thing named "$@"...

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
# @echo "Compiled "$<" succesfully!"

clean:
	$(RM) $(OBJDIR)/* $(EXEC)

distclean: clean
	$(RM) $(EXEC)
