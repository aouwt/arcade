#include <SDL.h>
#include <math.h>

#include "global.hpp"
#include "math.hpp"

void _Game::Demo (void) {
	Beam beam [MAX_BEAMS];
	
	while (Tokens == 0 && Exit == false) {
		SDL -> Frame ();
		Player.s.cpos.d += rndf;
		
		for (size_t i = 0; i != MAX_BEAMS; i ++)
			beam [i].Draw ();
	}
}

long _Game::Run (void) {
	Tokens --;
	Beam beam [MAX_BEAMS];
	size_t beams = 1;
	unsigned int flash = 0;
	long long nextbeamat = 2;
	float fuel = 0;
	
	Player.s.cpos = { 0, CIRC_RADIUS_DEF };
	Player.vel = { 0, 0 };
		
	while (Score -> current >= 0 && Exit == false) {
		Player.vel.x *= 0.99;
		if (fabs (Player.vel.x) <= 0.02)
			Player.vel.x += Player.dir.x / 1000;
		
		Player.vel.y *= 0.99;
		if (fabs (Player.vel.y) <= 1)
			Player.vel.y += Player.dir.y / 2;
		
		Player.s.cpos.r += Player.vel.y;
		Player.s.cpos.d += Player.vel.x;
		
		if (Player.s.cpos.r > CIRC_RADIUS_MAX)
			Player.s.cpos.r = CIRC_RADIUS_MAX;
		else
		if (Player.s.cpos.r < CIRC_RADIUS_MIN)
			Player.s.cpos.r = CIRC_RADIUS_MIN;
		
		if (!(flash & 1))
			Player.s.Place ();
		
		for (size_t i = 0; i != beams; i ++) {
			beam [i].Draw ();
			
			if (flash == 0) {
				if (beam [i].Collision ()) {
					Score -> Dec ((Score -> total / 4) * (beam [i].diameter / BEAM_MAXTHICKNESS));
					flash = FRAME_RATE * 5;
					beam [i].Restart ();
					Sound -> PlaySound (&Sound -> Effect.Damage);
				}
			}
		}
		
		fuel += (abs (Player.dir.x) + abs (Player.dir.y)) / (FRAME_RATE * 2);
		if (fuel > 1)
			Score -> Dec (fuel --);
		
		if (flash)
			flash --;
		
		if (Score -> total > nextbeamat) {
			nextbeamat += Score -> total;
			if (beams < MAX_BEAMS)
				beams ++;
		}
		
		SDL -> GameFrame ();
	}
	
	return Score -> maximum;
}


void _Game::ProcessEvent (SDL_JoyAxisEvent axis) {
	switch (axis.axis) {
		case 0: // X
			Player.dir.x = axis.value / 32767.0;
			break;
		case 1: // Y
			Player.dir.y = axis.value / 32767.0;
			break;
	}
}

void _Game::ProcessEvent (SDL_JoyButtonEvent btn) {
	switch (btn.button) {
		case BUTTON_TOKEN:
			Tokens ++;
			Sound -> PlaySound (&Sound -> Effect.Token);
			break;
	}
}

