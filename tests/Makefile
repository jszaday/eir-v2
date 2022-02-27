CXX=icpx
CXX_FLAGS:=$(CXX_FLAGS) -std=c++17 -I../include/

SRCS = $(wildcard test-*.cc)
OBJS = $(patsubst %.cc,%.o,$(SRCS))
EXES = $(patsubst %.cc,%.out,$(SRCS))

all: $(EXES)

%.o: %.cc
	$(CXX) $(CXX_FLAGS) -c $< -o $@

%.out: %.o
	$(CXX) $(LDFLAGS) $^ -o $@ $(LDLIBS)

test: all
	for x in *.out; do ./$$x; done

clean:
	rm -f $(OBJS) $(EXES)
