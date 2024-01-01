CPP_VER = c++20
FLAGS = -I ../include -O2 -std=$(CPP_VER)

.PHONY: all gomoku components core

all: gomoku

gomoku: components core
	mkdir -p ../bin
	g++ -o ../bin/gomoku $(FLAGS) main.cpp */*.o

components:
	$(MAKE) -C components all

core:
	$(MAKE) -C core all