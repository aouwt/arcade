#include <SDL.h>
#include <SDL_ttf.h>
#include <system.hpp>
#include <games.hpp>

int main (void) {
	SystemDrv::Init (INIT_SDL | INIT_FB);
	
	SDL_Surface *Screen = SystemDrv::ScreenSurface;
	
	TTF_Font *Font = TTF_OpenFont (DEFAULT_FONT, 64);
}
