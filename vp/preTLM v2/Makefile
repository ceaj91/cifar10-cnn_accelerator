# SystemC installation directory
SYSTEMC_HOME=/usr/local/systemc

# Compiler to use
CC=g++

# Compiler flags
CFLAGS=-c -Wall -I$(SYSTEMC_HOME)/include

# Linker flags
LDFLAGS=-L$(SYSTEMC_HOME)/lib-linux64 -lsystemc

# Source files
SOURCES=main.cpp memory.cpp conv.cpp hw.cpp cpu.cpp ../../specification/cpp_implementation/denselayer.cpp MaxPoolLayer.cpp

# Object files
OBJECTS=$(SOURCES:.cpp=.o)

# Executable name
EXECUTABLE=test

# Default target
all: $(SOURCES) $(EXECUTABLE)

# Build target
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

# Compile target
.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

# Clean target
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
