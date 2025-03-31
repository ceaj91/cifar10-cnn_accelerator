# Compiler to use
CC=g++

# Compiler flags
CFLAGS=-c -Wall

# Source files
SOURCES=main.cpp ConvLayer.cpp denselayer.cpp flattenlayer.cpp MaxPoolLayer.cpp

# Object files
OBJECTS=$(SOURCES:.cpp=.o)

# Executable name
EXECUTABLE=cnn

# Default target
all: $(SOURCES) $(EXECUTABLE)


# Build target
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

# Compile target
.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ 

run_valgrind:
	valgrind --tool=callgrind ./$(EXECUTABLE) ../files.txt 

# Clean target
clean:
	rm -f $(OBJECTS) $(EXECUTABLE) callgrind*
