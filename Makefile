MAKE_O=-r -no-pie

CFLAGS=-pipe -I./src/include -Wall -Wextra -Wpedantic -Wno-missing-field-initializers -std=c++20 ${_CFLAGS}
DEPS_FLAGS=$(shell sdl-config --cflags)
DEPS_LIBS=-lm -lX11 -lpthread -lstdc++ $(shell sdl-config --libs) -lSDL_ttf

DEPS=${DEPS_FLAGS} ${DEPS_LIBS}

all: ./arcade

@PHONY: all clean install uninstall


clean:
	rm -f ./obj/* ./arcade.service

install: ./arcade ./arcade.service
	cp ./arcade.service ~/.config/systemd/user/
	systemctl --user daemon-reload
	systemctl --user enable arcade.service

uninstall:
	systemctl --user disable arcade.service
	rm ~/.config/systemd/user/arcade.service
	systemctl --user daemon-reload

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
	cc ./obj/*.o ${DEPS} ${CFLAGS} -o ./arcade

./obj/ball_game.o: ./src/ball_game/*.cpp
	cc -x c++ ./src/ball_game/*.cpp ${MAKE_O} ${DEPS_FLAGS} ${CFLAGS} -o ./obj/ball_game.o

./obj/system.o: ./src/system/*.cpp
	cc -x c++ ./src/system/*.cpp ${MAKE_O} ${DEPS_FLAGS} ${CFLAGS} -o ./obj/system.o

./obj/main.o: ./src/main.cpp
	cc -x c++ ./src/main.cpp ${MAKE_O} ${DEPS_FLAGS} ${CFLAGS} -o ./obj/main.o
