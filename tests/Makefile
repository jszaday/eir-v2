CXX=icpx
INCLUDES=-I../include/ -I../
CXX_FLAGS:=$(CXX_FLAGS) -std=c++17 $(INCLUDES)

SRCS = $(wildcard test-*.cc)
OBJS = $(patsubst %.cc,%.o,$(SRCS)) tinyxml2.o
EXES = $(patsubst %.cc,%.out,$(SRCS))

all: $(EXES)

tinyxml2.o: ../tinyxml2/tinyxml2.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

%.o: %.cc
	$(CXX) $(CXX_FLAGS) -c $< -o $@

%.out: %.o tinyxml2.o
	$(CXX) $(LDFLAGS) $^ -o $@ $(LDLIBS)

test: all
	for x in *.out; do ./$$x; done

clean:
	rm -f $(OBJS) $(EXES)
