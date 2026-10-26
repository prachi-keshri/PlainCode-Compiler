CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -pedantic -I include

SOURCES = src/lexer.cpp \
          src/parser.cpp \
          src/semantic.cpp \
          src/tac_generator.cpp \
          src/optimizer.cpp \
          src/codegen.cpp \
          src/main.cpp

TARGET = build/plaincode

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES) $(wildcard include/*.h)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -rf build
