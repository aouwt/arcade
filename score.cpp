#include <SDL.h>

#include "global.hpp"


_Score::_Score (void) {
	Reset ();
}

_Score::~_Score (void) {
	SDL_FreeSurface (surface);
}

void _Score::Inc (unsigned int amt) {
	current += amt;
	total += amt;
	if (current > maximum)
		maximum = current;
	
	changed = true;
}

void _Score::Dec (unsigned int amt) {
	current -= amt;
	changed = true;
}

void _Score::Reset (void) {
	total = maximum = current = 100;
}
