CXX = g++
CXXFLAGS = -Wall

# Define the source files
SRC = test.cpp primitives/s_box.cpp primitives/placement.cpp primitives/bitstring.cpp primitives/feistel.cpp primitives/attack.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = test

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile the source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Subdirectory rule for primitives
primitives/%.o: primitives/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up the build files
clean:
	rm -f *.o $(TARGET)
