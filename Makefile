
# Skeleton make file. This is a WIP.

all : deck

deck:
	mkdir -p build
	cd build; $(MAKE) -f ../Deck/deck.mk

clean :
	cd build; $(MAKE) clean -f ../Deck/deck.mk

.PHONY: all
