MAKE_O:=-r -no-pie

CFLAGS:=-pipe -I./src/include -Ofast -march=native -mtune=native -Wall -Wextra -Wpedantic -Wno-missing-field-initializers -std=c++20
DEPS_FLAGS:=$(shell sdl-config --cflags)
DEPS_LIBS:=-lm -lX11 -lpthread -lstdc++ $(shell sdl-config --libs) -lSDL_ttf

DEPS:=${DEPS_FLAGS} ${DEPS_LIBS}

@PHONY: all clean install uninstall

all: ./arcade

clean:
	rm -f ./obj/* ./arcade.service

install: ./arcade ./arcade.service
	cp ./arcade.service $XDG_RUNTIME_DIR/systemd/user/
	systemctl --user reload-daemon
	systemctl --user enable arcade.service

uninstall:
	systemctl --user disable arcade.service
	rm $XDG_RUNTIME_DIR/systemd/user/arcade.service
	systemctl --user reload-daemon

./arcade.service:
	> ./arcade.service
	echo '[Unit]' >> ./arcade.service
	echo 'Description="Really cool arcade game :D"' >> ./arcade.service
	echo '' >> ./arcade.service
	echo '[Service]' >> ./arcade.service
	echo "ExecStart=$(shell pwd)/arcade" >> ./arcade.service
	echo '' >> ./arcade.service
	echo '[Install]' >> ./arcade.service
	echo 'WantedBy=multi-user.target' >> ./arcade.service

./arcade: ./obj/ball_game.o ./obj/system.o ./obj/main.o
	cc ${CFLAGS} ${DEPS_LIBS} ./obj/*.o -o ./arcade

./obj/ball_game.o:
	cc -x c++ ${MAKE_O} ${CFLAGS} ${DEPS_FLAGS} ./src/ball_game/*.cpp -o ./obj/ball_game.o

./obj/system.o:
	cc -x c++ ${MAKE_O} ${CFLAGS} ${DEPS_FLAGS} ./src/system/*.cpp -o ./obj/system.o

./obj/main.o:
	cc -x c++ ${MAKE_O} ${CFLAGS} ${DEPS_FLAGS} ./src/main.cpp -o ./obj/main.o

