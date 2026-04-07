
PROGRAMS := Sender ReceiverSync ReceiverAsync Socktest Echo

VPATH = ../Stomp ../Utility

CXX = g++
CXXFLAGS = -pthread -m64 -std=c++14 -Wall
INCLUDES = ../Utility
STOMPOBJS = StompImpl.o StompAgent.o

.cpp.o:
	$(CXX) -o $@ $(CXXFLAGS) -I $(INCLUDES) -c $<

all: $(PROGRAMS)

libstomp.a: $(STOMPOBJS)
	ar cr libstomp.a $^

Sender: Sender.o StrFile.o libstomp.a
	$(CXX) -o $@ $^ -lpthread

ReceiverSync: ReceiverSync.o libstomp.a
	$(CXX) -o $@ $^ -lpthread

ReceiverAsync: ReceiverAsync.o libstomp.a
	$(CXX) -o $@ $^ -lpthread

Socktest: Socktest.o libstomp.a
	$(CXX) -o $@ $^ -lpthread

Echo: Echo.o libstomp.a
	$(CXX) -o $@ $^ -lpthread

clean:
	rm -f $(PROGRAMS) libstomp.a *.o


.PHONY: all clean

