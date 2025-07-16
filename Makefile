
# Skeleton make file. This is a WIP.

all : logging stomp deck scoring

stomp:
	mkdir -p build
	cd build; $(MAKE) -f ../Stomp/stomp.mk

deck:
	mkdir -p build
	cd build; $(MAKE) -f ../Deck/deck.mk

scoring:
	mkdir -p build
	cd build; $(MAKE) -f ../Scoring/scoring.mk

logging:
	mkdir -p build
	cd build; $(MAKE) -f ../Logging/logging.mk

clean :
	cd build; $(MAKE) clean -f ../Stomp/stomp.mk
	cd build; $(MAKE) clean -f ../Deck/deck.mk
	cd build; $(MAKE) clean -f ../Scoring/scoring.mk
	cd build; $(MAKE) clean -f ../Logging/logging.mk

.PHONY: all clean
