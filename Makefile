MAKE_O:=-r -no-pie

CFLAGS:=-I./src/include -Ofast -march=native -mtune=native -Wall -Wextra -Wpedantic -Wno-missing-field-initializers -std=c++20
DEPS_FLAGS:=$(shell sdl-config --cflags)
DEPS_LIBS:=-lm -lX11 -lpthread $(shell sdl-config --libs) -lSDL_ttf

@PHONY:	all clean

all: ./arcade

clean:
	rm ./obj/*

./arcade: ./obj/ball_game.o ./obj/system.o ./obj/main.o
	g++ ${DEPS} ${CFLAGS} ./obj/*.o -o ./arcade

./obj/ball_game.o:
	g++ ${MAKE_O} ${DEPS_FLAGS} ${CFLAGS} ./src/ball_game/*.c* -o ./obj/ball_game.o

./obj/system.o:
	g++ ${MAKE_O} ${DEPS_FLAGS} ${CFLAGS} ./src/system/*.cpp -o ./obj/system.o

./obj/main.o:
	g++ ${MAKE_O} ${DEPS_FLAGS} ${CFLAGS} ./src/main.cpp -o ./obj/main.o
