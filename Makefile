.DEFAULT_GOAL := build_and_run
CXX = g++

# Includes
LIBPNG_INCLUDE=$(shell libpng-config --I_opts)
INCLUDES=-Ilib/headers $(LIBPNG_INCLUDE) -Isrc/*.h

#LDFlags
LIBPNG_LDFLAGS=$(shell libpng-config --L_opts)
LDFLAGS=$(LIBPNG_LDFLAGS) -lpng16 -framework OpenGL -framework GLUT -lobjc
CFLAGS_RELEASE += -O2 -s -DNDEBUG

# Compiler flags
CXXFLAGS=-std=c++17

# Source files
SRC=game.cpp

# Binary name
BIN=main

# Build rule
build: clean
	@which libpng-config > /dev/null || (echo "libpng-config not found, please install libpng" && exit 1)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $(BIN) $(LDFLAGS)
	@$(MAKE) pack

run:
	./$(BIN)

pack:
	./$(BIN) --pack

build_and_run: build run clean

release: clean
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $(BIN) $(LDFLAGS) $(CFLAGS_RELEASE)
	mkdir -p release
	cp $(BIN) release/$(BIN)
	cp *.dat release
	pushd release && zip -r $(BIN).zip $(BIN) pack.dat && popd

# Clean rule
clean:
	@rm -rf release
	@rm -f $(BIN)
	@rm -f pack.dat