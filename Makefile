# A sample Makefile for building Google Test and using it in user
# tests.  Please tweak it to suit your environment and project.  You
# may want to move it to your project's root directory.
#
# SYNOPSIS:
#
#   make [all]  - makes everything.
#   make TARGET - makes the given target.
#   make clean  - removes all files generated by make.

# Please tweak the following variable definitions as needed by your
# project, except GTEST_HEADERS, which you can use in your own targets
# but shouldn't modify.

# Points to the root of Google Test, relative to where this file is.
# Remember to tweak this if you move this file.
GTEST_DIR = /usr/src/gtest

# Where to find user code.
USER_DIR = .

# Flags passed to the preprocessor.
CPPFLAGS += -I$(GTEST_DIR)/include

# Flags passed to the C++ compiler.
CXXFLAGS += -g -O3 -march=native -Wall -Wextra -std=c++11 -Wno-narrowing -Wno-unused-parameter -lGL -lSDL

# All tests produced by this Makefile.  Remember to add new tests you
# created to the list.
TESTS = test

# All examples produced by this Makefile.  Remember to add new examples you
# created to the list.
EXAMPLES = example

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = /usr/include/gtest/*.h \
                /usr/include/gtest/internal/*.h

# All libglsl++ headers.
GLSLPP_HEADERS = $(USER_DIR)/*.h

# House-keeping build targets.

all : $(TESTS) $(EXAMPLES)

clean :
	rm -f $(TESTS) $(EXAMPLES) gtest.a gtest_main.a *.o

# Builds gtest.a and gtest_main.a.

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

# Builds a sample test.  A test should link with either gtest.a or
# gtest_main.a, depending on whether it defines its own main()
# function.

test.o : $(USER_DIR)/test.cpp $(GTEST_HEADERS) $(GLSLPP_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/test.cpp

test : test.o gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@

example.o: $(USER_DIR)/example.cpp $(GLSLPP_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/example.cpp

example : example.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@
