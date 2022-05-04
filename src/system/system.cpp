#include <SDL.h>
#include <SDL_ttf.h>

#include <system.hpp>

SDL_Surface *SystemDrv::ScreenSurface = NULL;
SDL_AudioSpec SystemDrv::AudioSpec = {
	.freq = 44100,
	.format = AUDIO_U8,
	.samples = 64,
	.callback = AudioCallback,
	.userdata = (void *) AudioCallbackData
};
SDL_Joystick *SystemDrv::Joystick = NULL;

void (*SystemDrv::AudioCallback) (void *, Uint8*, int) = NULL;
void *SystemDrv::AudioCallbackData = NULL;

bool SystemDrv::Run = false;


SystemDrv::Param Current = 0;


bool init_SDL (void) {
	return SDL_Init (SDL_INIT_EVERYTHING);
}

bool init_SND (void) {
	SDL_AudioSpec request = SystemDrv::AudioSpec;
	return SDL_OpenAudio (&request, &SystemDrv::AudioSpec);
}

bool init_FB (void) {
	return NULL == (
		SystemDrv::ScreenSurface = SDL_SetVideoMode (
			SCREEN_WIDTH, SCREEN_HEIGHT,
			32, SURFACE_TYPE
		)
	);
}

bool init_TTF (void) {
	return TTF_Init ();
}

bool init_JOYSTICK (void) {
	return NULL == (
		SystemDrv::Joystick = SDL_JoystickOpen (0)
	);
}


SystemDrv::Param SystemDrv::Init (SystemDrv::Param param) {
	Param to_be = param ^ (param & Current);
	Param init = 0;
	
	#define inits(what)	\
		if (to_be & (INIT_ ## what)) \
			init |= ((init_ ## what ()) == 0) * (INIT_ ## what);
	
	inits (SDL);
	inits (SND);
	inits (FB);
	inits (TTF);
	inits (JOYSTICK);
	
	#undef inits
	
	Current |= init;
	return init;
}



bool deinit_FB (void) {
	SDL_FreeSurface (SystemDrv::ScreenSurface);
	SystemDrv::ScreenSurface = NULL;
	return true;
}

bool deinit_SND (void) {
	SDL_CloseAudio ();
	return true;
}

bool deinit_JOYSTICK (void) {
	SDL_JoystickClose (SystemDrv::Joystick);
	SystemDrv::Joystick = NULL;
	return true;
}

bool deinit_TTF (void) {
	TTF_Quit ();
	return true;
}

bool deinit_SDL (void) {
	SDL_Quit ();
	return true;
}


SystemDrv::Param SystemDrv::DeInit (SystemDrv::Param param) {
	Param to_be = param & Current;
	Param deinit = 0;
	
	#define deinits(what) \
		if (to_be & (INIT_ ## what)) \
			deinit |= ((deinit_ ## what ()) == 0) * (INIT_ ## what);
	
	deinits (FB);
	deinits (SND);
	deinits (JOYSTICK);
	deinits (TTF);
	deinits (SDL);
	
	#undef deinits
	
	Current = !(deinit & Current);
	return deinit;
}

SystemDrv::Param SystemDrv::ForceInit (SystemDrv::Param param) {
	DeInit (Current);
	return Init (param);
}

bool SystemDrv::NextEvent (SDL_Event *event) {
	if (SDL_PollEvent (event)) {
		
		if (event -> type == SDL_QUIT)
			SystemDrv::Run = true;
		
		return true;
	
	} else
		return false;
}
