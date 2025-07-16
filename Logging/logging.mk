#
#

VPATH := ../Logging

CXX=g++

CXXFLAGS= -fPIC -O3

INCLUDES= -I ../Utility

OBJS := Logging.o LogImplStub.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

all: liblogging.a

liblogging.a : $(OBJS)
	ar cr liblogging.a $(OBJS)

clean:
	rm -f liblogging.a $(OBJS)

.PHONY: all clean
