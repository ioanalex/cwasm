.PHONY: all clean

CXX=c++
CXXFLAGS=-std=c++17 -Wall -O2
LDFLAGS=

PROGRAMS=fairsum-2N fairsum-ON2 fairsum-ON3 fairsum \
  longsumk-ON2 longsumk-ON3 longsumk \
  samenum-ON2-better samenum-ON2 samenum \
  cnteven-loop cnteven third-if third twobase vicky

%: %.cpp
	@echo "C++   "$@
	@$(CXX) $(CXXFLAGS) $< -o $@

all: $(PROGRAMS)

clean:
	$(RM) $(PROGRAMS)
	$(RM) *~
