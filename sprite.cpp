#include <SDL.h>

#include "global.hpp"
#include "math.hpp"

Sprite::~Sprite (void) {
	SDL_FreeSurface (surface); surface = NULL;
}

void Sprite::DefaultOrigin (void) {
	origin.x = surface -> w / 2;
	origin.y = surface -> h / 2;
}

void Sprite::Place (void) {
	pos.x = (SCR_WIDTH / 2) + (fastsin <float> (cpos.d) * cpos.r);
	pos.y = (SCR_HEIGHT/ 2) + (fastcos <float> (cpos.d) * cpos.r);
	
	SDL_Rect rect = {
		Sint16 (pos.x + origin.x),
		Sint16 (pos.y + origin.y),
		0, 0
	};
	
	SDL_BlitSurface (surface, NULL, SDL -> Screen, &rect);
}
