all: miner

# add header (.h) files here
HDR=siggen.h

# add source (.c) files here
SRC=siggen.c 

miner: $(SRC) $(HDR)
	gcc -Wall -o miner $(SRC) -l pthread

