Requires a joystick! Press start to, well, start, and use the stick to move.

Building dependencies:
 - `libsdl-dev` (SDL 1.2, not SDL 2.0)
 - `libsdl-ttf2.0-dev`
 - `pkg-config`

Build with `g++ *.cpp $(pkg-config --libs --cflags sdl SDL_ttf) -lm -O3`
