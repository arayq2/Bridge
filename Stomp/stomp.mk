
PROGRAMS := Sender ReceiverSync ReceiverAsync Socktest

VPATH = ../Stomp ../Utility

CXX = g++
CXXFLAGS = -pthread -m64 -std=c++14 -Wall
INCLUDES = ../Utility
STOMPOBJS = Stomp.o StompAgent.o

.cpp.o:
	$(CXX) -o $@ $(CXXFLAGS) -I $(INCLUDES) -c $<

all: $(PROGRAMS)


Sender: Sender.o StrFile.o $(STOMPOBJS)
	$(CXX) -o $@ $^

ReceiverSync: ReceiverSync.o $(STOMPOBJS)
	$(CXX) -o $@ $^

ReceiverAsync: ReceiverAsync.o $(STOMPOBJS)
	$(CXX) -o $@ $^

Socktest: Socktest.o $(STOMPOBJS)
	$(CXX) -o $@ $^

clean:
	rm -f $(PROGRAMS) *.o


.PHONY: all clean

