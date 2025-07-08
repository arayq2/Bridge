

all : deck

deck:
	export NLOHMANN="../Utility"
	cd Deck; $(MAKE) -f ./deck.mk

clean :
	cd Deck; $(MAKE) clean -f ./deck.mk

.PHONY: all
