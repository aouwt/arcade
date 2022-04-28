#define SCR_WIDTH	1280
#define SCR_HEIGHT	1024

#define FRAME_RATE	30

#define MAX_BEAMS	1024
#define PLAYER_RADIUS	8
#define BEAM_MAXTHICKNESS	16
#define BEAM_MINTHICKNESS	8

#define BUTTON_TOKEN	9

#define FONTPATH	"/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf"
#define FONTSZ	24

#include <SDL.h>
#include <SDL_ttf.h>
#include <math.h>


#define _(func, ...)	\
	if (func) {	\
		fprintf (stderr, __VA_ARGS__); \
		fprintf (stderr, " (%i)\n", __LINE__); \
		exit (1); \
	}
#define rnd(range)	((rand () / float (RAND_MAX)) * range)
#define rndf	(rand () / float (RAND_MAX))

#define MASK	0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000


#define CIRC_RADIUS_DEF	(SCR_WIDTH * 0.25)
#define CIRC_RADIUS_MIN	100
#define CIRC_RADIUS_MAX	((SCR_HEIGHT * 0.5) - 100)
#define CENTER_SCREEN	{ SCR_WIDTH / 2, SCR_HEIGHT / 2 }

#define RAD_MAX	(2 * M_PI)

typedef struct Coord {
	float x = 0;
	float y = 0;
} Coord;

typedef struct CCoord {
	float d = 0;
	float r = 0;
} CCoord;

typedef struct Sprite {
	Coord origin;
	SDL_Surface *surface = NULL;
	Coord pos;
	CCoord cpos;
} Sprite;


struct Player {
	Sprite s;
	Coord vel;
	Coord dir;
} Player;

struct Bkg {
	SDL_Surface *surface;
} Bkg;

int Tokens = 0;


namespace Score {
	SDL_Surface *surface;
	long current = 0;
	long long total = 0;
	long maximum = 0;
	bool changed = true;
	
	void Inc (unsigned int amt) {
		current += amt;
		total += amt;
		if (current > maximum)
			maximum = current;
		
		changed = true;
	}
	
	void Dec (unsigned int amt) {
		current -= amt;
		changed = true;
	}
	
	void Reset (void) {
		total = maximum = current = 100;
	}
}


namespace Sound {
#define LEN(sec)	(AudFmt.freq * (sec))
	SDL_AudioSpec AudFmt;
	
	typedef struct Audio {
		size_t len = 0;
		Uint8 *buf = NULL;
	} Audio;
	
	Audio Current;
	
	struct Effect {
		Audio Damage;
		Audio Token;
	} Effect;

	namespace Callback {
		static void audbuf (void *user, Uint8 *stream, int len) {
			if (Current.len == 0)
				return;
			
			for (int i = 0; i < len; i ++) {
				if (Current.len > 0) {
					Current.len --;
					Current.buf ++;
				} else
					break;
				
				stream [i] = *Current.buf;
			}
		}
	}
	
	Audio mk_sqr (float freq, unsigned char amp, size_t len, Uint8 *buf) {
		if (buf == NULL)
			buf = new Uint8 [len];
		
		for (size_t i = 0; i != len; i ++)
			buf [i] = ((i % int (AudFmt.freq / (freq / 2))) > (AudFmt.freq / freq)) * amp;
		
		return {
			.len = len,
			.buf = buf
		};
	}
	
	void Build (void) {
		Effect.Damage = mk_sqr (440, 64, LEN (0.5), NULL);
		Effect.Token = mk_sqr (783.990871963, 127, LEN (0.25), NULL);
	}
	
	void Init (void) {
		SDL_AudioSpec request = {
			.freq = 44100,
			.format = AUDIO_U8,
			.samples = 64,
			.callback = &Callback::audbuf
		};
		
		_ (SDL_OpenAudio (&request, &AudFmt), "Could not initialize sound!");
		SDL_PauseAudio (0);
		
		Build ();
	}
	
	void PlaySound (Audio *effect) {
		Current = *effect;
	}
#undef LEN
}


void ProcessEvent (SDL_JoyAxisEvent axis) {
	switch (axis.axis) {
		case 0: // X
			Player.dir.x = axis.value / 32768.0;
			break;
		case 1: // Y
			Player.dir.y = axis.value / 32768.0;
			break;
	}
}

void ProcessEvent (SDL_JoyButtonEvent btn) {
	switch (btn.button) {
		case BUTTON_TOKEN:
			Tokens ++;
			Sound::PlaySound (&Sound::Effect.Token);
			break;
	}
}



namespace SDL {
	SDL_Surface *Screen;
	SDL_Event Events;
	SDL_Joystick *Stick;
	TTF_Font *Font;
	
	
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
		
		//void Score (void) {
		//	::Score.surface.score = TTF_RenderText_Solid (Font, "Score: ", { 0, 0, 0, 0 });
		//}
	}
	
	void render_score (void) {
		char str [64];
		snprintf (str, 64, "%li / %lli", Score::current, Score::total);
		Score::surface = TTF_RenderText_Solid (Font, str, { 0, 0, 0, 0 });
	}
	
	void NextFrame (void) {
		static unsigned long last = 0;
		unsigned long now = SDL_GetTicks ();
		int diff;
		if ((diff = last + (FRAME_RATE / 1000) - now) > 0)
			SDL_Delay (diff);
	}
	
	
	void end (void) {
		TTF_CloseFont (Font);
		TTF_Quit ();
		SDL_FreeSurface (Score::surface);
		SDL_FreeSurface (Bkg.surface);
		SDL_FreeSurface (Player.s.surface);
		SDL_Quit ();
		exit (0);
	}
	
	
	void PollEvents (void) {
		while (SDL_PollEvent (&Events)) {
			switch (Events.type) {
				case SDL_QUIT: end ();
					break;
				
				case SDL_JOYAXISMOTION: ::ProcessEvent (Events.jaxis);
					break;
				
				case SDL_JOYBUTTONDOWN: ::ProcessEvent (Events.jbutton);
					break;
			}
		}
	}
	
	void Frame (void) {
		PollEvents ();
		NextFrame ();
	}
	
	void GameFrame (void) {
		SDL_Flip (Screen);
		
		Frame ();
		
		if (Score::changed) {
			render_score ();
			Score::changed = false;
		}
		SDL_BlitSurface (Bkg.surface, NULL, Screen, NULL);
		{	SDL_Rect rect = {
				0, 0,
				((float) Score::current / Score::total) * SCR_WIDTH, 16
			};
			SDL_FillRect (Screen, &rect, 0xFFFF00FF);
		}
		
		SDL_BlitSurface (Score::surface, NULL, Screen, NULL);
	}
	
	void Init (void) {
		fprintf (stderr, "Initializing...");
		
		fprintf (stderr, "\n\tSDL...");
		_ (SDL_Init (SDL_INIT_EVERYTHING), "Could not initialize SDL!");
		
		fprintf (stderr, "Done\n\tSDL_ttf...");
		_ (TTF_Init (), "Could not initialize SDL_ttf!");
		
		fprintf (stderr, "Done\n\tScreen...");
		_ (NULL == (
			Screen = SDL_SetVideoMode (
				SCR_WIDTH, SCR_HEIGHT,
				32, SDL_SWSURFACE/* | SDL_FULLSCREEN */
			)
		), "Could not initialize frame buffer!");
		
		fprintf (stderr, "Done\n\tFont...");
		_ (NULL == (
			Font = TTF_OpenFont (FONTPATH, FONTSZ)
		), "Could not open font \"%s\" at size %i", FONTPATH, FONTSZ)
		
		fprintf (stderr, "Done\n\tJoystick...");
		_ (NULL == (
			Stick = SDL_JoystickOpen (0)
		), "Could not open joystick!");
		
		fprintf (stderr, "Done\n\tSurfaces...");
		SurfaceGen::Bkg ();
		SurfaceGen::Player ();
		fprintf (stderr, "Done\nStarting game.\n");
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
		Sint16 (sprite -> pos.y + sprite -> origin.y),
		0, 0
	};
	
	SDL_BlitSurface (sprite -> surface, NULL, SDL::Screen, &rect);
}


class Beam {
	public:
		CCoord vel;
		unsigned int breaktime;
		unsigned short diameter;
		Sprite sprite;

		void reinit (void) {
			vel.d = (rnd (1) - 0.5) / 1000.0;
			vel.r = rnd (1) + 1;
			
			sprite.cpos.d = Player.s.cpos.d + ((rndf - .5) * (RAD_MAX / 5));// * (rndf - .5))) / 2;
			sprite.cpos.r = 0;
		}
		
		void Draw (void) {
			if (breaktime) {
				if (-- breaktime == 0)
					reinit ();
				return;
			}
			
			sprite.cpos.d += vel.d;
			sprite.cpos.r += vel.r;
			PlaceSprite (&sprite);
			
			if (sprite.cpos.r >= SCR_WIDTH / 2) {
				Score::Inc (vel.r * 10);
				Restart ();
			}
		}
		
		bool Collision (void) {
			if (breaktime)
				return false;
			else
				return sqrt (pow (sprite.pos.x - Player.s.pos.x, 2) + pow (sprite.pos.y - Player.s.pos.y, 2)) < diameter;
		}
		
		void Restart (void) { breaktime = rnd (128); }
		
		Beam (void) {
			diameter = (rndf * BEAM_MAXTHICKNESS) + BEAM_MINTHICKNESS;
			
			sprite.surface = SDL_CreateRGBSurface (
				SDL_SWSURFACE,
				diameter, diameter,
				32, MASK
			);
			
			SDL_FillRect (sprite.surface, NULL, 0xFF000000 | rand ());
			Restart ();
			//reinit ();
		}
		
		~Beam (void) {
			SDL_FreeSurface (sprite.surface);
		}
};


namespace Game {
	long Run (void) {
		Tokens --;
		Beam beam [MAX_BEAMS];
		size_t beams = 1;
		unsigned int flash = 0;
		long long nextbeamat = 2;
		float fuel = 0;
		
		Player.s.cpos = { 0, CIRC_RADIUS_DEF };
		Player.vel = { 0, 0 };
			
		while (Score::current >= 0) {
			Player.vel.x *= 0.9;
			if (fabs (Player.vel.x) < 0.02)
				Player.vel.x += Player.dir.x / 100;
			
			Player.vel.y *= 0.9;
			if (fabs (Player.vel.y) < 1)
				Player.vel.y += Player.dir.y;
			
			Player.s.cpos.r += Player.vel.y;
			Player.s.cpos.d += Player.vel.x;
			
			if (Player.s.cpos.r > CIRC_RADIUS_MAX)
				Player.s.cpos.r = CIRC_RADIUS_MAX;
			else
			if (Player.s.cpos.r < CIRC_RADIUS_MIN)
				Player.s.cpos.r = CIRC_RADIUS_MIN;
			
			if (!(flash & 1))
				PlaceSprite (&Player.s);
			
			for (size_t i = 0; i != beams; i ++) {
				beam [i].Draw ();
				
				if (flash == 0) {
					if (beam [i].Collision ()) {
						Score::Dec ((Score::total / 4) * (beam [i].diameter / BEAM_MAXTHICKNESS));
						flash = FRAME_RATE * 5;
						beam [i].Restart ();
						Sound::PlaySound (&Sound::Effect.Damage);
					}
				}
			}
			
			fuel += (abs (Player.dir.x) + abs (Player.dir.y)) / (FRAME_RATE * 2);
			if (fuel > 1)
				Score::Dec (fuel --);
			
			if (flash)
				flash --;
			
			if (Score::total > nextbeamat) {
				nextbeamat += Score::total;
				if (beams < MAX_BEAMS)
					beams ++;
			}
			
			SDL::GameFrame ();
		}
		
		return Score::maximum;
	}
}
		
int main (void) {
	SDL::Init ();
	Sound::Init ();
	
	for (;;) {
		while (Tokens == 0) {
			SDL::Frame ();
		}
		Game::Run ();
	}
}
