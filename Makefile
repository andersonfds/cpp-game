.DEFAULT_GOAL := build_and_run
CXX = g++

# Includes
LIBPNG_INCLUDE=$(shell libpng-config --I_opts)
INCLUDES=-Ilib/headers $(LIBPNG_INCLUDE) -Isrc/*.h

#LDFlags
LIBPNG_LDFLAGS=$(shell libpng-config --L_opts)
LDFLAGS=$(LIBPNG_LDFLAGS) -lpng16 -framework OpenGL -framework GLUT -lobjc

# Compiler flags
CXXFLAGS=-std=c++17

# Source files
SRC=game.cpp

# Binary name
BIN=main

# Build rule
build:
	@which libpng-config > /dev/null || (echo "libpng-config not found, please install libpng" && exit 1)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $(BIN) $(LDFLAGS)

run:
	./$(BIN)

build_and_run: build run clean

# Clean rule
clean:
	@rm -f $(BIN)