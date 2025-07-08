

PROGRAMS := SeeDeal SeeFmt ToHex ToPbn ToLin

VPATH = ../Deck

CXX = g++
CXXFLAGS = -g -pthread -m64 -std=c++14 -Wall

# Need to set this in the environment
INCLUDES = -I $(NLOHMANN)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

all: $(PROGRAMS)

SeeDeal: SeeDeal.o Deck.o Deal.o
	$(CXX) $(CXXFLAGS) -o $@ $^

SeeFmt: SeeFmt.o  Deck.o Deal.o
	$(CXX) $(CXXFLAGS) -o $@ $^

ToHex: ToHex.o  Deck.o Deal.o
	$(CXX) $(CXXFLAGS) -o $@ $^

ToPbn: ToPbn.o  Deck.o Deal.o
	$(CXX) $(CXXFLAGS) -o $@ $^

ToLin: ToLin.o  Deck.o Deal.o
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(PROGRAMS) *.o

.PHONY: all
