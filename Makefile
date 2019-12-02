# This Makefile will compile and run all test/*.cpp files

CPP_FILES := $(wildcard test/*.cpp)
EXE_FILES := $(addprefix build/,$(notdir $(CPP_FILES:.cpp=.exe)))
CC_FLAGS += -MMD -Wall -Werror -Wextra -std=c++2a -I include

all: $(EXE_FILES)
clean:
	rm -r build

build/%.exe: test/%.cpp
	mkdir -p build
	g++ $(CC_FLAGS) -o $@ $<
	./$@

-include $(EXE_FILES:.exe=.d)
