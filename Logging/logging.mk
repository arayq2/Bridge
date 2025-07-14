#
#

VPATH = ../Utility

CXX=g++

CXXFLAGS= -fPIC -O3

OBJS = Logging.o LogImplStub.o

%.o: %.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: liblogging.a

liblogging.a : $(OBJS)
	ar cr liblogging.a $(OBJS)

.PHONY: all