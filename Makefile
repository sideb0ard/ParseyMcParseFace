CC=clang++

GTEST_DIR =  /Users/sideboard/Code/googletest/googletest
GTEST_LIB_DIR = /Users/sideboard/Code/googletest/googletest/make

# Flags passed to the preprocessor.
CPPFLAGS=-isystem $(GTEST_DIR)/include

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -pthread -std=c++17

# Google Test libraries
GTEST_LIBS = libgtest.a libgtest_main.a

DEPS = parsey.hpp lexer.hpp token.hpp
MAIN = parsey.cpp
TARGET = slang
OBJ = lexer.cpp token.cpp ast.cpp parser.cpp
LEXER_TESTS = lexer_test.cpp
PARSER_TESTS = parser_test.cpp
INC=-I${HOME}/Code/range-v3/include/

CTAGS:
	@ctags -R *

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h


all: $(TARGET)


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CPPFLAGS) $(INC)

$(TARGET): $(MAIN) $(OBJ)
	$(CTAGS)
	$(CC) $(CPPFLAGS) $(INC) $(CXXFLAGS) $^ -o $@

lexer_test: $(GTEST_LIBS) $(LEXER_TESTS) $(OBJ)
	$(CTAGS)
	$(CC) $(CPPFLAGS) $(INC) $(CXXFLAGS) -L$(GTEST_LIB_DIR) -lgtest -lpthread $^ -o $@


parser_test: $(GTEST_LIBS) $(PARSER_TESTS) $(OBJ)
	$(CTAGS)
	$(CC) $(CPPFLAGS) $(INC) $(CXXFLAGS) -L$(GTEST_LIB_DIR) -lgtest -lpthread $^ -o $@

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

libgtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

libgtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^
