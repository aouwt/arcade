#include <SDL.h>
#include <SDL_thread.h>
#include <pthread.h>
#include <math.h>

#include "global.hpp"
#include "math.hpp"


int draw_thread (void *obj) {
	_Game *_this = (_Game *) obj;
	
	while (Exit == false) {
		pthread_mutex_lock (&_this -> Thread_Mutex);
		pthread_cond_wait (&_this -> Thread_Cond, &_this -> Thread_Mutex);
		pthread_mutex_unlock (&_this -> Thread_Mutex);
		
		for (size_t i = 0; i != _this -> Beams; i ++)
			_this -> beam [i].Draw ();
		
		if (_this -> DemoOn)
			SDL -> Frame ();
		
		else {
			if (!(_this -> Player.flash & 1)) {
				_this -> Player.s.Place ();
			}
			
			if (_this -> Player.flash)
				_this -> Player.flash --;
			
			SDL -> GameFrame ();
		}
	}
	
	return 0;
}


_Game::_Game (void) {
	Thread_Thread = SDL_CreateThread (draw_thread, this);
}

_Game::~_Game (void) {
}

void _Game::Demo (void) {
	DemoOn = true;
	Beams = MAX_BEAMS;
	while (Tokens == 0 && Exit == false) {
		pthread_mutex_lock (&Thread_Mutex);
		pthread_cond_signal (&Thread_Cond);
		pthread_mutex_unlock (&Thread_Mutex);
		
		Player.s.cpos.d += rndf;
		
		for (size_t i = 0; i != MAX_BEAMS; i ++)
			beam [i].Frame ();
		
		SDL -> NextFrame ();
	}
}

long _Game::Run (void) {
	DemoOn = false;
	Tokens --;
	long long nextbeamat = 2;
	Player.flash = FRAME_RATE * 5;
	
	Player.s.cpos = { 0, CIRC_RADIUS_DEF };
	Player.vel = { 0, 0 };
	Beams = 10;
		
	while (Score -> current >= 0 && Exit == false) {
		pthread_mutex_lock (&Thread_Mutex);
		pthread_cond_signal (&Thread_Cond);
		pthread_mutex_unlock (&Thread_Mutex);
		
		Player.vel.x *= 0.99;
		if (fabs (Player.vel.x) <= 0.02)
			Player.vel.x += Player.dir.x / 1000.0;
		
		Player.vel.y *= 0.99;
		if (fabs (Player.vel.y) <= 1)
			Player.vel.y += Player.dir.y / 2.0;
		
		Player.s.cpos.r += Player.vel.y;
		Player.s.cpos.d += Player.vel.x;
		
		if (Player.s.cpos.r > CIRC_RADIUS_MAX)
			Player.s.cpos.r = CIRC_RADIUS_MAX;
		else
		if (Player.s.cpos.r < CIRC_RADIUS_MIN)
			Player.s.cpos.r = CIRC_RADIUS_MIN;
		
		for (size_t i = 0; i != Beams; i ++) {
			beam [i].Frame ();
			
			if (Player.flash == 0) {
				if (beam [i].Collision ()) {
					Score -> Dec ((Score -> total / 4) * (beam [i].diameter / BEAM_MAXTHICKNESS));
					Player.flash = FRAME_RATE * 5;
					beam [i].Restart ();
					Sound -> PlaySound (&Sound -> Effect.Damage);
				}
			}
		}
		
		if (Player.flash)
			Score -> Dec ((abs (Player.dir.x) + abs (Player.dir.y)) * Score -> current * 0.001);
		
		if (Score -> total > nextbeamat) {
			nextbeamat += Score -> total;
			Beams *= 1.1;
			if (Beams >= MAX_BEAMS)
				Beams = MAX_BEAMS;
		}
		
		SDL -> NextFrame ();
	}
	
	return Score -> maximum;
}


void _Game::ProcessEvent (SDL_JoyAxisEvent axis) {
	switch (axis.axis) {
		case 0: // X
			Player.dir.x = (axis.value > 0) ? 1 : (axis.value < 0) ? -1 : 0;
			break;
		case 1: // Y
			Player.dir.y = (axis.value > 0) ? 1 : (axis.value < 0) ? -1 : 0;
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

