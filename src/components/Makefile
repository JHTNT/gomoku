CPP_VER = c++20
FLAGS = -I ../../include -O2 -std=$(CPP_VER)
SRC := $(wildcard *.cpp)

.PHONY: all

all: $(SRC)
	g++ -c $^ $(FLAGS)