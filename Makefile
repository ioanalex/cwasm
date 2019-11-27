CC=g++

CFLAGS=-Werror
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
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS) $(LDFLAGS)
	@echo Compilation compelte. Run the thing named cwasm...

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
# @echo "Compiled "$<" succesfully!"

clean: 
	$(RM) $(OBJDIR)/* $(EXEC)
