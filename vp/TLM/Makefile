# SystemC installation directory
SYSTEMC_HOME=/usr/local/systemc

# Compiler to use
CC=g++

# Compiler flags
CFLAGS=-c -I$(SYSTEMC_HOME)/include

# Linker flags
LDFLAGS=-L$(SYSTEMC_HOME)/lib-linux64 -lsystemc

# Source files
SOURCES=main.cpp memory.cpp cpu.cpp DMA.cpp InterCon.cpp hw.cpp ../../specification/cpp_implementation/MaxPoolLayer.cpp ../../specification/cpp_implementation/denselayer.cpp

# Object files
OBJECTS=$(SOURCES:.cpp=.o)

# Executable name
EXECUTABLE=main

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

