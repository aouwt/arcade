#ifndef SYSTEM_HPP
	#define SYSTEM_HPP
	
	#include <SDL.h>
	#include <stdint.h>
	#include <stdio.h>

	#define INIT_SDL	1
	#define INIT_SND	2
	#define INIT_TTF	4
	#define INIT_FB	8
	#define INIT_JOYSTICK	16

	#define INIT_ALL	255

	#define SCREEN_WIDTH	1280
	#define SCREEN_HEIGHT	1024

	#define DEFAULT_FONT	"/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"

	#define SURFACE_TYPE	SDL_HWSURFACE

	namespace SystemDrv {
		typedef uint_least8_t Param;
		typedef bool (*GameFunc) (void);
		
		extern Param Init (Param param);
		extern Param DeInit (Param param);
		extern Param ForceInit (Param param);
		extern bool NextEvent (SDL_Event *event);
		
		extern SDL_Surface *ScreenSurface;
		extern SDL_AudioSpec AudioSpec;
		extern SDL_Joystick *Joystick;
		
		extern void (*AudioCallback) (void *, Uint8*, int);
		extern void *AudioCallbackData;
		
		extern bool Run;
	};


	class SSystem {
		private:
			SystemDrv::Param init;
			
		public:
			System (SystemDrv::Param what) {
				if (SystemDrv::Init (what) != what) {
					fprintf (stderr, "Failed to initialize one or more modules!\n");
					exit (1);
				}
				init = what;
			}
			
			~System (void) {
				if (SystemDrv::DeInit (init) != init) {
					fprintf (stderr, "Failed to deinitialize one or more modules!\n");
					exit (1);
				}
				init = 0;
			}
	};
#endif
