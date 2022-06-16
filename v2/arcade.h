#ifndef ARCADE_H
	#define ARCADE_H
	
	#include <SDL.h>
	
	struct System;
	struct GameInfo;
	struct GameContext;
	
	typedef int (* GameInfo_Func) (struct GameContext *);
	
	struct System {
		SDL_Surface *Screen;
		SDL_Joystick *Joy;
		
		int Tokens;
	};
	
	struct GameInfo {
		char *Name;
		int Tokens;
		
		GameInfo_Func New, Del, Select;
	};

	struct GameContext {
		struct System *System;
		const struct GameInfo *Info;
		SDL_Surface *Cover;
		const SDL_AudSpec *Audio;
		
		void *Obj;
		void *Persist;
		void *DlHdl;
	};
	
#endif
