CXX = g++
CXXFLAGS = -Wall -Wextra
TARGET = ptrace_traceme

all: $(TARGET)

$(TARGET): ptrace_traceme.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) ptrace_traceme.cpp

clean:
	rm -f $(TARGET)

.PHONY: all clean
