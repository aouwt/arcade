INCLUDE:=-I./src/include/
MAKE_O:=-r -no-pie

CFLAGS:=-Ofast -Wall -Wextra -Wpedantic -Wno-missing-initializers ${INCLUDE} -std=c++20
DEPS_FLAGS:=$(shell pkg-config --cflags sdl SDL_ttf)
DEPS_LIBS:=$(shell pkg-config --libs sdl SDL_ttf) -lm -lX11

@PHONY:	all clean

all: ./arcade

clean:
	rm ./obj/*

./arcade: ./obj/ball_game.o ./obj/system.o
	g++ ./obj/*.o ${CFLAGS} -o ./arcade ${DEPS_LIBS}

./obj/ball_game.o:
	g++ ./src/ball_game/*.cpp ${MAKE_O} ${CFLAGS} ${DEPS_FLAGS} -o ./obj/ball_game.o

./obj/system.o:
	g++ ./src/system/*.cpp ${MAKE_O} ${CFLAGS} ${DEPS_FLAGS} -o ./obj/system.o
