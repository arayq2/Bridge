
PROGRAMS := Score

VPATH = ../Scoring

CXX = g++
CXXFLAGS = -pthread -m64 -std=c++14 -Wall

INCLUDES = -I ../Utility

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

all: $(PROGRAMS)

Score: Score.o Contract.o
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(PROGRAMS) *.o

.PHONY: all clean

