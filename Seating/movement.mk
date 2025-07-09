
PROGRAMS := Movement

VPATH = ../Seating

CXX = g++
CXXFLAGS = -pthread -m64 -std=c++14 -Wall

INCLUDES = -I ../Utility

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

all: $(PROGRAMS)

Movement: Movement_main.o Movement.o
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(PROGRAMS) *.o

.PHONY: all clean

