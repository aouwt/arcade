#include <SDL.h>
#include <SDL_ttf.h>
//#include <smpeg.h>
#include <system.hpp>
#include <games.hpp>



int main (void) {
	using namespace SystemDrv;
	Init (INIT_SDL | INIT_FB | INIT_JOYSTICK);
	
	SDL_Surface *Screen = ScreenSurface;
	
	TTF_Font *Font = TTF_OpenFont (DEFAULT_FONT, 64);
	//SDL_Surface *InsertTokenSurface = TTF_RenderSurfaceSmooth (
	
	while (Run) {
		
	}
	
	SDL_FreeSurface (Screen);
	TTF_CloseFont (Font);
	DeInit (INIT_ALL);
}
