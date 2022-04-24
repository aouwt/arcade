#define SCR_WIDTH	1280
#define SCR_HEIGHT	1024

#define FRAME_RATE	30

#define MAX_BEAMS	100
#define PLAYER_RADIUS	8
#define BEAM_THICKNESS	8


#include <SDL.h>
#include <math.h>


#define _(func, ...)	\
	if (func) {	\
		fprintf (stderr, __VA_ARGS__); \
		fputc ('\n', stderr); \
		exit (1); \
	}
#define rnd(range)	((rand () / float (RAND_MAX)) * range)

#define MASK	0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000


#define CIRC_RADIUS_DEF	(SCR_WIDTH * 0.25)
#define CIRC_RADIUS_MIN	100
#define CIRC_RADIUS_MAX	((SCR_HEIGHT * 0.5) - 100)
#define CENTER_SCREEN	{ SCR_WIDTH / 2, SCR_HEIGHT / 2 }

#define RAD_MAX	(2 * M_PI)

typedef struct {
	float x = 0;
	float y = 0;
} Coord;

typedef struct {
	float d = 0;
	float r = 0;
} CCoord;

typedef struct {
	Coord origin;
	SDL_Surface *surface = NULL;
	Coord pos;
	CCoord cpos;
} Sprite;


struct {
	Sprite s;
	Coord vel;
} Player;

struct {
	SDL_Surface *surface;
} Bkg;

void ProcessEvent (SDL_JoyAxisEvent axis) {
	switch (axis.axis) {
		case 0: // X
			Player.vel.x = axis.value / (32768.0 * 100.0);
			break;
		case 1: // Y
			Player.vel.y = axis.value / (32768.0 * 0.5);
			break;
	}
}


namespace SDL {
	SDL_Surface *Screen;
	SDL_Event Events;
	SDL_Joystick *Stick;
	
	
	namespace SurfaceGen {
		void Player (void) {
			::Player.s.surface = SDL_CreateRGBSurface (
				SDL_SWSURFACE,
				16, 16,
				32, MASK
			);
			
			SDL_FillRect (::Player.s.surface, NULL, 0xFF000000);
		}
		
		void Bkg (void) {
			::Bkg.surface = SDL_CreateRGBSurface (
				SDL_SWSURFACE,
				SCR_WIDTH, SCR_HEIGHT,
				32, MASK
			);
			
			SDL_FillRect (::Bkg.surface, NULL, 0xFFFFFFFF);
		}
	}
	
	void wait_frame (void) {
		static unsigned long last = 0;
		unsigned long now = SDL_GetTicks ();
		int diff;
		if ((diff = last + (FRAME_RATE / 1000) - now) > 0)
			SDL_Delay (diff);
	}
	
	
	void end (void) {
		SDL_Quit ();
		exit (0);
	}
	
	
	void frame_done (void) {
		while (SDL_PollEvent (&Events)) {
			switch (Events.type) {
				case SDL_QUIT: end ();
					break;
				
				case SDL_JOYAXISMOTION: ::ProcessEvent (Events.jaxis);
					break;
				
			//	case SDL_JOYBUTTONDOWN: ::ProcessEvent (Events.jbutton);
			//		break;
			}
		}
		
		wait_frame ();
	}
	
	void Frame (void) {
		SDL_Flip (Screen);
		
		frame_done ();
		
		SDL_BlitSurface (Bkg.surface, NULL, Screen, NULL);
	}
	
	void Init (void) {
		_ (SDL_Init (SDL_INIT_EVERYTHING), "Could not initialize SDL!");
		
		_ (NULL == (
			Screen = SDL_SetVideoMode (
				SCR_WIDTH, SCR_HEIGHT,
				32, SDL_SWSURFACE/* | SDL_FULLSCREEN */
			)
		), "Could not initialize frame buffer!");
		
		_ (NULL == (
			Stick = SDL_JoystickOpen (0)
		), "Could not open joystick!");
		
		SurfaceGen::Bkg ();
		SurfaceGen::Player ();
	}
}

void DefaultOrigin (Sprite *sprite) {
	sprite -> origin.x = sprite -> surface -> w / 2;
	sprite -> origin.y = sprite -> surface -> h / 2;
}


void PlaceSprite (Sprite *sprite) {
	// polar positions
	sprite -> pos.x = (SCR_WIDTH  / 2) + (sinf (sprite -> cpos.d) * sprite -> cpos.r);
	sprite -> pos.y = (SCR_HEIGHT / 2) + (cosf (sprite -> cpos.d) * sprite -> cpos.r);
	
	SDL_Rect rect = {
		Sint16 (sprite -> pos.x + sprite -> origin.x),
		Sint16 (sprite -> pos.y + sprite -> origin.y)
	};
	
	SDL_BlitSurface (sprite -> surface, NULL, SDL::Screen, &rect);
}


class Beam {
	private:
		CCoord vel;
		unsigned int breaktime;
		Sprite sprite;

	public:
		Beam (void) {
			vel.d = (rnd (1) - 0.5) / 100.0;
			vel.r = rnd (1) + .5;
			
			sprite.cpos.d = rnd (RAD_MAX);
			sprite.cpos.r = 0;
			breaktime = rnd (10000);
			
			
			SDL_FreeSurface (sprite.surface);
			
			sprite.surface = SDL_CreateRGBSurface (
				SDL_SWSURFACE,
				BEAM_THICKNESS, BEAM_THICKNESS,
				32, MASK
			);
			
			SDL_FillRect (sprite.surface, NULL, 0xFF000000);
		}
		
		void Draw (void) {
			if (breaktime) {
				breaktime --;
				return;
			}
			
			sprite.cpos.d += vel.d;
			sprite.cpos.r += vel.r;
			PlaceSprite (&sprite);
			
			if (sprite.cpos.r >= CIRC_RADIUS_MAX)
				*this = Beam (); // reinit (memory leaks?)
		}
		
		bool Collision (void) {
			return sqrt (pow (sprite.pos.x - Player.s.pos.x, 2) + pow (sprite.pos.y - Player.s.pos.y, 2)) < PLAYER_RADIUS;
		}
};



int main (void) {
	Beam *beam = new Beam [MAX_BEAMS];
	size_t beams = 0;
	SDL::Init ();
	
	Player.s.cpos = { 0, CIRC_RADIUS_DEF };
	Player.vel = { 0, 0 };
	while (true) {
		for (size_t i = 0; i != 100; i ++) {
			beam [i].Draw ();
		}
		
		Player.s.cpos.r += Player.vel.y;
		Player.s.cpos.d += Player.vel.x;
		
		if (Player.s.cpos.r > CIRC_RADIUS_MAX)
			Player.s.cpos.r = CIRC_RADIUS_MAX;
		else
		if (Player.s.cpos.r < CIRC_RADIUS_MIN)
			Player.s.cpos.r = CIRC_RADIUS_MIN;
		
		PlaceSprite (&Player.s);
		
		SDL::Frame ();
	}
}
