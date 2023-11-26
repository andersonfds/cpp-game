CXX = g++

# Includes
INCLUDES=-I lib/headers -I/Users/anderson/homebrew/Cellar/libpng/1.6.40/include/libpng16 -Isrc/*.h

#LDFlags
LDFLAGS=-L/Users/anderson/homebrew/Cellar/libpng/1.6.40/lib -lpng16 -framework OpenGL -framework GLUT -lobjc

# Compiler flags
CXXFLAGS=-std=c++17

# Source files
SRC=game.cpp

# Binary name
BIN=main

# Build rule
build:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $(BIN) $(LDFLAGS)
	./$(BIN)

# Clean rule
clean:
	rm -f $(BIN)