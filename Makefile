
# Skeleton make file. This is a WIP.

all : stomp deck

stomp:
	mkdir -p build
	cd build; $(MAKE) -f ../Stomp/stomp.mk

deck:
	mkdir -p build
	cd build; $(MAKE) -f ../Deck/deck.mk

clean :
	cd build; $(MAKE) clean -f ../Stomp/stomp.mk
	cd build; $(MAKE) clean -f ../Deck/deck.mk

.PHONY: all
