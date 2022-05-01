#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
/*#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kd.h>*/

#include "global.hpp"


_SDL::_SDL (void) {
	_ ("SDL",
		SDL_Init (SDL_INIT_EVERYTHING),
	"Could not initialize SDL!");
	
	_ ("TTF",
		TTF_Init (),
	"Could not initialize SDL_ttf!");
	
	/*fprintf (stderr, "Done\n\tFB Setup...");
	{	int fd = open ("/dev/tty0", O_RDONLY);
		_ (fd < 0, "Could not open frame buffer!");
		ioctl (fd, KDSETMODE, KD_TEXT);
		close (fd);
	}*/
	
	_ ("FB",
		NULL == (
			Screen = SDL_SetVideoMode (
				SCR_WIDTH, SCR_HEIGHT,
				32, SDL_HWSURFACE
			)
		),
	"Could not initialize frame buffer!");
	
	_ ("Font",
		NULL == (
			Font = TTF_OpenFont (FONTPATH, FONTSZ)
		),
	"Could not open font \"%s\" at size %i", FONTPATH, FONTSZ)
	
	_ ("Joystick",
		NULL == (
			Stick = SDL_JoystickOpen (0)
		),
	"Could not open joystick!");

	
	// player surface
	Game -> Player.s.surface = SDL_CreateRGBSurface (
		SDL_HWSURFACE,
		16, 16,
		32, MASK
	);
	
	SDL_FillRect (Game -> Player.s.surface, NULL, 0xFF000000);
	
	// bkg surface
	Bkg = SDL_CreateRGBSurface (
		SDL_HWSURFACE,
		SCR_WIDTH, SCR_HEIGHT,
		32, MASK
	);
			
	SDL_FillRect (Bkg, NULL, 0xFFFFFFFF);
}

_SDL::~_SDL (void) {
	TTF_CloseFont (Font);
	TTF_Quit ();
	SDL_FreeSurface (Score -> surface); Score -> surface = NULL;
	SDL_FreeSurface (Bkg); Bkg = NULL;
	SDL_FreeSurface (Game -> Player.s.surface); Game -> Player.s.surface = NULL;
	SDL_Quit ();
}

void _SDL::NextFrame (void) {
	static unsigned long last = 0;
	unsigned long now = SDL_GetTicks ();
	int diff = (last + (1000 / FRAME_RATE)) - now;
	if (diff > 10)
		SDL_Delay (diff);
	last = now;
}

void _SDL::Frame (void) {
	PollEvents ();
	
	SDL_Flip (Screen);
	SDL_BlitSurface (Bkg, NULL, Screen, NULL);
}

void _SDL::GameFrame (void) {
	Frame ();
	
	if (Score -> changed) {
		// Make score surface
		char str [64];
		snprintf (str, 64, "%li / %lli", Score -> current, Score -> total);
		Score -> surface = TTF_RenderText_Solid (Font, str, { 0, 0, 0, 0 });
		Score -> changed = false;
	}
	
	{	SDL_Rect rect = {
			0, 0,
			Uint16 (((float) Score -> current / Score -> total) * SCR_WIDTH), 16
		};
		SDL_FillRect (Screen, &rect, 0xFFFF00FF);
	}
	
	SDL_BlitSurface (Score -> surface, NULL, Screen, NULL);
}

	

void _SDL::PollEvents (void) {
	SDL_Event event;
	while (SDL_PollEvent (&event)) {
		switch (event.type) {
			case SDL_QUIT: Exit = true;
				break;
			
			case SDL_JOYAXISMOTION: Game -> ProcessEvent (event.jaxis);
				break;
			
			case SDL_JOYBUTTONDOWN: Game -> ProcessEvent (event.jbutton);
				break;
		}
	}
}
