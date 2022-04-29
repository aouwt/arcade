#include <SDL.h>
#include <math.h>

#include "game.hpp"

Beam::Beam (void) {
	diameter = (rndf * BEAM_MAXTHICKNESS) + BEAM_MINTHICKNESS;
	
	sprite.surface = SDL_CreateRGBSurface (
		SDL_SWSURFACE,
		diameter, diameter,
		32, MASK
	);
	
	SDL_FillRect (sprite.surface, NULL, 0xFF000000 | rand ());
	Restart ();
}

Beam::~Beam (void) {
	SDL_FreeSurface (sprite.surface); sprite.surface = NULL;
}


void Beam::Draw (void) {
	if (breaktime) {
		if (-- breaktime == 0) {
			vel.d = (rnd (1) - 0.5) / 1000.0;
			vel.r = rnd (1) + 1;
			
			sprite.cpos.d = Game -> Player.s.cpos.d + ((rndf - .5) * (RAD_MAX / 5));
			sprite.cpos.r = 0;
		}
		return;
	}
	
	sprite.cpos.d += vel.d;
	sprite.cpos.r += vel.r;
	sprite.Place ();
	
	if (sprite.cpos.r >= SCR_WIDTH / 2) {
		Score -> Inc (vel.r * 10);
		Restart ();
	}
}

void Beam::Restart (void) {
	breaktime = rnd (128);
}

bool Beam::Collision (void) {
	if (breaktime)
		return false;
	else
		return sqrt (
			pow (sprite.pos.x - Game -> Player.s.pos.x, 2) +
			pow (sprite.pos.y - Game -> Player.s.pos.y, 2)
		) < diameter;
}
