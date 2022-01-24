CXX=icpx
CXX_FLAGS:=$(CXX_FLAGS) -std=c++17

all: test-property

test-property: test-property.cc
	$(CXX) $(CXX_FLAGS) -I../include/ test-property.cc -o test-property

test: all
	./test-property

clean:
	rm -f ./test-property
